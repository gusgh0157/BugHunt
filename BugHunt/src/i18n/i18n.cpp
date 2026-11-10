#include "i18n.h"

#include "../platform/platform.h"

#include <iostream>
#include <string_view>

namespace i18n
{
namespace
{
Lang g_lang = Lang::ko;

// "ja", "JA", "ja_JP.UTF-8", "ja-JP" → ja / "ko..." → ko / 그 밖 → false
// [JA] "ja", "JA", "ja_JP.UTF-8", "ja-JP" → ja / "ko..." → ko / それ以外 → false
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
        {
            // 아직 언어를 모르므로 두 언어를 모두 보여 준다
            // [JA] まだ言語が分からないので、両方の言語を表示します
            constexpr Text kUsage = { "ko 또는 ja", "ko または ja" };
            std::cout << "--lang: " << kUsage.ko << " / " << kUsage.ja << " (" << value << ")\n";
        }
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
}
