#include "../labs.h"
#include <functional>
#include <string>
#include <vector>

// 챌린지 B-8
namespace blind8
{
class AlarmBell
{
public:
    void onRing(std::function<void(int)> listener) { listeners_.push_back(std::move(listener)); }
    void ring(int threat)
    {
        for (auto& listener : listeners_)
            listener(threat);
    }

private:
    std::vector<std::function<void(int)>> listeners_;
};

class Turret
{
public:
    explicit Turret(std::string name) : name_(std::move(name)) {}

    void connect(AlarmBell& bell)
    {
        bell.onRing([this](int threat) { shots_ += threat; });
    }

    const std::string& name() const { return name_; }
    int shots() const { return shots_; }

private:
    std::string name_;
    int shots_ = 0;
};

// 성벽에 포탑을 세우고 경보에 연결한다. 세울 때마다 바로 연결한다.
std::vector<Turret> buildWall(AlarmBell& bell, const std::vector<std::string>& names)
{
    std::vector<Turret> turrets;
    for (const std::string& n : names)
    {
        turrets.emplace_back(n);
        turrets.back().connect(bell);
    }
    return turrets;
}
}

void chB8()
{
    using namespace blind8;
    AlarmBell bell;
    const auto turrets = buildWall(bell, { "북문", "동문", "서문" });
    bell.ring(3);
    bell.ring(2);
    for (const Turret& t : turrets)
        std::cout << "  " << t.name() << " 포탑 발사 " << t.shots() << "번\n";
    expect("북문 포탑", 5, turrets.at(0).shots());
    expect("서문 포탑", 5, turrets.at(2).shots());
}

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
void chB8_hidden()
{
    using namespace blind8;
    AlarmBell bell;
    const auto turrets = buildWall(bell, { "a", "b", "c", "d", "e", "f", "g" });
    bell.ring(4);
    int total = 0;
    for (const Turret& t : turrets)
        total += t.shots();
    expect("모든 포탑이 한 번씩 쏨", 28, total);
    expect("첫 포탑", 4, turrets.front().shots());
    expect("마지막 포탑", 4, turrets.back().shots());
}
#pragma endregion
