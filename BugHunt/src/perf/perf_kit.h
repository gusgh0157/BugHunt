#pragma once
#include <chrono>
#include <cstddef>
#include <memory>
#include <streambuf>

// 7부(성능) 랩용 계측 도구.
// 판정은 PC 속도에 흔들리지 않도록 "시간"이 아니라 "횟수"(할당 바이트, 복사, flush 등)로 한다.
// 시간은 참고로만 출력한다. 실제 느린 곳은 Release + 성능 프로파일러(Alt+F2)로 찾아보자.
namespace perf
{
// 이 할당자를 쓰는 컨테이너가 할당한 횟수와 바이트 수를 센다.
inline long long g_allocCount = 0;
inline long long g_allocBytes = 0;

inline void resetAllocCounters()
{
    g_allocCount = 0;
    g_allocBytes = 0;
}

template <class T>
struct CountingAllocator
{
    using value_type = T;

    CountingAllocator() = default;
    template <class U>
    CountingAllocator(const CountingAllocator<U>&) noexcept {}

    T* allocate(std::size_t n)
    {
        ++g_allocCount;
        g_allocBytes += static_cast<long long>(n * sizeof(T));
        return std::allocator<T>().allocate(n);
    }

    void deallocate(T* p, std::size_t n) noexcept { std::allocator<T>().deallocate(p, n); }

    template <class U>
    bool operator==(const CountingAllocator<U>&) const noexcept { return true; }
};

// 로그 "파일"을 흉내 내는 출력 버퍼. flush(sync)할 때마다 디스크에 쓰는 비용을 흉내 낸다.
class FakeLogFile : public std::streambuf
{
public:
    long long flushes() const { return flushes_; }
    long long bytes() const { return bytes_; }

protected:
    int_type overflow(int_type ch) override
    {
        if (ch != traits_type::eof())
            ++bytes_;
        return ch;
    }

    int sync() override
    {
        ++flushes_;
        // 디스크 쓰기 흉내: 약 20마이크로초 바쁘게 기다린다.
        const auto until = std::chrono::steady_clock::now() + std::chrono::microseconds(20);
        while (std::chrono::steady_clock::now() < until)
        {
        }
        return 0;
    }

private:
    long long flushes_ = 0;
    long long bytes_ = 0;
};

// 걸린 시간(밀리초)을 재는 작은 도구.
class Stopwatch
{
public:
    double ms() const { return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start_).count(); }

private:
    std::chrono::steady_clock::time_point start_ = std::chrono::steady_clock::now();
};
}
