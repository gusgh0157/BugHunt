#include "../labs.h"
#include "../platform/asan_detect.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>

// BugHunt 메모리 검사기. 표준이 허용하는 "전역 operator new/delete 교체"로 만든다.
// 컴파일러나 Debug/Release 구성과 상관없이 같은 방식으로 동작하므로,
// 메모리 버그 랩의 결과가 컴파일러의 디버그 힙(채움 값 등)에 따라 달라지지 않는다.
//
//   새로 할당한 메모리     → 0xA5로 채운다 (초기화하지 않은 값을 읽으면 엉뚱한 큰 수가 나온다)
//   해제한 메모리          → 0xEE로 덮고 바로 돌려주지 않는다 (해제 후 사용하면 엉뚱한 값이 나온다)
//   블록 뒤 16바이트       → 0xBD 경계. 해제할 때 바뀌어 있으면 버퍼 오버런으로 보고
//   이중 해제, new[]/delete 짝 틀림, new로 받지 않은 주소 해제 → [CRASH] 메시지 후 abort()
//
// ASan 구성에서는 ASan이 같은 일을 더 정확하게 하므로 교체하지 않는다.
namespace
{
constexpr unsigned char kFreshByte = 0xA5;
constexpr unsigned char kFreedByte = 0xEE;
constexpr unsigned char kGuardByte = 0xBD;
constexpr std::size_t kGuardSize = 16;

[[noreturn]] void fail(const char* what, const char* checker = "메모리 검사")
{
    std::fflush(stdout);
    std::fprintf(stderr, "\n[CRASH] %s: %s\n", checker, what);
    std::fflush(stderr);
    std::abort();
}
}

void* bughunt::recycledStorage(std::size_t bytes)
{
    alignas(std::max_align_t) static unsigned char pool[4096];
    if (bytes > sizeof(pool))
        fail("recycledStorage: 요청이 너무 크다");
    std::memset(pool, kFreshByte, bytes);   // 앞 사용자가 남긴 값
    return pool;
}

void bughunt::invalidComparator()
{
    fail("비교 함수가 엄격한 약순서(strict weak ordering)를 어겼다: comp(a, a)가 true이거나, comp(a, b)와 comp(b, a)가 둘 다 true", "정렬 검사");
}

