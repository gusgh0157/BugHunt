#include "../labs.h"
#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// 세이브 파일은 내용을 버퍼에 모았다가 닫힐 때(소멸자) 한꺼번에 디스크에 쓴다.
// 디스크 공간이 모자라면 SaveError를 던지고, saveGame()이 잡아서 "저장 실패" 메시지를 띄운다.
// 디스크는 std::map으로 흉내 낸다 (실제 파일은 쓰지 않는다).

// [JA] セーブファイルは内容をバッファーにためておき、閉じられるとき(デストラクター)にまとめてディスクに書き込みます。
// [JA] ディスクの空きが足りなければ SaveError を投げ、saveGame() がそれを捕まえて「セーブ失敗」メッセージを出します。
// [JA] ディスクは std::map で模倣します(実際のファイルには書き込みません)。
namespace lang8
{
std::map<std::string, std::string> g_disk;
std::string g_lastMessage;

struct SaveError : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class SaveFile
{
public:
    SaveFile(std::string slot, std::size_t freeBytes) : slot_(std::move(slot)), freeBytes_(freeBytes) {}

    void write(const std::string& line)
    {
        if (line.empty())
            throw SaveError(i18n::L("빈 줄은 저장할 수 없다", "空の行はセーブできない"));
        buffer_ += line;
        buffer_ += '\n';
    }

    // 닫힐 때 버퍼를 디스크에 쓴다. 공간이 모자라면 저장 실패를 알린다.
    // [JA] 閉じるときにバッファーをディスクに書き込みます。容量が足りなければ保存の失敗を知らせます。
    ~SaveFile()
    {
        if (buffer_.size() > freeBytes_)
            throw SaveError(i18n::L("디스크 공간이 모자란다", "ディスク容量が足りない"));
        g_disk[slot_] = buffer_;
    }

private:
    std::string slot_;
    std::size_t freeBytes_;
    std::string buffer_;
};

bool saveGame(const std::string& slot, const std::vector<std::string>& lines, std::size_t freeBytes)
{
    try
    {
        SaveFile file(slot, freeBytes);
        for (const std::string& line : lines)
            file.write(line);
    }
    catch (const SaveError& e)
    {
        g_lastMessage = std::string(i18n::L("저장 실패: ", "セーブ失敗: ")) + e.what();
        return false;
    }
    g_lastMessage = i18n::L("저장 완료", "セーブ完了");
    return true;
}
}

