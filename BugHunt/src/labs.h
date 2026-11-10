#pragma once
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <vector>

#include "i18n/i18n.h"

// 랩 프로세스의 종료 코드. 채점기(--grade)가 이 값으로 판정한다.
//   0 = 모든 검사 통과, 10 = [FAIL]이 하나라도 있음, 11 = 감시 타이머 초과(응답 없음)
//   그 밖의 값(0xC0000005 등)은 크래시로 본다.
// [JA] ラボプロセスの終了コードです。採点器(--grade)はこの値で判定します。
// [JA]   0 = すべての検査に合格、10 = [FAIL] が1つでもある、11 = 監視タイマー超過(応答なし)
// [JA]   それ以外の値(0xC0000005 など)はクラッシュとみなします。
constexpr int kExitPass = 0;
constexpr int kExitFail = 10;
constexpr int kExitTimeout = 11;

// expect가 [FAIL]을 낸 횟수. main이 종료 코드를 정할 때 쓴다.
// [JA] expect が [FAIL] を出した回数です。main が終了コードを決めるときに使います。
inline int g_failCount = 0;

// 각 랩은 "기대값 vs 실제값"을 출력한다.
// [FAIL]이 나오거나 프로그램이 멈추면, 그때부터 버그 추적 시작.
// [JA] 各ラボは「期待値 vs 実際の値」を出力します。
// [JA] [FAIL] が出たりプログラムが止まったりしたら、そこからバグの追跡を始めます。
// what은 두 언어를 함께 적은 설명이다: expect(i18n::L("골드 상한", "ゴールドの上限"), 기대값, 실제값)
// [JA] what は 2 つの言語を並べて書いた説明です: expect(i18n::L("골드 상한", "ゴールドの上限"), 期待値, 実際の値)
template <class T>
bool expect(const char* what, const T& expected, const T& actual)
{
    const bool ok = (expected == actual);
    if (!ok)
        ++g_failCount;
    std::cout << std::boolalpha
              << (ok ? "  [PASS] " : "  [FAIL] ") << what
              << i18n::L("  (기대: ", "  (期待: ") << expected << i18n::L(", 실제: ", ", 実際: ") << actual << ")\n";
    return ok;
}

// 10부(비동기와 동시성)와 11부(멈춤) 랩용 감시 타이머 (src/watchdog.cpp)
// work를 작업 스레드에서 실행하고, timeoutMs 안에 끝나지 않으면
// "[FAIL] what  응답 없음 (N ms 초과)"를 출력한 뒤 프로세스를 끝낸다 (종료 코드 kExitTimeout).
// 디버거가 붙어 있으면 타이머를 끄고 끝날 때까지 기다린다. 멈춘 상태를 "모두 중단(break all)"으로 관찰하라는 뜻이다.
// [JA] 第10部(非同期と並行性)と第11部(停止)のラボ用の監視タイマー (src/watchdog.cpp)
// [JA] work を作業スレッドで実行し、timeoutMs 以内に終わらなければ
// [JA] 「[FAIL] what  応答なし (N ms 超過)」を出力してからプロセスを終了します (終了コード kExitTimeout)。
// [JA] デバッガーがアタッチされているときはタイマーを止め、終わるまで待ちます。止まった状態を「すべて中断(break all)」で観察してください、という意味です。
constexpr int kHangTimeoutMs = 2000;
void runWithWatchdog(const char* what, const std::function<void()>& work, int timeoutMs = kHangTimeoutMs);

