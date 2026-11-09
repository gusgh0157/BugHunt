#include "../labs.h"
#include <new>
#include <string>

// 파티원의 공격력 = 기본 공격력 + 장착한 무기 보너스. 무기가 없으면 맨손(보너스 0).

// [JA] パーティーメンバーの攻撃力 = 基本攻撃力 + 装備した武器のボーナスです。武器がなければ素手(ボーナス 0)です。
namespace ptr5
{
struct Weapon
{
    std::string name;
    int bonus;
};

class Hero
{
public:
    Hero(const std::string& name, int baseAttack)
        : name_(name), baseAttack_(baseAttack)
    {
    }

    Hero(const std::string& name, int baseAttack, const Weapon* weapon)
        : name_(name), baseAttack_(baseAttack), weapon_(weapon)
    {
    }

    void equip(const Weapon* weapon) { weapon_ = weapon; }

    int attackPower() const
    {
        int power = baseAttack_;
        if (weapon_ != nullptr)
            power += weapon_->bonus;
        return power;
    }

    const std::string& name() const { return name_; }

private:
    std::string name_;
    int baseAttack_;
    const Weapon* weapon_;
};

int partyAttack(const Hero* heroes, int count)
{
    int total = 0;
    for (int i = 0; i < count; ++i)
    {
        const int power = heroes[i].attackPower();
        std::cout << "  " << heroes[i].name() << i18n::L(": 공격력 ", ": 攻撃力 ") << power << '\n';
        total += power;
    }
    return total;
}
}

void pt05_equipment()
{
    using namespace ptr5;

    const Weapon flameSword = { "Flame Sword", 25 };
    const Weapon oakStaff = { "Oak Staff", 10 };

    // 파티 슬롯은 던전마다 새로 할당하지 않고, 지난 던전에 쓰던 메모리 자리에 다시 만든다. (오브젝트 풀)
    Hero* party = static_cast<Hero*>(bughunt::recycledStorage(sizeof(Hero) * 3));
    new (&party[0]) Hero("Kael", 30, &flameSword);
    new (&party[1]) Hero("Mira", 18, &oakStaff);
    new (&party[2]) Hero("Doran", 35);   // 맨손 격투가

    expect(i18n::L("파티 총 공격력 (55 + 28 + 35)", "パーティーの合計攻撃力(55 + 28 + 35)"), 118, partyAttack(party, 3));

    for (int i = 0; i < 3; ++i)
        party[i].~Hero();
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 디버거로 실행하면 잘못된 메모리 접근(Windows: 액세스 위반, POSIX: SIGSEGV)에서 멈춥니다. this를 펼쳐 weapon_ 값을 읽어 보세요.
// 0x0000000000000000(nullptr)이 아니라면 어떤 값인가요? 0xA5A5A5A5A5A5A5A5는 그 자리에 앞 사용자가 남긴 값입니다.
// (x64에서 "있을 수 없는 주소"를 읽으면 디버거가 주소를 0xFFFFFFFFFFFFFFFF나 0으로 보여 주기도 합니다.)

// [JA] デバッガーで実行すると、不正なメモリアクセス(Windows: アクセス違反、POSIX: SIGSEGV)で止まります。this を展開して weapon_ の値を読んでみてください。
// [JA] 0x0000000000000000(nullptr)でないなら、どんな値でしょうか? 0xA5A5A5A5A5A5A5A5 は、その場所に前の使用者が残した値です。
// [JA] (x64 で「ありえないアドレス」を読むと、デバッガーがアドレスを 0xFFFFFFFFFFFFFFFF や 0 と表示することもあります。)
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// weapon_의 값은 누가 넣었나요?
// Doran을 만든 생성자에 중단점을 걸고, 생성자가 끝난 직후 weapon_을 확인해 보세요.

// [JA] weapon_ の値は誰が入れたのでしょうか?
// [JA] Doran を作ったコンストラクターにブレークポイントを置き、コンストラクターが終わった直後に weapon_ を確認してみてください。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 진짜 프로그램에서 초기화하지 않은 멤버는 "그 자리에 있던 아무 값"입니다. 우연히 0이면 멀쩡해 보이고,
// 구성이나 컴파일러를 바꾸면 갑자기 죽습니다. 이 랩은 결과가 흔들리지 않도록 남은 값을 일부러 채워 두었습니다.
// "초기화하지 않은 포인터"는 nullptr 검사로 막을 수 없습니다. 검사하려면 먼저 nullptr로 만들어 둬야 합니다.

// [JA] 実際のプログラムでは、初期化していないメンバーは「その場所にあった何かの値」です。偶然0なら正常に見え、
// [JA] 構成やコンパイラーを変えると突然クラッシュします。このラボでは結果がぶれないよう、残っている値をわざと埋めておきました。
// [JA] 「初期化していないポインター」は nullptr チェックでは防げません。チェックするには、まず nullptr にしておく必要があります。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void pt05_equipment_hidden()
{
    using namespace ptr5;

    const Weapon bow = { "Long Bow", 14 };

    Hero rookie("Nell", 12);             // 맨손으로 시작했다가
    expect(i18n::L("Nell 맨손 공격력", "Nell の素手の攻撃力"), 12, rookie.attackPower());
    rookie.equip(&bow);                  // 활을 든다
    expect(i18n::L("Nell 활 장착 공격력", "Nell の弓装備時の攻撃力"), 26, rookie.attackPower());

    const Hero duo[] = { Hero("Ivo", 40), Hero("Pia", 22, &bow) };
    expect(i18n::L("두 명 총 공격력", "2人の合計攻撃力"), 76, partyAttack(duo, 2));
}
#pragma endregion
