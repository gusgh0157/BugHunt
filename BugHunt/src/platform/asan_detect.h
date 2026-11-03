#pragma once

// AddressSanitizer로 빌드했는지. GCC와 MSVC는 __SANITIZE_ADDRESS__를, Clang은 __has_feature를 쓴다.
#if defined(__SANITIZE_ADDRESS__)
#define BUGHUNT_ASAN 1
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
#define BUGHUNT_ASAN 1
#endif
#endif
