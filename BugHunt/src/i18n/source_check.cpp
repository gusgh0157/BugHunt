#include "i18n.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// 빌드 뒤 검사 (BugHunt.exe --check-i18n <src 폴더>). CMake와 Visual Studio 프로젝트가 빌드할 때마다 실행한다.
// 한글이 든 문자열 리터럴은 반드시 일본어와 짝을 지어 적어야 한다. 허용하는 모양은 두 가지뿐이다.
//   i18n::L("한국어", "日本語")      호출하는 곳에서 고른다
//   { "한국어", "日本語" }           i18n::Text 표 (랩 목록 등)
// 일본어 쪽은 비어 있으면 안 되고, 한글이 들어 있으면 안 되며, 가나나 한자가 있어야 한다.
// static_assert 메시지는 L()을 쓸 수 없으므로 한 문자열에 "한국어 / 日本語"를 함께 적는다.
// 주석과 #pragma region 제목 안의 한글은 검사하지 않는다.
//
// [JA] ビルド後の検査 (BugHunt.exe --check-i18n <src フォルダー>)。CMake と Visual Studio のプロジェクトがビルドのたびに実行します。
// [JA] ハングルを含む文字列リテラルは、必ず日本語と対にして書きます。許される形は次の 2 つだけです。
// [JA]   i18n::L("한국어", "日本語")      呼び出し側で選ぶ
// [JA]   { "한국어", "日本語" }           i18n::Text の表(ラボ一覧など)
// [JA] 日本語側は空ではいけず、ハングルを含んでもいけず、かなか漢字が必要です。
// [JA] static_assert のメッセージには L() が使えないので、1 つの文字列に "한국어 / 日本語" を並べて書きます。
// [JA] コメントと #pragma region の見出しの中のハングルは検査しません。
namespace i18n
{
namespace
{
enum class Kind
{
    str,
    ident,
    punct,
    bad,   // 닫히지 않은 문자열이나 주석
};

struct Token
{
    Kind kind;
    std::string text;   // str: 리터럴 내용(일반 문자열은 이스케이프를 푼 것), ident: 이름, punct: 한 글자
    int line;
    size_t end = 0;     // ident: 소스에서 끝난 위치 (문자열 접두사인지 볼 때 쓴다)
};

bool isIdentChar(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

// UTF-8 문자열에 한글(자모 포함)이 있는가
bool hasHangul(const std::string& s)
{
    for (size_t i = 0; i < s.size();)
    {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        char32_t cp = c;
        size_t len = 1;
        if (c >= 0xF0 && i + 3 < s.size())
        {
            cp = ((c & 0x07u) << 18) | ((s[i + 1] & 0x3Fu) << 12) | ((s[i + 2] & 0x3Fu) << 6) | (s[i + 3] & 0x3Fu);
            len = 4;
        }
        else if (c >= 0xE0 && i + 2 < s.size())
        {
            cp = ((c & 0x0Fu) << 12) | ((s[i + 1] & 0x3Fu) << 6) | (s[i + 2] & 0x3Fu);
            len = 3;
        }
        else if (c >= 0xC0 && i + 1 < s.size())
        {
            cp = ((c & 0x1Fu) << 6) | (s[i + 1] & 0x3Fu);
            len = 2;
        }
        if ((cp >= 0x1100 && cp <= 0x11FF) || (cp >= 0x3130 && cp <= 0x318F) || (cp >= 0xA960 && cp <= 0xA97F)
            || (cp >= 0xAC00 && cp <= 0xD7FF))
            return true;
        i += len;
    }
    return false;
}

// 일본어 글자(가나, 한자)가 있는가
bool hasJapanese(const std::string& s)
{
    for (size_t i = 0; i + 2 < s.size(); ++i)
    {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0xE0 || c >= 0xF0)
            continue;
        const char32_t cp = ((c & 0x0Fu) << 12) | ((s[i + 1] & 0x3Fu) << 6) | (s[i + 2] & 0x3Fu);
        // 가나, 々, 한자(확장 A 포함), 반각 가타카나
        if ((cp >= 0x3040 && cp <= 0x30FF) || cp == 0x3005 || (cp >= 0x3400 && cp <= 0x4DBF) || (cp >= 0x4E00 && cp <= 0x9FFF)
            || (cp >= 0xFF66 && cp <= 0xFF9F))
            return true;
    }
    return false;
}

// src[i]의 '#'이 줄 첫 글자(앞은 공백만)이고 "#pragma region" / "#pragma endregion"인가
bool isPragmaRegion(const std::string& src, size_t i)
{
    for (size_t b = i; b > 0 && src[b - 1] != '\n'; --b)
    {
        if (src[b - 1] != ' ' && src[b - 1] != '\t')
            return false;
    }
    size_t p = i + 1;
    auto word = [&src, &p]() {
        while (p < src.size() && (src[p] == ' ' || src[p] == '\t'))
            ++p;
        const size_t b = p;
        while (p < src.size() && isIdentChar(src[p]))
            ++p;
        return src.substr(b, p - b);
    };
    if (word() != "pragma")
        return false;
    const std::string w = word();
    return w == "region" || w == "endregion";
}

// 코드 포인트 하나를 UTF-8로 붙인다
void appendUtf8(std::string& out, char32_t cp)
{
    if (cp < 0x80)
        out += static_cast<char>(cp);
    else if (cp < 0x800)
    {
        out += static_cast<char>(0xC0 | (cp >> 6));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    }
    else if (cp < 0x10000)
    {
        out += static_cast<char>(0xE0 | (cp >> 12));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    }
    else
    {
        out += static_cast<char>(0xF0 | ((cp >> 18) & 0x07));
        out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    }
}

int hexValue(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

// src[i]가 줄 잇기(\ 뒤 줄바꿈, \r\n 포함)이면 그 길이, 아니면 0
size_t spliceLength(const std::string& src, size_t i)
{
    if (src[i] != '\\')
        return 0;
    if (i + 1 < src.size() && src[i + 1] == '\n')
        return 2;
    if (i + 2 < src.size() && src[i + 1] == '\r' && src[i + 2] == '\n')
        return 3;
    return 0;
}

// 주석을 건너뛰고 문자열 리터럴, 이름, 기호로 나눈다. 숫자는 이름으로 친다.
// 일반 문자열의 이스케이프(가, \xEA 등)는 풀어서 담는다. 이스케이프로 적은 한글도 잡으려고.
std::vector<Token> tokenize(const std::string& src)
{
    std::vector<Token> out;
    int line = 1;
    size_t i = 0;
    const size_t n = src.size();
    auto countLines = [&line, &src](size_t from, size_t to) {
        line += static_cast<int>(std::count(src.begin() + static_cast<std::ptrdiff_t>(from), src.begin() + static_cast<std::ptrdiff_t>(to), '\n'));
    };
    auto isPrefix = [](const std::string& s, bool raw) {
        return raw ? (s == "R" || s == "u8R" || s == "LR" || s == "uR" || s == "UR") : (s == "u8" || s == "L" || s == "u" || s == "U");
    };
    while (i < n)
    {
        const char c = src[i];
        if (const size_t splice = spliceLength(src, i))
        {
            ++line;
            i += splice;
        }
        else if (c == '\n')
        {
            ++line;
            ++i;
        }
        else if (c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v' || c == '\\')
        {
            ++i;
        }
        else if (c == '#' && isPragmaRegion(src, i))
        {
            // #pragma region 힌트 1 / ヒント 1 (...) 은 접기용 제목이다. 주석처럼 건너뛴다
            while (i < n && src[i] != '\n')
                ++i;
        }
        else if (c == '/' && i + 1 < n && src[i + 1] == '/')
        {
            // 줄 끝의 \ 는 주석을 다음 줄로 잇는다
            while (i < n && src[i] != '\n')
            {
                if (const size_t continued = spliceLength(src, i))
                {
                    ++line;
                    i += continued;
                }
                else
                    ++i;
            }
        }
        else if (c == '/' && i + 1 < n && src[i + 1] == '*')
        {
            const size_t end = src.find("*/", i + 2);
            const size_t stop = end == std::string::npos ? n : end + 2;
            countLines(i, stop);
            i = stop;
            if (end == std::string::npos)
                out.push_back({ Kind::bad, "/*", line });
        }
        else if (c == '"')
        {
            // 접두사(u8, L, R, u8R ...)는 바로 앞 이름 토큰으로 이미 들어와 있다. 따옴표에 바로 붙어 있을 때만 접두사다.
            // (i18n::L("..")의 L은 괄호가 사이에 있으므로 접두사가 아니다)
            bool raw = false;
            if (!out.empty() && out.back().kind == Kind::ident && out.back().end == i)
            {
                raw = isPrefix(out.back().text, true);
                if (raw || isPrefix(out.back().text, false))
                    out.pop_back();
            }
            const int startLine = line;
            std::string text;
            bool closed = false;
            if (raw)
            {
                // R"구분자( ... )구분자"  구분자는 16자 이하, 공백·괄호·역슬래시 없음
                size_t open = i + 1;
                while (open < n && open - i - 1 <= 16 && src[open] != '(' && src[open] != ' ' && src[open] != ')' && src[open] != '\\'
                       && src[open] != '\n' && src[open] != '"')
                    ++open;
                if (open < n && src[open] == '(')
                {
                    const std::string delim = ")" + src.substr(i + 1, open - i - 1) + "\"";
                    const size_t close = src.find(delim, open + 1);
                    if (close != std::string::npos)
                    {
                        text = src.substr(open + 1, close - open - 1);
                        countLines(i, close + delim.size());
                        i = close + delim.size();
                        closed = true;
                    }
                }
            }
            else
            {
                ++i;
                while (i < n && src[i] != '"' && src[i] != '\n')
                {
                    if (const size_t continued = spliceLength(src, i))
                    {
                        ++line;
                        i += continued;
                    }
                    else if (src[i] == '\\' && i + 1 < n)
                    {
                        const char e = src[i + 1];
                        i += 2;
                        if (e == 'u' || e == 'U')
                        {
                            const size_t digits = e == 'u' ? 4 : 8;
                            char32_t cp = 0;
                            size_t k = 0;
                            for (; k < digits && i < n && hexValue(src[i]) >= 0; ++k, ++i)
                                cp = cp * 16 + static_cast<char32_t>(hexValue(src[i]));
                            appendUtf8(text, cp);
                        }
                        else if (e == 'x')
                        {
                            unsigned v = 0;
                            while (i < n && hexValue(src[i]) >= 0)
                                v = v * 16 + static_cast<unsigned>(hexValue(src[i++]));
                            text += static_cast<char>(v & 0xFF);
                        }
                        else if (e >= '0' && e <= '7')
                        {
                            unsigned v = static_cast<unsigned>(e - '0');
                            for (int k = 0; k < 2 && i < n && src[i] >= '0' && src[i] <= '7'; ++k)
                                v = v * 8 + static_cast<unsigned>(src[i++] - '0');
                            text += static_cast<char>(v & 0xFF);
                        }
                        else
                            text += e == 'n' ? '\n' : e == 't' ? '\t' : e;
                    }
                    else
                        text += src[i++];
                }
                closed = i < n && src[i] == '"';
                if (closed)
                    ++i;
            }
            if (!closed)
            {
                // 닫히지 않은 문자열: 그 뒤는 믿을 수 없으므로 알리고 파일 검사를 멈춘다
                out.push_back({ Kind::bad, text, startLine });
                return out;
            }
            while (i < n && isIdentChar(src[i]))   // 사용자 정의 리터럴 접미사 ("..."s, "..."sv)
                ++i;
            out.push_back({ Kind::str, text, startLine });
        }
        else if (c == '\'')
        {
            // 문자 리터럴 (숫자 구분자 1'000은 숫자 토큰이 이미 먹었다. 접두사 L'x' 등은 앞 이름 토큰에 남는다)
            ++i;
            while (i < n && src[i] != '\'' && src[i] != '\n')
                i += (src[i] == '\\' && i + 1 < n) ? 2 : 1;
            if (i < n && src[i] == '\'')
                ++i;
            out.push_back({ Kind::punct, "'", line });
        }
        else if (isIdentChar(c))
        {
            const size_t b = i;
            const bool number = c >= '0' && c <= '9';
            while (i < n
                   && (isIdentChar(src[i]) || (number && src[i] == '.')
                       || (number && src[i] == '\'' && i + 1 < n && isIdentChar(src[i + 1]))))
                ++i;
            out.push_back({ Kind::ident, src.substr(b, i - b), line, i });
        }
        else
        {
            out.push_back({ Kind::punct, std::string(1, c), line });
            ++i;
        }
    }
    return out;
}

// tokens[i]부터 이어진 문자열 리터럴들(이어 붙이기)의 끝을 돌려준다. 내용은 joined에 담는다.
size_t literalGroup(const std::vector<Token>& tokens, size_t i, std::string& joined)
{
    joined.clear();
    while (i < tokens.size() && tokens[i].kind == Kind::str)
        joined += tokens[i++].text;
    return i;
}

bool isPunct(const std::vector<Token>& tokens, size_t i, char c)
{
    return i < tokens.size() && tokens[i].kind == Kind::punct && tokens[i].text[0] == c;
}

// 파일 하나를 검사해 문제를 problems에 더한다
void checkFile(const std::filesystem::path& path, const std::string& shown, std::vector<std::string>& problems)
{
    std::ifstream in(path, std::ios::binary);
    if (!in)
    {
        problems.push_back(shown + ": " + L("파일을 읽을 수 없다", "ファイルを読めない"));
        return;
    }
    const std::string src((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    const std::vector<Token> tokens = tokenize(src);
    if (!tokens.empty() && tokens.back().kind == Kind::bad)
        problems.push_back(shown + ":" + std::to_string(tokens.back().line) + ": "
                           + L("닫히지 않은 문자열이나 주석. 이 뒤는 검사하지 못했다", "閉じていない文字列かコメント。ここから後は検査できなかった"));

    std::vector<std::string> calls;   // 열린 괄호마다 바로 앞의 이름 (static_assert 안인지 알려고)
    for (size_t i = 0; i < tokens.size();)
    {
        if (tokens[i].kind != Kind::str)
        {
            if (isPunct(tokens, i, '('))
                calls.push_back(i > 0 && tokens[i - 1].kind == Kind::ident ? tokens[i - 1].text : std::string());
            else if (isPunct(tokens, i, ')') && !calls.empty())
                calls.pop_back();
            ++i;
            continue;
        }
        std::string first;
        const size_t afterFirst = literalGroup(tokens, i, first);
        const int line = tokens[i].line;
        const size_t start = i;
        i = afterFirst;
        if (!hasHangul(first))
            continue;

        auto report = [&](const char* message) {
            problems.push_back(shown + ":" + std::to_string(line) + ": " + message + "\n    \"" + first + "\"");
        };

        // static_assert의 메시지는 L()을 쓸 수 없으므로 한 문자열에 두 언어를 함께 적는다 ("한국어 / 日本語")
        if (!calls.empty() && calls.back() == "static_assert")
        {
            if (!hasJapanese(first))
                report(L("static_assert 메시지에 일본어가 없다. \"한국어 / 日本語\"처럼 함께 적으세요",
                         "static_assert のメッセージに日本語がない。\"ko / 日本語\" のように並べて書いてください"));
            continue;
        }

        const bool afterL = start >= 2 && isPunct(tokens, start - 1, '(') && tokens[start - 2].kind == Kind::ident && tokens[start - 2].text == "L";
        const bool afterBrace = start >= 1 && isPunct(tokens, start - 1, '{');
        if (!afterL && !afterBrace)
        {
            report(L("한글 문자열에 일본어 짝이 없다. i18n::L(\"ko\", \"ja\")로 적으세요",
                     "ハングルの文字列に日本語の対がない。i18n::L(\"ko\", \"ja\") と書いてください"));
            continue;
        }
        std::string second;
        const size_t afterSecond = isPunct(tokens, afterFirst, ',') ? literalGroup(tokens, afterFirst + 1, second) : afterFirst;
        const bool closed = afterSecond > afterFirst + 1 && isPunct(tokens, afterSecond, afterL ? ')' : '}');
        if (!closed)
        {
            if (afterL)
                report(L("i18n::L의 두 인자가 모두 문자열 리터럴이어야 한다", "i18n::L の 2 つの引数は両方とも文字列リテラルでなければならない"));
            else
                report(L("{ \"ko\", \"ja\" } 모양이 아니다", "{ \"ko\", \"ja\" } の形になっていない"));
            continue;
        }
        if (second.empty())
            report(L("일본어 쪽이 비어 있다", "日本語側が空になっている"));
        else if (hasHangul(second))
            report(L("일본어 쪽에 한글이 들어 있다", "日本語側にハングルが入っている"));
        else if (!hasJapanese(second))
            report(L("일본어 쪽에 가나나 한자가 없다", "日本語側にかなも漢字もない"));
        i = afterSecond;
    }
}
}

int checkSources(const std::string& dir)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    if (!fs::is_directory(dir, ec))
    {
        std::cout << "--check-i18n: " << L("폴더가 없다: ", "フォルダーがない: ") << dir << '\n';
        return 2;
    }
    // 예외를 던지지 않는 error_code 판을 쓴다 (빌드 중에 abort 대화 상자가 뜨지 않게)
    std::vector<fs::path> files;
    for (fs::recursive_directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec))
    {
        const fs::path ext = it->path().extension();
        std::error_code fileEc;
        if (it->is_regular_file(fileEc) && (ext == ".cpp" || ext == ".h" || ext == ".inc"))
            files.push_back(it->path());
    }
    if (ec)
    {
        std::cout << "--check-i18n: " << L("폴더를 읽을 수 없다: ", "フォルダーを読めない: ") << dir << " (" << ec.message() << ")\n";
        return 2;
    }
    std::sort(files.begin(), files.end());

    std::vector<std::string> problems;
    for (const fs::path& f : files)
    {
        std::error_code relEc;
        const fs::path rel = fs::relative(f, dir, relEc);
        checkFile(f, (relEc ? f : rel).generic_string(), problems);
    }

    for (const std::string& p : problems)
        std::cout << p << '\n';
    if (!problems.empty())
    {
        std::cout << L("--check-i18n: 문제 ", "--check-i18n: 問題 ") << problems.size()
                  << L("개\n", " 個\n");
        return 1;
    }
    std::cout << "--check-i18n: OK (" << files.size() << L("개 파일)\n", " ファイル)\n");
    return 0;
}
}
