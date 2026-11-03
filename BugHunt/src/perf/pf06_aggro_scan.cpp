#include "../labs.h"
#include "perf_kit.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

// 어그로 검사. 매 틱 모든 몬스터의 위협 수치를 계산해 가장 위험한 몬스터를 고른다.
// 몬스터는 여러 시스템이 함께 쓰므로 shared_ptr로 들고 있다.
// 몬스터가 많은 필드에서 서버 틱이 밀린다. 고르는 몬스터는 맞다.
namespace perf6
{
struct Monster : std::enable_shared_from_this<Monster>
{
    std::string name;
    int level;
    int distance;

    Monster(std::string n, int lv, int dist) : name(std::move(n)), level(lv), distance(dist) {}

    // 계측용: 이 몬스터를 동시에 붙잡고 있는 shared_ptr이 몇 개인지 기록한다.
    // 몬스터 목록 하나만 들고 있다면 항상 1이어야 한다.
    void noteHolders() const { g_maxHolders = std::max(g_maxHolders, weak_from_this().use_count()); }

    static inline long g_maxHolders = 0;
};

using MonsterPtr = std::shared_ptr<Monster>;

int threatOf(MonsterPtr monster)
{
    monster->noteHolders();
    return monster->level * 10 - monster->distance;
}

MonsterPtr mostDangerous(const std::vector<MonsterPtr>& monsters)
{
    MonsterPtr best;
    int bestThreat = -1000000;
    for (MonsterPtr m : monsters)
    {
        const int threat = threatOf(m);
        if (threat > bestThreat)
        {
            bestThreat = threat;
            best = m;
        }
    }
    return best;
}

std::vector<MonsterPtr> makeField(int count)
{
    std::vector<MonsterPtr> monsters;
    for (int i = 0; i < count; ++i)
        monsters.push_back(std::make_shared<Monster>("Mob" + std::to_string(i), 1 + (i * 11) % 60, (i * 17) % 400));
    return monsters;
}

std::string scan(const std::vector<MonsterPtr>& monsters, int ticks)
{
    std::string target;
    for (int t = 0; t < ticks; ++t)
    {
        const MonsterPtr m = mostDangerous(monsters);
        target = m ? m->name : "";
    }
    return target;
}
}

void pf06_aggro_scan()
{
    const auto field = perf6::makeField(3000);

    perf6::Monster::g_maxHolders = 0;
    const perf::Stopwatch sw;
    const std::string target = perf6::scan(field, 300);
    std::cout << "  몬스터 3000마리 x 300틱: " << sw.ms() << " ms, 한 몬스터를 동시에 붙잡은 shared_ptr 최대 "
              << perf6::Monster::g_maxHolders << "개\n";

    expect("가장 위험한 몬스터", std::string("Mob589"), target);
    expect("검사 중 한 몬스터를 동시에 붙잡은 shared_ptr 수 (목록 하나뿐)", 1L, perf6::Monster::g_maxHolders);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// Release + 성능 프로파일러(Alt+F2). 핫 경로에 _Incref/_Decref나 Interlocked(원자적 증감) 같은 이름이 보이나요?
// shared_ptr를 복사하면 무슨 일이 일어나나요? 그 일은 왜 일반 정수 증가보다 비쌀까요?
#pragma endregion

#pragma region 힌트 2
// noteHolders에 조건부 중단점(조건: weak_from_this().use_count() > 1)을 걸고, 멈추면 호출 스택을 보세요.
// 이 몬스터를 붙잡은 shared_ptr들은 각각 어느 줄에서 만들어졌나요? 한 곳만이 아닐 수 있습니다.
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void pf06_aggro_scan_hidden()
{
    const auto field = perf6::makeField(77);
    perf6::Monster::g_maxHolders = 0;
    expect("가장 위험한 몬스터", std::string("Mob49"), perf6::scan(field, 5));
    expect("위협 수치", 250, perf6::threatOf(std::make_shared<perf6::Monster>("x", 30, 50)));
    perf6::Monster::g_maxHolders = 0;
    perf6::scan(field, 3);
    expect("동시에 붙잡은 shared_ptr 수", 1L, perf6::Monster::g_maxHolders);
}
#pragma endregion
