#include "labs.h"

#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <string>

namespace
{
struct LabEntry
{
    const char* title;
    const char* symptom;
    void (*run)();
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
};
constexpr int kLabCount = static_cast<int>(std::size(kLabs));
constexpr int kBasicLabCount = 8;

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
        std::cout << "  " << (i + 1 < 10 ? " " : "") << (i + 1) << ". "
                  << kLabs[i].title << "  - " << kLabs[i].symptom << '\n';
    }
    std::cout << "  (명령줄 인자로 번호를 넘겨도 된다: BugHunt.exe 3)\n";
}
}

int main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);

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

    if (choice < 1 || choice > kLabCount)
    {
        std::cout << "1 ~ " << kLabCount << " 사이의 번호를 입력하세요.\n";
        return 1;
    }

    const LabEntry& lab = kLabs[choice - 1];
    std::cout << "\n--- Lab " << choice << ": " << lab.title << " [" << buildName() << "] ---\n";
    lab.run();
    std::cout << "--- 종료 ---\n";
    return 0;
}
