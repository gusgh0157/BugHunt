#include "../labs.h"
#include <string>
#include <vector>

// 챌린지 B-5

// [JA] チャレンジ B-5
namespace blind5
{
struct Unit
{
    std::string name;
    int summons;   // 등장할 때 부르는 부하 수
};

// 소환사는 등장하자마자 부하를 부른다. 부하는 아무도 부르지 않는다.

// [JA] 召喚士は登場するとすぐに手下を呼びます。手下は誰も呼びません。
std::vector<Unit> spawnWave(std::vector<Unit> wave)
{
    for (const Unit& u : wave)
    {
        for (int i = 0; i < u.summons; ++i)
            wave.push_back({ u.name + "의 부하", 0 });
    }
    return wave;
}
}

void chB5()
{
    using namespace blind5;
    const auto wave = spawnWave({ { "해골 병사", 0 }, { "강령술사", 2 }, { "해골 궁수", 0 } });
    for (const Unit& u : wave)
        std::cout << "  " << u.name << '\n';
    expect("전체 유닛 수", static_cast<size_t>(5), wave.size());
    expect("마지막 유닛", std::string("강령술사의 부하"), wave.back().name);
}

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
void chB5_hidden()
{
    using namespace blind5;
    const auto wave = spawnWave({ { "A", 1 }, { "B", 0 }, { "C", 3 } });
    expect("전체 유닛 수", static_cast<size_t>(7), wave.size());
    expect("넷째", std::string("A의 부하"), wave.at(3).name);
    expect("마지막", std::string("C의 부하"), wave.back().name);
    expect("빈 웨이브", static_cast<size_t>(0), spawnWave({}).size());
}
#pragma endregion
