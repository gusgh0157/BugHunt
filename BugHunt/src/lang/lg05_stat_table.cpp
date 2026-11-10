#include "../labs.h"
#include "lg05_stats.h"

// 게임을 켤 때 설정 파일에서 기본 스탯을 읽고, 보스와 직업의 스탯을 기본값에서 계산해 전역 변수에 담아 둔다.
// 스탯은 두 파일에 나뉘어 있다: 이 파일(기본 체력·마나, 보스)과 lg05_class_stats.cpp(기본 공격력·방어력, 직업).

// [JA] ゲームの起動時に設定ファイルから基本ステータスを読み、ボスと職業のステータスを基本値から計算してグローバル変数に入れておきます。
// [JA] ステータスは二つのファイルに分かれています: このファイル(基本HP・マナ、ボス)と lg05_class_stats.cpp(基本攻撃力・防御力、職業)です。
namespace lang5
{
// 실제 게임에서는 설정 파일에서 읽는다. 컴파일러가 값을 미리 계산해 두지 못하게 실행 중에 읽는다.
// [JA] 実際のゲームでは設定ファイルから読みます。コンパイラーが値を前もって計算しておけないよう、実行中に読みます。
int readStat(const char*, int fallback)
{
    volatile int value = fallback;
    return value;
}

int g_baseHp = readStat("base_hp", 100);
int g_baseMana = readStat("base_mana", 40);

int g_bossAttack = baseAttack() * 4;   // 보스는 기본 공격력의 4배 [JA] ボスは基本攻撃力の 4 倍
int g_eliteArmor = baseArmor() * 2;    // 정예 몬스터는 기본 방어력의 2배 [JA] 精鋭モンスターは基本防御力の 2 倍

int baseHp() { return g_baseHp; }
int baseMana() { return g_baseMana; }
int bossAttack() { return g_bossAttack; }
int eliteArmor() { return g_eliteArmor; }
}

void lg05_stat_table()
{
    using namespace lang5;

    std::cout << i18n::L("  기본: 체력 ", "  基本: HP ") << baseHp() << i18n::L(", 마나 ", "、マナ ") << baseMana() << i18n::L(", 공격력 ", "、攻撃力 ") << baseAttack() << i18n::L(", 방어력 ", "、防御力 ") << baseArmor() << '\n';
    std::cout << i18n::L("  보스 공격력: ", "  ボスの攻撃力: ") << bossAttack() << '\n';
    std::cout << i18n::L("  전사 체력: ", "  戦士のHP: ") << warriorHp() << '\n';

    expect(i18n::L("보스 공격력 (기본 공격력 x 4)", "ボスの攻撃力(基本攻撃力 x 4)"), baseAttack() * 4, bossAttack());
    expect(i18n::L("전사 체력 (기본 체력 x 3)", "戦士のHP(基本HP x 3)"), baseHp() * 3, warriorHp());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// g_bossAttack = ... 줄과 lg05_class_stats.cpp의 g_warriorHp = ... 줄에 중단점을 걸고 처음부터 디버깅을 시작하세요.
// 어느 줄이 먼저 멈추나요? 멈췄을 때 호출 스택에 main이 있나요? 조사식에 g_baseAttack과 g_baseHp를 넣어 보세요.

// [JA] g_bossAttack = ... の行と lg05_class_stats.cpp の g_warriorHp = ... の行にブレークポイントを置き、最初からデバッグを始めてください。
// [JA] どちらの行が先に止まりますか? 止まったとき、呼び出し履歴に main はありますか? ウォッチに g_baseAttack と g_baseHp を入れてみてください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 전역 변수의 초기화는 main보다 먼저 실행됩니다. 한 파일 안에서는 위에서 아래 순서지만, 서로 다른 파일(번역 단위) 사이의 순서는 정해져 있지 않습니다.
// 보통은 링크 순서를 따르므로, 빌드 설정이나 파일 이름만 바꿔도 결과가 바뀝니다(static initialization order fiasco).
// 아직 초기화되지 않은 int 전역은 0입니다. 이 랩은 두 파일이 서로의 값을 쓰므로 어느 쪽이 먼저든 하나는 0을 읽습니다.

// [JA] グローバル変数の初期化は main より先に実行されます。一つのファイルの中では上から下の順ですが、異なるファイル(翻訳単位)の間の順序は決まっていません。
// [JA] 普通はリンク順に従うので、ビルド設定やファイル名を変えるだけで結果が変わります(static initialization order fiasco)。
// [JA] まだ初期化されていない int のグローバル変数は 0 です。このラボは二つのファイルが互いの値を使うので、どちらが先でも一方は 0 を読みます。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 값을 처음 쓸 때 만들도록 바꾸세요. 함수 안의 static 지역 변수는 그 함수가 처음 불릴 때 초기화됩니다(스레드 안전).
//   int baseAttack() { static const int value = readStat("base_attack", 12); return value; }
//   int bossAttack() { static const int value = baseAttack() * 4; return value; }
// 의존하는 전역 변수 여덟 개 모두를 이렇게 바꾸면 순서와 상관없이 맞습니다. (constexpr로 만들 수 있는 값이면 컴파일할 때 정해지므로 더 좋습니다)

// [JA] 値を最初に使うときに作るように変えてください。関数の中の static ローカル変数は、その関数が最初に呼ばれたときに初期化されます(スレッドセーフ)。
// [JA]   int baseAttack() { static const int value = readStat("base_attack", 12); return value; }
// [JA]   int bossAttack() { static const int value = baseAttack() * 4; return value; }
// [JA] 依存しているグローバル変数の八つすべてをこのように変えれば、順序に関係なく正しくなります。(constexpr にできる値ならコンパイル時に決まるので、さらに良いです)
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void lg05_stat_table_hidden()
{
    using namespace lang5;
    expect(i18n::L("숨은 스탯: 기본 공격력", "隠しステータス: 基本攻撃力"), 12, baseAttack());
    expect(i18n::L("숨은 스탯: 기본 체력", "隠しステータス: 基本HP"), 100, baseHp());
    expect(i18n::L("숨은 스탯: 정예 방어력 (기본 방어력 x 2)", "隠しステータス: 精鋭の防御力(基本防御力 x 2)"), 14, eliteArmor());
    expect(i18n::L("숨은 스탯: 마법사 마나 (기본 마나 x 5)", "隠しステータス: 魔法使いのマナ(基本マナ x 5)"), 200, mageMana());
}
#pragma endregion
