#ifndef _WIN32
#include "platform.h"

#include "../i18n/i18n.h"

#include <chrono>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <thread>

#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <sys/sysctl.h>
#endif

namespace
{
std::string exePath()
{
#ifdef __APPLE__
    char path[4096];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0)
        return path;
    return {};
#else
    char path[4096];
    const ssize_t n = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (n <= 0)
        return {};
    path[n] = '\0';
    return path;
#endif
}
}

void platform::initConsole()
{
    // 대부분의 터미널은 이미 UTF-8이다.
    // [JA] ほとんどの端末はすでに UTF-8 です。
}

std::string platform::envVar(const char* name)
{
    const char* value = std::getenv(name);
    return value != nullptr ? std::string(value) : std::string();
}

std::string platform::osLanguage()
{
    // POSIX 로캘 변수의 우선순위: LC_ALL > LC_MESSAGES > LANG (예: "ja_JP.UTF-8")
    // [JA] POSIX のロケール変数の優先順位: LC_ALL > LC_MESSAGES > LANG (例: "ja_JP.UTF-8")
    for (const char* name : { "LC_ALL", "LC_MESSAGES", "LANG" })
    {
        const char* value = std::getenv(name);
        if (value != nullptr && value[0] != '\0')
            return std::string(value).substr(0, 2);
    }
    return "";
}

void platform::prepareCrashExit()
{
    // POSIX에서는 크래시가 신호로 끝나고 대화 상자가 없으므로 할 일이 없다.
    // [JA] POSIX ではクラッシュはシグナルで終わり、ダイアログもないので、することはありません。
}

bool platform::debuggerAttached()
{
#ifdef __APPLE__
    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };
    struct kinfo_proc info = {};
    size_t size = sizeof(info);
    if (sysctl(mib, 4, &info, &size, nullptr, 0) != 0)
        return false;
    return (info.kp_proc.p_flag & P_TRACED) != 0;
#else
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line))
    {
        if (line.rfind("TracerPid:", 0) == 0)
            return std::stoi(line.substr(10)) != 0;
    }
    return false;
#endif
}

void platform::nameThisThread(const char* name)
{
    // Linux는 이름이 15바이트까지라 ASCII로 짧게 붙인다. 한글 이름은 잘릴 수 있다.
    // [JA] Linux は名前が 15 バイトまでなので、ASCII で短く付けます。ハングルの名前は切れることがあります。
    char shortName[16] = {};
    std::strncpy(shortName, name, sizeof(shortName) - 1);
#ifdef __APPLE__
    pthread_setname_np(shortName);
#else
    pthread_setname_np(pthread_self(), shortName);
#endif
}

platform::ProcessResult platform::runSelf(const std::vector<std::string>& args, int timeoutMs)
{
    ProcessResult result;

    const std::string path = exePath();
    std::vector<std::string> argStrings;
    argStrings.push_back(path);
    argStrings.insert(argStrings.end(), args.begin(), args.end());
    std::vector<char*> argv;
    for (std::string& a : argStrings)
        argv.push_back(a.data());
    argv.push_back(nullptr);

    int fds[2];
    if (pipe(fds) != 0)
    {
        result.exitCode = 127;
        result.output = i18n::L("파이프를 만들지 못했다", "パイプを作れなかった");
        return result;
    }

    const auto start = std::chrono::steady_clock::now();
    const pid_t pid = fork();
    if (pid == 0)
    {
        dup2(fds[1], STDOUT_FILENO);
        dup2(fds[1], STDERR_FILENO);
        close(fds[0]);
        close(fds[1]);
        execv(path.c_str(), argv.data());
        _exit(127);
    }
    close(fds[1]);
    if (pid < 0)
    {
        close(fds[0]);
        result.exitCode = 127;
        result.output = i18n::L("자식 프로세스를 시작하지 못했다", "子プロセスを開始できなかった");
        return result;
    }

    // 파이프 버퍼가 차서 자식이 멈추지 않도록 따로 읽는다.
    // [JA] パイプのバッファーが一杯になって子プロセスが止まらないよう、別に読み取ります。
    std::thread reader([&] {
        char buffer[4096];
        ssize_t n = 0;
        while ((n = read(fds[0], buffer, sizeof(buffer))) > 0)
            result.output.append(buffer, static_cast<size_t>(n));
    });

    const auto deadline = start + std::chrono::milliseconds(timeoutMs);
    int status = 0;
    for (;;)
    {
        if (waitpid(pid, &status, WNOHANG) == pid)
            break;
        if (std::chrono::steady_clock::now() >= deadline)
        {
            result.timedOut = true;
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    result.ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    if (WIFEXITED(status))
        result.exitCode = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        result.exitCode = -WTERMSIG(status);

    reader.join();
    close(fds[0]);
    return result;
}

std::string platform::describeCrash(int exitCode)
{
    if (exitCode >= 0)
        return i18n::L("비정상 종료 (종료 코드 ", "異常終了 (終了コード ") + std::to_string(exitCode) + ")";
    const int sig = -exitCode;
    const char* name = i18n::L("신호로 종료", "シグナルで終了");
    switch (sig)
    {
    case SIGSEGV: name = i18n::L("SIGSEGV 잘못된 메모리 접근", "SIGSEGV 不正なメモリアクセス"); break;
    case SIGBUS: name = i18n::L("SIGBUS 잘못된 메모리 접근", "SIGBUS 不正なメモリアクセス"); break;
    case SIGFPE: name = i18n::L("SIGFPE 산술 오류 (0으로 나눔 등)", "SIGFPE 算術エラー (0 での除算など)"); break;
    case SIGABRT: name = i18n::L("SIGABRT abort() 호출 (처리되지 않은 예외 등)", "SIGABRT abort() の呼び出し (処理されない例外など)"); break;
    case SIGILL: name = i18n::L("SIGILL 잘못된 명령", "SIGILL 不正な命令"); break;
    case SIGKILL: name = i18n::L("SIGKILL 강제 종료", "SIGKILL 強制終了"); break;
    default: break;
    }
    return std::string(name) + i18n::L(" (신호 ", " (シグナル ") + std::to_string(sig) + ")";
}
#endif
