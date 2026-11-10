#include "platform.h"
#include "asan_detect.h"

// 빌드 구성은 표준 매크로 NDEBUG와 컴파일러가 알려 주는 ASan 표시로 판단한다.
// [JA] ビルド構成は、標準マクロ NDEBUG とコンパイラーが知らせる ASan の印で判断します。
const char* platform::buildName()
{
#if defined(BUGHUNT_ASAN)
    return "ASan";
#elif defined(NDEBUG)
    return "Release";
#else
    return "Debug";
#endif
}
