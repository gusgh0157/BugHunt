#include "i18n.h"

#include "../platform/platform.h"

#include <iostream>
#include <string_view>

namespace i18n
{
namespace
{
// expect 설명 → 일본어. 키는 랩 코드의 한국어 문자열과 글자 하나까지 같아야 한다.
constexpr Text kExpectTexts[] = {
#include "strings_expect.inc"
};
static_assert(allTranslated(kExpectTexts, [](const Text& t) { return t; }), "strings_expect.inc에 빈 번역이 있다");

Lang g_lang = Lang::ko;

// "ja", "JA", "ja_JP.UTF-8", "ja-JP" → ja / "ko..." → ko / 그 밖 → false
bool parseLang(std::string_view text, Lang& out)
{
    if (text.size() < 2)
        return false;
    const char a = static_cast<char>(text[0] | 0x20);
    const char b = static_cast<char>(text[1] | 0x20);
    if (text.size() > 2 && text[2] != '_' && text[2] != '-' && text[2] != '.')
        return false;
    if (a == 'j' && b == 'a')
        out = Lang::ja;
    else if (a == 'k' && b == 'o')
        out = Lang::ko;
    else
        return false;
    return true;
}
}

std::vector<std::string> init(int argc, char** argv)
{
    std::vector<std::string> rest;
    bool fromArgs = false;
    for (int i = 0; i < argc; ++i)
    {
        const std::string_view arg = argv[i];
        std::string_view value;
        if (i > 0 && arg == "--lang" && i + 1 < argc)
            value = argv[++i];
        else if (i > 0 && arg.substr(0, 7) == "--lang=")
            value = arg.substr(7);
        else
        {
            rest.emplace_back(arg);
            continue;
        }
        if (parseLang(value, g_lang))
            fromArgs = true;
        else
            std::cout << "--lang: ko 또는 ja / ko または ja (" << value << ")\n";
    }
    if (fromArgs)
        return rest;

    if (parseLang(platform::envVar("BUGHUNT_LANG"), g_lang))
        return rest;

    const std::string os = platform::osLanguage();
    if (!parseLang(os, g_lang))
        g_lang = Lang::ko;
    return rest;
}

Lang current()
{
    return g_lang;
}

const char* code()
{
    return g_lang == Lang::ja ? "ja" : "ko";
}

const char* tr(const char* ko)
{
    if (g_lang == Lang::ko)
        return ko;
    const std::string_view key = ko;
    for (const Text& t : kExpectTexts)
    {
        if (key == t.ko)
            return t.ja;
    }

    // 같은 문구를 여러 번 알리지 않는다. (할당하지 않으려고 고정 크기 배열)
    static const char* reported[64] = {};
    static int reportedCount = 0;
    for (int i = 0; i < reportedCount; ++i)
    {
        if (reported[i] == ko)
            return ko;
    }
    if (reportedCount < 64)
        reported[reportedCount++] = ko;
    std::cout << "  " << kMissingMark << ko << '\n';
    return ko;
}
}
