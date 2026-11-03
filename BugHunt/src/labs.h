#pragma once
#include <iostream>

// 랩 프로세스의 종료 코드. 채점기(--grade)가 이 값으로 판정한다.
//   0 = 모든 검사 통과, 10 = [FAIL]이 하나라도 있음, 11 = 감시 타이머 초과(응답 없음)
//   그 밖의 값(0xC0000005 등)은 크래시로 본다.
constexpr int kExitPass = 0;
constexpr int kExitFail = 10;
constexpr int kExitTimeout = 11;

// expect가 [FAIL]을 낸 횟수. main이 종료 코드를 정할 때 쓴다.
inline int g_failCount = 0;

// 각 랩은 "기대값 vs 실제값"을 출력한다.
// [FAIL]이 나오거나 프로그램이 멈추면, 그때부터 버그 추적 시작.
template <class T>
bool expect(const char* what, const T& expected, const T& actual)
{
    const bool ok = (expected == actual);
    if (!ok)
        ++g_failCount;
    std::cout << std::boolalpha
              << (ok ? "  [PASS] " : "  [FAIL] ") << what
              << "  (기대: " << expected << ", 실제: " << actual << ")\n";
    return ok;
}

// 1부: 호출 스택 입문 (src/basics)
void cs01_profile();
void cs02_quickslot();
void cs03_battle_log();
void cs04_party_board();
void cs05_fireworks();
void cs06_shop();
void cs07_events();
void cs08_waves();

// 2부: 심화 (src/advanced)
void lab01_average();
void lab02_inventory();
void lab03_titles();
void lab04_combo();
void lab05_enemies();
void lab06_backpack();
void lab07_packets();
void lab08_effects();
void lab09_guild_bank();
void lab10_party();
void lab11_quests();
void lab12_status_flags();

// 3부: 흐름 추적 - 크래시 없음 (src/flow)
void ft01_attendance();
void ft02_critical();
void ft03_shield();
void ft04_quest();
void ft05_discount();
void ft06_repair();
void ft07_achievements();
void ft08_difficulty();

// 4부: 포인터 오용 (src/pointers)
void pt01_ranking();
void pt02_bounty();
void pt03_particles();
void pt04_loot_packet();
void pt05_equipment();
void pt06_guild();

// 채점용 숨은 테스트 케이스: 화면에 나오는 것과 다른 데이터로 같은 코드를 검사한다.
// 각 랩 파일 맨 아래 "채점용 숨은 테스트" region에 있다. (src/grader.cpp 참고)
void ft01_attendance_hidden();
void ft02_critical_hidden();
void ft03_shield_hidden();
void ft04_quest_hidden();
void ft05_discount_hidden();
void ft06_repair_hidden();
void ft07_achievements_hidden();
void ft08_difficulty_hidden();
void pt01_ranking_hidden();
void pt02_bounty_hidden();
void pt03_particles_hidden();
void pt04_loot_packet_hidden();
void pt05_equipment_hidden();
void pt06_guild_hidden();
