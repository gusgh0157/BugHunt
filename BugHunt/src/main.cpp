#include "labs.h"
#include "grader.h"

#include <windows.h>
#include <cctype>
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
    // 4부: 컨테이너
    { "가방 조회",          "상점을 열었을 뿐인데 가방이 꽉 찬다",  ct01_bag_lookup,   ct01_bag_lookup_hidden },
    { "파티 찾기",          "고른 사람과 다른 사람이 초대된다",     ct02_party_finder, ct02_party_finder_hidden },
    { "아레나 순위표",      "순위표를 정렬하다가 멈춘다",           ct03_leaderboard,  ct03_leaderboard_hidden },
    { "장비 정리",          "부서진 장비를 버렸는데 가방이 그대로다", ct04_repair_shop, ct04_repair_shop_hidden },
    { "던전 문 퍼즐",       "퍼즐 점수가 거꾸로 매겨진다",          ct05_dungeon_doors, ct05_dungeon_doors_hidden },
    { "이름표",             "칭호를 받으면 이름표가 깨진다",        ct06_nameplates,   ct06_nameplates_hidden },
    { "시즌 랭킹",          "우승자가 1위에 오르지 않는다",         ct07_season_ranking, ct07_season_ranking_hidden },
    // 5부: 포인터 오용
    { "랭킹 보드",          "랭킹 줄이 전부 깨져서 나온다",        pt01_ranking,      pt01_ranking_hidden },
    { "현상금 사냥",        "골드가 음수 수억이 된다",              pt02_bounty,       pt02_bounty_hidden },
    { "폭발 이펙트",        "이펙트가 끝날 때 멈춘다",              pt03_particles,    pt03_particles_hidden },
    { "전리품 패킷",        "받은 아이템 개수가 엉망이다",          pt04_loot_packet,  pt04_loot_packet_hidden },
    { "장비 공격력",        "맨손 파티원에서 죽는다",               pt05_equipment,    pt05_equipment_hidden },
    { "길드 해산",          "길드를 내려도 메모리가 남는다",        pt06_guild,        pt06_guild_hidden },
    { "분노 버프",          "분노가 끝나도 공격력이 원래대로 안 돌아온다", pt07_rage_timer, pt07_rage_timer_hidden },
    // 6부: 멈춤 - 무한루프와 데드락
    { "스킬 쿨다운",        "파이어볼 쿨다운 계산에서 멈춘다",      hg01_cooldown,     hg01_cooldown_hidden },
    { "부활 카운트다운",    "부활 알림을 세다가 멈춘다",            hg02_respawn,      hg02_respawn_hidden },
    { "채팅 명령어",        "명령어를 입력하면 멈춘다",             hg03_chat_command, hg03_chat_command_hidden },
    { "자동 이동 경로",     "숲을 경로에 넣으면 멈춘다",            hg04_route,        hg04_route_hidden },
    { "동시 거래",          "두 사람이 서로 팔면 둘 다 멈춘다",     hg05_trade,        hg05_trade_hidden },
    // 7부: 성능 - 틀리지는 않지만 느리다
    { "채팅 큐",            "메시지가 몰리면 서버가 굳는다",        pf01_chat_queue,   pf01_chat_queue_hidden },
    { "장비 무게 HUD",      "장비가 많으면 프레임이 떨어진다",      pf02_loadout,      pf02_loadout_hidden },
    { "전투 기록 내보내기", "긴 레이드일수록 내보내기가 오래 걸린다", pf03_combat_log, pf03_combat_log_hidden },
    { "세이브 파일",        "큰 맵에서 저장이 몇 초씩 걸린다",      pf04_save_log,     pf04_save_log_hidden },
    { "전장의 안개",        "큰 맵에서만 프레임이 뚝 떨어진다",     pf05_fog_map,      pf05_fog_map_hidden },
    { "어그로 검사",        "몬스터가 많으면 서버 틱이 밀린다",     pf06_aggro_scan,   pf06_aggro_scan_hidden },
    // 추가 랩 (임시 번호 54~. 최종 번호와 부 배치는 재배치 때 정한다)
    // 연산자와 평가 순서 (3부 흐름 추적 끝에 붙일 예정)
    { "길드 회원 번호",     "새 회원이 기존 회원과 같은 번호를 받는다", ft09_member_ids, ft09_member_ids_hidden },
    { "최고 콤보",          "기록을 갱신할 때마다 콤보가 2씩 오른다", ft10_best_combo, ft10_best_combo_hidden },
    { "어둠의 동굴",        "열쇠도 횃불도 있는데 입장을 못 한다",  ft11_cave_gate,    ft11_cave_gate_hidden },
    { "자물쇠 따개",        "열쇠가 있는데도 따개가 줄어든다",      ft12_lockpicks,    ft12_lockpicks_hidden },
    // 파일 입출력
    { "패치된 가격표",      "어떤 실행에서는 패치 전 가격이 나온다", io01_patch_prices, io01_patch_prices_hidden },
    { "모험 일지",          "방금 적은 일지가 일지 창에 안 보인다", io02_quest_journal, io02_quest_journal_hidden },
    { "점수판",             "마지막 기록이 두 번 나온다",           io03_high_scores,  io03_high_scores_hidden },
    { "여관 주인 대사",     "첫 대사가 비고 마지막 대사가 빠진다",  io04_npc_dialogue, io04_npc_dialogue_hidden },
    { "보상표",             "보상 목록이 통째로 비어 있다",         io05_reward_table, io05_reward_table_hidden },
    { "설정 파일",          "메모장으로 고친 첫 줄 설정만 무시된다", io06_settings_bom, io06_settings_bom_hidden },
    { "안전한 저장",        "저장에 실패하면 이전 세이브까지 날아간다", io07_safe_save, io07_safe_save_hidden },
};
// 챌린지: 기본 부를 모두 끝낸 뒤 푸는 문제. 힌트가 없고, 블라인드(B)는 종류도 밝히지 않는다.
// 채점 번호는 깊은 추적 C-n = 100 + n, 블라인드 B-n = 200 + n (grade-report.json의 id, BugHunt 네 판 공통)
struct ChallengeEntry
{
    const char* code;
    int gradeId;
    const char* title;
    const char* symptom;
    void (*run)();
    void (*hidden)();
};

