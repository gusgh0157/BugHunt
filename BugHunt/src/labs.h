#pragma once
#include <cstddef>
#include <functional>
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

// 6부(멈춤) 랩용 감시 타이머 (src/watchdog.cpp)
// work를 작업 스레드에서 실행하고, timeoutMs 안에 끝나지 않으면
// "[FAIL] what  응답 없음 (N ms 초과)"를 출력한 뒤 프로세스를 끝낸다 (종료 코드 kExitTimeout).
// 디버거가 붙어 있으면 타이머를 끄고 끝날 때까지 기다린다. 멈춘 상태를 "모두 중단(break all)"으로 관찰하라는 뜻이다.
constexpr int kHangTimeoutMs = 2000;
void runWithWatchdog(const char* what, const std::function<void()>& work, int timeoutMs = kHangTimeoutMs);

// 랩을 돕는 도구 (src/support/heap_guard.cpp). 모두 표준 C++이라 어느 컴파일러에서나 똑같이 동작한다.
namespace bughunt
{
// 오브젝트 풀처럼 "전에 쓰던 메모리"를 다시 쓰는 상황을 흉내 낸다.
// 돌려주는 메모리에는 앞 사용자가 남긴 값(0xA5 바이트)이 그대로 들어 있다.
void* recycledStorage(std::size_t bytes);

[[noreturn]] void invalidComparator();

// 정렬 비교 함수를 감싸서, 표준 알고리즘이 요구하는 엄격한 약순서를 어기면 바로 멈춘다.
// (표준 라이브러리의 디버그 모드가 하는 검사를 어느 구성에서나 똑같이 하려고 쓴다)
template <class Compare>
auto checkedOrder(Compare comp)
{
    return [comp](const auto& a, const auto& b) {
        if (comp(a, a) || (comp(a, b) && comp(b, a)))
            invalidComparator();
        return comp(a, b);
    };
}
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

// 4부: 컨테이너 (src/containers)
void ct01_bag_lookup();
void ct02_party_finder();
void ct03_leaderboard();
void ct04_repair_shop();
void ct05_dungeon_doors();
void ct06_nameplates();
void ct07_season_ranking();

// 5부: 포인터 오용 (src/pointers)
void pt01_ranking();
void pt02_bounty();
void pt03_particles();
void pt04_loot_packet();
void pt05_equipment();
void pt06_guild();
void pt07_rage_timer();

// 6부: 멈춤 - 무한루프와 데드락 (src/hangs)
void hg01_cooldown();
void hg02_respawn();
void hg03_chat_command();
void hg04_route();
void hg05_trade();

// 7부: 성능 - 틀리지 않지만 느리다 (src/perf)
void pf01_chat_queue();
void pf02_loadout();
void pf03_combat_log();
void pf04_save_log();
void pf05_fog_map();
void pf06_aggro_scan();

// 비동기와 동시성 (src/async). 임시 번호 54~60, 마지막 번호 재배치 때 멈춤 앞으로 옮긴다
void as01_loading();
void as02_crafting();
void as03_raid_ready();
void as04_auction_notice();
void as05_limited_sale();
void as06_matchmaking();
void as07_cutscene();
// 추가 랩 (임시 번호, 최종 배치는 재배치 때)
// 연산자와 평가 순서 (src/flow)
void ft09_member_ids();
void ft10_best_combo();
void ft11_cave_gate();
void ft12_lockpicks();
// 파일 입출력 (src/fileio)
void io01_patch_prices();
void io02_quest_journal();
void io03_high_scores();
void io04_npc_dialogue();
void io05_reward_table();
void io06_settings_bom();
void io07_safe_save();
// 직렬화 - JSON/XML (src/serialize)
void sz01_pet_slot();
void sz02_crit_multiplier();
void sz03_skill_bar();
void sz04_nickname_cut();
void sz05_float_position();
void sz06_bestiary();
void sz07_guild_list();
// 인터페이스와 상속 (src/inherit)
void ih01_backstab();
void ih02_boss_loot();
void ih03_sniper_tower();
void ih04_priest_heal();
void ih05_reward_mail();
void ih06_mana_potion();
// C++ 언어 함정 (src/lang)
void lg01_trade_items();
void lg02_exp_cap();
void lg03_exchange();
void lg04_level_bonus();
void lg05_stat_table();
void lg06_save_screen();
void lg07_dungeon_load();
void lg08_save_close();
void lg09_quest_chain();
// 스택/힙 오버플로 (src/overflow, 포인터 오용 부에 붙일 예정)
void of01_minimap();
void of02_dungeon_gen();
void of03_replay_buffer();
void of04_name_packet();
void of05_skill_tree();

// 챌린지 (src/challenge). 힌트 없음
void chC1_auction_night();
void chC2_raid_night();
void chC3_shrine();
void chB1();
void chB2();
void chB3();
void chB4();
void chB5();
void chB6();
void chB7();
void chB8();

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
void ct01_bag_lookup_hidden();
void ct02_party_finder_hidden();
void ct03_leaderboard_hidden();
void ct04_repair_shop_hidden();
void ct05_dungeon_doors_hidden();
void ct06_nameplates_hidden();
void ct07_season_ranking_hidden();
void pt01_ranking_hidden();
void pt02_bounty_hidden();
void pt03_particles_hidden();
void pt04_loot_packet_hidden();
void pt05_equipment_hidden();
void pt06_guild_hidden();
void pt07_rage_timer_hidden();
void hg01_cooldown_hidden();
void hg02_respawn_hidden();
void hg03_chat_command_hidden();
void hg04_route_hidden();
void hg05_trade_hidden();
void pf01_chat_queue_hidden();
void pf02_loadout_hidden();
void pf03_combat_log_hidden();
void pf04_save_log_hidden();
void pf05_fog_map_hidden();
void pf06_aggro_scan_hidden();
void as01_loading_hidden();
void as02_crafting_hidden();
void as03_raid_ready_hidden();
void as04_auction_notice_hidden();
void as05_limited_sale_hidden();
void as06_matchmaking_hidden();
void as07_cutscene_hidden();
void chC1_auction_night_hidden();
void chC2_raid_night_hidden();
void chC3_shrine_hidden();
void chB1_hidden();
void chB2_hidden();
void chB3_hidden();
void chB4_hidden();
void chB5_hidden();
void chB6_hidden();
void chB7_hidden();
void chB8_hidden();
void ft09_member_ids_hidden();
void ft10_best_combo_hidden();
void ft11_cave_gate_hidden();
void ft12_lockpicks_hidden();
void io01_patch_prices_hidden();
void io02_quest_journal_hidden();
void io03_high_scores_hidden();
void io04_npc_dialogue_hidden();
void io05_reward_table_hidden();
void io06_settings_bom_hidden();
void io07_safe_save_hidden();
void sz01_pet_slot_hidden();
void sz02_crit_multiplier_hidden();
void sz03_skill_bar_hidden();
void sz04_nickname_cut_hidden();
void sz05_float_position_hidden();
void sz06_bestiary_hidden();
void sz07_guild_list_hidden();
void ih01_backstab_hidden();
void ih02_boss_loot_hidden();
void ih03_sniper_tower_hidden();
void ih04_priest_heal_hidden();
void ih05_reward_mail_hidden();
void ih06_mana_potion_hidden();
void lg01_trade_items_hidden();
void lg02_exp_cap_hidden();
void lg03_exchange_hidden();
void lg04_level_bonus_hidden();
void lg05_stat_table_hidden();
void lg06_save_screen_hidden();
void lg07_dungeon_load_hidden();
void lg08_save_close_hidden();
void lg09_quest_chain_hidden();
void of01_minimap_hidden();
void of02_dungeon_gen_hidden();
void of03_replay_buffer_hidden();
void of04_name_packet_hidden();
void of05_skill_tree_hidden();
