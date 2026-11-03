#pragma once
#include <vector>

// 채점기 (BugHunt.exe --grade). 랩마다 자기 자신을 자식 프로세스로 다시 띄워
// 종료 코드와 시간으로 PASS / FAIL / TIMEOUT / CRASH를 판정한다.
struct GradeTarget
{
    int id;
    const char* title;
    int part;
    bool hasHiddenCase;
};

// 표를 콘솔에 출력하고 grade-report.json을 현재 폴더에 쓴다. 만점이면 0을 돌려준다.
int runGrader(const std::vector<GradeTarget>& targets, int totalLabCount, const char* buildName);

// 자식 프로세스(--grade-child)용: 크래시나 어설션이 대화 상자를 띄우지 않고 바로 끝나게 한다.
void prepareGradeChild();
