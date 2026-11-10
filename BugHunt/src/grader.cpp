#include "grader.h"
#include "labs.h"

#include "platform/platform.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

// 채점기 구조
//   BugHunt.exe --grade            → 아래 runGrader. 랩마다 자식 프로세스를 띄운다.
//   BugHunt.exe --grade-child N    → 랩 N을 평소 데이터로 실행
//   BugHunt.exe --grade-child N --hidden → 같은 랩을 화면에 나오지 않는 데이터로 실행 (숨은 케이스)
// 자식이 크래시하거나 멈춰도 채점기와 다른 랩은 영향을 받지 않는다.
// 판정은 자식의 종료 코드(labs.h의 kExit*)와 시간 제한으로만 한다.
// [JA] 採点器の構造
// [JA]   BugHunt.exe --grade            → 下の runGrader。ラボごとに子プロセスを起動します。
// [JA]   BugHunt.exe --grade-child N    → ラボ N を通常のデータで実行
// [JA]   BugHunt.exe --grade-child N --hidden → 同じラボを画面に出ないデータで実行 (隠しケース)
// [JA] 子プロセスがクラッシュしたり止まったりしても、採点器とほかのラボは影響を受けません。
// [JA] 判定は子プロセスの終了コード(labs.h の kExit*)と制限時間だけで行います。
namespace
{
constexpr int kChildTimeoutMs = 15000;   // 랩 하나(케이스 하나)의 최대 실행 시간 [JA] ラボ 1 つ(ケース 1 つ)の最大実行時間

using ChildResult = platform::ProcessResult;

// 자식 프로세스 실행은 운영체제마다 다르므로 src/platform에 맡긴다.
// [JA] 子プロセスの実行は OS ごとに異なるので、src/platform に任せます。
ChildResult runChild(int labId, bool hiddenCase)
{
    // 자식도 같은 언어로 출력하게 한다 (title, detail이 고른 언어로 나오도록)
    // [JA] 子プロセスも同じ言語で出力させます (title、detail が選んだ言語で出るように)
    std::vector<std::string> args = { "--lang", i18n::code(), "--grade-child", std::to_string(labId) };
    if (hiddenCase)
        args.push_back("--hidden");
    return platform::runSelf(args, kChildTimeoutMs);
}

std::string trim(const std::string& s)
{
    const size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos)
        return {};
    const size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

// output에서 needle이 들어 있는 첫 줄
// [JA] output の中で needle を含む最初の行
std::string firstLineWith(const std::string& output, const char* needle)
{
    const size_t at = output.find(needle);
    if (at == std::string::npos)
        return {};
    const size_t b = output.rfind('\n', at);
    const size_t e = output.find('\n', at);
    return trim(output.substr(b == std::string::npos ? 0 : b + 1,
                              e == std::string::npos ? std::string::npos : e - (b == std::string::npos ? 0 : b + 1)));
}

// 자식 출력은 깨진 문자열(랩 11 등)을 담을 수 있다. JSON이 깨지지 않도록 올바른 UTF-8만 남기고 길이를 줄인다.
// [JA] 子プロセスの出力には壊れた文字列(ラボ 11 など)が含まれることがあります。JSON が壊れないよう、正しい UTF-8 だけを残して長さを縮めます。
std::string cleanDetail(std::string s)
{
    // "[CRASH] C:\...\include\vector(1949) : Assertion failed: ..." → "[CRASH] vector(1949) : ..."
    const size_t colon = s.find(") : ");
    const size_t slash = colon == std::string::npos ? std::string::npos : s.find_last_of("\\/", colon);
    const size_t crash = s.find("[CRASH] ");
    if (slash != std::string::npos && crash != std::string::npos && crash + 8 <= slash)
        s.erase(crash + 8, slash + 1 - (crash + 8));

    // "==1234==ERROR: AddressSanitizer: heap-use-after-free on address 0x... at pc ..." → "ERROR: AddressSanitizer: heap-use-after-free"
    const size_t asanError = s.find("ERROR: AddressSanitizer");
    if (asanError != std::string::npos)
    {
        const size_t pid = asanError >= 3 ? s.rfind("==", asanError - 3) : std::string::npos;
        if (pid != std::string::npos)
            s.erase(pid, asanError - pid);
        size_t address = s.find(" on address 0x");
        if (address == std::string::npos)
            address = s.find(": 0x");
        if (address != std::string::npos)
            s.erase(address);
    }

    std::string out;
    for (size_t i = 0; i < s.size() && out.size() < 300;)
    {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        const size_t len = c < 0x80 ? 1 : (c >> 5) == 0x6 ? 2 : (c >> 4) == 0xE ? 3 : (c >> 3) == 0x1E ? 4 : 0;
        bool valid = len != 0 && i + len <= s.size();
        for (size_t k = 1; valid && k < len; ++k)
            valid = (static_cast<unsigned char>(s[i + k]) & 0xC0) == 0x80;
        if (valid)
        {
            out.append(s, i, len);
            i += len;
        }
        else
        {
            out += '?';
            ++i;
        }
    }
    return out;
}

struct Verdict
{
    const char* result;
    std::string detail;
};

Verdict judge(const ChildResult& r, bool hiddenCase)
{
    // 평소 데이터는 통과했는데 숨은 케이스에서 실패 = 증상만 가린 수정 (BugHunt 네 판 공통 문구)
    // [JA] 通常のデータでは通ったのに隠しケースで失敗 = 症状だけを隠した修正 (BugHunt 4 版共通の文言)
    const std::string prefix = hiddenCase ? i18n::L("증상은 사라졌지만 원인이 남아 있음 (숨은 케이스) ", "症状は消えたが原因が残っている (隠しケース) ") : "";
    if (r.timedOut)
        return { "TIMEOUT", prefix + i18n::L("채점기 시간 제한 ", "採点器の制限時間 ") + std::to_string(kChildTimeoutMs) + i18n::L(" ms 초과", " ms 超過") };

    switch (r.exitCode)
    {
    case kExitPass:
        return { "PASS", "" };
    case kExitFail:
        return { "FAIL", cleanDetail(prefix + firstLineWith(r.output, "[FAIL]")) };
    case kExitTimeout:
        return { "TIMEOUT", cleanDetail(prefix + firstLineWith(r.output, "[FAIL]")) };
    default:
    {
        std::string detail = prefix + platform::describeCrash(r.exitCode);
        std::string clue = firstLineWith(r.output, "ERROR: AddressSanitizer");
        if (clue.empty())
            clue = firstLineWith(r.output, "[CRASH]");
        if (clue.empty())
            clue = firstLineWith(r.output, "[FAIL]");
        if (!clue.empty())
            detail += " - " + clue;
        if (r.exitCode == 1 && r.output.find("AddressSanitizer") != std::string::npos)
            detail = prefix + clue;
        return { "CRASH", cleanDetail(detail) };
    }
    }
}

std::string jsonEscape(const std::string& s)
{
    std::string out;
    for (const char ch : s)
    {
        const unsigned char c = static_cast<unsigned char>(ch);
        switch (c)
        {
        case '"':  out += "\\\""; break;
        case '\\': out += "\\\\"; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
            if (c < 0x20)
            {
                char esc[8];
                std::snprintf(esc, sizeof(esc), "\\u%04x", c);
                out += esc;
            }
            else
            {
                out += ch;
            }
        }
    }
    return out;
}

struct LabRecord
{
    const GradeTarget* target;
    const char* result;
    long long ms;
    std::string detail;
};

void writeJson(const char* path, const std::vector<LabRecord>& records, int passed)
{
    std::ofstream out(path, std::ios::binary);
    out << "{\n"
        << "  \"edition\": \"cpp\",\n"
        << "  \"version\": 1,\n"
        << "  \"total\": " << records.size() << ",\n"
        << "  \"passed\": " << passed << ",\n"
        << "  \"labs\": [\n";
    for (size_t i = 0; i < records.size(); ++i)
    {
        const LabRecord& r = records[i];
        out << "    { \"id\": " << r.target->id
            << ", \"title\": \"" << jsonEscape(r.target->title) << "\""
            << ", \"part\": " << r.target->part
            << ", \"result\": \"" << r.result << "\""
            << ", \"ms\": " << r.ms
            << ", \"detail\": \"" << jsonEscape(r.detail) << "\" }"
            << (i + 1 < records.size() ? ",\n" : "\n");
    }
    out << "  ]\n}\n";
}
}

