#include "../labs.h"
#include <stdexcept>
#include <string>
#include <vector>

// 던전에 들어갈 때 바닥·벽 텍스처를 만들고 지도 데이터를 해석한다. 지도가 깨져 있으면 예외로 알리고,
// 호출한 쪽은 "불러오기 실패" 메시지를 띄운 뒤 다음 던전으로 넘어간다. Texture는 살아 있는 개수를 센다.

// [JA] ダンジョンに入るとき、床・壁のテクスチャーを作ってマップデータを解析します。マップが壊れていれば例外で知らせ、
// [JA] 呼び出した側は「読み込み失敗」メッセージを出してから次のダンジョンへ進みます。Texture は生きている個数を数えます。
namespace lang7
{
struct Texture
{
    static inline int alive = 0;
    std::string name;

    explicit Texture(std::string n) : name(std::move(n)) { ++alive; }
    ~Texture() { --alive; }
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
};

struct Dungeon
{
    std::string name;
    Texture* floor;
    Texture* walls;
    int rooms;

    ~Dungeon()
    {
        delete floor;
        delete walls;
    }
};

struct DungeonFile
{
    std::string name;
    std::string layout;   // 방마다 'R', 깨진 칸은 '?'
};

int countRooms(const std::string& layout)
{
    int rooms = 0;
    for (char c : layout)
    {
        if (c == '?')
            throw std::runtime_error(i18n::L("지도 데이터가 깨졌다", "マップデータが壊れている"));
        if (c == 'R')
            ++rooms;
    }
    return rooms;
}

Dungeon* loadDungeon(const DungeonFile& file)
{
    Texture* floor = new Texture(file.name + "/floor");
    Texture* walls = new Texture(file.name + "/walls");
    const int rooms = countRooms(file.layout);
    return new Dungeon{ file.name, floor, walls, rooms };
}

// 던전 목록을 차례로 불러와 방 수의 합을 돌려준다. 실패한 던전은 건너뛴다.
int exploreAll(const std::vector<DungeonFile>& files)
{
    int totalRooms = 0;
    for (const DungeonFile& file : files)
    {
        try
        {
            Dungeon* d = loadDungeon(file);
            std::cout << "  " << d->name << i18n::L(": 방 ", ": 部屋") << d->rooms << i18n::L("개 탐험\n", "個を探検\n");
            totalRooms += d->rooms;
            delete d;
        }
        catch (const std::exception& e)
        {
            std::cout << "  " << file.name << i18n::L(": 불러오기 실패 (", ": 読み込み失敗 (") << e.what() << ")\n";
        }
    }
    return totalRooms;
}
}

void lg07_dungeon_load()
{
    using namespace lang7;
    Texture::alive = 0;

    const int rooms = exploreAll({
        { "고블린 굴", "RRR" },
        { "저주받은 성", "RR?R" },
        { "버섯 숲", "RRRRR" },
    });
    std::cout << i18n::L("  탐험이 끝난 뒤 남은 텍스처: ", "  探検後に残ったテクスチャー: ") << Texture::alive << i18n::L("개\n", "個\n");

    expect("탐험한 방 수", 8, rooms);
    expect("남은 텍스처 수", 0, Texture::alive);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// Texture의 생성자와 소멸자에 중단점을 걸고, 멈출 때마다 name을 적어 보세요. 생성은 6번입니다. 소멸은 몇 번인가요?
// 소멸자가 불리지 않은 텍스처는 어느 던전의 것인가요? 그 던전에서 countRooms가 던진 뒤, 실행은 어디로 갔나요?

// [JA] Texture のコンストラクターとデストラクターにブレークポイントを置き、止まるたびに name を書き留めてみてください。生成は6回です。破棄は何回ですか?
// [JA] デストラクターが呼ばれなかったテクスチャーはどのダンジョンのものですか? そのダンジョンで countRooms が例外を投げた後、実行はどこへ行きましたか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 예외가 던져지면 실행은 catch까지 곧바로 뛰어가고, 그 사이 함수들의 남은 줄은 실행되지 않습니다.
// 그 사이에서 소멸자가 불리는 것은 "스택에 있는 객체"뿐입니다. loadDungeon의 floor, walls는 포인터(원시 포인터)라서 포인터만 사라지고 가리키던 Texture는 남습니다.
// 정상 경로만 보면 new와 delete의 짝이 맞아 보이기 때문에, 예외 경로에서만 새는 메모리는 찾기 어렵습니다.

// [JA] 例外が投げられると、実行は catch までまっすぐ飛び、その間にある関数の残りの行は実行されません。
// [JA] その間でデストラクターが呼ばれるのは「スタックにあるオブジェクト」だけです。loadDungeon の floor と walls はポインター(生ポインター)なので、ポインターだけが消えて指していた Texture は残ります。
// [JA] 正常な経路だけを見ると new と delete の対応が合っているように見えるため、例外の経路でだけ漏れるメモリは見つけにくいです。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 자원은 만들자마자 소유 객체에 맡기세요(RAII): std::unique_ptr<Texture> floor = std::make_unique<Texture>(...);
// Dungeon도 unique_ptr 멤버로 바꾸고, loadDungeon은 std::unique_ptr<Dungeon>을 돌려주면 delete가 코드에서 모두 사라집니다.
// 그러면 어느 줄에서 예외가 나도 스택 풀기(stack unwinding)가 소멸자를 불러 줍니다.

// [JA] 資源は作ったらすぐ所有オブジェクトに任せてください(RAII): std::unique_ptr<Texture> floor = std::make_unique<Texture>(...);
// [JA] Dungeon も unique_ptr のメンバーに変え、loadDungeon が std::unique_ptr<Dungeon> を返すようにすれば、delete がコードからすべて消えます。
// [JA] そうすれば、どの行で例外が起きてもスタックの巻き戻し(stack unwinding)がデストラクターを呼んでくれます。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void lg07_dungeon_load_hidden()
{
    using namespace lang7;
    Texture::alive = 0;
    const int rooms = exploreAll({
        { "얼음 동굴", "?RR" },
        { "사막 신전", "RRRR" },
        { "늪지", "R?" },
        { "용암 지대", "RR?RRR" },
        { "하늘 섬", "RR" },
    });
    expect("숨은 탐험: 방 수", 6, rooms);
    expect("숨은 탐험: 남은 텍스처 수", 0, Texture::alive);

    const DungeonFile broken{ "폐허", "R?R" };
    for (int i = 0; i < 3; ++i)
    {
        try
        {
            (void)loadDungeon(broken);   // 깨진 지도라 항상 예외가 난다
        }
        catch (const std::exception&)
        {
        }
    }
    expect("숨은 탐험: 직접 불러오기 실패 뒤 남은 텍스처", 0, Texture::alive);
}
#pragma endregion
