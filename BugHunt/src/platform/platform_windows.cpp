#ifdef _WIN32
#include "platform.h"

#include "../i18n/i18n.h"

#include <windows.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

namespace
{
constexpr DWORD kStatusAssertion = 0xC0000420;
constexpr DWORD kStatusInvalidParameter = 0xC0000417;

std::wstring widen(const std::string& s)
{
    if (s.empty())
        return {};
    const int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
    std::wstring w(static_cast<size_t>(n), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), w.data(), n);
    return w;
}

std::wstring exePath()
{
    std::wstring path(MAX_PATH, L'\0');
    for (;;)
    {
        const DWORD n = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
        if (n < path.size())
        {
            path.resize(n);
            return path;
        }
        path.resize(path.size() * 2);
    }
}

#ifdef _MSC_VER
// MSVC 런타임(Debug)의 어설션 창 대신 메시지 한 줄을 남기고 바로 끝낸다.
// [JA] MSVC ランタイム(Debug)のアサーションウィンドウの代わりにメッセージを1行残して、すぐに終了します。
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
    std::fprintf(stderr, "\n[CRASH] %s\n", text.c_str());
    std::fflush(stderr);
    *returnValue = 0;
    TerminateProcess(GetCurrentProcess(), kStatusAssertion);
    return TRUE;
}

void invalidParameterHandler(const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, uintptr_t)
{
    std::cout.flush();
    std::fputs(i18n::L("\n[CRASH] CRT 함수에 잘못된 매개변수가 전달됨\n", "\n[CRASH] CRT 関数に不正な引数が渡された\n"), stderr);
    std::fflush(stderr);
    TerminateProcess(GetCurrentProcess(), kStatusInvalidParameter);
}
#endif
}

void platform::initConsole()
{
    SetConsoleOutputCP(CP_UTF8);
}

std::string platform::envVar(const char* name)
{
    char value[256];
    const DWORD n = GetEnvironmentVariableA(name, value, sizeof(value));
    return n > 0 && n < sizeof(value) ? std::string(value, n) : std::string();
}

std::string platform::osLanguage()
{
    switch (PRIMARYLANGID(GetUserDefaultUILanguage()))
    {
    case LANG_JAPANESE: return "ja";
    case LANG_KOREAN: return "ko";
    default: return "";
    }
}

void platform::prepareCrashExit()
{
    // 크래시 대화 상자(WER)를 끈다.
    // [JA] クラッシュのダイアログ(WER)を無効にします。
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
#ifdef _MSC_VER
    // Debug 런타임의 어설션 창, abort() 메시지 창도 끈다.
    // [JA] Debug ランタイムのアサーションウィンドウと abort() のメッセージウィンドウも無効にします。
    _CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, reportHook);
    _set_invalid_parameter_handler(invalidParameterHandler);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif
}

bool platform::debuggerAttached()
{
    return IsDebuggerPresent() != FALSE;
}

void platform::nameThisThread(const char* name)
{
    // Windows 10 1607 이상. 없는 시스템에서는 조용히 넘어간다.
    // [JA] Windows 10 1607 以降。ないシステムでは何もせずに進みます。
    using SetDescription = HRESULT(WINAPI*)(HANDLE, PCWSTR);
    const HMODULE kernel = GetModuleHandleW(L"kernel32.dll");
    const auto set = kernel ? reinterpret_cast<SetDescription>(reinterpret_cast<void*>(GetProcAddress(kernel, "SetThreadDescription"))) : nullptr;
    if (set)
        set(GetCurrentThread(), widen(name).c_str());
}

platform::ProcessResult platform::runSelf(const std::vector<std::string>& args, int timeoutMs)
{
    ProcessResult result;

    SECURITY_ATTRIBUTES sa = { sizeof(sa), nullptr, TRUE };
    HANDLE readPipe = nullptr;
    HANDLE writePipe = nullptr;
    CreatePipe(&readPipe, &writePipe, &sa, 0);
    SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);

    std::wstring cmd = L"\"" + exePath() + L"\"";
    for (const std::string& a : args)
        cmd += L" " + widen(a);

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
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
        result.exitCode = static_cast<int>(GetLastError());
        result.output = i18n::L("자식 프로세스를 시작하지 못했다", "子プロセスを開始できなかった");
        return result;
    }
    CloseHandle(writePipe);   // 자식이 끝나면 파이프가 닫혀 읽기 스레드가 끝나도록 [JA] 子が終わるとパイプが閉じて、読み取りスレッドが終わるように

    // 파이프 버퍼가 차서 자식이 멈추지 않도록 따로 읽는다.
    // [JA] パイプのバッファーがいっぱいになって子が止まらないように、別に読み取ります。
    std::thread reader([&] {
        char buffer[4096];
        DWORD n = 0;
        while (ReadFile(readPipe, buffer, sizeof(buffer), &n, nullptr) && n > 0)
            result.output.append(buffer, n);
    });

    if (WaitForSingleObject(pi.hProcess, static_cast<DWORD>(timeoutMs)) == WAIT_TIMEOUT)
    {
        result.timedOut = true;
        TerminateProcess(pi.hProcess, 1);
        WaitForSingleObject(pi.hProcess, INFINITE);
    }
    result.ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    DWORD code = 0;
    GetExitCodeProcess(pi.hProcess, &code);
    result.exitCode = static_cast<int>(code);

    reader.join();
    CloseHandle(readPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return result;
}

std::string platform::describeCrash(int exitCode)
{
    const DWORD code = static_cast<DWORD>(exitCode);
    const char* name = i18n::L("비정상 종료", "異常終了");
    switch (code)
    {
    case 0xC0000005: name = i18n::L("액세스 위반", "アクセス違反"); break;
    case 0xC00000FD: name = i18n::L("스택 오버플로", "スタックオーバーフロー"); break;
    case 0xC0000374: name = i18n::L("힙 손상", "ヒープ破損"); break;
    case 0xC0000409: name = i18n::L("빠른 실패(__fastfail) / 스택 버퍼 오버런", "高速失敗(__fastfail) / スタックバッファーオーバーラン"); break;
    case 0xC0000094: name = i18n::L("정수를 0으로 나눔", "整数を 0 で除算"); break;
    case kStatusAssertion: name = i18n::L("Debug 어설션 실패", "Debug アサーション失敗"); break;
    case kStatusInvalidParameter: name = i18n::L("CRT 잘못된 매개변수", "CRT 不正な引数"); break;
    case 3: name = i18n::L("abort() 호출 (처리되지 않은 예외 등)", "abort() の呼び出し (処理されない例外など)"); break;
    default: break;
    }
    char hex[16];
    std::snprintf(hex, sizeof(hex), "0x%08X", static_cast<unsigned>(code));
    return std::string(name) + " (" + hex + ")";
}
#endif
