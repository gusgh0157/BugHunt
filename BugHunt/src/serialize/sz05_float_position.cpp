#include "../labs.h"
#include <nlohmann/json.hpp>
#include <iomanip>
#include <sstream>
#include <string>

// 캐릭터 위치를 직접 만든 JSON 문자열로 저장하고, 불러올 때는 nlohmann::json으로 읽는다.
// 저장하고 불러올 때마다 캐릭터가 아주 조금씩 다른 자리에 나타난다는 제보가 들어왔다.

// [JA] キャラクターの位置を自作のJSON文字列で保存し、読み込みは nlohmann::json で行います。
// [JA] 保存して読み込むたびに、キャラクターがほんの少しずつ違う場所に現れるという報告がありました。
namespace sz5
{
using nlohmann::json;

struct Vec3
{
    float x = 0, y = 0, z = 0;
};

std::string savePosition(const Vec3& p)
{
    std::ostringstream out;
    out << "{\"x\":" << p.x << ",\"y\":" << p.y << ",\"z\":" << p.z << "}";
    return out.str();
}

Vec3 loadPosition(const std::string& text)
{
    const json j = json::parse(text);
    return { j.at("x").get<float>(), j.at("y").get<float>(), j.at("z").get<float>() };
}

bool same(const Vec3& a, const Vec3& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
}

void sz05_float_position()
{
    using namespace sz5;

    const Vec3 pos = { 1234.5678f, -87.65432f, 3.3333333f };
    const std::string text = savePosition(pos);
    const Vec3 back = loadPosition(text);

    std::cout << i18n::L("  저장한 문자열: ", "  保存した文字列: ") << text << '\n';
    std::cout << std::setprecision(9)
              << i18n::L("  원래 위치    : ", "  元の位置       : ") << pos.x << ", " << pos.y << ", " << pos.z << '\n'
              << i18n::L("  불러온 위치  : ", "  読み込んだ位置 : ") << back.x << ", " << back.y << ", " << back.z << '\n'
              << std::setprecision(6);

    expect(i18n::L("저장 → 불러오기 후 같은 위치인가?", "保存 → 読み込み後も同じ位置か?"), true, same(pos, back));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// savePosition의 return 줄에서 out.str()을, loadPosition의 return 줄에서 j를 보세요.
// 디버거는 float을 충분한 자릿수로 보여 줍니다. 저장된 문자열의 자릿수와 비교해 보세요.

// [JA] savePosition の return 行で out.str() を、loadPosition の return 行で j を見てください。
// [JA] デバッガーは float を十分な桁数で表示します。保存された文字列の桁数と比べてみてください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// ostream의 기본 정밀도는 유효 숫자 6자리입니다. float을 정확히 되살리려면 몇 자리가 필요한가요?
// 조사식에 std::numeric_limits<float>::max_digits10 을 넣어 보세요.

// [JA] ostream の既定の精度は有効数字6桁です。float を正確に復元するには何桁必要でしょうか?
// [JA] ウォッチに std::numeric_limits<float>::max_digits10 を入れてみてください。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// out << std::setprecision(std::numeric_limits<float>::max_digits10) 처럼 왕복에 필요한 자릿수를 정하세요.
// 또는 직접 문자열을 만들지 말고 json 객체에 값을 넣어 dump()하세요(nlohmann은 왕복 가능한 최소 자릿수로 씁니다).
// std::fixed는 "소수점 아래" 자릿수라서 아주 작은 값을 망칩니다.

// [JA] out << std::setprecision(std::numeric_limits<float>::max_digits10) のように、往復に必要な桁数を指定してください。
// [JA] または文字列を自作せず、json オブジェクトに値を入れて dump() してください(nlohmann は往復できる最小の桁数で書きます)。
// [JA] std::fixed は「小数点以下」の桁数なので、とても小さい値を壊してしまいます。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void sz05_float_position_hidden()
{
    using namespace sz5;

    const Vec3 cases[] = {
        { 123456.79f, 2.7182817f, 1.0000001f },
        { 0.007812501f, -0.0003141f, 16777215.0f },
        { 98765.43f, 0.1f, 9.9999999e-5f },
    };
    for (const Vec3& p : cases)
        expect(i18n::L("숨은 위치 왕복", "隠し位置の往復"), true, same(p, loadPosition(savePosition(p))));
}
#pragma endregion