constexpr ChallengeEntry kChallenges[] = {
    // 깊은 추적
    { "C-1", 101, "챌린지 C-1", "낙찰자와 금화가 엉뚱하게 정산된다",     chC1_auction_night, chC1_auction_night_hidden },
    { "C-2", 102, "챌린지 C-2", "무피격 연속 처치 칭호를 받지 못한다",   chC2_raid_night,    chC2_raid_night_hidden },
    { "C-3", 103, "챌린지 C-3", "부활하고 나면 피해량이 줄어든다",       chC3_shrine,        chC3_shrine_hidden },
    // 블라인드
    { "B-1", 201, "챌린지 B-1", "시장 진열대를 검사하다가 죽는다",       chB1, chB1_hidden },
    { "B-2", 202, "챌린지 B-2", "공지 게시판의 작성자 이름이 깨진다",    chB2, chB2_hidden },
    { "B-3", 203, "챌린지 B-3", "넓은 지형에서 계산이 끝나지 않는다",    chB3, chB3_hidden },
    { "B-4", 204, "챌린지 B-4", "몬스터 체력 바가 이상하다",             chB4, chB4_hidden },
    { "B-5", 205, "챌린지 B-5", "소환사가 부른 부하가 일부만 나온다",     chB5, chB5_hidden },
    { "B-6", 206, "챌린지 B-6", "접속 기록 정리가 너무 오래 걸린다",     chB6, chB6_hidden },
    { "B-7", 207, "챌린지 B-7", "무료 배송 금액인데 배송비가 붙는다",    chB7, chB7_hidden },
    { "B-8", 208, "챌린지 B-8", "포탑이 경보에 제대로 반응하지 않는다",  chB8, chB8_hidden },
};
constexpr int kChallengePart = 8;   // grade-report.json의 part (마지막 부 다음 번호)

constexpr int kLabCount = static_cast<int>(std::size(kLabs));
constexpr int kBasicLabCount = 8;
constexpr int kAdvancedLabEnd = 20;
constexpr int kFlowLabEnd = 28;
constexpr int kContainerLabEnd = 35;
constexpr int kPointerLabEnd = 42;
constexpr int kHangLabEnd = 47;

int partOf(int labNumber)
{
    if (labNumber <= kBasicLabCount) return 1;
    if (labNumber <= kAdvancedLabEnd) return 2;
    if (labNumber <= kFlowLabEnd) return 3;
    if (labNumber <= kContainerLabEnd) return 4;
    if (labNumber <= kPointerLabEnd) return 5;
    if (labNumber <= kHangLabEnd) return 6;
    return 7;
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
            std::cout << "\n[4부] 컨테이너 - 표준 컨테이너와 알고리즘의 함정\n";
        else if (i == kContainerLabEnd)
            std::cout << "\n[5부] 포인터 오용 - Debug, Release, ASan에서 각각 실행해 비교해 보세요\n";
        else if (i == kPointerLabEnd)
            std::cout << "\n[6부] 멈춤 - 무한루프와 데드락. 멈추면 디버그 → 모두 중단(Ctrl+Alt+Break)\n";
        else if (i == kHangLabEnd)
            std::cout << "\n[7부] 성능 - 틀리지 않지만 느리다. Release + 성능 프로파일러(Alt+F2)\n";
        std::cout << "  " << (i + 1 < 10 ? " " : "") << (i + 1) << ". "
                  << kLabs[i].title << "  - " << kLabs[i].symptom << '\n';
    }
    std::cout << "\n[챌린지] 기본 부를 모두 끝낸 뒤에 푸세요. 힌트 없음 (README의 \"챌린지\")\n";
    for (const ChallengeEntry& c : kChallenges)
        std::cout << "  " << c.code << ". " << c.symptom << '\n';
    std::cout << "  (명령줄 인자로 번호를 넘겨도 된다: BugHunt.exe 3, BugHunt.exe C-1)\n";
    std::cout << "  (채점: grade.bat 또는 BugHunt.exe --grade)\n";
}

