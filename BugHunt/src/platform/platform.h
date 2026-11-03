#pragma once
#include <string>
#include <vector>

// 운영체제마다 달라지는 코드는 모두 이 폴더(src/platform)에만 둔다.
// 랩 코드와 채점 로직은 표준 C++만 쓰고, 필요한 기능은 아래 함수로 부른다.
//   platform_windows.cpp : Windows (MSVC, clang-cl, MinGW)
//   platform_posix.cpp   : Linux, macOS (GCC, Clang)
namespace platform
{
// 콘솔이 UTF-8 출력을 그대로 보여 주게 한다.
void initConsole();

// 채점기 자식 프로세스용: 크래시나 abort()가 대화 상자를 띄우지 않고 바로 끝나게 한다.
void prepareCrashExit();

// 디버거가 붙어 있는가 (모르면 false)
bool debuggerAttached();

// 디버거의 스레드 목록에 보일 이름을 붙인다. (지원하지 않으면 아무 일도 하지 않는다)
void nameThisThread(const char* name);

// 운영체제의 표시 언어: "ja", "ko" 등 앞 두 글자(모르면 ""). 실행 출력 언어의 기본값에 쓴다 (src/i18n)
std::string osLanguage();

// 환경 변수 값 (없으면 ""). MSVC가 std::getenv에 사용 중단 경고를 내서 여기로 모은다.
std::string envVar(const char* name);

// 빌드 구성 이름: "Debug", "Release", "ASan"
const char* buildName();

struct ProcessResult
{
    int exitCode = 0;        // 정상 종료면 종료 코드. 신호(POSIX)로 끝났으면 -신호 번호
    bool timedOut = false;   // timeoutMs가 지나 강제로 끝냈다
    long long ms = 0;
    std::string output;      // stdout + stderr
};

// 지금 실행 중인 프로그램을 args로 다시 띄워, 끝날 때까지(최대 timeoutMs) 기다린다.
ProcessResult runSelf(const std::vector<std::string>& args, int timeoutMs);

// 크래시로 끝난 종료 코드를 사람이 읽을 수 있게: "액세스 위반 (0xC0000005)", "SIGSEGV (신호 11)"
std::string describeCrash(int exitCode);
}
