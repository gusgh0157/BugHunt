#pragma once
#include <cstddef>
#include <string>
#include <vector>

// 실행 출력의 언어 (한국어 / 일본어).
// 정하는 순서: 명령줄 --lang ja|ko > 환경 변수 BUGHUNT_LANG > 운영체제 언어 > 기본 ko
// 번역하는 것: 랩 제목·증상, 메뉴, expect 설명, 감시 타이머·메모리 검사기·채점기의 메시지.
// 랩 코드가 따로 출력하는 진행 문구와 예외 메시지도 랩 안에서 L()로 번역한다. 게임 데이터(이름, 파일 내용 등)는 한국어 그대로 둔다.
// grade-report.json의 키와 result 값은 언어와 상관없이 같고, title과 detail만 선택한 언어로 나온다.
//
// [JA] 実行出力の言語(韓国語 / 日本語)。
// [JA] 決める順序: コマンドライン --lang ja|ko > 環境変数 BUGHUNT_LANG > OS の言語 > 既定は ko
// [JA] 翻訳するもの: ラボのタイトル・症状、メニュー、expect の説明、監視タイマー・メモリ検査器・採点器のメッセージ。
// [JA] ラボのコードが独自に出力する進行メッセージと例外メッセージも、ラボの中で L() で翻訳します。ゲームのデータ(名前、ファイルの内容など)は韓国語のままにします。
// [JA] grade-report.json のキーと result の値は言語に関係なく同じで、title と detail だけが選んだ言語になります。
namespace i18n
{
enum class Lang
{
    ko,
    ja,
};

// 한 문구의 두 언어. 표(랩 목록 등)에 쓴다. 표는 static_assert로 빈 번역이 없는지 검사한다.
struct Text
{
    const char* ko;
    const char* ja;
};

// argv에서 --lang ja|ko(또는 --lang=ja)를 읽고 지운 나머지 인자를 돌려준다. argv[0]은 그대로 둔다.
std::vector<std::string> init(int argc, char** argv);

Lang current();
const char* code();   // "ko" 또는 "ja" (자식 프로세스에 넘길 때)

// 고른 언어의 문구. 인자가 둘이라 한쪽을 빠뜨리면 컴파일 오류가 난다.
inline const char* L(const char* ko, const char* ja)
{
    return current() == Lang::ja ? ja : ko;
}

inline const char* pick(const Text& t)
{
    return L(t.ko, t.ja);
}

// expect 설명처럼 랩 코드에 한국어로 적힌 문구 → 고른 언어 (src/i18n/strings_expect.inc의 표).
// 표에 없으면 한국어 그대로 돌려주고 "[NO-JA] 문구" 줄을 출력한다. 채점기(--grade)가 이 줄을 모아 마지막에 보여 준다.
// 메모리를 할당하지 않는다(할당 횟수를 세는 랩의 결과를 바꾸지 않으려고).
const char* tr(const char* ko);

// 번역이 빠졌다는 표시 (채점기가 자식 출력에서 찾는다)
inline constexpr const char* kMissingMark = "[NO-JA] ";

// 표에 빈 문구가 없는가 (static_assert용)
template <class T, std::size_t N, class Get>
constexpr bool allTranslated(const T (&table)[N], Get get)
{
    for (const T& row : table)
    {
        const Text t = get(row);
        if (t.ko == nullptr || t.ja == nullptr || t.ko[0] == '\0' || t.ja[0] == '\0')
            return false;
    }
    return true;
}
}