// 랩을 돕는 도구 (src/support/heap_guard.cpp). 모두 표준 C++이라 어느 컴파일러에서나 똑같이 동작한다.
// [JA] ラボを助けるツール (src/support/heap_guard.cpp)。すべて標準 C++ なので、どのコンパイラーでも同じように動作します。
namespace bughunt
{
// 오브젝트 풀처럼 "전에 쓰던 메모리"를 다시 쓰는 상황을 흉내 낸다.
// 돌려주는 메모리에는 앞 사용자가 남긴 값(0xA5 바이트)이 그대로 들어 있다.
// [JA] オブジェクトプールのように「以前使っていたメモリ」を再利用する状況をまねます。
// [JA] 返すメモリには、前の使用者が残した値(0xA5 バイト)がそのまま入っています。
void* recycledStorage(std::size_t bytes);

[[noreturn]] void invalidComparator();

// 정렬 비교 함수를 감싸서, 표준 알고리즘이 요구하는 엄격한 약순서를 어기면 바로 멈춘다.
// (표준 라이브러리의 디버그 모드가 하는 검사를 어느 구성에서나 똑같이 하려고 쓴다)
// [JA] ソートの比較関数をラップし、標準アルゴリズムが要求する狭義の弱順序に違反するとすぐに止まります。
// [JA] (標準ライブラリのデバッグモードが行う検査を、どの構成でも同じように行うために使います)
template <class Compare>
auto checkedOrder(Compare comp)
{
    return [comp](const auto& a, const auto& b) {
        if (comp(a, a) || (comp(a, b) && comp(b, a)))
            invalidComparator();
        return comp(a, b);
    };
}

[[noreturn]] void invalidIterator();

// 무효가 된 반복자를 쓰면 바로 멈추는 작은 vector.
// (표준 라이브러리 디버그 모드의 반복자 검사를 어느 구성에서나 똑같이 하려고 쓴다)
// std::vector와 규칙이 같다: erase가 원소를 지우면 그 전에 받은 반복자는 모두 무효가 되고,
// erase는 지운 원소 다음을 가리키는 새 반복자를 돌려준다.
// [JA] 無効になったイテレーターを使うとすぐに止まる小さな vector です。
// [JA] (標準ライブラリのデバッグモードのイテレーター検査を、どの構成でも同じように行うために使います)
// [JA] std::vector と規則は同じです: erase が要素を消すと、それより前に受け取ったイテレーターはすべて無効になり、
// [JA] erase は消した要素の次を指す新しいイテレーターを返します。
template <class T>
class CheckedList
{
public:
    class iterator
    {
    public:
        T& operator*() const { return owner_->element(*this); }
        T* operator->() const { return &owner_->element(*this); }
        iterator& operator++()
        {
            owner_->check(*this);
            ++index_;
            return *this;
        }
        iterator operator+(std::ptrdiff_t n) const
        {
            owner_->check(*this);
            return iterator(owner_, index_ + static_cast<std::size_t>(n), generation_);
        }
        bool operator==(const iterator& other) const
        {
            owner_->check(*this);
            owner_->check(other);
            return index_ == other.index_;
        }

    private:
        friend class CheckedList;
        iterator(CheckedList* owner, std::size_t index, unsigned generation) : owner_(owner), index_(index), generation_(generation) {}

        CheckedList* owner_;
        std::size_t index_;
        unsigned generation_;
    };

    CheckedList(std::initializer_list<T> items) : items_(items) {}

    iterator begin() { return iterator(this, 0, generation_); }
    iterator end() { return iterator(this, items_.size(), generation_); }
    std::size_t size() const { return items_.size(); }

    iterator erase(iterator it)
    {
        element(it);   // 유효하고 끝이 아닌지 확인 [JA] 有効で、終端ではないことを確認
        items_.erase(items_.begin() + static_cast<std::ptrdiff_t>(it.index_));
        ++generation_;
        return iterator(this, it.index_, generation_);
    }

private:
    void check(const iterator& it) const
    {
        if (it.owner_ != this || it.generation_ != generation_ || it.index_ > items_.size())
            invalidIterator();
    }
    T& element(const iterator& it)
    {
        check(it);
        if (it.index_ >= items_.size())
            invalidIterator();
        return items_[it.index_];
    }

    std::vector<T> items_;
    unsigned generation_ = 0;
};
}

// 1부: 호출 스택 입문 (src/basics)
// [JA] 第1部: 呼び出し履歴入門 (src/basics)
void cs01_profile();
void cs02_quickslot();
void cs03_battle_log();
void cs04_party_board();
void cs05_fireworks();
void cs06_shop();
void cs07_events();
void cs08_waves();

