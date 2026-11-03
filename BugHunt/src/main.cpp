#include "labs.h"
#include "grader.h"

#include <windows.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace
{
struct LabEntry
{
    const char* title;
    const char* symptom;
    void (*run)();
    void (*hidden)() = nullptr;   // 채점용 숨은 테스트 케이스 (없으면 nullptr)
};

constexpr LabEntry kLabs[] = {
    // 1부: 호출 스택 입문
    { "친구 프로필",        "세 번째 프로필에서 죽는다",           cs01_profile },
    { "퀵슬롯",             "엉뚱한 아이템이 나오다가 멈춘다",     cs02_quickslot },
    { "전투 기록 DPS",      "DPS 계산 중에 죽는다",                cs03_battle_log },
    { "파티 게시판",        "빈 파티는 처리했는데도 멈춘다",       cs04_party_board },
    { "불꽃놀이",           "카운트다운이 끝나지 않고 죽는다",     cs05_fireworks },
    { "상점 계산",          "합계를 내다가 abort()가 뜬다",        cs06_shop },
    { "HP 이벤트",          "HP 바 갱신에서 죽는다",               cs07_events },
    { "몬스터 웨이브",      "3번째 웨이브에서 멈춘다",             cs08_waves },
    // 2부: 심화
    { "점수 평균",          "실행하자마자 멈춘다",                 lab01_average },
    { "인벤토리 보상",      "보상으로 준 아이템 개수가 이상하다",  lab02_inventory },
    { "플레이어 칭호",      "칭호 문자열이 깨진다",                lab03_titles },
    { "콤보 카운터",        "최고 콤보가 틀린다 (Debug/Release 차이)", lab04_combo },
    { "죽은 적 제거",       "적 정리 중 멈춘다",                   lab05_enemies },
    { "배낭 용량",          "꽉 찬 배낭에 아이템이 들어간다",      lab06_backpack },
    { "패킷 버퍼",          "내용이 깨지고 종료할 때 멈춘다",      lab07_packets },
    { "상태 이상 효과",     "메모리가 조금씩 샌다",                lab08_effects },
    { "길드 은행",          "골드 합계가 매번 다르다",             lab09_guild_bank },
    { "파티 칭호 수여",     "아무도 때리지 않았는데 HP가 바뀐다",  lab10_party },
    { "퀘스트 선행 조건",   "프로그램이 즉시 죽는다",              lab11_quests },
    { "상태 플래그",        "독에 걸렸는데 아니라고 한다",         lab12_status_flags },
    // 3부: 흐름 추적 (크래시 없음)
    { "출석 보상",          "7일 연속 출석했는데 보너스가 없다",    ft01_attendance,   ft01_attendance_hidden },
    { "치명타 피해",        "치명타가 일반 공격과 똑같이 들어간다", ft02_critical,     ft02_critical_hidden },
    { "보호막 탱커",        "피해 합계가 맞지 않는다",              ft03_shield,       ft03_shield_hidden },
    { "늑대 사냥 퀘스트",   "5마리를 잡았는데 퀘스트가 안 끝난다",  ft04_quest,        ft04_quest_hidden },
    { "상점 할인",          "VIP가 VIP 할인을 못 받는다",           ft05_discount,     ft05_discount_hidden },
    { "장비 수리",          "골드는 빠졌는데 내구도가 그대로다",    ft06_repair,       ft06_repair_hidden },
    { "업적 해금",          "조건을 채운 업적 하나가 안 열린다",    ft07_achievements, ft07_achievements_hidden },
    { "난이도 설정",        "어려움으로 저장했는데 적이 약하다",    ft08_difficulty,   ft08_difficulty_hidden },
    // 4부: 포인터 오용
    { "랭킹 보드",          "랭킹 줄이 전부 깨져서 나온다",        pt01_ranking,      pt01_ranking_hidden },
    { "현상금 사냥",        "골드가 음수 수억이 된다",              pt02_bounty,       pt02_bounty_hidden },
    { "폭발 이펙트",        "이펙트가 끝날 때 멈춘다",              pt03_particles,    pt03_particles_hidden },
    { "전리품 패킷",        "받은 아이템 개수가 엉망이다",          pt04_loot_packet,  pt04_loot_packet_hidden },
    { "장비 공격력",        "맨손 파티원에서 죽는다",               pt05_equipment,    pt05_equipment_hidden },
    { "길드 해산",          "길드를 내려도 메모리가 남는다",        pt06_guild,        pt06_guild_hidden },
};
constexpr int kLabCount = static_cast<int>(std::size(kLabs));
constexpr int kBasicLabCount = 8;
constexpr int kAdvancedLabEnd = 20;
constexpr int kFlowLabEnd = 28;
constexpr int kPointerLabEnd = 34;

int partOf(int labNumber)
{
    if (labNumber <= kBasicLabCount) return 1;
    if (labNumber <= kAdvancedLabEnd) return 2;
    if (labNumber <= kFlowLabEnd) return 3;
    return 4;
}

const char* buildName()
{
#if defined(__SANITIZE_ADDRESS__)
    return "ASan";
#elif defined(_DEBUG)
    return "Debug";
#else
    return "Release";
#endif
}

void printMenu()
{
    std::cout << "=== BugHunt: C++ 버그 추적 실습 [" << buildName() << "] ===\n";
    for (int i = 0; i < kLabCount; ++i)
    {
        if (i == 0)
            std::cout << "\n[1부] 호출 스택 입문 - VS에서 F5로 실행하세요\n";
        else if (i == kBasicLabCount)
            std::cout << "\n[2부] 심화\n";
        else if (i == kAdvancedLabEnd)
            std::cout << "\n[3부] 흐름 추적 - 크래시 없음. 중단점을 걸고 F10/F11로 따라가세요\n";
        else if (i == kFlowLabEnd)
            std::cout << "\n[4부] 포인터 오용 - Debug, Release, ASan에서 각각 실행해 비교해 보세요\n";
        std::cout << "  " << (i + 1 < 10 ? " " : "") << (i + 1) << ". "
                  << kLabs[i].title << "  - " << kLabs[i].symptom << '\n';
    }
    std::cout << "  (명령줄 인자로 번호를 넘겨도 된다: BugHunt.exe 3)\n";
    std::cout << "  (채점: grade.bat 또는 BugHunt.exe --grade)\n";
}

int grade(int onlyLab)
{
    std::vector<GradeTarget> targets;
    for (int i = 0; i < kLabCount; ++i)
    {
        const int id = i + 1;
        if (onlyLab == 0 || onlyLab == id)
            targets.push_back({ id, kLabs[i].title, partOf(id), kLabs[i].hidden != nullptr });
    }
    if (targets.empty())
    {
        std::cout << "1 ~ " << kLabCount << " 사이의 번호를 입력하세요.\n";
        return 1;
    }
    return runGrader(targets, kLabCount, buildName());
}

int runLab(int choice, bool hiddenCase)
{
    if (choice < 1 || choice > kLabCount)
    {
        std::cout << "1 ~ " << kLabCount << " 사이의 번호를 입력하세요.\n";
        return 1;
    }

    const LabEntry& lab = kLabs[choice - 1];
    if (hiddenCase)
    {
        if (lab.hidden == nullptr)
            return kExitPass;
        std::cout << "\n--- Lab " << choice << ": " << lab.title << " (숨은 케이스) [" << buildName() << "] ---\n";
        lab.hidden();
    }
    else
    {
        std::cout << "\n--- Lab " << choice << ": " << lab.title << " [" << buildName() << "] ---\n";
        lab.run();
    }
    std::cout << "--- 종료 ---\n";
    return g_failCount == 0 ? kExitPass : kExitFail;
}
}

int main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);

    // 채점: BugHunt.exe --grade [번호]
    if (argc > 1 && std::strcmp(argv[1], "--grade") == 0)
        return grade(argc > 2 ? std::atoi(argv[2]) : 0);

    // 채점기가 띄우는 자식 프로세스: BugHunt.exe --grade-child 번호 [--hidden]
    if (argc > 2 && std::strcmp(argv[1], "--grade-child") == 0)
    {
        prepareGradeChild();
        const bool hiddenCase = argc > 3 && std::strcmp(argv[3], "--hidden") == 0;
        return runLab(std::atoi(argv[2]), hiddenCase);
    }

    int choice = 0;
    if (argc > 1)
    {
        choice = std::atoi(argv[1]);
    }
    else
    {
        printMenu();
        std::cout << "\n실행할 랩 번호: ";
        std::cin >> choice;
    }

    return runLab(choice, false);
}
