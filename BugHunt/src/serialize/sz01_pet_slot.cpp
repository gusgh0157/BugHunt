#include "../labs.h"
#include <nlohmann/json.hpp>
#include <string>

// 세이브(JSON)를 불러와 상태 창을 그리고, 펫이 있을 때만 "펫 소환" 버튼을 켠다.
// 펫을 한 번도 얻지 않은 캐릭터는 세이브에 "pet" 키가 아예 없다.

// [JA] セーブ(JSON)を読み込んでステータス画面を描き、ペットがいるときだけ「ペット召喚」ボタンを有効にします。
// [JA] 一度もペットを手に入れていないキャラクターは、セーブに "pet" キー自体がありません。
namespace sz1
{
using nlohmann::json;

// 상태 창: 펫 이름 칸
std::string petLabel(json& save)
{
    if (save["pet"].is_null())
        return i18n::L("(펫 없음)", "(ペットなし)");
    return save["pet"]["name"].get<std::string>();
}

// 소환 버튼: 펫이 있을 때만 켠다
bool canSummonPet(const json& save)
{
    return save.contains("pet");
}
}

void sz01_pet_slot()
{
    using namespace sz1;

    const std::string text = R"({"name":"아린","level":12,"gold":340})";
    json save = json::parse(text);
    const std::string before = save.dump();

    const std::string label = petLabel(save);         // 상태 창을 먼저 그리고
    const bool summon = canSummonPet(save);           // 버튼을 정한다

    std::cout << i18n::L("  펫 칸: ", "  ペット欄: ") << label << i18n::L(", 소환 버튼: ", "、召喚ボタン: ")
              << (summon ? i18n::L("켬", "オン") : i18n::L("끔", "オフ")) << '\n';
    std::cout << i18n::L("  다시 저장한 세이브: ", "  保存し直したセーブ: ") << save.dump() << '\n';

    expect("펫 칸", std::string(i18n::L("(펫 없음)", "(ペットなし)")), label);
    expect("소환 버튼", false, summon);
    expect("다시 저장해도 세이브가 그대로인가?", before, save.dump());
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// petLabel 호출 줄과 canSummonPet 호출 줄에 중단점을 걸고, 두 곳에서 save.size()와 save.dump()를 조사식으로 보세요.
// 두 줄 사이에 무엇이 달라졌나요? (조사식에서 dump()를 계산할 수 없으면 save를 펼쳐 m_data를 보세요.)

// [JA] petLabel を呼ぶ行と canSummonPet を呼ぶ行にブレークポイントを置き、両方で save.size() と save.dump() をウォッチで見てください。
// [JA] 2行の間で何が変わりましたか? (ウォッチで dump() を評価できなければ、save を展開して m_data を見てください。)
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// const가 아닌 json의 operator[]는 std::map의 operator[](4부 랩 33)처럼, 없는 키를 null 값으로 "만들어서" 돌려줍니다.
// 그래서 "없는지 확인"하려고 쓴 save["pet"]이 키를 만들어 버립니다.

// [JA] const でない json の operator[] は、std::map の operator[](第4部ラボ33)と同じく、ないキーを null 値で「作って」返します。
// [JA] そのため「ないことを確認」しようと書いた save["pet"] が、キーを作ってしまいます。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 있는지 확인은 contains(키) 또는 find(키)로 하세요. 읽기만 하는 함수는 매개변수를 const json&로 받으면
// 실수로 키를 만드는 코드를 컴파일러가 막아 줍니다. (const json에 없는 키로 []를 쓰는 것은 정의되지 않은 동작이니 at/find를 쓰세요.)

// [JA] あるかどうかの確認は contains(キー) か find(キー) で行ってください。読むだけの関数は引数を const json& で受け取ると、
// [JA] うっかりキーを作るコードをコンパイラーが止めてくれます。(const json にないキーで [] を使うのは未定義動作なので、at/find を使ってください。)
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void sz01_pet_slot_hidden()
{
    using namespace sz1;

    json withPet = json::parse(R"({"name":"보람","pet":{"name":"구름","kind":"여우"}})");
    expect("펫 있음: 펫 칸", std::string("구름"), petLabel(withPet));
    expect("펫 있음: 소환 버튼", true, canSummonPet(withPet));

    json noPet = json::parse(R"({"name":"도윤","class":"궁수","quests":[1,2,3]})");
    const size_t keys = noPet.size();
    expect("펫 없음: 펫 칸", std::string(i18n::L("(펫 없음)", "(ペットなし)")), petLabel(noPet));
    expect("펫 없음: 소환 버튼", false, canSummonPet(noPet));
    expect("펫 없음: 키 수가 그대로인가?", keys, noPet.size());
}
#pragma endregion
