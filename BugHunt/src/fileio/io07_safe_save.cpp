#include "../labs.h"
#include "io_kit.h"
#include <stdexcept>
#include <string>
#include <vector>

// 인벤토리를 save.txt에 저장한다. 저장하다가 문제가 생기면 저장을 포기하고 "저장 실패"를 띄운다.
// 저장에 실패해도 지난번 세이브는 멀쩡히 남아 있어야 한다.

// [JA] インベントリを save.txt に保存します。保存中に問題が起きたら保存をあきらめて「保存失敗」を表示します。
// [JA] 保存に失敗しても、前回のセーブは無傷で残っていなければなりません。
namespace io7
{
namespace fs = std::filesystem;

struct Item
{
    std::string name;
    int count = 0;
};

void writeItem(std::ostream& out, const Item& item)
{
    if (item.count < 0)
        throw std::runtime_error("개수가 음수인 아이템: " + item.name);
    out << item.name << ' ' << item.count << '\n';
}

bool saveInventory(const fs::path& file, const std::vector<Item>& items)
{
    try
    {
        std::ofstream out(file, std::ios::trunc);
        for (const Item& item : items)
            writeItem(out, item);
        return static_cast<bool>(out);
    }
    catch (const std::exception& e)
    {
        std::cout << "  저장 실패: " << e.what() << '\n';
        return false;
    }
}

std::vector<Item> loadInventory(const fs::path& file)
{
    std::vector<Item> items;
    std::ifstream in(file);
    Item item;
    while (in >> item.name >> item.count)
        items.push_back(item);
    return items;
}
}

void io07_safe_save()
{
    using namespace io7;

    iokit::LabDir lab("io07_safe_save");
    const fs::path file = lab / "save.txt";

    std::vector<Item> bag = { { "포션", 5 }, { "해독제", 2 }, { "귀환서", 1 }, { "화살", 40 }, { "금화주머니", 3 } };
    expect("첫 저장 성공", true, saveInventory(file, bag));

    bag[3].count = -1;                                   // 버그로 개수가 깨진 아이템
    expect("깨진 아이템이 있으면 저장 실패", false, saveInventory(file, bag));

    const std::vector<Item> loaded = loadInventory(file);
    std::cout << "  불러온 아이템 " << loaded.size() << "종\n";
    expect("불러온 세이브의 아이템 수", static_cast<size_t>(5), loaded.size());
    expect("마지막 아이템", std::string("금화주머니"), loaded.empty() ? std::string() : loaded.back().name);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 두 번째 saveInventory의 throw 줄에 중단점을 거세요. 멈춘 순간 save.txt를 다른 편집기로 열어 보세요.
// 지난번 세이브(아이템 5종)는 아직 파일에 있나요?

// [JA] 2回目の saveInventory で throw の行にブレークポイントを置いてください。止まった瞬間に save.txt を別のエディターで開いてみてください。
// [JA] 前回のセーブ(アイテム5種)はまだファイルにありますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// std::ofstream을 trunc로 여는 순간 이전 내용은 지워집니다. 예외가 나서 함수를 빠져나갈 때
// ofstream 소멸자는 그때까지 쓴 앞부분을 파일에 내려 놓고 닫습니다. 반쯤 쓴 세이브가 남습니다.

// [JA] std::ofstream を trunc で開いた瞬間に以前の内容は消えます。例外で関数を抜けるとき、
// [JA] ofstream のデストラクターはそれまでに書いた前半部分をファイルに書き出して閉じます。半分だけ書かれたセーブが残ります。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 새 내용은 같은 폴더의 임시 파일(예: save.txt.tmp)에 쓰고, 끝까지 성공해서 닫은 뒤에만
// std::filesystem::rename(임시, 진짜)로 바꿔치기하세요. 실패하면 임시 파일을 지우세요.
// (rename 전에 ofstream을 닫아야 합니다. 열린 파일은 OS에 따라 이름을 바꿀 수 없습니다.)

// [JA] 新しい内容は同じフォルダーの一時ファイル(例: save.txt.tmp)に書き、最後まで成功して閉じた後でだけ
// [JA] std::filesystem::rename(一時, 本物) で置き換えてください。失敗したら一時ファイルを削除してください。
// [JA] (rename の前に ofstream を閉じる必要があります。開いているファイルはOSによっては名前を変更できません。)
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void io07_safe_save_hidden()
{
    using namespace io7;

    iokit::LabDir lab("io07_safe_save_hidden");
    const fs::path file = lab / "slot2.sav";

    expect("숨은: 첫 저장", true, saveInventory(file, { { "나무검", 1 }, { "빵", 7 } }));
    expect("숨은: 깨진 첫 아이템", false, saveInventory(file, { { "유령", -3 }, { "빵", 9 } }));
    std::vector<Item> loaded = loadInventory(file);
    expect("숨은: 실패 뒤 아이템 수", static_cast<size_t>(2), loaded.size());
    expect("숨은: 실패 뒤 빵 개수", 7, loaded.size() == 2 ? loaded[1].count : -1);

    expect("숨은: 정상 덮어쓰기", true, saveInventory(file, { { "강철검", 1 }, { "빵", 2 }, { "열쇠", 1 } }));
    loaded = loadInventory(file);
    expect("숨은: 덮어쓴 아이템 수", static_cast<size_t>(3), loaded.size());

    size_t filesInFolder = 0;                            // 남은 임시 파일이 없어야 한다
    for ([[maybe_unused]] const auto& entry : fs::directory_iterator(lab.path()))
        ++filesInFolder;
    expect("숨은: 폴더에 남은 파일 수", static_cast<size_t>(1), filesInFolder);
}
#pragma endregion