void lg08_save_close()
{
    using namespace lang8;
    g_disk.clear();

    const bool first = saveGame(i18n::L("슬롯 1", "スロット1"), { i18n::L("레벨=12", "レベル=12"), i18n::L("골드=340", "ゴールド=340") }, 1024);
    std::cout << i18n::L("  슬롯 1 저장 (여유 1024바이트): ", "  スロット1のセーブ(空き1024バイト): ") << g_lastMessage << '\n';

    const bool second = saveGame(i18n::L("슬롯 2", "スロット2"), { i18n::L("레벨=30", "レベル=30"), i18n::L("골드=98000", "ゴールド=98000"), i18n::L("위치=용의 둥지", "位置=竜の巣") }, 16);
    std::cout << i18n::L("  슬롯 2 저장 (여유 16바이트): ", "  スロット2のセーブ(空き16バイト): ") << g_lastMessage << '\n';

    expect(i18n::L("슬롯 1 저장 성공", "スロット1のセーブ成功"), true, first);
    expect(i18n::L("슬롯 1 내용", "スロット1の内容"), std::string(i18n::L("레벨=12\n골드=340\n", "レベル=12\nゴールド=340\n")), g_disk[i18n::L("슬롯 1", "スロット1")]);
    expect(i18n::L("슬롯 2 저장 실패를 알림", "スロット2のセーブ失敗を知らせる"), false, second);
    expect(i18n::L("실패 메시지", "失敗メッセージ"), std::string(i18n::L("저장 실패: 디스크 공간이 모자란다", "セーブ失敗: ディスク容量が足りない")), g_lastMessage);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 디버거로 실행하면 예외가 던져진 곳에서 멈춥니다. 호출 스택을 아래로 내려가 보세요. ~SaveFile 아래에 saveGame의 catch가 아니라 무엇이 있나요?
// 출력에 "저장 실패" 메시지가 나왔나요? catch 블록 첫 줄에 중단점을 걸어 보세요. 멈추나요?

// [JA] デバッガーで実行すると、例外が投げられた場所で止まります。呼び出し履歴を下へたどってみてください。~SaveFile の下に、saveGame の catch ではなく何がありますか?
// [JA] 出力に「セーブ失敗」メッセージは出ましたか? catch ブロックの最初の行にブレークポイントを置いてみてください。止まりますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// C++11부터 소멸자는 따로 적지 않아도 noexcept입니다. noexcept 함수 밖으로 예외가 나가려 하면 catch를 찾지 않고 곧바로 std::terminate가 불려 프로그램이 끝납니다.
// 소멸자를 noexcept(false)로 바꾸는 것도 답이 아닙니다. 다른 예외 때문에 스택을 풀던 중에 소멸자가 또 던지면, 그때도 std::terminate입니다.
// (함수에 noexcept를 붙여 두고 안에서 던지는 경우도 똑같이 끝납니다)

// [JA] C++11 からは、デストラクターは何も書かなくても noexcept です。noexcept の関数の外へ例外が出ようとすると、catch を探さずにすぐ std::terminate が呼ばれてプログラムが終わります。
// [JA] デストラクターを noexcept(false) に変えるのも答えではありません。別の例外でスタックを巻き戻している最中にデストラクターがさらに投げると、そのときも std::terminate です。
// [JA] (関数に noexcept を付けておいて中で投げる場合も、同じように終わります)
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 실패할 수 있는 일은 소멸자 밖으로 꺼내세요. void close()를 만들어 거기서 디스크에 쓰고 실패하면 던지게 하고, saveGame의 try 안 마지막 줄에서 file.close()를 부릅니다.
// 소멸자는 닫히지 않은 경우에만 정리하고, 그 안의 오류는 삼킵니다(try { ... } catch (...) {}). 소멸자는 실패를 알릴 길이 없는 곳입니다.

// [JA] 失敗しうる処理はデストラクターの外へ出してください。void close() を作ってそこでディスクに書き込み、失敗したら投げるようにして、saveGame の try の中の最後の行で file.close() を呼びます。
// [JA] デストラクターは閉じられていない場合だけ後始末をし、その中のエラーは握りつぶします(try { ... } catch (...) {})。デストラクターは失敗を知らせる手段がない場所です。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void lg08_save_close_hidden()
{
    using namespace lang8;
    g_disk.clear();

    expect(i18n::L("숨은 저장: 보통 저장", "隠しセーブ: 通常のセーブ"), true, saveGame("A", { i18n::L("직업=도적", "職業=盗賊") }, 64));
    expect(i18n::L("숨은 저장: 공간 부족", "隠しセーブ: 空き不足"), false, saveGame("A", { i18n::L("직업=성기사", "職業=聖騎士"), i18n::L("칭호=빛의 수호자", "称号=光の守護者") }, 20));
    expect(i18n::L("숨은 저장: 실패한 저장은 이전 내용을 지키는가?", "隠しセーブ: 失敗したセーブは以前の内容を守るか?"), std::string(i18n::L("직업=도적\n", "職業=盗賊\n")), g_disk["A"]);

    // 빈 줄 때문에 write가 던지고, 스택을 푸는 동안 세이브 파일이 닫힌다 (공간도 모자람)
    // [JA] 空行のせいで write がスローし、スタックを巻き戻す間にセーブファイルが閉じられます(容量も足りない)
    expect(i18n::L("숨은 저장: 빈 줄", "隠しセーブ: 空行"), false, saveGame("B", { i18n::L("이름=카일", "名前=カイル"), "", i18n::L("골드=1", "ゴールド=1") }, 4));
    expect(i18n::L("숨은 저장: 첫 오류 메시지", "隠しセーブ: 最初のエラーメッセージ"), std::string(i18n::L("저장 실패: 빈 줄은 저장할 수 없다", "セーブ失敗: 空の行はセーブできない")), g_lastMessage);
    expect(i18n::L("숨은 저장: 다시 보통 저장", "隠しセーブ: 再び通常のセーブ"), true, saveGame("C", { i18n::L("레벨=1", "レベル=1") }, 64));
}
#pragma endregion
