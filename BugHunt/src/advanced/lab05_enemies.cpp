#include "../labs.h"
#include <string>

namespace
{
struct Enemy
{
    std::string name;
    int hp;
};

// HP가 0 이하인 적을 목록에서 제거한다.
// 적 목록은 bughunt::CheckedList(src/labs.h)다. std::vector와 같지만, 무효가 된 반복자를 쓰면 어느 구성에서나 바로 멈춘다.

// [JA] HP が 0 以下の敵を一覧から取り除きます。
// [JA] 敵の一覧は bughunt::CheckedList(src/labs.h)です。std::vector と同じですが、無効になったイテレーターを使うと、どの構成でもすぐに止まります。
void removeDead(bughunt::CheckedList<Enemy>& enemies)
{
    for (auto it = enemies.begin(); it != enemies.end(); ++it)
    {
        if (it->hp <= 0)
            enemies.erase(it);
    }
}
}

void lab05_enemies()
{
    bughunt::CheckedList<Enemy> enemies = {
        { "Goblin",   0 },
        { "Orc",     35 },
        { "Slime",   -4 },
        { "Skeleton", 0 },
        { "Troll",   80 },
    };

    removeDead(enemies);

    expect("남은 적 수", size_t{ 2 }, enemies.size());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// erase 다음 줄에서 it가 가리키는 원소를 조사식으로 보세요. 지운 원소를 가리키던 이터레이터는 어떻게 되나요?
// (조사식에 it와 enemies를 넣고 둘의 generation_을 비교해 보세요.)

// [JA] erase の次の行で、it が指している要素をウォッチで見てください。消した要素を指していたイテレーターはどうなりますか?
// [JA] (ウォッチに it と enemies を入れて、両方の generation_ を比べてみてください。)
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// erase의 리턴값은 무엇인가요? (cppreference에서 확인)

// [JA] erase の戻り値は何ですか?(cppreference で確認)
#pragma endregion