void prepareGradeChild()
{
    // 크래시 대화 상자, Debug 어설션 대화 상자, abort() 메시지 창을 모두 끈다. (운영체제별 코드는 src/platform)
    // [JA] クラッシュダイアログ、Debug アサーションダイアログ、abort() のメッセージウィンドウをすべて無効にします。(OS 別のコードは src/platform)
    platform::prepareCrashExit();

    // 크래시 직전까지의 출력이 채점기에 전달되도록 버퍼를 쓰지 않는다.
    // [JA] クラッシュ直前までの出力が採点器に届くよう、バッファーを使いません。
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::cout << std::unitbuf;
}

int runGrader(const std::vector<GradeTarget>& targets, int totalCount, const char* buildName)
{
    platform::prepareCrashExit();

    std::cout << i18n::L("=== BugHunt 채점 [", "=== BugHunt 採点 [") << buildName << i18n::L("] - 랩 ", "] - ラボ ") << targets.size()
              << i18n::L("개 (전체 ", " 個 (全 ") << totalCount << i18n::L("개) ===\n", " 個) ===\n")
              << i18n::L("  랩마다 프로세스를 따로 띄워 실행합니다. 숨은 케이스가 있는 랩은 다른 데이터로 한 번 더 실행합니다.\n\n",
                         "  ラボごとに別のプロセスで実行します。隠しケースがあるラボは別のデータでもう一度実行します。\n\n")
              << i18n::L("    #  부  결과       시간  랩\n", "    #  部    結果       時間  ラボ\n");

    std::vector<LabRecord> records;
    int passed = 0;
    int challengeCount = 0;
    int challengePassed = 0;
    for (const GradeTarget& t : targets)
    {
        ChildResult r = runChild(t.id, false);
        Verdict v = judge(r, false);
        long long ms = r.ms;
        if (std::string(v.result) == "PASS" && t.hasHiddenCase)
        {
            const ChildResult hidden = runChild(t.id, true);
            v = judge(hidden, true);
            ms += hidden.ms;
        }
        const bool pass = std::string(v.result) == "PASS";
        if (pass)
            ++passed;

        const bool challenge = t.id > 100;
        if (challenge)
        {
            if (challengeCount == 0)
                std::cout << i18n::L("  ---- 챌린지 ----\n", "  ---- チャレンジ ----\n");
            ++challengeCount;
            if (pass)
                ++challengePassed;
        }

        std::string part = challenge ? " C " : std::to_string(t.part) + i18n::L("부", "部");
        if (!challenge && i18n::current() == i18n::Lang::ja)
            part = "第" + part;
        else if (challenge && i18n::current() == i18n::Lang::ja)
            part = "  " + part;
        char line[96];
        std::snprintf(line, sizeof(line), "  %3s  %s  %-7s %6lld ms  ", t.label.c_str(), part.c_str(), v.result, ms);
        std::cout << line << t.title;
        if (!v.detail.empty())
            std::cout << "\n                              └ " << v.detail;
        std::cout << std::endl;

        records.push_back({ &t, v.result, ms, v.detail });
    }

    const char* reportPath = "grade-report.json";
    writeJson(reportPath, records, passed);

    std::cout << i18n::L("\n총점: ", "\n総合点: ") << passed << "/" << targets.size() << '\n';
    if (challengeCount > 0 && challengeCount < static_cast<int>(targets.size()))
        std::cout << i18n::L("  (기본 랩 ", "  (基本ラボ ") << passed - challengePassed << "/" << targets.size() - challengeCount
                  << i18n::L(", 챌린지 ", ", チャレンジ ")
                  << challengePassed << "/" << challengeCount << ")\n";
    std::cout << i18n::L("결과 파일: ", "結果ファイル: ") << reportPath << '\n';
    return passed == static_cast<int>(targets.size()) ? 0 : 1;
}
