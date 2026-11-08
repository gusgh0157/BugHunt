#include "../labs.h"
#include "io_kit.h"
#include <string>
#include <vector>

// 상점은 게임 폴더(실행 파일이 있는 폴더)의 data/prices.txt에서 가격표를 읽는다.
// 패치로 포션 가격이 100 → 120이 되었는데, 어떤 실행에서는 옛 가격이 나온다.

// [JA] ショップはゲームフォルダー(実行ファイルがあるフォルダー)の data/prices.txt から価格表を読み込みます。
// [JA] パッチでポーションの価格が100 → 120になったのに、起動のしかたによっては古い価格が表示されます。
namespace io1
{
namespace fs = std::filesystem;

struct Item
{
    std::string name;
    int price = 0;
};

// 실행 파일이 있는 폴더. 실제 게임에서는 시작할 때 argv[0]이나 OS API로 알아낸다.
fs::path g_gameDir;

const fs::path& gameDir()
{
    return g_gameDir;
}

std::vector<Item> loadPrices()
{
    std::vector<Item> items;
    std::ifstream in("data/prices.txt");
    Item item;
    while (in >> item.name >> item.price)
        items.push_back(item);
    return items;
}

int priceOf(const std::vector<Item>& items, const std::string& name)
{
    for (const Item& item : items)
    {
        if (item.name == name)
            return item.price;
    }
    return -1;
}

// 작업 디렉터리를 잠깐 바꿨다가 되돌린다 (버그 없음)
struct CurrentDirGuard
{
    fs::path saved = fs::current_path();
    explicit CurrentDirGuard(const fs::path& dir) { fs::current_path(dir); }
    ~CurrentDirGuard() { fs::current_path(saved); }
};
}

void io01_patch_prices()
{
    using namespace io1;

    iokit::LabDir lab("io01_patch_prices");

    // 설치된 게임: 패치된 가격표
    g_gameDir = lab / "game";
    iokit::writeBytes(g_gameDir / "data/prices.txt", i18n::L("포션 120\n해독제 80\n귀환서 300\n", "ポーション 120\n解毒剤 80\n帰還の書 300\n"));
    // 개발자가 IDE에서 실행할 때의 작업 디렉터리: 예전에 복사해 둔 가격표가 남아 있다
    const fs::path workspace = lab / "workspace";
    iokit::writeBytes(workspace / "data/prices.txt", i18n::L("포션 100\n해독제 80\n", "ポーション 100\n解毒剤 80\n"));

    // IDE, 바로 가기, 다른 프로그램이 게임을 띄우면 작업 디렉터리는 실행 파일 폴더와 다를 수 있다.
    std::vector<Item> items;
    {
        CurrentDirGuard launchedFrom(workspace);
        items = loadPrices();
    }

    std::cout << i18n::L("  읽은 품목 ", "  読み込んだ品目") << items.size() << i18n::L("개\n", "個\n");
    for (const Item& item : items)
        std::cout << "    " << item.name << " " << item.price << "G\n";

    expect("포션 가격", 120, priceOf(items, i18n::L("포션", "ポーション")));
    expect("귀환서 가격", 300, priceOf(items, i18n::L("귀환서", "帰還の書")));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// loadPrices의 std::ifstream 줄에 중단점을 거세요. 여는 파일은 정확히 어느 폴더의 파일인가요?
// 조사식에서 std::filesystem::current_path()를 계산할 수 없으면, 잠깐 이 줄을 넣어 출력해 보세요:
//     std::cout << std::filesystem::absolute("data/prices.txt") << '\n';

// [JA] loadPricesの std::ifstream の行にブレークポイントを置いてください。開いているのは正確にどのフォルダーのファイルですか?
// [JA] ウォッチで std::filesystem::current_path() を評価できない場合は、一時的に次の行を入れて出力してみてください:
// [JA]     std::cout << std::filesystem::absolute("data/prices.txt") << '\n';
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// "data/prices.txt" 같은 상대 경로는 실행 파일 위치가 아니라 "현재 작업 디렉터리" 기준입니다.
// 작업 디렉터리는 프로그램을 띄운 쪽이 정합니다. (VS의 디버깅 → 작업 디렉터리 기본값은 프로젝트 폴더)

// [JA] "data/prices.txt" のような相対パスは、実行ファイルの場所ではなく「現在の作業ディレクトリ」が基準です。
// [JA] 作業ディレクトリはプログラムを起動した側が決めます。(VSの デバッグ → 作業ディレクトリ の既定値はプロジェクト フォルダー)
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 게임 데이터처럼 "실행 파일 옆에 있어야 하는" 파일은 gameDir() 같은 기준 폴더를 앞에 붙여 절대 경로로 여세요.
// 파일을 못 열었을 때 조용히 빈 목록을 돌려주지 않는지도 확인해 보세요.

// [JA] ゲームデータのように「実行ファイルの隣にあるべき」ファイルは、gameDir() のような基準フォルダーを前に付けて絶対パスで開いてください。
// [JA] ファイルを開けなかったときに、黙って空のリストを返していないかも確認してください。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void io01_patch_prices_hidden()
{
    using namespace io1;

    iokit::LabDir lab("io01_patch_prices_hidden");
    g_gameDir = lab / "Program Files" / "Dragon Inn";
    iokit::writeBytes(g_gameDir / "data/prices.txt", i18n::L("횃불 15\n밧줄 40\n포션 135\n", "たいまつ 15\nロープ 40\nポーション 135\n"));
    const fs::path desktop = lab / "desktop";       // 데이터 폴더가 아예 없는 곳에서 실행
    fs::create_directories(desktop);

    std::vector<Item> items;
    {
        CurrentDirGuard launchedFrom(desktop);
        items = loadPrices();
    }
    expect("숨은 가격표: 품목 수", static_cast<size_t>(3), items.size());
    expect("숨은 가격표: 밧줄", 40, priceOf(items, i18n::L("밧줄", "ロープ")));
    expect("숨은 가격표: 포션", 135, priceOf(items, i18n::L("포션", "ポーション")));
}
#pragma endregion
