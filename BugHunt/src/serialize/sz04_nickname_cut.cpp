#include "../labs.h"
#include <nlohmann/json.hpp>
#include <string>

// 서버 규칙: 닉네임은 최대 12바이트까지만 저장한다. 넘으면 잘라서 저장한다.
// 영어 닉네임은 잘 저장되는데, 어떤 한글 닉네임은 저장 자체가 실패한다.

// [JA] サーバーの規則: ニックネームは最大12バイトまでしか保存しません。超えたら切って保存します。
// [JA] 英語のニックネームはちゃんと保存されるのに、一部のハングル(や日本語)のニックネームは保存そのものが失敗します。
namespace sz4
{
using nlohmann::json;

constexpr size_t kMaxNameBytes = 12;

std::string clampName(const std::string& name)
{
    if (name.size() <= kMaxNameBytes)
        return name;
    return name.substr(0, kMaxNameBytes);
}

bool saveProfile(const std::string& name, int level, std::string& out)
{
    json j;
    j["name"] = clampName(name);
    j["level"] = level;
    try
    {
        out = j.dump();
        return true;
    }
    catch (const json::exception& e)
    {
        std::cout << i18n::L("  저장 실패: ", "  保存失敗: ") << e.what() << '\n';
        return false;
    }
}
}

void sz04_nickname_cut()
{
    using namespace sz4;

    std::string saved;
    expect(i18n::L("영어 닉네임 저장", "英語ニックネームの保存"), true, saveProfile("DragonSlayer99", 30, saved));
    std::cout << "  " << saved << '\n';

    const bool ok = saveProfile(i18n::L("Lv99용감한전사", "Lv99勇敢な戦士"), 99, saved);
    if (ok)
        std::cout << "  " << saved << '\n';
    expect(i18n::L("한글 닉네임 저장", "日本語ニックネームの保存"), true, ok);
    expect(i18n::L("잘린 닉네임", "切り詰めたニックネーム"), std::string(i18n::L("Lv99용감", "Lv99勇敢")), clampName(i18n::L("Lv99용감한전사", "Lv99勇敢な戦士")));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// catch 블록에 중단점을 걸고 e.what()을 읽어 보세요. 어떤 바이트를 문제 삼나요?
// 그다음 clampName의 return 줄에서 돌려줄 문자열의 마지막 바이트들을 (int)(unsigned char)s[i] 로 보세요.

// [JA] catch ブロックにブレークポイントを置き、e.what() を読んでみてください。どのバイトを問題にしていますか?
// [JA] 次に clampName の return 行で、返す文字列の最後の数バイトを (int)(unsigned char)s[i] で見てください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// "Lv99" 4바이트 + 한글 한 글자 3바이트씩. 12바이트에서 자르면 세 번째 한글 글자의 어디에서 잘리나요?
// JSON 문자열은 올바른 UTF-8이어야 해서, 반쪽짜리 글자가 있으면 dump가 예외를 던집니다.

// [JA] "Lv99" が4バイト + ハングル1文字が3バイトずつ(日本語のかな・漢字も多くは3バイト)。12バイトで切ると、3文字目のどこで切れますか?
// [JA] JSONの文字列は正しいUTF-8でなければならないので、半端な文字があると dump が例外を投げます。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 자를 위치가 글자의 중간(이어지는 바이트: 상위 2비트가 10, 즉 (b & 0xC0) == 0x80)이면 앞으로 물러나세요.
// dump의 오류 처리기(replace/ignore)로 예외만 막으면 화면과 서버에 깨진 이름이 남습니다.

// [JA] 切る位置が文字の途中(後続バイト: 上位2ビットが10、つまり (b & 0xC0) == 0x80)なら、前に戻ってください。
// [JA] dump のエラーハンドラー(replace/ignore)で例外だけを止めると、画面とサーバーに壊れた名前が残ります。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void sz04_nickname_cut_hidden()
{
    using namespace sz4;

    expect(i18n::L("한글만 15바이트", "日本語だけ15バイト"), std::string(i18n::L("가나다라", "あいうえ")), clampName(i18n::L("가나다라마", "あいうえお")));
    expect(i18n::L("영어 2 + 한글", "英字2 + 日本語"), std::string(i18n::L("ab가나다", "abあいう")), clampName(i18n::L("ab가나다라", "abあいうえ")));
    expect(i18n::L("4바이트 글자", "4バイト文字"), std::string("a\xF0\x9F\x90\x89\xF0\x9F\x90\x89"),
           clampName("a\xF0\x9F\x90\x89\xF0\x9F\x90\x89\xF0\x9F\x90\x89"));   // a + 용 이모지 3개
    expect(i18n::L("짧은 이름", "短い名前"), std::string("Kim"), clampName("Kim"));
    std::string saved;
    expect(i18n::L("숨은 닉네임 저장", "隠しニックネームの保存"), true, saveProfile(i18n::L("마법사ab다람쥐", "魔術師abコリス"), 7, saved));
}
#pragma endregion