const ChallengeEntry* findChallenge(int gradeId)
{
    for (const ChallengeEntry& c : kChallenges)
    {
        if (c.gradeId == gradeId)
            return &c;
    }
    return nullptr;
}

// "7" → 7, "C-1" / "c1" / "101" → 101, "B-3" → 203. 알 수 없으면 0
int parseId(const std::string& text)
{
    if (text.empty())
        return 0;
    const char head = static_cast<char>(std::toupper(static_cast<unsigned char>(text[0])));
    if (head == 'C' || head == 'B')
    {
        const size_t digits = text.find_first_of("0123456789");
        if (digits == std::string::npos)
            return 0;
        const int id = (head == 'C' ? 100 : 200) + std::atoi(text.c_str() + digits);
        return findChallenge(id) ? id : 0;
    }
    const int id = std::atoi(text.c_str());
    return (id >= 1 && id <= kLabCount) || findChallenge(id) ? id : 0;
}

// 대상: "" = 전부, "labs" = 기본 랩만, "challenges" = 챌린지만, 그 밖에는 번호 하나
int grade(const std::string& what)
{
    const bool all = what.empty();
    const int only = all || what == "labs" || what == "challenges" ? 0 : parseId(what);

    std::vector<GradeTarget> targets;
    for (int i = 0; i < kLabCount; ++i)
    {
        const int id = i + 1;
        if (all || what == "labs" || only == id)
            targets.push_back({ id, std::to_string(id), kLabs[i].title, partOf(id), kLabs[i].hidden != nullptr });
    }
    for (const ChallengeEntry& c : kChallenges)
    {
        if (all || what == "challenges" || only == c.gradeId)
            targets.push_back({ c.gradeId, c.code, c.title, kChallengePart, c.hidden != nullptr });
    }
    if (targets.empty())
    {
        std::cout << "1 ~ " << kLabCount << " 사이의 번호, C-1 같은 챌린지 번호, labs, challenges 중 하나를 넣으세요.\n";
        return 1;
    }
    return runGrader(targets, kLabCount + static_cast<int>(std::size(kChallenges)), buildName());
}

int runLab(int id, bool hiddenCase)
{
    void (*run)() = nullptr;
    void (*hidden)() = nullptr;
    std::string heading;
    if (id >= 1 && id <= kLabCount)
    {
        const LabEntry& lab = kLabs[id - 1];
        heading = "Lab " + std::to_string(id) + ": " + lab.title;
        run = lab.run;
        hidden = lab.hidden;
    }
    else if (const ChallengeEntry* c = findChallenge(id))
    {
        heading = c->title;
        run = c->run;
        hidden = c->hidden;
    }
    else
    {
        std::cout << "1 ~ " << kLabCount << " 사이의 번호나 C-1 같은 챌린지 번호를 입력하세요.\n";
        return 1;
    }
    if (hiddenCase)
    {
        if (hidden == nullptr)
            return kExitPass;
        std::cout << "\n--- " << heading << " (숨은 케이스) [" << buildName() << "] ---\n";
        hidden();
    }
    else
    {
        std::cout << "\n--- " << heading << " [" << buildName() << "] ---\n";
        run();
    }
    std::cout << "--- 종료 ---\n";
    return g_failCount == 0 ? kExitPass : kExitFail;
}
}

int main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);

    // 채점: BugHunt.exe --grade [번호 | C-1 | labs | challenges]
    if (argc > 1 && std::strcmp(argv[1], "--grade") == 0)
        return grade(argc > 2 ? argv[2] : "");

    // 채점기가 띄우는 자식 프로세스: BugHunt.exe --grade-child 채점번호 [--hidden]
    if (argc > 2 && std::strcmp(argv[1], "--grade-child") == 0)
    {
        prepareGradeChild();
        const bool hiddenCase = argc > 3 && std::strcmp(argv[3], "--hidden") == 0;
        return runLab(std::atoi(argv[2]), hiddenCase);
    }

    std::string choice;
    if (argc > 1)
    {
        choice = argv[1];
    }
    else
    {
        printMenu();
        std::cout << "\n실행할 랩 번호 (챌린지는 C-1, B-1처럼): ";
        std::cin >> choice;
    }

    return runLab(parseId(choice), false);
}