// 2부: 심화 (src/advanced)
// [JA] 第2部: 応用 (src/advanced)
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
// [JA] 第3部: 流れの追跡 - クラッシュなし (src/flow)
void ft01_attendance();
void ft02_critical();
void ft03_shield();
void ft04_quest();
void ft05_discount();
void ft06_repair();
void ft07_achievements();
void ft08_difficulty();
// 3부 끝: 연산자와 평가 순서 (src/flow)
// [JA] 第3部の終わり: 演算子と評価順序 (src/flow)
void ft09_member_ids();
void ft10_best_combo();
void ft11_cave_gate();
void ft12_lockpicks();

// 4부: 컨테이너 (src/containers)
// [JA] 第4部: コンテナー (src/containers)
void ct01_bag_lookup();
void ct02_party_finder();
void ct03_leaderboard();
void ct04_repair_shop();
void ct05_dungeon_doors();
void ct06_nameplates();
void ct07_season_ranking();

// 5부: C++ 언어 함정 (src/lang)
// [JA] 第5部: C++ 言語の落とし穴 (src/lang)
void lg01_trade_items();
void lg02_exp_cap();
void lg03_exchange();
void lg04_level_bonus();
void lg05_stat_table();
void lg06_save_screen();
void lg07_dungeon_load();
void lg08_save_close();
void lg09_quest_chain();

// 6부: 인터페이스와 상속 (src/inherit)
// [JA] 第6部: インターフェースと継承 (src/inherit)
void ih01_backstab();
void ih02_boss_loot();
void ih03_sniper_tower();
void ih04_priest_heal();
void ih05_reward_mail();
void ih06_mana_potion();

// 7부: 파일 입출력 (src/fileio)
// [JA] 第7部: ファイル入出力 (src/fileio)
void io01_patch_prices();
void io02_quest_journal();
void io03_high_scores();
void io04_npc_dialogue();
void io05_reward_table();
void io06_settings_bom();
void io07_safe_save();

// 8부: 직렬화 (JSON/XML) (src/serialize)
// [JA] 第8部: シリアライズ (JSON/XML) (src/serialize)
void sz01_pet_slot();
void sz02_crit_multiplier();
void sz03_skill_bar();
void sz04_nickname_cut();
void sz05_float_position();
void sz06_bestiary();
void sz07_guild_list();

// 9부: 포인터 오용 (src/pointers)
// [JA] 第9部: ポインターの誤用 (src/pointers)
void pt01_ranking();
void pt02_bounty();
void pt03_particles();
void pt04_loot_packet();
void pt05_equipment();
void pt06_guild();
void pt07_rage_timer();
// 9부 끝: 스택/힙 오버플로 (src/overflow)
// [JA] 第9部の終わり: スタック/ヒープのオーバーフロー (src/overflow)
void of01_minimap();
void of02_dungeon_gen();
void of03_replay_buffer();
void of04_name_packet();
void of05_skill_tree();

// 10부: 비동기와 동시성 (src/async)
// [JA] 第10部: 非同期と並行性 (src/async)
void as01_loading();
void as02_crafting();
void as03_raid_ready();
void as04_auction_notice();
void as05_limited_sale();
void as06_matchmaking();
void as07_cutscene();

// 11부: 멈춤 - 무한루프와 데드락 (src/hangs)
// [JA] 第11部: 停止 - 無限ループとデッドロック (src/hangs)
void hg01_cooldown();
void hg02_respawn();
void hg03_chat_command();
void hg04_route();
void hg05_trade();

// 12부: 성능 - 틀리지 않지만 느리다 (src/perf)
// [JA] 第12部: パフォーマンス - 間違ってはいないが遅い (src/perf)
void pf01_chat_queue();
void pf02_loadout();
void pf03_combat_log();
void pf04_save_log();
void pf05_fog_map();
void pf06_aggro_scan();

// 챌린지 (src/challenge). 힌트 없음
// [JA] チャレンジ (src/challenge)。ヒントなし
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
// [JA] 採点用の隠しテストケース: 画面に表示されるものとは別のデータで同じコードを検査します。
// [JA] 各ラボファイルのいちばん下の「採点用の隠しテスト」region にあります。(src/grader.cpp 参照)
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
