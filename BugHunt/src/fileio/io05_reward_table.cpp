#include "../labs.h"
#include "io_kit.h"
#include <string>
#include <vector>

// 보상표 파일: 몬스터 번호들이 나오고, "보상"이라는 머리글 뒤에 같은 순서로 골드가 나온다.
//     101 102 103
//     보상
//     50 75 120

// [JA] 報酬表ファイル: モンスター番号が並び、「報酬」(`--lang ko` では「보상」)という見出しの後に同じ順番でゴールドが並びます。
namespace io5
{
namespace fs = std::filesystem;

struct RewardTable
{
    std::vector<int> monsterIds;
    std::vector<int> gold;
};

RewardTable loadRewards(const fs::path& file)
{
    RewardTable table;
    std::ifstream in(file);

    int id = 0;
    while (in >> id)                  // 숫자가 아닌 "보상"에서 멈춘다
        table.monsterIds.push_back(id);

    std::string header;
    in >> header;

    int gold = 0;
    while (in >> gold)
        table.gold.push_back(gold);
    return table;
}

int goldFor(const RewardTable& table, int monsterId)
{
    for (size_t i = 0; i < table.monsterIds.size() && i < table.gold.size(); ++i)
    {
        if (table.monsterIds[i] == monsterId)
            return table.gold[i];
    }
    return 0;
}
}

void io05_reward_table()
{
    using namespace io5;

    iokit::LabDir lab("io05_reward_table");
    const fs::path file = lab / "rewards.txt";
    iokit::writeBytes(file, i18n::L("101 102 103\n보상\n50 75 120\n", "101 102 103\n報酬\n50 75 120\n"));

    const RewardTable table = loadRewards(file);
    std::cout << i18n::L("  몬스터 ", "  モンスター") << table.monsterIds.size() << i18n::L("종, 보상 ", "種、報酬") << table.gold.size() << i18n::L("개\n", "個\n");

    expect(i18n::L("보상 개수", "報酬の数"), static_cast<size_t>(3), table.gold.size());
    expect(i18n::L("102번 몬스터 보상", "102番モンスターの報酬"), 75, goldFor(table, 102));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// in >> header 줄에 중단점을 거세요. 멈추면 조사식에 in.fail(), in.eof(), in.good()을 넣고,
// 한 줄 실행한 뒤 header에 무엇이 들어갔는지 보세요.

// [JA] in >> header の行にブレークポイントを置いてください。止まったらウォッチに in.fail()、in.eof()、in.good() を入れ、
// [JA] 1行実行したあと header に何が入ったかを見てください。
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 첫 while은 "보상"을 숫자로 읽으려다 실패해서 끝났습니다. 그때 스트림에 실패 표시(failbit)가 켜집니다.
// 실패 표시가 켜진 스트림에서 >> 를 하면 어떻게 되나요? 표시는 저절로 꺼지나요?

// [JA] 最初の while は「報酬」を数値として読もうとして失敗し、終わりました。そのときストリームに失敗フラグ(failbit)が立ちます。
// [JA] 失敗フラグが立ったストリームで >> をするとどうなるでしょうか? フラグは自然に消えるでしょうか?
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 실패를 "예상한" 곳에서 멈췄다면 in.clear()로 실패 표시를 지운 뒤에 다음 읽기를 하세요.
// clear는 글자를 버리지 않으므로 "보상"은 그대로 남아 header로 읽힙니다.

// [JA] 失敗を「想定した」場所で止まったのなら、in.clear() で失敗フラグを消してから次を読んでください。
// [JA] clear は文字を捨てないので、「報酬」はそのまま残って header として読まれます。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void io05_reward_table_hidden()
{
    using namespace io5;

    iokit::LabDir lab("io05_reward_table_hidden");
    iokit::writeBytes(lab / "boss.txt", "900 901 902 903 904\nREWARD\n1000 1500 2000 2500 9999\n");
    const RewardTable table = loadRewards(lab / "boss.txt");
    expect(i18n::L("숨은 보상표: 보상 개수", "隠し報酬表: 報酬の数"), static_cast<size_t>(5), table.gold.size());
    expect(i18n::L("숨은 보상표: 901", "隠し報酬表: 901"), 1500, goldFor(table, 901));
    expect(i18n::L("숨은 보상표: 904", "隠し報酬表: 904"), 9999, goldFor(table, 904));
}
#pragma endregion