#if !defined(BUGHUNT_ASAN)
namespace
{
constexpr std::uint64_t kAliveMagic = 0x4255474855'4E5441ull;   // "BUGHUNTA"
constexpr std::uint64_t kFreedMagic = 0x4255474855'4E5446ull;   // "BUGHUNTF"

enum class Kind : std::uint32_t
{
    Single = 1,   // new
    Array = 2,    // new[]
};

struct Header
{
    std::uint64_t magic;
    std::size_t size;
    Kind kind;
};
// 사용자에게 주는 주소가 new의 기본 정렬을 지키도록 머리 크기를 정렬 단위의 배수로 잡는다.
constexpr std::size_t kHeaderSize = (sizeof(Header) + __STDCPP_DEFAULT_NEW_ALIGNMENT__ - 1) / __STDCPP_DEFAULT_NEW_ALIGNMENT__ * __STDCPP_DEFAULT_NEW_ALIGNMENT__;

// 해제한 블록을 바로 돌려주지 않고 잠시 붙잡아 둔다. 곧바로 다른 할당에 재사용되면
// 해제 후 사용이 우연히 "맞는 값"을 읽을 수 있기 때문이다. (ASan의 격리 구역과 같은 생각)
constexpr std::size_t kQuarantineSlots = 4096;
constexpr std::size_t kQuarantineBytes = 8u << 20;
unsigned char* g_quarantine[kQuarantineSlots];
std::size_t g_quarantineSizes[kQuarantineSlots];
std::size_t g_quarantineNext = 0;
std::size_t g_quarantineTotal = 0;
std::atomic_flag g_lock = ATOMIC_FLAG_INIT;   // 정적 초기화 순서와 상관없이 쓸 수 있는 잠금

struct LockGuard
{
    LockGuard()
    {
        while (g_lock.test_and_set(std::memory_order_acquire))
        {
        }
    }
    ~LockGuard() { g_lock.clear(std::memory_order_release); }
};

void* allocate(std::size_t size, Kind kind)
{
    if (size > static_cast<std::size_t>(-1) - kHeaderSize - kGuardSize)
        throw std::bad_alloc();
    auto* raw = static_cast<unsigned char*>(std::malloc(kHeaderSize + size + kGuardSize));
    if (raw == nullptr)
        throw std::bad_alloc();
    ::new (raw) Header{ kAliveMagic, size, kind };
    unsigned char* user = raw + kHeaderSize;
    std::memset(user, kFreshByte, size);
    std::memset(user + size, kGuardByte, kGuardSize);
    return user;
}

void release(void* p, Kind kind)
{
    if (p == nullptr)
        return;
    auto* user = static_cast<unsigned char*>(p);
    auto* raw = user - kHeaderSize;
    auto* header = reinterpret_cast<Header*>(raw);

    if (header->magic == kFreedMagic)
        fail("이미 해제한 메모리를 다시 해제했다 (이중 해제)");
    if (header->magic != kAliveMagic)
        fail("new로 받은 주소가 아닌 곳을 해제했다 (new[]로 만든 배열을 delete로 지웠거나, 잘못된 포인터)");
    if (header->kind != kind)
        fail(kind == Kind::Single ? "new[]로 만든 배열을 delete로 해제했다 (delete[]를 써야 한다)"
                                  : "new로 만든 객체를 delete[]로 해제했다 (delete를 써야 한다)");
    for (std::size_t i = 0; i < kGuardSize; ++i)
    {
        if (user[header->size + i] != kGuardByte)
            fail("할당한 크기를 넘어서 썼다 (버퍼 오버런). 블록 바로 뒤 경계가 바뀌어 있다");
    }

    header->magic = kFreedMagic;
    std::memset(user, kFreedByte, header->size);

    const std::size_t blockBytes = kHeaderSize + header->size + kGuardSize;
    LockGuard lock;
    if (g_quarantine[g_quarantineNext] != nullptr)
    {
        std::free(g_quarantine[g_quarantineNext]);
        g_quarantineTotal -= g_quarantineSizes[g_quarantineNext];
    }
    g_quarantine[g_quarantineNext] = raw;
    g_quarantineSizes[g_quarantineNext] = blockBytes;
    g_quarantineTotal += blockBytes;
    g_quarantineNext = (g_quarantineNext + 1) % kQuarantineSlots;

    // 너무 많이 붙잡고 있으면 오래된 것부터 돌려준다.
    for (std::size_t i = 0; g_quarantineTotal > kQuarantineBytes && i < kQuarantineSlots; ++i)
    {
        const std::size_t slot = (g_quarantineNext + i) % kQuarantineSlots;
        if (g_quarantine[slot] != nullptr)
        {
            std::free(g_quarantine[slot]);
            g_quarantineTotal -= g_quarantineSizes[slot];
            g_quarantine[slot] = nullptr;
        }
    }
}
}

void* operator new(std::size_t size) { return allocate(size, Kind::Single); }
void* operator new[](std::size_t size) { return allocate(size, Kind::Array); }

void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
    try
    {
        return allocate(size, Kind::Single);
    }
    catch (...)
    {
        return nullptr;
    }
}

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept
{
    try
    {
        return allocate(size, Kind::Array);
    }
    catch (...)
    {
        return nullptr;
    }
}

void operator delete(void* p) noexcept { release(p, Kind::Single); }
void operator delete[](void* p) noexcept { release(p, Kind::Array); }
void operator delete(void* p, std::size_t) noexcept { release(p, Kind::Single); }
void operator delete[](void* p, std::size_t) noexcept { release(p, Kind::Array); }
void operator delete(void* p, const std::nothrow_t&) noexcept { release(p, Kind::Single); }
void operator delete[](void* p, const std::nothrow_t&) noexcept { release(p, Kind::Array); }
#endif
