#include "grader.h"
#include "labs.h"

#include <windows.h>
#include <crtdbg.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

// 채점기 구조
//   BugHunt.exe --grade            → 아래 runGrader. 랩마다 자식 프로세스를 띄운다.
//   BugHunt.exe --grade-child N    → 랩 N을 평소 데이터로 실행
//   BugHunt.exe --grade-child N --hidden → 같은 랩을 화면에 나오지 않는 데이터로 실행 (숨은 케이스)
// 자식이 크래시하거나 멈춰도 채점기와 다른 랩은 영향을 받지 않는다.
// 판정은 자식의 종료 코드(labs.h의 kExit*)와 시간 제한으로만 한다.
namespace
{
constexpr DWORD kChildTimeoutMs = 15000;   // 랩 하나(케이스 하나)의 최대 실행 시간
constexpr DWORD kStatusAssertion = 0xC0000420;
constexpr DWORD kStatusInvalidParameter = 0xC0000417;

struct ChildResult
{
    DWORD exitCode = 0;
    bool timedOut = false;
    long long ms = 0;
    std::string output;   // 자식의 stdout + stderr (UTF-8)
};

std::wstring exePath()
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    return path;
}

ChildResult runChild(int labId, bool hiddenCase)
{
    ChildResult result;

    SECURITY_ATTRIBUTES sa = { sizeof(sa), nullptr, TRUE };
    HANDLE readPipe = nullptr;
    HANDLE writePipe = nullptr;
    CreatePipe(&readPipe, &writePipe, &sa, 0);
    SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);

    std::wstring cmd = L"\"" + exePath() + L"\" --grade-child " + std::to_wstring(labId);
    if (hiddenCase)
        cmd += L" --hidden";

    STARTUPINFOW si = { sizeof(si) };
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = writePipe;
    si.hStdError = writePipe;
    PROCESS_INFORMATION pi = {};

    const auto start = std::chrono::steady_clock::now();
    if (!CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi))
    {
        CloseHandle(readPipe);
        CloseHandle(writePipe);
        result.exitCode = GetLastError();
        result.output = "자식 프로세스를 시작하지 못했다";
        return result;
    }
    CloseHandle(writePipe);   // 자식이 끝나면 파이프가 닫혀 읽기 스레드가 끝나도록

    // 파이프 버퍼가 차서 자식이 멈추지 않도록 따로 읽는다.
    std::thread reader([&] {
        char buffer[4096];
        DWORD n = 0;
        while (ReadFile(readPipe, buffer, sizeof(buffer), &n, nullptr) && n > 0)
            result.output.append(buffer, n);
    });

    if (WaitForSingleObject(pi.hProcess, kChildTimeoutMs) == WAIT_TIMEOUT)
    {
        result.timedOut = true;
        TerminateProcess(pi.hProcess, kExitTimeout);
        WaitForSingleObject(pi.hProcess, INFINITE);
    }
    result.ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    GetExitCodeProcess(pi.hProcess, &result.exitCode);

    reader.join();
    CloseHandle(readPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return result;
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
std::string cleanDetail(std::string s)
{
    // "[CRASH] C:\...\include\vector(1949) : Assertion failed: ..." → "[CRASH] vector(1949) : ..."
    const size_t colon = s.find(") : ");
    const size_t slash = colon == std::string::npos ? std::string::npos : s.find_last_of("\\/", colon);
    const size_t crash = s.find("[CRASH] ");
    if (slash != std::string::npos && crash != std::string::npos && crash + 8 <= slash)
        s.erase(crash + 8, slash + 1 - (crash + 8));

    const size_t asanAddress = s.find(" on address ");
    if (s.find("AddressSanitizer") != std::string::npos && asanAddress != std::string::npos)
        s.erase(asanAddress);

    std::string out;
    for (size_t i = 0; i < s.size() && out.size() < 200;)
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

std::string hex(DWORD code)
{
    char text[16];
    std::snprintf(text, sizeof(text), "0x%08X", static_cast<unsigned>(code));
    return text;
}

std::string crashName(DWORD code)
{
    switch (code)
    {
    case 0xC0000005: return "액세스 위반";
    case 0xC00000FD: return "스택 오버플로";
    case 0xC0000374: return "힙 손상";
    case 0xC0000409: return "빠른 실패(__fastfail) / 스택 버퍼 오버런";
    case 0xC0000094: return "정수를 0으로 나눔";
    case kStatusAssertion: return "Debug 어설션 실패";
    case kStatusInvalidParameter: return "CRT 잘못된 매개변수";
    case 3: return "abort() 호출 (처리되지 않은 예외 등)";
    default: return "비정상 종료";
    }
}

struct Verdict
{
    const char* result;
    std::string detail;
};

Verdict judge(const ChildResult& r, bool hiddenCase)
{
    const std::string prefix = hiddenCase ? "숨은 케이스: " : "";
    if (r.timedOut)
        return { "TIMEOUT", prefix + "채점기 시간 제한 " + std::to_string(kChildTimeoutMs) + " ms 초과" };

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
        std::string detail = prefix + crashName(r.exitCode) + " (" + hex(r.exitCode) + ")";
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

int __cdecl reportHook(int reportType, char* message, int* returnValue)
{
    if (reportType == _CRT_WARN)
        return FALSE;
    std::string text = message ? message : "";
    for (char& c : text)
    {
        if (c == '\r' || c == '\n')
            c = ' ';
    }
    std::cout.flush();
    std::fprintf(stderr, "\n[CRASH] %s\n", trim(text).c_str());
    std::fflush(stderr);
    *returnValue = 0;
    TerminateProcess(GetCurrentProcess(), kStatusAssertion);
    return TRUE;
}

void invalidParameterHandler(const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, uintptr_t)
{
    std::cout.flush();
    std::fprintf(stderr, "\n[CRASH] CRT 함수에 잘못된 매개변수가 전달됨\n");
    std::fflush(stderr);
    TerminateProcess(GetCurrentProcess(), kStatusInvalidParameter);
}
}

void prepareGradeChild()
{
    // 크래시 대화 상자(WER), Debug 어설션 창, abort() 메시지 창을 모두 끈다.
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
    _CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, reportHook);
    _set_invalid_parameter_handler(invalidParameterHandler);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

    // 크래시 직전까지의 출력이 채점기에 전달되도록 버퍼를 쓰지 않는다.
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::cout << std::unitbuf;
}

int runGrader(const std::vector<GradeTarget>& targets, int totalLabCount, const char* buildName)
{
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);

    std::cout << "=== BugHunt 채점 [" << buildName << "] - 랩 " << targets.size()
              << "개 (전체 " << totalLabCount << "개) ===\n"
              << "  랩마다 프로세스를 따로 띄워 실행합니다. 숨은 케이스가 있는 랩은 다른 데이터로 한 번 더 실행합니다.\n\n"
              << "   #  부  결과       시간  랩\n";

    std::vector<LabRecord> records;
    int passed = 0;
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
        if (std::string(v.result) == "PASS")
            ++passed;

        char line[64];
        std::snprintf(line, sizeof(line), "  %2d  %d부  %-7s %6lld ms  ", t.id, t.part, v.result, ms);
        std::cout << line << t.title;
        if (!v.detail.empty())
            std::cout << "\n                              └ " << v.detail;
        std::cout << std::endl;

        records.push_back({ &t, v.result, ms, v.detail });
    }

    const char* reportPath = "grade-report.json";
    writeJson(reportPath, records, passed);

    std::cout << "\n총점: " << passed << "/" << targets.size() << '\n'
              << "결과 파일: " << reportPath << '\n';
    return passed == static_cast<int>(targets.size()) ? 0 : 1;
}
