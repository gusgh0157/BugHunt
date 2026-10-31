#pragma once
#include <iostream>

// 각 랩은 "기대값 vs 실제값"을 출력한다.
// [FAIL]이 나오거나 프로그램이 멈추면, 그때부터 버그 추적 시작.
template <class T>
bool expect(const char* what, const T& expected, const T& actual)
{
    const bool ok = (expected == actual);
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
