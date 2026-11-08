#include "../labs.h"
#include <string>

// 타워 디펜스: 탑은 만들어질 때 사거리를 정한다. 탑 종류마다 기본 사거리가 다르다.
// 저격 탑은 사거리 8이어야 하는데, 거리 7의 적을 쏘지 못한다.

// [JA] タワーディフェンス: タワーは作られるときに射程を決めます。タワーの種類ごとに基本射程が違います。
// [JA] スナイパータワーは射程8のはずなのに、距離7の敵を撃てません。
namespace inh3
{
class Tower
{
public:
    Tower()
    {
        range_ = baseRange();
    }
    virtual ~Tower() = default;

    virtual int baseRange() const { return 3; }
    virtual std::string kind() const { return i18n::L("기본 탑", "基本タワー"); }

    int range() const { return range_; }
    bool canHit(int distance) const { return distance <= range_; }

private:
    int range_ = 0;
};

class SniperTower : public Tower
{
public:
    int baseRange() const override { return 8; }
    std::string kind() const override { return i18n::L("저격 탑", "スナイパータワー"); }
};

class MortarTower : public Tower
{
public:
    int baseRange() const override { return 6; }
    std::string kind() const override { return i18n::L("박격포 탑", "迫撃砲タワー"); }
};
}

void ih03_sniper_tower()
{
    using namespace inh3;

    const Tower basic;
    const SniperTower sniper;
    std::cout << "  " << basic.kind() << i18n::L(" 사거리 ", "の射程") << basic.range()
              << i18n::L(", ", "、") << sniper.kind() << i18n::L(" 사거리 ", "の射程") << sniper.range() << '\n';

    expect("저격 탑 사거리", 8, sniper.range());
    expect("저격 탑이 거리 7을 쏘는가?", true, sniper.canHit(7));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// SniperTower::baseRange에 중단점을 걸어 보세요. 멈추나요? Tower::baseRange에도 걸고 다시 실행하세요.
// Tower::baseRange에서 멈추면 호출 스택을 보세요. 누가 불렀나요? 그때 *this는 무엇으로 보이나요?

// [JA] SniperTower::baseRange にブレークポイントを置いてみてください。止まりますか? Tower::baseRange にも置いて再実行してください。
// [JA] Tower::baseRange で止まったら呼び出し履歴を見てください。誰が呼びましたか? そのとき *this は何に見えますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// SniperTower를 만들면 먼저 Tower 생성자가 실행되고, 그다음 SniperTower 부분이 만들어집니다.
// Tower 생성자가 도는 동안 객체는 아직 "Tower"입니다. 그래서 가상 함수도 Tower 버전이 불립니다(소멸자에서도 마찬가지).

// [JA] SniperTower を作ると、まず Tower のコンストラクターが実行され、その後に SniperTower の部分が作られます。
// [JA] Tower のコンストラクターが動いている間、オブジェクトはまだ「Tower」です。だから仮想関数も Tower 版が呼ばれます(デストラクターでも同じです)。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 생성자·소멸자 안에서는 가상 함수에 기대지 마세요. 사거리를 생성자 매개변수로 넘기거나(Tower(int range). 기본 탑을 만드는 기본 생성자는 남겨 두세요),
// range()가 필요할 때 baseRange()를 부르게 바꾸세요.

// [JA] コンストラクター・デストラクターの中では仮想関数に頼らないでください。射程をコンストラクターの引数で渡す(Tower(int range)。基本タワーを作る既定コンストラクターは残してください)か、
// [JA] range() が必要なときに baseRange() を呼ぶように変えてください。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ih03_sniper_tower_hidden()
{
    using namespace inh3;

    const MortarTower mortar;
    expect("박격포 탑 사거리", 6, mortar.range());
    expect("박격포 탑: 거리 5", true, mortar.canHit(5));
    expect("박격포 탑: 거리 7", false, mortar.canHit(7));

    const Tower basic;
    expect("기본 탑 사거리", 3, basic.range());
    expect("기본 탑: 거리 4", false, basic.canHit(4));
}
#pragma endregion
