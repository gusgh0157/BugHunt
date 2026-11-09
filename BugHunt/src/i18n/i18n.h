#pragma once
#include <cstddef>
#include <string>
#include <vector>

// 실행 출력의 언어 (한국어 / 일본어).
// 정하는 순서: 명령줄 --lang ja|ko > 환경 변수 BUGHUNT_LANG > 운영체제 언어 > 기본 ko
// 번역하는 것: 랩 제목·증상, 메뉴, expect 설명, 감시 타이머·메모리 검사기·채점기의 메시지.
// 랩 코드가 따로 출력하는 진행 문구와 예외 메시지도 랩 안에서 L()로 번역한다. 게임 데이터(이름, 파일 내용 등)도 L()로 두 언어를 적는다. 비교하는 기대값도 같은 언어로 맞춘다.
// grade-report.json의 키와 result 값은 언어와 상관없이 같고, title과 detail만 선택한 언어로 나온다.
//
// [JA] 実行出力の言語(韓国語 / 日本語)。
// [JA] 決める順序: コマンドライン --lang ja|ko > 環境変数 BUGHUNT_LANG > OS の言語 > 既定は ko
// [JA] 翻訳するもの: ラボのタイトル・症状、メニュー、expect の説明、監視タイマー・メモリ検査器・採点器のメッセージ。
// [JA] ラボのコードが独自に出力する進行メッセージと例外メッセージも、ラボの中で L() で翻訳します。ゲームのデータ(名前、ファイルの内容など)も L() で両方の言語を書きます。比較する期待値も同じ言語にそろえます。
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

// 소스 검사 (BugHunt.exe --check-i18n <src 폴더>, src/i18n/source_check.cpp). 빌드할 때마다 실행된다.
// 한글 문자열 리터럴이 L("ko", "ja") 또는 { "ko", "ja" } 짝 밖에 있으면, 또는 일본어 쪽이 비었거나 한글이면 1을 돌려준다.
// [JA] ソース検査 (BugHunt.exe --check-i18n <src フォルダー>、src/i18n/source_check.cpp)。ビルドのたびに実行されます。
// [JA] ハングルの文字列リテラルが L("ko", "ja") または { "ko", "ja" } の対の外にあるとき、または日本語側が空かハングルのときに 1 を返します。
int checkSources(const std::string& dir);

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
