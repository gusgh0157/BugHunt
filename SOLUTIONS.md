# 해답 (스포일러!)

1~12부의 해답입니다. 챌린지(C, B)의 해답은 `CHALLENGE_SOLUTIONS.md`에 따로 있습니다.

# 1부: 호출 스택 입문

## Lab 1 — nullptr 역참조
```
멈춘 함수:  printPlayer           이상한 값: player == nullptr
값의 출처:  showProfile ← findPlayer가 못 찾아서 nullptr 반환
진짜 원인:  openFriendList에서 "Doran"을 "Dorna"로 오타
```
**수정:** 오타를 고친다. `cs01_profile.cpp`의 `openFriendList` 함수 전체를 다음으로 바꾼다.
```cpp
void openFriendList()
{
    showProfile("Kael");
    showProfile("Mira");
    showProfile("Doran");
}
```
방어도 추가한다. `findPlayer`가 `nullptr`를 돌려주면 출력하지 않고 알리도록, 같은 파일의 `showProfile` 함수 전체를 다음으로 바꾼다.
```cpp
void showProfile(const std::string& name)
{
    Player* player = findPlayer(name);
    if (!player)
    {
        std::cout << "  " << name << ": " << i18n::L("없는 플레이어", "存在しないプレイヤー") << '\n';
        return;
    }
    printPlayer(player);
}
```
**교훈:** 죽은 곳(printPlayer)은 아무 잘못이 없다. nullptr를 **돌려받고 검사하지 않은** 곳과 **잘못된 이름을 넘긴** 곳이 원인.

## Lab 2 — 1부터 세는 키를 0부터 세는 인덱스로 그대로 사용
```
멈춘 함수:  [외부 코드] std::out_of_range 던지기 ← vector::at → 첫 내 코드: QuickSlotBar::itemAt(slotIndex = 4)
값의 출처:  onNumberKey(key = 4) ← replayInput
진짜 원인:  키 번호(1~4)를 슬롯 인덱스(0~3)로 바꾸지 않음
```
**수정:** 키 번호에서 1을 빼서 슬롯 인덱스로 만든다. `cs02_quickslot.cpp`의 `onNumberKey` 함수 전체를 다음으로 바꾼다.
```cpp
void onNumberKey(const QuickSlotBar& bar, int key)
{
    useItem(bar.itemAt(key - 1));
}
```
**교훈:** 외부 코드는 건너뛰고 내 코드 첫 줄부터. 그리고 크래시 전에 나온 "틀린 출력"도 같은 버그의 증상이다.

## Lab 3 — 정수 나눗셈으로 0초가 됨
```
멈춘 함수:  damagePerSecond(seconds = 0)이 std::invalid_argument를 던짐 (예외가 던져질 때 멈춤을 켜면 throw 줄에서 멈춘다)
값의 출처:  printBattleSummary: battle = {"Slime", 35, 800}, toSeconds(800) == 0
진짜 원인:  1초 미만 전투가 정수 나눗셈으로 0초가 됨
```
**수정:** 초가 아니라 밀리초로 계산한다. 바뀌는 시그니처는 `damagePerSecond`의 둘째 매개변수(`seconds` → `durationMs`)이고, 호출하는 `printBattleSummary`도 `battle.durationMs`를 넘기도록 고친다. `toSeconds`는 화면에 초를 보여 주는 데만 남긴다.
`cs03_battle_log.cpp`의 `damagePerSecond` 함수 전체를 다음으로 바꾼다.
```cpp
int damagePerSecond(int totalDamage, int durationMs)
{
    if (durationMs <= 0)
        throw std::invalid_argument(i18n::L("damagePerSecond: 전투 시간이 0ms 이하", "damagePerSecond: 戦闘時間が0ms以下"));
    return totalDamage * 1000 / durationMs;
}
```
같은 파일의 `printBattleSummary` 함수 전체를 다음으로 바꾼다.
```cpp
void printBattleSummary(const BattleRecord& battle)
{
    const int seconds = toSeconds(battle.durationMs);                          // 화면 표시용
    const int dps = damagePerSecond(battle.totalDamage, battle.durationMs);   // 계산은 밀리초로
    std::cout << "  " << battle.enemy << ": " << seconds << i18n::L("초, DPS ", "秒、DPS ") << dps << '\n';
}
```
(또는 `static_cast<double>(totalDamage) / (durationMs / 1000.0)`처럼 `double`로 계산한다.)
**교훈:** 값의 출처를 여러 프레임 거슬러 올라가야 한다. 0으로 나눈 함수는 받은 값을 썼을 뿐이다.

## Lab 4 — 검사가 한 경로에만 있음
```
멈춘 함수:  leaderOf ← sendRaidReminders  (printPartyBoard가 아님!)
이상한 값:  party.name == "Silent Hollow", members 비어 있음
```
**수정:** `printPartyBoard`에 있는 빈 파티 검사를 `sendRaidReminders`에도 넣는다. `cs04_party_board.cpp`의 `sendRaidReminders` 함수 전체를 다음으로 바꾼다.
```cpp
void sendRaidReminders(const std::vector<Party>& parties)
{
    std::cout << i18n::L("  [레이드 알림]\n", "  [レイド通知]\n");
    for (const Party& party : parties)
    {
        if (party.members.empty())
            continue;   // 파티장이 없는 파티에는 보내지 않는다
        std::cout << "    " << leaderOf(party) << i18n::L("님, 20시 레이드를 잊지 마세요!\n", "さん、20時のレイドを忘れないでください!\n");
    }
}
```
더 좋은 방법은 `leaderOf`가 빈 파티를 안전하게 처리하도록 하는 것이다 (`const std::string*` 반환, `std::optional` 등).
**교훈:** 같은 함수라도 **어느 경로로 불렸는지**는 호출 스택만이 정확히 알려 준다.

## Lab 5 — 종료 조건을 건너뛰는 재귀
```
멈춘 함수:  countdown(n = -수만) 이 수만 번 반복 → 스택 오버플로
값의 출처:  n이 홀수 → startFireworks(seconds = 5)
진짜 원인:  5, 3, 1, -1, ... 로 0을 건너뛰어 끝나지 않음
```
**수정:** 종료 조건을 `n == 0`에서 `n <= 0`으로 넓혀서 0을 건너뛰어도 멈추게 한다. `cs05_fireworks.cpp`의 `countdown` 함수 전체를 다음으로 바꾼다.
```cpp
std::string countdown(int n)
{
    if (n <= 0)
        return i18n::L("발사!", "発射!");
    return std::to_string(n) + " " + countdown(n - 2);
}
```
**교훈:** 스택이 같은 함수로 가득 차면 재귀가 끝나지 않는 것. 매개변수 값의 변화를 보면 이유가 보인다.

## Lab 6 — 없는 키로 map::at
```
멈춘 함수:  [외부 코드] std::out_of_range 던지기 ← std::map::at → 첫 내 코드: priceOf(itemId = "mana_poiton")
값의 출처:  cartTotal ← checkout의 장바구니 목록
진짜 원인:  "mana_potion" 오타
```
**수정:** 오타를 고친다. `cs06_shop.cpp`의 `checkout` 함수 전체를 다음으로 바꾼다.
```cpp
void checkout()
{
    const std::vector<std::string> cart = {
        "iron_sword",
        "healing_potion",
        "healing_potion",
        "mana_potion",
        "oak_shield",
    };
    std::cout << i18n::L("  합계: ", "  合計: ") << cartTotal(cart) << i18n::L(" 골드\n", " ゴールド\n");
}
```
방어도 추가한다. 가격표에 없는 ID면 경고를 내고 0골드로 치도록, 같은 파일의 `priceOf` 함수 전체를 다음으로 바꾼다.
```cpp
int priceOf(const std::string& itemId)
{
    const auto it = kPrices.find(itemId);
    if (it == kPrices.end())
    {
        std::cout << i18n::L("  경고: 가격표에 없는 아이템 ", "  警告: 価格表にないアイテム ") << itemId << '\n';
        return 0;
    }
    return it->second;
}
```
**교훈:** 예외도 호출 스택으로 추적한다. throw한 곳 → 처음 나오는 내 코드.

## Lab 7 — 너무 일찍 캡처한 포인터
```
멈춘 함수:  HealthBar::setValue (this == nullptr)
아래 프레임: connectHealthBar의 람다 — 캡처된 bar == nullptr
          ← std::function (외부 코드) ← EventBus::publishHpChanged ← startGame
진짜 원인:  startGame에서 connectHealthBar(game)를 createUi(game)보다 먼저 호출.
           그 순간 game.healthBar.get()은 nullptr였고, 람다는 그 값을 복사해 저장했다.
```
**수정:** `createUi(game);`를 `connectHealthBar(game);` 위로 옮긴다. `cs07_events.cpp`의 `startGame` 함수 전체를 다음으로 바꾼다.
```cpp
void startGame()
{
    Game game;
    connectLogger(game);
    createUi(game);          // HP 바를 먼저 만들고 나서
    connectHealthBar(game);  // 그 포인터를 캡처한다

    // 플레이어가 피해를 입었다.
    game.events.publishHpChanged(75);
}
```
또는 람다가 `&game`을 캡처해서 호출 시점에 `game.healthBar`를 확인하게 한다.
**교훈:** 호출 스택은 **지금 어떻게 여기 왔는지**만 보여 준다. "그 값이 **언제** 저장됐는지"는 중단점을 걸고 다시 실행해서 찾는다.

## Lab 8 — 인자 순서가 뒤바뀜
```
멈춘 함수:  [외부 코드] std::out_of_range 던지기 ← std::array::at → statsFor(level = 200)  (테이블은 0~50)
값의 출처:  createMonster(level = 200) ← spawnMonster(name = "Ogre Boss", level = 200, bonusHp = 13)
          ← spawnWave(wave = 3)
진짜 원인:  spawnMonster("Ogre Boss", 200, 10 + wave)
           → 의도는 레벨 10 + wave, 보너스 HP 200
```
**수정:** 보스 호출에서 레벨과 보너스 HP 인자를 올바른 순서로 되돌린다. `cs08_waves.cpp`의 `spawnWave` 함수 전체를 다음으로 바꾼다.
```cpp
void spawnWave(int wave, std::vector<Monster>& field)
{
    field.push_back(spawnMonster("Goblin", 3 + wave, 0));
    field.push_back(spawnMonster("Orc", 5 + wave, 20));

    if (wave % 3 == 0)
        field.push_back(spawnMonster("Ogre Boss", 10 + wave, 200));
}
```
**교훈:** 같은 타입(int, int) 매개변수가 나란히 있으면 순서를 바꿔 넣기 쉽다. 강한 타입(`struct Level { int value; };`)이나 지정 초기화로 예방할 수 있다.

---

# 2부: 심화

## Lab 9 — off-by-one
`i <= scores.size()` → 마지막 반복에서 `scores.at(5)` 범위 밖 접근 → `std::out_of_range`.
**수정:** 반복 조건을 `i < scores.size()`로 한다. `lab01_average.cpp`의 `average` 함수 전체를 다음으로 바꾼다.
```cpp
double average(const std::vector<int>& scores)
{
    int sum = 0;
    for (size_t i = 0; i < scores.size(); ++i)
        sum += scores.at(i);
    return static_cast<double>(sum) / scores.size();
}
```
(범위 기반 for나 `std::accumulate`로 쓰면 인덱스 경계를 틀릴 여지 자체가 없어진다.)
**교훈:** 예외(또는 어설션)에서 멈춤 → 호출 스택 → 첫 내 코드는 가장 기본적인 추적 루트. `[]` 대신 `at()`을 쓰면 어느 컴파일러, 어느 구성에서나 범위 밖 접근이 예외로 드러난다.

## Lab 10 — 참조 무효화 (vector 재할당)
`add`가 `items_.back()`의 참조를 돌려주는데, 이후 `push_back`에서 capacity가 늘면 원소들이 새 메모리로 옮겨지고 `sword`는 해제된 메모리를 가리킨다(use-after-free).
**수정:** `add`가 참조 대신 인덱스를 돌려주고, 쓰는 쪽은 그 인덱스로 매번 다시 찾는다. 호출하는 `lab02_inventory`도 함께 바뀐다.
`lab02_inventory.cpp`의 `Inventory` 클래스 전체(`class Inventory`부터 `};`까지)를 다음으로 바꾼다.
```cpp
class Inventory
{
public:
    // 아이템을 추가하고, 나중에 다시 찾을 수 있도록 인덱스를 돌려준다. (참조는 push_back 뒤에 무효가 될 수 있다)
    std::size_t add(const std::string& name, int count)
    {
        items_.push_back({ name, count });
        return items_.size() - 1;
    }

    Item& at(std::size_t index) { return items_[index]; }

    int countOf(const std::string& name) const
    {
        for (const Item& item : items_)
            if (item.name == name)
                return item.count;
        return 0;
    }

private:
    std::vector<Item> items_;
};
```
같은 파일의 `lab02_inventory` 함수 전체를 다음으로 바꾼다.
```cpp
void lab02_inventory()
{
    Inventory inv;
    const std::size_t sword = inv.add("Sword of the Northern Watch", 1);
    inv.add("Oak Shield", 1);
    inv.add("Healing Potion", 5);
    inv.add("Iron Arrow", 30);

    // 퀘스트 보상: 검을 하나 더 준다.
    inv.at(sword).count += 1;

    expect(i18n::L("검 개수", "剣の数"), 2, inv.countOf("Sword of the Northern Watch"));
    expect(i18n::L("포션 개수", "ポーションの数"), 5, inv.countOf("Healing Potion"));
}
```
다른 길: `lab02_inventory` 첫 줄에서 `inv.reserve(4)`를 불러 재할당을 막거나(`Inventory`에 `void reserve(std::size_t n) { items_.reserve(n); }` 추가), `items_`를 `std::deque<Item>`처럼 `push_back`해도 원소 주소가 안 바뀌는 컨테이너로 바꾼다.
**교훈:** "참조/포인터/이터레이터를 들고 있는 동안 컨테이너를 바꾸지 말 것."

## Lab 11 — 임시 객체를 가리키는 string_view
`makeTitle`이 만든 `std::string`은 `titleOf`의 return 문이 끝나면 파괴되고, `string_view`는 그 해제된 버퍼를 가리킨다.
**수정:** `titleOf`가 `std::string`을 돌려주고, 호출하는 `lab03_titles`의 `titles`도 `std::vector<std::string>`으로 바꾼다. `string_view`는 소유자가 더 오래 살아 있을 때만 쓴다.
`lab03_titles.cpp`에서 더 이상 쓰지 않는 `#include <string_view>` 줄을 지운다. `titleOf` 위의 주석 두 줄(한국어 `// 호출할 때마다 문자열을 복사하지 않도록 string_view로 돌려준다. (최적화)`와 그 아래 `// [JA] 呼び出すたびに文字列をコピーしないよう、string_view で返します。(最適化)`)도 더 이상 맞지 않으므로 사이의 빈 줄과 함께 지운다.
그다음 `titleOf` 함수 전체를 다음으로 바꾼다.
```cpp
std::string titleOf(const Player& p)
{
    return makeTitle(p);
}
```
같은 파일의 `lab03_titles` 함수 전체를 다음으로 바꾼다.
```cpp
void lab03_titles()
{
    const std::vector<Player> players = {
        { "Kael the Wanderer", "Northwind Rangers" },
        { "Mira Stormcaller",  "Order of the Ember" },
        { "Doran Ironhand",    "Deepforge Clan" },
    };

    std::vector<std::string> titles;
    for (const Player& p : players)
        titles.push_back(titleOf(p));

    expect(i18n::L("첫 번째 칭호", "1番目の称号"), std::string("[Northwind Rangers] Kael the Wanderer"), std::string(titles[0]));
    expect(i18n::L("세 번째 칭호", "3番目の称号"), std::string("[Deepforge Clan] Doran Ironhand"), std::string(titles[2]));
}
```
`expect` 두 줄의 `std::string(titles[0])`은 `std::string`을 복사할 뿐이라 원래 그대로다.

## Lab 12 — 초기화 안 된 멤버
`int combo_;`에 초기값이 없다. 카운터는 오브젝트 풀(`bughunt::recycledStorage`)의 재사용 메모리 위에 만들어지는데, 이 자리에는 전에 쓰던 값(BugHunt에서는 `0xA5A5A5A5` = -1515870811)이 남아 있어서 처음 4연속 명중이 음수로 기록되어 무시된다. 평범한 지역 변수였다면 컴파일러와 구성에 따라 우연히 0이라 맞게 나올 수도 있다.
**수정:** `combo_`에 초기값 `0`을 준다(`int combo_ = 0;`). `lab04_combo.cpp`의 `ComboCounter` 클래스 전체(`class ComboCounter`부터 `};`까지)를 다음으로 바꾼다.
```cpp
class ComboCounter
{
public:
    void hit()
    {
        ++combo_;
        if (combo_ > best_)
            best_ = combo_;
    }

    void miss() { combo_ = 0; }

    int best() const { return best_; }

private:
    int combo_ = 0;
    int best_ = 0;
};
```
**교훈:** 생성자가 모든 멤버를 초기화하는지 확인. 값이 실행마다, 구성마다 다르면 미초기화 변수를 1순위로 의심. (컴파일러의 "멤버를 초기화하지 않음" 정적 분석 경고. 예: MSVC 코드 분석 C26495, clang-tidy `cppcoreguidelines-pro-type-member-init`)

## Lab 13 — erase 후 이터레이터 무효화
`erase(it)` 후 `it`는 무효인데 `++it`를 한다.
**수정:** `erase`가 돌려주는 "지운 원소 다음"을 가리키는 새 이터레이터를 받고, 지우지 않았을 때만 `++it`를 한다. `lab05_enemies.cpp`의 `removeDead` 함수 전체를 다음으로 바꾼다.
```cpp
void removeDead(bughunt::CheckedList<Enemy>& enemies)
{
    for (auto it = enemies.begin(); it != enemies.end(); )
    {
        if (it->hp <= 0)
            it = enemies.erase(it);   // erase가 돌려준 새 이터레이터를 쓴다
        else
            ++it;
    }
}
```
(목록이 `std::vector`라면 C++20 `std::erase_if(enemies, [](const Enemy& e) { return e.hp <= 0; });`로 한 줄에 된다. 이 랩의 `bughunt::CheckedList`에는 `std::erase_if`가 없으므로 위의 반복문을 쓴다.)

## Lab 14 — unsigned 언더플로
`20u - 22u` = 4294967294. "남은 칸"이 엄청 커서 무엇이든 들어간다.
**수정:** 빼기 전에 사용 중인 칸이 최대치 이상인지 먼저 검사한다. `lab06_backpack.cpp`의 `canPickUp` 함수 전체를 다음으로 바꾼다.
```cpp
bool canPickUp(const Backpack& bag, unsigned itemSize)
{
    if (bag.usedSlots >= bag.maxSlots)
        return false;   // 빈 칸이 없다 (저주로 최대치를 넘었을 수도 있다)
    const unsigned freeSlots = bag.maxSlots - bag.usedSlots;
    return freeSlots >= itemSize;
}
```
(또는 부호 있는 타입으로 계산한다.)

## Lab 15 — Rule of Three 위반 (이중 해제)
`PacketBuffer`는 raw 포인터를 소유하지만 복사 생성자/대입이 기본(얕은 복사)이다. 소멸자를 직접 선언했으므로 이동 생성자는 자동 생성되지 않아 `push_back(PacketBuffer(64))`에서 **복사**가 일어남 → 임시 객체 소멸 시 `data_` 해제 → 벡터 안 객체는 해제된 버퍼를 가리킨다 → **두 번째 `push_back`**에서 vector가 재할당하며 옛 원소를 파괴할 때 같은 버퍼를 또 해제(double free). 그 자리에서 메모리 검사기가 `[CRASH] 메모리 검사: 이미 해제한 메모리를 다시 해제했다`로 멈추므로 `expect` 줄까지 가지 못한다.
**수정(권장, Rule of Zero):** 버퍼를 `std::vector<char>`에 맡기고 소멸자를 지운다. 복사도 이동도 `std::vector`가 올바르게 하므로 특수 멤버 함수를 하나도 쓸 필요가 없다. `lab07_packets.cpp`의 `PacketBuffer` 클래스 전체(`class PacketBuffer`부터 `};`까지)를 다음으로 바꾼다.
```cpp
class PacketBuffer
{
public:
    explicit PacketBuffer(size_t size)
        : data_(size, '\0')   // 0으로 채운 size 바이트
    {
    }

    void write(const char* text) { std::snprintf(data_.data(), data_.size(), "%s", text); }
    const char* text() const { return data_.data(); }

private:
    std::vector<char> data_;   // 크기도 data_.size()가 기억한다
};
```
`std::memset`을 쓰지 않게 되므로 `#include <cstring>`은 필요 없어지지만 남겨 두어도 된다. (`std::unique_ptr<char[]>`로도 이중 해제는 없어지지만, 복사는 안 되고 이동만 된다. 또는 복사 생성자/대입/이동을 직접 구현한다.)

## Lab 16 — 가상 소멸자 누락 (메모리 누수 + UB)
`StatusEffect`의 소멸자가 virtual이 아니라 `unique_ptr<StatusEffect>`가 `delete`할 때 `Burn`의 소멸자가 불리지 않는다 → 멤버 `schedule_`(`DamageSchedule`)도 파괴되지 않아 전투 1000번 뒤 1000개가 살아 남는다(그 안의 벡터 메모리도 누수). 엄밀히는 정의되지 않은 동작이다.
**추적:** `DamageSchedule`의 소멸자에 중단점을 걸면 한 번도 멈추지 않는다. 생성자에는 멈춘다. → 누가 지워야 했나? `unique_ptr`의 `delete`가 `StatusEffect`의 소멸자만 부른다.
**수정:** 기반 클래스의 소멸자를 `virtual ~StatusEffect() = default;`로 한다. `lab08_effects.cpp`의 `StatusEffect` 클래스 전체(`class StatusEffect`부터 `};`까지)를 다음으로 바꾼다.
```cpp
class StatusEffect
{
public:
    virtual ~StatusEffect() = default;
    virtual void tick(int& hp) = 0;
};
```
**교훈:** 다형적으로 delete될 기반 클래스는 반드시 virtual 소멸자. (경고: MSVC C5205(표준 헤더 안 템플릿이라 `/external:templates-`가 있어야 보인다. 이 저장소의 CMake는 켜 둔다), Clang `-Wall`의 `-Wdelete-abstract-non-virtual-dtor`. GCC는 시스템 헤더 안이라 보이지 않을 수 있다)

## Lab 17 — 데이터 레이스
`gold_` 읽기-수정-쓰기가 원자적이지 않다. 두 스레드가 같은 값을 읽고 각각 +1을 쓰면 하나가 사라진다. 읽기와 쓰기 사이의 `std::this_thread::sleep_for(1ms)`는 그 틈을 넓혀 어느 컴파일러에서나 증상이 드러나게 할 뿐, 원인이 아니다(지워도 레이스는 남는다).
**수정:** `gold_`를 `std::atomic<int>`로 바꾸고 더하기를 원자 연산 하나로 한다. 읽기와 쓰기를 벌려 놓던 `current`와 `sleep_for` 줄은 지운다(틈을 벌리는 것이 역할이었으므로 남길 이유가 없다).
`lab09_guild_bank.cpp`의 파일 맨 위 `#include`에 다음을 추가한다.
```cpp
#include <atomic>
```
같은 파일의 `GuildBank` 클래스 전체(`class GuildBank`부터 `};`까지)를 다음으로 바꾼다.
```cpp
class GuildBank
{
public:
    void deposit(int amount)
    {
        gold_ += amount;   // 읽기-수정-쓰기가 원자 연산 하나로 끝난다
    }

    int gold() const { return gold_; }

private:
    std::atomic<int> gold_{ 0 };
};
```
`std::mutex`로 보호해도 된다: `std::lock_guard<std::mutex> lock(mutex_);`를 `deposit` 첫 줄에 두고 `gold_ += amount;`만 남긴다(`std::mutex mutex_;` 멤버, `#include <mutex>`). 이때도 `sleep_for`는 지운다. 잠금 안에서 자면 입금 200번이 잠금을 하나씩 기다려 랩이 눈에 띄게 느려진다. (`#include <chrono>`는 쓰이지 않게 되지만 남겨 두어도 된다.)

## Lab 18 — 구조체 내부 버퍼 오버플로
`char name[8]`에 `"Sir Kael"`(8글자 + NUL = 9바이트)을 `memcpy` → 바로 뒤의 `hp` 첫 바이트를 0으로 덮음 → 100(0x64)이 0이 됨. `"Sir Doran"`은 10바이트라 `hp` 하위 2바이트를 `'n'`, `0`으로 덮어 120 → 110.
**수정:** 길이를 제한해서 복사한다. `std::snprintf`는 배열 크기(NUL 포함 8바이트)를 넘어 쓰지 않으므로 긴 이름은 잘리고 `hp`는 건드리지 않는다. `lab10_party.cpp`의 `setName` 함수 전체를 다음으로 바꾼다.
```cpp
void setName(Character& c, const char* newName)
{
    std::snprintf(c.name, sizeof(c.name), "%s", newName);   // NUL 포함 sizeof(c.name) 바이트까지
}
```
(이름이 잘리는 게 싫다면 `name`을 `std::string`으로 바꾼다. 이때는 `makeCharacter`와 `grantKnighthood`의 `char` 배열 처리도 함께 바뀐다.)
**교훈:** "누가 이 값을 바꿨지?"는 데이터 중단점으로 바로 답이 나온다.

## Lab 19 — 순환 데이터로 인한 무한 재귀 (스택 오버플로)
테이블에 `village_intro: forge_sword` 오타가 있어 `forge_sword → meet_blacksmith → village_intro → forge_sword ...` 순환.
**수정:** 데이터를 `village_intro:`(선행 퀘스트 없음)로 고친다. `lab11_quests.cpp`의 `kQuestTable` 정의를 `const char* kQuestTable = R"(` 줄부터 `)";` 줄까지 통째로 다음으로 바꾼다.
```cpp
const char* kQuestTable = R"(
dragon_hunt: forge_sword find_map
forge_sword: meet_blacksmith
meet_blacksmith: village_intro
find_map: visit_cartographer
visit_cartographer:
village_intro:
)";
```
(선택: 코드로도 방어하려면 `countAllPrereqs`에 방문 중인 집합(`std::set<std::string>`)을 넘겨서, 이미 따라가는 중인 퀘스트로 돌아오면 순환으로 보고하는 방법이 있다. 이 랩의 수정은 데이터 수정만으로 충분하다.)
**교훈:** 호출 스택이 같은 패턴으로 반복되면 재귀 종료 조건/데이터 순환을 의심.

## Lab 20 — 연산자 우선순위
`==`가 `&`보다 먼저 계산되어 `flags & (status == status)` = `flags & 1` → 항상 STUN 비트만 검사.
**수정:** `&`를 괄호로 묶어 먼저 계산하게 한다. `lab12_status_flags.cpp`의 `hasStatus` 함수 전체를 다음으로 바꾼다.
```cpp
bool hasStatus(unsigned flags, unsigned status)
{
    return (flags & status) == status;
}
```
**교훈:** 컴파일러 경고(MSVC C4554, GCC·Clang `-Wparentheses`)가 이미 알려 주고 있었다. 경고를 0개로 유지하자.

---

# 3부: 흐름 추적 (크래시 없음)

## Lab 21 — 반복문 안에서 초기화되는 누적 변수
```
틀린 결과:     최장 연속 1, 골드 900 (보너스 없음)
처음 어긋난 줄: 둘째 바퀴의 int streak = 0;  (1이었던 값이 0으로 돌아감)
진짜 원인:     streak가 for 안에 선언되어 매 바퀴 새로 만들어짐
```
**수정:** `int streak = 0;`을 `for` 위로 옮긴다. (결석 처리 `streak = 0;`은 그대로 둔다.) `ft01_attendance.cpp`의 `calculateRewards` 함수 전체를 다음으로 바꾼다.
```cpp
RewardResult calculateRewards(const std::vector<bool>& loginLog)
{
    RewardResult result;
    bool bonusGiven = false;
    int streak = 0;   // 반복문 밖에 선언해서 날을 넘어 계속 센다

    for (bool loggedIn : loginLog)
    {
        if (loggedIn)
        {
            result.gold += kDailyGold;
            ++streak;
            if (streak > result.bestStreak)
                result.bestStreak = streak;
            if (streak == kStreakGoal && !bonusGiven)
            {
                result.gold += kStreakBonus;
                bonusGiven = true;
            }
        }
        else
        {
            streak = 0;
        }
    }
    return result;
}
```
**교훈:** 크래시가 없으면 호출 스택은 쓸모없다. **기대값을 먼저 적고**, 한 줄씩 실행하며 처음 어긋나는 줄을 찾는다.

## Lab 22 — 계산한 값이 아닌 원본을 반환
```
단계별 값:     기대 70 → 105 → 84 → 126 / 실제 70 → 70 → 56 → 84
처음 어긋난 곳: applyCrit 반환 (함수에서 나온 직후 디버거가 보여 주는 반환값: "applyCrit 반환 70")
진짜 원인:     result에 원본을 복사해 두고 damage만 바꾼 뒤 result를 반환
```
**수정:** `applyCrit`가 계산한 `damage`를 돌려준다. `result` 변수는 쓸모가 없어지므로 지운다(남기면 "초기화됐지만 쓰이지 않는 변수" 경고가 난다). `ft02_critical.cpp`의 `applyCrit` 함수 전체를 다음으로 바꾼다.
```cpp
int applyCrit(int damage, bool isCrit, int critBonusPercent)
{
    if (isCrit)
        damage = damage * (100 + critBonusPercent) / 100;
    return damage;
}
```
(또는 `return isCrit ? damage * (100 + critBonusPercent) / 100 : damage;` 한 줄.)
**교훈:** 파이프라인은 단계별 기대값을 정해 두고 한 줄씩 넘기면 범인 함수가 바로 나온다. 함수 안으로는 step into, 나와서는 **디버거가 보여 주는 반환값**(Visual Studio 자동 창, gdb `finish`).

## Lab 23 — 보호막이 깨질 때 남은 피해가 사라짐
```
틀린 결과:     받은 피해 160, 흡수 70 + HP 감소 87 = 157 (3이 사라짐)
멈춘 위치:     조건부 중단점 (before.shield - tank.shield) + (before.hp - tank.hp) != damage
처음 어긋난 곳: 보호막이 damage보다 적게 남은 틱 (예: 보호막 6에 피해 7)
진짜 원인:     takeDamage가 보호막을 0으로 깎고 바로 return → 넘친 피해가 HP로 안 감
```
**수정:** 보호막이 흡수할 수 있는 만큼만 흡수하고, 나머지를 HP에서 뺀다. `ft03_shield.cpp`의 `takeDamage` 함수 전체를 다음으로 바꾼다(`std::min`에 필요한 `<algorithm>`은 이 파일에 이미 있다).
```cpp
void takeDamage(Unit& unit, int damage)
{
    const int absorbed = std::min(unit.shield, damage);   // 보호막이 막을 수 있는 양
    unit.shield -= absorbed;
    unit.hp -= damage - absorbed;                         // 막지 못한 나머지는 HP로
}
```
**교훈:** 수백 번 중 몇 번만 틀리면 한 줄씩 실행해서는 못 찾는다. "틀린 순간"을 **조건식**으로 표현해서 중단점에 넣는다. 합계 검증(불변식)은 그 조건을 만드는 좋은 재료다.

## Lab 24 — 이벤트 구독 순서
```
추적점 출력 (5번째 늑대):  퀘스트 검사: 카운터 값 4  →  카운터: Wolf, 증가 전 4
진짜 원인:     Game::setup에서 퀘스트를 카운터보다 먼저 구독 → 퀘스트가 갱신 전 값을 읽음
```
**수정:** `Game::setup`에서 카운터를 먼저 구독한다. 구독 순서를 설명하던 주석도 새 순서에 맞게 고친다. `ft04_quest.cpp`의 `struct Game` 안에 있는 `setup` 함수 전체를 다음으로 바꾼다.
```cpp
    void setup()
    {
        // 통계(카운터)를 먼저 붙여야 퀘스트가 갱신된 값을 본다.
        bus.subscribe([this](const std::string& e) { counter.onKill(e); });
        bus.subscribe([this](const std::string& e) { quest.onKill(e); });
    }
```
더 튼튼한 방법은 퀘스트가 다른 구독자의 상태에 기대지 않고 직접 세거나(`WolfQuest::onKill`에서 `wolfKillsSeen_ >= kRequired`로 판정), 카운터가 "갱신 후" 이벤트를 따로 내보내는 것.
**교훈:** "누가 먼저 실행되는가" 문제는 멈추면 오히려 헷갈린다. **추적점**으로 멈추지 않고 순서를 기록하자.

## Lab 25 — else if 순서 (넓은 조건이 좁은 조건을 가림)
```
조사식 (Selene):  c.isFirstOrder() false / c.isMember() true / c.isVip() true
화살표:          else if (c.isMember()) → return 5
진짜 원인:       VIP는 항상 멤버이므로 isVip() 분기에 영원히 도달하지 못함
```
**수정:** 좁은 조건(VIP)을 넓은 조건(멤버)보다 먼저 검사한다. `ft05_discount.cpp`의 `discountPercent` 함수 전체를 다음으로 바꾼다.
```cpp
int discountPercent(const Customer& c, int total)
{
    if (c.isFirstOrder())
        return 10;
    else if (c.isVip())        // VIP는 항상 멤버이므로 멤버보다 먼저 검사한다
        return 15;
    else if (c.isMember())
        return 5;
    else if (total >= 50000)
        return 3;
    return 0;
}
```
규칙이 "가장 큰 할인 하나"이므로 순서에 의존하지 않게 `std::max`로 해당하는 할인 중 최댓값을 고르는 편이 더 안전하다.
**교훈:** 분기 버그는 조사식에 **조건 하나하나를 따로 넣어 계산**해 보면 한눈에 보인다.

## Lab 26 — auto가 참조를 버리고 복사본을 만듦
```
조사식:      &item != &backpack.get("Iron Sword")
진짜 원인:   auto item = backpack.get(...)  → get()은 Equipment&를 주지만 auto는 값으로 받아 복사
             복사본을 수리하고, 복사본으로 "수리 완료"를 출력함
```
**수정:** `get()`이 돌려주는 참조를 그대로 참조로 받는다(`auto& item = backpack.get(itemName);`). `ft06_repair.cpp`의 `visitBlacksmith` 함수 전체를 다음으로 바꾼다.
```cpp
void visitBlacksmith(Backpack& backpack, int& gold, const std::string& itemName)
{
    auto& item = backpack.get(itemName);   // 복사본이 아니라 배낭 속 장비 그 자체
    const int cost = repairCost(item);
    if (cost == 0 || gold < cost)
        return;

    gold -= cost;
    repair(item);
    std::cout << i18n::L("  수리 완료: ", "  修理完了: ") << item.name << " " << item.durability << "/" << item.maxDurability
              << " (-" << cost << i18n::L(" 골드)\n", " ゴールド)\n");
}
```
**교훈:** "고쳤다고 출력되는데 반영이 안 된다"면 **같은 객체인지부터** 의심하라. 주소 비교나 개체 ID로 확인한다.

## Lab 27 — || 단락 평가로 뒤쪽 검사가 실행되지 않음
```
중단점:      checkKill 걸림, checkGold·checkLevel 안 걸림
진짜 원인:   checkKillAchievements(p)가 true → || 뒤는 계산하지 않음 → 베테랑 업적 검사 자체가 실행 안 됨
```
**수정:** 부수 효과가 있는 호출은 각각 실행한 뒤 합친다. `ft07_achievements.cpp`의 `onBattleEnd` 함수 전체를 다음으로 바꾼다.
```cpp
void onBattleEnd(Player& p)
{
    // 검사를 모두 먼저 실행한 뒤 결과를 합친다.
    const bool kill = checkKillAchievements(p);
    const bool gold = checkGoldAchievements(p);
    const bool level = checkLevelAchievements(p);
    if (kill || gold || level)
        std::cout << i18n::L("  (축하 폭죽!)\n", "  (お祝いの花火!)\n");
}
```
(`|`로 바꿔도 동작은 하지만 의도가 숨겨지므로 권장하지 않는다.)
**교훈:** **걸리지 않은 중단점**도 결과다. "이 함수는 불리긴 했나?"부터 확인하자.

## Lab 28 — 설정을 읽기 전에 복사해 둔 멤버
```
조사식 (boot 안, loadSettings 직후):
    g_settings.difficulty         Hard
    world_.settings_.difficulty   Normal
진짜 원인:   World는 Settings를 값으로 보관하고, Game 생성자에서(= loadSettings 전에) 복사됨.
             AudioSystem은 참조로 보관하므로 나중에 읽은 볼륨이 반영됨.
```
**수정:** `World`도 `AudioSystem`처럼 설정을 `const Settings&`로 보관해서, `loadSettings`가 나중에 쓴 값을 보게 한다(설정 변경이 즉시 반영된다. 대신 참조하는 설정이 `World`보다 오래 살아야 한다. 여기서는 `g_settings`가 전역이라 문제없다). `ft08_difficulty.cpp`의 `class World` 전체를 다음으로 바꾼다.
```cpp
class World
{
public:
    explicit World(const Settings& settings) : settings_(settings) {}

    int spawnWave()
    {
        constexpr int kBaseHp = 100;
        switch (settings_.difficulty)
        {
        case Difficulty::Easy:   enemyHp_ = kBaseHp * 7 / 10; break;
        case Difficulty::Normal: enemyHp_ = kBaseHp;          break;
        case Difficulty::Hard:   enemyHp_ = kBaseHp * 3 / 2;  break;
        }
        return enemyHp_;
    }

private:
    const Settings& settings_;   // 복사본이 아니라 참조: 나중에 읽은 설정도 보인다
    int enemyHp_ = 0;
};
```
다른 방법: 설정을 다 읽은 뒤에 `World`를 만든다(초기화 순서를 명확히), 또는 `spawnWave(const Settings&)`처럼 필요할 때 인자로 받는다.
**교훈:** 버그가 "어떤 입력에서" 나는 게 아니라 "어떤 **순서**로" 나는 경우, 시작부터 따라가며 **두 사본을 나란히** 조사식에 띄워 언제 갈라지는지 본다.

## 연산자 실수 (랩 29~32)


## Lab 29 — 후위 증가의 값은 올리기 전 값
```
틀린 결과:     새봄이 1007번 (도윤과 같은 번호). lastId는 1010으로 맞지만 실제로 준 번호는 1007~1009
멈춘 위치:     issueId의 return 줄, 첫 join
기대값 → 실제값: lastId 1007 → 새 번호 1008 기대 / 실제 반환 1007
진짜 원인:     return guild.lastId++; 는 올리기 전 값(마지막으로 이미 준 번호)을 돌려준다
```
**수정:** 전위 증가로 올린 뒤의 값(새 번호)을 돌려준다. `ft09_member_ids.cpp`의 `issueId` 함수 전체를 다음으로 바꾼다.
```cpp
int issueId(Guild& guild)
{
    return ++guild.lastId;
}
```
**교훈:** `x++`는 "x를 올린다" + "식의 값은 옛 x"라는 두 가지 일을 한다. 변수의 뜻("마지막으로 준 번호" vs "다음에 줄 번호")을 먼저 정하고 전위/후위를 고른다. `if (hits++ == 3)`도 같은 실수로, 네 번째 타격에서야 참이 된다.

## Lab 30 — 매크로가 부작용 있는 인자를 두 번 평가
```
틀린 결과:     화면 콤보 2 4 6 0 1 2 (기대 1 2 3 0 1 2), 최고 콤보 6
멈춘 위치:     c.current 데이터 중단점 → 공격 한 번에 두 번 멈춤
처음 어긋난 줄: c.best = BUGHUNT_MAX(c.best, ++c.current);
진짜 원인:     ((a) > (b) ? (a) : (b))로 펼쳐져 조건에서 한 번, 고른 쪽에서 또 한 번 ++c.current
```
**수정:** 값을 먼저 올리고 비교는 따로 한다. `ft10_best_combo.cpp`의 `onAttack` 함수 전체를 다음으로 바꾼다.
```cpp
void onAttack(Combo& c, bool hit)
{
    if (!hit)
    {
        c.current = 0;
        return;
    }
    ++c.current;
    c.best = std::max(c.best, c.current);
}
```
`std::max`를 쓰므로 `ft10_best_combo.cpp`의 파일 맨 위 `#include`에 다음을 추가한다.
```cpp
#include <algorithm>
```
매크로는 쓰이지 않게 되므로 `#define BUGHUNT_MAX(a, b) ((a) > (b) ? (a) : (b))` 줄을 지운다.
**교훈:** 매크로는 함수가 아니라 글자 치환이다. 인자에 `++`, `--`, 함수 호출을 넣지 않는다. 가능하면 `std::max`나 `inline`/`constexpr` 함수를 쓴다. (`?:`는 조건을 먼저 계산하고 고른 쪽만 계산하도록 순서가 정해져 있어서, 이 결과는 어느 컴파일러에서나 같다.)

## Lab 31 — `&&` 자리에 비트 AND `&`
```
틀린 결과:     보람(열쇠 2, 횃불 1) 입장 불가, 들어간 인원 1
멈춘 위치:     canEnterCave, 조건 a.name == "보람"
기대값 → 실제값: a.keys & a.torches → 기대 참 / 실제 0 (2 = 10, 1 = 01)
진짜 원인:     정수 두 개를 &로 묶어 공통 비트가 없으면 0(거짓)
```
**수정:** 개수를 각각 `> 0`으로 참/거짓으로 만든 뒤 `&&`로 묶는다. `ft11_cave_gate.cpp`의 `canEnterCave` 함수 전체를 다음으로 바꾼다.
```cpp
bool canEnterCave(const Adventurer& a)
{
    return a.keys > 0 && a.torches > 0;
}
```
**교훈:** `&`는 정수의 비트끼리, `&&`는 참/거짓끼리 계산한다. 둘 다 1일 때만 우연히 같은 답이 나와서 처음 테스트(1, 1)를 통과한다. 개수를 참/거짓으로 쓸 때는 `> 0`으로 뜻을 적어 준다.

## Lab 32 — `||` 자리에 `|` → 단락 평가가 사라짐
```
틀린 결과:     연 문 4 (기대 5), 남은 따개 0 (기대 1)
멈춘 위치:     useLockpick의 --t.lockpicks 줄, 호출 스택에서 openDoor의 door 확인
처음 어긋난 곳: "감옥" 문 (열쇠가 있는데 따개가 줄어듦)
진짜 원인:     hasKey(...) | useLockpick(...) 는 앞이 true여도 뒤를 호출한다
```
**수정:** `|`를 `||`로 되돌려 열쇠가 있으면 따개를 쓰지 않게 한다. `ft12_lockpicks.cpp`의 `openDoor` 함수 전체를 다음으로 바꾼다.
```cpp
bool openDoor(Thief& t, const std::string& door)
{
    return hasKey(t, door) || useLockpick(t);
}
```
**교훈:** `||`/`&&`만 단락 평가를 한다. 뒤쪽 피연산자에 부작용(소모, 저장, 로그)이 있으면 `|`/`&`는 그 부작용을 항상 일으킨다. 랩 27과 짝: 27은 단락 평가 때문에 **필요한 호출이 빠졌고**, 32는 단락 평가가 없어서 **불필요한 호출이 일어났다**. `p == nullptr | p->hp <= 0`처럼 앞의 검사가 뒤를 지켜 주는 식에서는 `|`가 null 역참조 크래시가 된다.

---

# 4부: 컨테이너

## Lab 33 — map::operator[]가 없는 키를 만듦
```
관찰:        openShop 전 bag.size() == 3, 후 == 6 ("Mana Potion", "Fire Scroll", "Rope"가 개수 0으로 생김)
진짜 원인:   countOf의 bag[item]은 키가 없으면 기본값(0)으로 원소를 새로 만든다
```
**수정:** 읽기만 할 때는 `find`(또는 C++20 `contains`)를 쓰고 `const Bag&`로 받는다. `ct01_bag_lookup.cpp`의 `countOf` 함수 전체를 다음으로 바꾼다.
```cpp
int countOf(const Bag& bag, const std::string& item)
{
    const auto it = bag.find(item);
    return it == bag.end() ? 0 : it->second;
}
```
`const Bag&`로 바꾸면 `bag[item]`은 컴파일 오류가 되어 같은 실수를 막아 준다.
**교훈:** `operator[]`는 "찾기"가 아니라 "찾고 없으면 만들기"다.

## Lab 34 — rehash 뒤에 "몇 번째 줄"이 다른 사람을 가리킴
```
조사식:      registerWave(..., 60) 전후로 bucket_count가 크게 늘고, selectedName()이 Mira → 다른 사람으로 바뀜
진짜 원인:   선택을 순회 순서의 위치(줄 번호)로 기억함. rehash가 원소를 새 버킷에 다시 나눠 담으면서 순서가 바뀐다
```
**수정:** rehash에도 변하지 않는 것으로 기억한다. 표준은 rehash가 이터레이터를 무효화해도 **원소에 대한 참조와 포인터는 유지**된다고 보장한다. 멤버 `selectedRow_`를 원소 포인터 `selected_`로 바꾸고, 그것을 쓰던 세 함수를 모두 고친다. `ct02_party_finder.cpp`의 `class PartyFinder` 전체를 다음으로 바꾼다(`std::next`에 필요한 `<iterator>`는 이 파일에 이미 있다).
```cpp
class PartyFinder
{
public:
    void registerPlayer(const std::string& name, const std::string& role, int level) { board_[name] = { role, level }; }

    // 줄 번호가 아니라 원소 자체를 기억한다. rehash가 일어나도 원소의 주소는 그대로다.
    void select(size_t row) { selected_ = &*std::next(board_.begin(), row); }

    const std::string& selectedName() const { return selected_->first; }

    std::string confirmInvite()
    {
        std::string name = selected_->first;
        board_.erase(name);      // 키로 지운다
        selected_ = nullptr;     // 지운 원소를 더 가리키지 않는다
        return name;
    }

    Board& board() { return board_; }

private:
    Board board_;
    Board::value_type* selected_ = nullptr;
};
```
키(이름)를 복사해 두어도 된다: `std::string selectedName_;`에 `select`가 `std::next(board_.begin(), row)->first`를 복사하고, `confirmInvite`는 `board_.erase(selectedName_)`.
**교훈:** `unordered_map`의 순회 순서는 약속이 아니다. 오래 들고 있을 것은 키나 원소 포인터로.

## Lab 35 — 비교자가 strict weak ordering을 어김
```
멈춘 곳:     [CRASH] 정렬 검사: 비교 함수가 엄격한 약순서를 어겼다 ← bughunt::checkedOrder ← std::sort
조사식:      comp(Kael, Doran) == true, comp(Doran, Kael) == true  (둘 다 1500점)
진짜 원인:   >= 는 같은 값에 대해 true를 돌려준다. "a가 b보다 앞서고 b도 a보다 앞선다"는 모순
```
**수정:** 비교를 `>=`에서 `>`로 바꿔 같은 점수에는 false를 돌려준다. `ct03_leaderboard.cpp`의 `sortByScore` 함수 전체를 다음으로 바꾼다.
```cpp
void sortByScore(std::vector<Entry>& entries)
{
    std::sort(entries.begin(), entries.end(), bughunt::checkedOrder([](const Entry& a, const Entry& b) {
        return a.score > b.score;   // 높은 점수가 앞으로 (같으면 false)
    }));
}
```
**구성별:** 랩은 비교자를 `bughunt::checkedOrder`로 감싸서 어느 컴파일러, 어느 구성에서나 같은 자리에서 멈추게 했다. 검사가 없으면 표준 라이브러리마다 다르다. MSVC의 Debug STL은 `invalid comparator` 어설션을 내고, libstdc++는 `-D_GLIBCXX_DEBUG`, libc++는 하드닝 모드에서만 검사한다. 검사 없는 Release에서는 원소가 적으면 우연히 맞게 정렬되고, 같은 값이 아주 많으면 정렬이 배열 밖까지 읽어 나가 죽기도 한다.
**교훈:** 비교자는 "엄격하게 앞서는가"를 답한다. 같으면 false.

## Lab 36 — remove_if만 하고 erase를 빼먹음
```
관찰:        호출 뒤에도 size 6. 앞 3칸은 남길 장비, 뒤 3칸은 옮겨지고 남은 찌꺼기 (remove_if가 원소를 **이동**하므로 이름은 비고 내구도만 남음)
경고:        반환값을 버리고 있다 (MSVC C4858, GCC·Clang -Wunused-result. remove_if는 [[nodiscard]])
```
**수정:** C++20 `std::erase_if`로 조건에 맞는 원소를 지우고 컨테이너 크기까지 줄인다(`<vector>`에 들어 있는 함수다). `ct04_repair_shop.cpp`의 `discardBroken` 함수 전체를 다음으로 바꾼다.
```cpp
void discardBroken(std::vector<Gear>& bag)
{
    std::erase_if(bag, [](const Gear& g) { return g.durability == 0; });
}
```
C++17 이전이라면 erase-remove 관용구 `bag.erase(std::remove_if(bag.begin(), bag.end(), pred), bag.end());`(`pred`는 위와 같은 람다)를 쓴다.
**교훈:** 알고리즘은 이터레이터만 받으므로 컨테이너의 크기를 바꿀 수 없다. 경고를 0개로 유지하면 이 버그는 빌드할 때 보인다.

## Lab 37 — vector<bool>의 프록시 참조
```
지역 변수:   wasOpen의 형식이 bool이 아니라 비트 프록시 (MSVC std::_Vb_reference, libstdc++ std::_Bit_reference, libc++ std::__bit_reference)
             토글한 다음 줄에서 wasOpen도 같이 바뀜
진짜 원인:   vector<bool>은 비트로 압축되어 있어 operator[]가 bool&를 줄 수 없고, 비트를 가리키는 프록시 객체를 준다.
             auto는 그 프록시를 복사하므로 "이전 값"이 아니라 "지금 값"을 계속 가리킨다
```
**수정:** 타입을 적어서 값으로 꺼낸다(`const bool wasOpen = doorsOpen[door];`). `ct05_dungeon_doors.cpp`의 `pressSwitches` 함수 전체를 다음으로 바꾼다.
```cpp
int pressSwitches(std::vector<bool>& doorsOpen, const std::vector<int>& presses)
{
    int score = 0;
    for (int door : presses)
    {
        const bool wasOpen = doorsOpen[door];   // 프록시가 아니라 지금 값의 복사본
        doorsOpen[door] = !doorsOpen[door];
        if (!wasOpen)
            ++score;
    }
    return score;
}
```
비트 압축이 필요 없다면 `std::vector<char>`나 `std::vector<std::uint8_t>`도 좋다.
**교훈:** `auto x = v[i];`가 복사라는 믿음은 `vector<bool>`에서 깨진다. 디버거에서 변수의 형식을 보자(gdb `ptype`, lldb `frame variable -T`).

## Lab 38 — c_str() 포인터 수명과 SSO
```
조사식:      (void*)heroes[i].name.c_str() 가 grantTitle 뒤에 바뀜 (Doran, Selene), Kael은 그대로
Kael:        "Kael the Brave" 14글자 → 여전히 객체 안 버퍼(SSO) → 같은 주소라 우연히 맞음
Doran:       21글자 → 객체 안 버퍼에서 힙으로 이사. 이름표는 객체 안의 옛 버퍼(지금은 힙 포인터가 들어 있음)를 읽어 깨진 글자
Selene:      원래 힙(18글자, MSVC STL에서 용량 31) → 37글자로 재할당. 이름표는 해제된 힙을 읽음 (메모리 검사기의 0xEE 글자)
```
**수정:** 이름표가 문자열 버퍼가 아니라 문자열을 가진 객체를 가리키게 한다. `ct06_nameplates.cpp`의 `class Nameplate` 전체를 다음으로 바꾼다.
```cpp
class Nameplate
{
public:
    explicit Nameplate(const Hero& hero) : hero_(&hero) {}

    // 매번 지금 문자열의 버퍼를 물어본다.
    const char* text() const { return hero_->name.c_str(); }

private:
    const Hero* hero_;
};
```
**구성별:** 위 글자 수 경계는 MSVC STL과 libstdc++(객체 안 버퍼 15글자) 기준이다. 힙 용량도 구현마다 다르다(18글자일 때 MSVC STL 31, libstdc++ 18). libc++(macOS 기본)는 22글자까지 객체 안에 두므로 Doran도 우연히 맞을 수 있고, Selene도 처음에는 객체 안에 있다가 Doran처럼 힙으로 이사한다. ASan은 Selene만 잡는다(heap-use-after-free, MSVC STL·libstdc++ 기준). Doran은 아직 살아 있는 `Hero` 객체 안의 메모리를 읽으므로 ASan이 보기에는 정상 접근이다.
**교훈:** `c_str()`/`data()`/`string_view`는 문자열이 바뀌면 무효가 될 수 있다. 짧은 문자열은 SSO 때문에 "되는 것처럼" 보여서 더 늦게 발견된다.

## Lab 39 — set 안에 있는 원소의 정렬 기준을 바꿈
```
조사식:      season.ranked_ 순서가 Mira(1450), Selene, Kael, Doran(1700) → 점수 순이 아님
진짜 원인:   recordMatch가 set에 들어 있는 Player의 score를 직접 바꿈. set은 넣을 때만 자리를 정하므로 트리가 정렬 불변식을 잃는다.
             그 뒤의 find/erase는 엉뚱한 가지로 내려가 원소를 못 찾을 수 있다
```
**수정:** 꺼내고, 바꾸고, 다시 넣는다. `ct07_season_ranking.cpp`의 `class SeasonRanking` 안에 있는 `recordMatch` 함수 전체를 다음으로 바꾼다.
```cpp
    void recordMatch(Player* p, int points)
    {
        ranked_.erase(p);       // 아직 올바른 자리에 있을 때 꺼낸다
        p->score += points;
        ranked_.insert(p);      // 새 점수로 다시 넣는다
    }
```
(C++17의 `extract`로 노드를 꺼내 고친 뒤 `insert`해도 된다.)
**교훈:** `set`/`map`의 키는 const지만, 포인터를 키로 쓰면 가리키는 대상까지 지켜 주지 않는다.

---

# 5부: C++ 언어 함정


## Lab 40 — `const`에 `std::move`는 복사, 옮긴 뒤 원본에 기대기
```
틀린 결과:     내 거래 창에 3개가 그대로 (기대 0), 복사 3번 (기대 0)
멈춘 위치:     Item의 복사 생성자 중단점 → 호출 스택 한 칸 위 trade의 push_back(const Item&)
진짜 원인:     from이 const Trader&라 item은 const Item&. std::move(item)은 const Item&&가 되어 복사 생성자로 간다.
               그리고 옮겼더라도 원소를 옮긴 것이지 vector에서 지운 것이 아니다
```
**수정:** `from`을 `const`가 아닌 참조로 받아 원소를 실제로 옮기고, 옮긴 뒤 거래 창을 비운다. `lg01_trade_items.cpp`의 `trade` 함수 전체를 다음으로 바꾼다.
```cpp
void trade(Trader& from, Trader& to)
{
    for (Item& item : from.offer)
        to.received.push_back(std::move(item));
    from.offer.clear();
}
```
**교훈:** `std::move`는 캐스트일 뿐 아무것도 옮기지 않는다. `const` 객체에 쓰면 조용히 복사가 된다. 옮겨진 객체는 "유효하지만 정해지지 않은 상태"라서 대입하거나 비우는 것만 안전하고, 크기나 내용에 기대면 안 된다(use-after-move). 정적 분석 도구에는 두 실수를 모두 찾는 검사가 있다.

## Lab 41 — 부호 있는 정수 오버플로, 넘친 뒤에 하는 검사
```
틀린 결과:     경험치 -2144967296 (기대 2000000000)
멈춘 위치:     addExp의 if 줄, 조건부 중단점 total < 0. current 1950000000, baseExp * multiplier 200000000
진짜 원인:     int 덧셈이 넘쳐 음수가 됐고, 상한 검사는 이미 망가진 값을 본다
```
**수정:** 더 넓은 형식(`long long`)으로 계산한 뒤 상한을 검사한다. `lg02_exp_cap.cpp`의 `addExp` 함수 전체를 다음으로 바꾼다.
```cpp
int addExp(int current, int baseExp, int multiplier)
{
    long long total = static_cast<long long>(current) + static_cast<long long>(baseExp) * multiplier;
    if (total > kMaxExp)            // 상한 검사 (넘치기 전의 값으로)
        total = kMaxExp;
    return static_cast<int>(total);
}
```
**교훈:** 부호 있는 정수의 오버플로는 정의되지 않은 동작이다. 대개 음수로 돌아가지만 컴파일러는 "넘치지 않는다"고 가정해도 되므로, `if (a + b < a)` 같은 "넘쳤는지 보는" 검사는 Release에서 통째로 지워지기도 한다(이 저장소의 랩은 그런 검사를 쓰지 않아 구성마다 결과가 같다). 검사는 넘치기 **전에**: 더 넓은 형식으로 계산하거나 `std::numeric_limits<int>::max() - current < gain`처럼 남은 여유를 본다. 넘친 뒤 `total < 0`을 보는 땜질은 곱셈이 넘쳐 양수로 돌아오는 경우(숨은 테스트)를 놓친다.

## Lab 42 — 축소 변환 두 곳 (`double`→`int`, `int`→`short`)
```
틀린 결과:     물약 1개 은화 114 (기대 115), 300개 묶음 -31336 (기대 34500)
멈춘 위치:     toSilver의 return 줄, 조사식 gold * 100 = 114.99999999999999 / sellBundle에서 toSilver(unitGold) * count와 record.totalSilver
진짜 원인:     1.15는 2진수로 정확히 나타낼 수 없어 1.1499...로 저장되고, double→int는 버림. 총액 필드가 short라 34200이 잘림
```
**수정:** 은화로 바꿀 때 반올림하고(`std::lround`. `<cmath>`는 이미 포함되어 있다), 총액 필드를 `int`로 넓힌다. `lg03_exchange.cpp`의 `SaleRecord` 구조체 정의 전체(`};`까지)를 다음으로 바꾼다.
```cpp
struct SaleRecord
{
    int count;
    int totalSilver;     // 판매 총액 (은화)
};
```
같은 파일의 `toSilver` 함수 전체를 다음으로 바꾼다.
```cpp
int toSilver(double gold)
{
    return static_cast<int>(std::lround(gold * 100));
}
```
**교훈:** `=`와 `return`의 축소 변환은 경고만 나거나(MSVC C4244) 아무 말이 없다. 중괄호 초기화(`int silver{ gold * 100 };`)는 축소 변환을 컴파일 오류로 만들어(MSVC·Clang. GCC는 상수가 아닌 값에는 경고만 내므로 `-Werror=narrowing`) 두 곳을 한 번에 찾아 준다. 돈은 처음부터 가장 작은 단위의 정수로 들고 다닌다. `+ 1`로 맞춘 땜질은 2.5골드(정확히 250)에서 틀린다.

## Lab 43 — `std::min<std::size_t>`가 음수를 아주 큰 양수로
```
틀린 결과:     궁수 진(평균보다 12 높음) 보너스 20 (기대 0)
멈춘 위치:     levelBonus, 조건부 중단점 myLevel > average. average - myLevel = -12, size_t로는 18446744073709551604
진짜 원인:     컴파일 오류를 피하려고 std::min의 형식을 std::size_t로 정해 두어 음수가 부호 없는 수로 변환됨
```
**수정:** 부호 있는 `int`로 계산하고 0과 `maxBonus` 사이로 자른다(`std::clamp`. `<algorithm>`은 이미 포함되어 있다). `lg04_level_bonus.cpp`의 `levelBonus` 함수 전체를 다음으로 바꾼다.
```cpp
int levelBonus(int myLevel, int average, std::size_t maxBonus)
{
    return std::clamp(average - myLevel, 0, static_cast<int>(maxBonus));
}
```
**교훈:** 부호 있는 값과 없는 값이 만나면 부호 있는 쪽이 부호 없는 형식으로 바뀐다(`-1 < v.size()`가 false인 것과 같은 규칙). 형식이 안 맞아 나는 컴파일 오류를 캐스트로 누르면 그 오류가 알려 주던 문제가 숨는다. 섞어 비교해야 하면 C++20 `std::cmp_less` 등을 쓴다.

## Lab 44 — 두 파일의 전역 변수가 서로를 씀 (초기화 순서)
```
틀린 결과:     전사 체력 0 (기대 300). 파일 순서를 바꿔 빌드하면 보스 공격력 0
멈춘 위치:     g_warriorHp 초기화 줄 중단점, 호출 스택에 main이 없음. 그때 g_baseHp == 0
진짜 원인:     번역 단위 사이의 전역 초기화 순서는 정해져 있지 않다(보통 링크 순서). 두 파일이 서로의 값을 쓰므로 어느 쪽이 먼저든 하나는 0을 읽는다
```
**수정:** 전역 변수 여덟 개(`g_baseHp`, `g_baseMana`, `g_bossAttack`, `g_eliteArmor`, `g_baseAttack`, `g_baseArmor`, `g_warriorHp`, `g_mageMana`)를 모두 지우고, 같은 값을 돌려주던 함수 여덟 개 안에 함수 지역 static으로 넣는다. `readStat`와 헤더 `lg05_stats.h`는 그대로다.

`lg05_stat_table.cpp`에서 `int g_baseHp = readStat("base_hp", 100);` 줄부터 `int eliteArmor() { return g_eliteArmor; }` 줄까지(전역 변수 네 개와 함수 네 개)를 다음으로 바꾼다.
```cpp
int baseHp()     { static const int value = readStat("base_hp", 100); return value; }
int baseMana()   { static const int value = readStat("base_mana", 40); return value; }
int bossAttack() { static const int value = baseAttack() * 4; return value; }   // 보스는 기본 공격력의 4배
int eliteArmor() { static const int value = baseArmor() * 2; return value; }    // 정예 몬스터는 기본 방어력의 2배
```
`lg05_class_stats.cpp`에서 `int g_baseAttack = readStat("base_attack", 12);` 줄부터 `int mageMana() { return g_mageMana; }` 줄까지를 다음으로 바꾼다.
```cpp
int baseAttack() { static const int value = readStat("base_attack", 12); return value; }
int baseArmor()  { static const int value = readStat("base_armor", 7); return value; }
int warriorHp()  { static const int value = baseHp() * 3; return value; }       // 전사는 기본 체력의 3배
int mageMana()   { static const int value = baseMana() * 5; return value; }     // 마법사는 기본 마나의 5배
```
`lg05_stat_table.cpp` 맨 위 설명 주석 가운데 "…계산해 전역 변수에 담아 둔다."로 끝나는 한국어 줄과, 같은 내용의 `// [JA]` 일본어 줄("…グローバル変数に入れておきます。"로 끝나는 줄)은 이제 사실과 다르므로 두 줄 모두 지운다. 이 주석은 `lg05_stat_table.cpp`에만 있다(`lg05_class_stats.cpp`의 주석은 그대로 둔다).
**교훈:** 다른 파일의 전역 객체에 기대는 전역 초기화는 순서에 맡긴 도박이다. 함수 안의 static은 처음 불릴 때 만들어지고(C++11부터 스레드 안전) 의존하는 쪽이 부르는 순간 준비된다. 컴파일할 때 정할 수 있는 값이면 `constexpr`/`constinit`이 가장 좋다. 이 랩은 두 방향으로 의존하게 만들어, 링크 순서가 바뀌어도 버그 상태가 늘 FAIL이다. 화면의 두 값만 상수로 박는 땜질은 숨은 테스트의 다른 두 값에서 걸린다.

## Lab 45 — 헤더의 `static` 변수가 파일마다 따로
```
틀린 결과:     저장 횟수 0, 마지막 슬롯 "" (기대 3, "auto-용의 둥지")
멈춘 위치:     lg06_autosave.cpp의 ++g_saveStats.count와 showSaveScreen에서 &g_saveStats가 서로 다름
진짜 원인:     네임스페이스 범위의 static은 내부 링크. 헤더를 포함한 .cpp마다 별개의 g_saveStats가 생긴다
```
**수정:** 헤더의 변수를 C++17 inline 변수로 바꿔 프로그램 전체에 하나만 있게 한다. `lg06_save_stats.h`의 `static SaveStats g_saveStats;` 줄을 다음으로 바꾼다.
```cpp
inline SaveStats g_saveStats;
```
(또는 헤더에 `extern SaveStats g_saveStats;` 선언만 두고, .cpp 한 곳에 `SaveStats g_saveStats;`를 정의해도 된다.)
**교훈:** 헤더에 정의를 둘 때 `static`은 "하나만"이 아니라 "파일마다 하나씩"이다. 컴파일러도 링커도 경고하지 않는다. 헤더의 `const`/`constexpr` 변수도 기본으로 내부 링크라 값은 같지만 주소는 다르다.

## Lab 46 — 예외 경로에서만 새는 텍스처
```
틀린 결과:     남은 텍스처 2개 (기대 0)
멈춘 위치:     Texture 생성자/소멸자 중단점: 생성 6번, 소멸 4번. 남은 둘은 "저주받은 성"
진짜 원인:     loadDungeon이 텍스처를 new한 뒤 countRooms가 던지면, 원시 포인터 floor/walls만 사라지고 Texture는 남는다
```
**수정:** 텍스처를 만들자마자 `std::unique_ptr`에 맡긴다. `countRooms`가 던지면 스택 풀기가 두 소멸자를 불러 준다. `lg07_dungeon_load.cpp`의 파일 맨 위 `#include`에 다음을 추가한다.
```cpp
#include <memory>
```
같은 파일의 `loadDungeon` 함수 전체를 다음으로 바꾼다.
```cpp
Dungeon* loadDungeon(const DungeonFile& file)
{
    auto floor = std::make_unique<Texture>(file.name + "/floor");
    auto walls = std::make_unique<Texture>(file.name + "/walls");
    const int rooms = countRooms(file.layout);   // 여기서 던져도 floor, walls가 Texture를 지운다
    return new Dungeon{ file.name, floor.release(), walls.release(), rooms };
}
```
(더 나아가려면 `Dungeon`의 멤버도 `std::unique_ptr<Texture>`로 바꾸고(소멸자의 `delete` 두 줄 삭제), `loadDungeon`이 `std::unique_ptr<Dungeon>`을 돌려주게 하면 `exploreAll`의 `delete d;`도 사라진다. 이 랩을 통과하는 데는 필요 없다.)
**교훈:** 예외가 나면 그 사이 함수들의 남은 줄은 실행되지 않고, 소멸자가 불리는 것은 스택의 객체뿐이다. 자원은 만들자마자 소유 객체(RAII)에 맡긴다. 정상 경로만 보면 new/delete 짝이 맞아 보여서 찾기 어려운 누수다.

## Lab 47 — 소멸자에서 던져 `std::terminate`
```
틀린 결과:     CRASH, abort() (저장 실패 메시지가 나오지 않음)
멈춘 위치:     예외가 던져진 ~SaveFile. 호출 스택 아래에 saveGame의 catch가 아니라 std::terminate
진짜 원인:     C++11부터 소멸자는 암시적 noexcept. noexcept 함수 밖으로 나가려는 예외는 catch를 찾지 않고 terminate
```
**수정:** 실패할 수 있는 쓰기를 `close()`로 꺼내고, 닫혔는지 기억하는 멤버 `closed_`를 추가한다. 소멸자는 아직 안 닫힌 경우에만 시도하고 실패는 삼킨다. `lg08_save_close.cpp`의 `class SaveFile` 정의 전체(`};`까지)를 다음으로 바꾼다.
```cpp
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

    // 버퍼를 디스크에 쓴다. 실패할 수 있는 일은 소멸자가 아니라 여기서 한다.
    void close()
    {
        if (buffer_.size() > freeBytes_)
            throw SaveError(i18n::L("디스크 공간이 모자란다", "ディスク容量が足りない"));
        g_disk[slot_] = buffer_;
        closed_ = true;
    }

    // close() 없이 닫히면(예외로 스택을 푸는 중 등) 마지막으로 시도만 하고, 실패는 삼킨다.
    ~SaveFile()
    {
        if (!closed_)
        {
            try { close(); } catch (...) {}
        }
    }

private:
    std::string slot_;
    std::size_t freeBytes_;
    std::string buffer_;
    bool closed_ = false;
};
```
같은 파일의 `saveGame` 함수 전체를 다음으로 바꾼다. try 블록 마지막 줄에서 `file.close()`를 부른다.
```cpp
bool saveGame(const std::string& slot, const std::vector<std::string>& lines, std::size_t freeBytes)
{
    try
    {
        SaveFile file(slot, freeBytes);
        for (const std::string& line : lines)
            file.write(line);
        file.close();   // 여기서 던진 SaveError는 아래 catch가 잡는다
    }
    catch (const SaveError& e)
    {
        g_lastMessage = std::string(i18n::L("저장 실패: ", "セーブ失敗: ")) + e.what();
        return false;
    }
    g_lastMessage = i18n::L("저장 완료", "セーブ完了");
    return true;
}
```
**교훈:** 소멸자는 실패를 알릴 길이 없는 곳이다. 실패할 수 있는 정리는 명시적인 함수(`close`, `commit`)로 꺼내고, 소멸자는 삼킨다. `noexcept(false)`로 바꾸는 땜질은 다른 예외로 스택을 푸는 중에 소멸자가 또 던지면 여전히 terminate다(숨은 테스트). MSVC는 C4297, clang은 `-Wexceptions`, GCC는 `-Wterminate`로 이미 경고한다. 함수에 `noexcept`를 붙여 두고 안에서 던지는 경우도 같은 결과다.

## Lab 48 — 범위 for는 범위를 처음 한 번만 평가한다
```
틀린 결과:     이번 차례 완료 1 (기대 3), 일지 3개 (기대 5)
멈춘 위치:     endTurn의 for. 반복은 두 번, 그 사이 log.size()는 3으로 늘었다. &q는 일지 안이 아니라 entries()가 돌려준 사본 안
진짜 원인:     범위 for는 log.entries()를 한 번 평가하고 그 begin/end를 잡아 둔다. 반복 중에 일지에 붙은 퀘스트는 사본에도, 잡아 둔 끝 안에도 없다
```
**수정:** 범위 for 대신 인덱스로 돌며 매 반복 `log.size()`를 다시 읽는다. `lg09_quest_chain.cpp`의 `endTurn` 함수 전체를 다음으로 바꾼다.
```cpp
int endTurn(QuestLog& log, const QuestBook& book, const std::set<std::string>& achieved)
{
    int completed = 0;
    for (std::size_t i = 0; i < log.size(); ++i)
    {
        const Quest q = log.at(i);   // 복사로 받는다 (complete 안의 push_back이 재할당해도 안전)
        if (q.done || achieved.count(q.goal) == 0)
            continue;
        log.complete(q.name, book);   // 이어지는 퀘스트가 일지 끝에 붙고, 매번 size()를 다시 읽으므로 이번 차례에 확인된다
        ++completed;
    }
    return completed;
}
```
**교훈:** 범위 for는 `auto&& r = 범위식; auto b = r.begin(); auto e = r.end();`로 펼쳐진다. 반복 중에 커지는 컨테이너는 인덱스로 돌고 매번 `size()`를 다시 읽는다. `entries()`가 참조를 돌려주게 바꾸는 것은 답이 아니다. 반복 중인 vector에 push_back하면 끝 이터레이터가 무효가 되고(정의되지 않은 동작), 재할당이 일어나면 `q`도 해제된 메모리를 가리킨다(랩 10).

> 설계 메모: 처음에는 `vector`를 직접 범위 for로 돌며 `reserve`해 둔 뒤 push_back하는 랩으로 만들었다. 그런데 이것은 끝 이터레이터 무효화(정의되지 않은 동작)라, 같은 MSVC STL Debug에서도 MSVC로 빌드하면 FAIL, clang++로 빌드하면 "vector iterators incompatible" 어설션으로 CRASH가 되었다. 판정이 컴파일러마다 달라 지금처럼 "사본을 도는" 정의된 형태로 바꾸고, 무효화는 힌트와 교훈에 남겼다.

---

# 6부: 인터페이스와 상속


## Lab 49 — `const`가 빠져 재정의가 아니라 숨김
```
틀린 결과:     등 뒤 백스탭 20 (기대 60)
멈춘 위치:     Backstab::damage 중단점은 걸리지 않음 → useSkill에서 Step Into → Skill::damage
진짜 원인:     Backstab::damage에 const가 없어 Skill::damage(const Target&) const와 다른 함수. Skill&로 부르면 기반 버전
```
**수정:** `Backstab::damage`에 `const`를 붙여 기반과 시그니처를 맞추고, `override`로 컴파일러가 확인하게 한다. `ih01_backstab.cpp`의 `Backstab` 클래스 안에 있는 한 줄짜리 함수 `virtual int damage(const Target& t) { return t.facingAway ? power_ * 3 : power_; }`를 다음으로 바꾼다.
```cpp
    int damage(const Target& t) const override { return t.facingAway ? power_ * 3 : power_; }
```
**교훈:** 재정의하려는 함수에는 항상 `override`. 붙이는 순간 `const` 누락, 매개변수 형식 차이(`int` vs `long`, `const&` vs 값), 기반 쪽 이름 변경을 컴파일러가 오류로 알려 준다. clang과 GCC 13 이상은 `-Wall`에서 `-Woverloaded-virtual` 경고를 내지만, MSVC는 기본 경고 수준(/W3)에서 아무 말이 없다.

## Lab 50 — `vector<Monster>`에 담아 Boss가 잘림 (슬라이싱)
```
틀린 결과:     전리품 3개, 황금 왕관 없음 (기대 4개)
멈춘 위치:     Boss::loot 중단점이 걸리지 않음 → collectLoot에서 wave[2]를 펼치면 형식이 Monster, rare_ 없음
진짜 원인:     wave.push_back(Boss(...))가 Boss의 Monster 부분만 복사해 담는다
```
**수정:** 몬스터를 `std::unique_ptr<Monster>`로 담아 Boss가 Boss로 남게 한다. `ih02_boss_loot.cpp`의 파일 맨 위 `#include`에 다음을 추가한다.
```cpp
#include <memory>
```
같은 파일의 `spawnWave` 함수 전체를 다음으로 바꾼다.
```cpp
std::vector<std::unique_ptr<Monster>> spawnWave(const std::vector<SpawnInfo>& infos)
{
    std::vector<std::unique_ptr<Monster>> wave;
    for (const SpawnInfo& info : infos)
    {
        if (info.rareDrop.empty())
            wave.push_back(std::make_unique<Monster>(info.name, info.hp));
        else
            wave.push_back(std::make_unique<Boss>(info.name, info.hp, info.rareDrop));
    }
    return wave;
}
```
`collectLoot`도 같은 형식을 받도록, 같은 파일의 `collectLoot` 함수 전체를 다음으로 바꾼다.
```cpp
std::vector<std::string> collectLoot(const std::vector<std::unique_ptr<Monster>>& wave)
{
    std::vector<std::string> bag;
    for (const auto& m : wave)
    {
        for (const std::string& item : m->loot())   // 실제 형식(Boss)의 loot가 불린다
            bag.push_back(item);
    }
    return bag;
}
```
**교훈:** 다형 객체는 포인터(소유는 `unique_ptr`)로 담는다. 값으로 받는 함수 매개변수(`void f(Monster m)`)도 똑같이 자른다. 다형 기반 클래스의 복사를 `= delete`로 막아 두면 이 실수가 컴파일 오류가 된다.

## Lab 51 — 생성자 안의 가상 호출
```
틀린 결과:     저격 탑 사거리 3 (기대 8), 거리 7을 못 쏨
멈춘 위치:     Tower::baseRange 중단점 → 호출 스택에 Tower::Tower ← SniperTower::SniperTower
진짜 원인:     기반 생성자가 도는 동안 객체의 동적 형식은 아직 Tower. 가상 호출도 Tower 버전으로 간다
```
**수정:** 사거리를 생성자에서 저장하지 않고 필요할 때 묻는다. `ih03_sniper_tower.cpp`의 `class Tower` 정의 전체(`};`까지)를 다음으로 바꾼다. 생성자 `Tower()`와 멤버 `range_`는 없어진다.
```cpp
class Tower
{
public:
    virtual ~Tower() = default;

    virtual int baseRange() const { return 3; }
    virtual std::string kind() const { return i18n::L("기본 탑", "基本タワー"); }

    int range() const { return baseRange(); }   // 다 만들어진 뒤에 물으므로 파생 클래스의 버전이 불린다
    bool canHit(int distance) const { return distance <= range(); }
};
```
(또는 `Tower(int range)` 생성자로 파생 클래스가 값을 넘긴다)
**교훈:** 생성자와 소멸자 안에서 가상 함수는 "지금 만들고 있는(부수고 있는) 클래스"의 버전이 불린다. 이것은 표준이 정한 동작이라 어느 컴파일러에서나 같다. 소멸자에서 **순수** 가상 함수를 부르는 경우는 정의되지 않은 동작이라 이 랩에서 뺐다(아래 미포함).

## Lab 52 — 가상 함수의 기본 인자는 정적 형식을 따름
```
틀린 결과:     사제 기본 치유 15 (기대 30), 파티 총 25 (기대 40). 직접 priest.heal()은 30
멈춘 위치:     Priest::heal 안, castDefaultHeal에서 들어왔을 때 amount == 10
진짜 원인:     어느 heal이 불릴지는 동적 형식(Priest)으로, 기본 인자는 정적 형식(Healer)으로 정해진다
```
**수정:** 기본값도 가상 함수로 묻는다. `ih04_priest_heal.cpp`의 `Healer`, `Priest`, `Druid` 클래스 정의 전체(각각 `};`까지)를 차례로 다음으로 바꾸고, `castDefaultHeal` 함수 전체도 바꾼다. `heal`의 기본 인자는 `priest.heal()`처럼 직접 부르는 코드가 있으므로 그대로 둔다.
```cpp
class Healer
{
public:
    virtual ~Healer() = default;
    virtual int heal(int amount = 10) const { return amount; }
    virtual int defaultAmount() const { return 10; }
};
```
```cpp
class Priest : public Healer
{
public:
    int heal(int amount = 25) const override { return amount + 5; }    // 신성 보너스
    int defaultAmount() const override { return 25; }
};
```
```cpp
class Druid : public Healer
{
public:
    int heal(int amount = 18) const override { return amount + 2; }    // 자연 보너스
    int defaultAmount() const override { return 18; }
};
```
```cpp
int castDefaultHeal(const Healer& healer)
{
    return healer.heal(healer.defaultAmount());
}
```
**교훈:** 가상 함수에 기본 인자를 두지 않는다(두더라도 기반과 같은 값만). 흔한 정석은 기본값을 쓰는 비가상 함수가 가상 함수를 부르는 NVI(non-virtual interface) 형태다. 기반의 기본값만 25로 바꾸면 드루이드(18)가 틀린다(숨은 테스트).

## Lab 53 — 파생 클래스의 오버로드 하나가 기반 오버로드를 전부 가림
```
틀린 결과:     골드 0 (기대 500), 아이템 2개 (기대 1). 아이템 목록에 500번
멈춘 위치:     box.give(500)에서 Step Into → Item::Item(int)로 들어감
진짜 원인:     EventMailbox가 give(const Item&)를 선언해 Mailbox::give(int)가 가려짐. 500은 explicit 없는 Item(int)로 변환
```
**수정:** `EventMailbox`에 `using Mailbox::give;`를 넣어 기반의 `give(int)`가 다시 보이게 하고, `Item`의 생성자에 `explicit`을 붙여 숫자가 몰래 `Item`으로 바뀌지 않게 한다. `ih05_reward_mail.cpp`의 `struct Item` 정의 전체(`};`까지)를 다음으로 바꾼다.
```cpp
struct Item
{
    int id = 0;
    explicit Item(int itemId) : id(itemId) {}       // 아이템 번호로 만든다
};
```
같은 파일의 `class EventMailbox` 정의 전체(`};`까지)를 다음으로 바꾼다.
```cpp
class EventMailbox : public Mailbox
{
public:
    using Mailbox::give;   // 기반의 give(int)도 보이게 한다

    void give(const Item& item) override
    {
        Mailbox::give(item);
        ++eventItems_;
    }

    int eventItems() const { return eventItems_; }

private:
    int eventItems_ = 0;
};
```
**교훈:** 이름 찾기가 오버로드 결정보다 먼저다. 파생 클래스에서 같은 이름을 하나라도 선언하면 기반의 같은 이름은 모두 안 보인다. 보이는 후보가 하나뿐이고 암시적 변환이 가능하면 오류 없이 엉뚱한 함수가 불린다. 숫자 하나로 만드는 생성자에는 `explicit`.

## Lab 54 — `dynamic_cast` 결과의 nullptr 확인 누락
```
틀린 결과:     크래시 (액세스 위반 / SIGSEGV)
멈춘 위치:     Mage::restoreMana, this == nullptr → 호출 스택 한 칸 위 drinkManaPotions: unit은 Warrior, mage는 nullptr
진짜 원인:     포인터에 대한 dynamic_cast는 형식이 맞지 않으면 nullptr를 돌려준다. 확인 없이 호출
```
**수정:** `dynamic_cast`의 결과가 nullptr가 아닐 때만 쓴다. `ih06_mana_potion.cpp`의 `drinkManaPotions` 함수 전체를 다음으로 바꾼다.
```cpp
int drinkManaPotions(Party& party, int amount)
{
    int used = 0;
    for (const auto& unit : party)
    {
        if (Mage* mage = dynamic_cast<Mage*>(unit.get()))   // 마법사가 아니면 nullptr라 건너뛴다
        {
            mage->restoreMana(amount);
            ++used;
        }
    }
    return used;
}
```
**교훈:** `dynamic_cast<T*>`는 "형식이 맞는지 묻는" 연산이다. 결과를 확인하지 않을 거라면 질문할 이유가 없다. 실패를 예외로 받고 싶으면 참조 버전(`dynamic_cast<T&>`, 실패 시 `std::bad_cast`). 형식에 따라 분기가 많아지면 가상 함수로 옮길 신호다.

---

# 7부: 파일 입출력


## Lab 55 — 상대 경로가 작업 디렉터리 기준으로 열림
```
틀린 결과:     포션 100 (기대 120), 귀환서 -1 (가격표에 없음)
멈춘 위치:     loadPrices의 std::ifstream 줄
기대값 → 실제값: 열린 파일 game/data/prices.txt 기대 / 실제 workspace/data/prices.txt (예전 복사본)
진짜 원인:     "data/prices.txt"는 실행 파일 폴더가 아니라 현재 작업 디렉터리 기준
```
**수정:** 작업 디렉터리가 아니라 게임 폴더(`gameDir()`)를 기준으로 연다. `io01_patch_prices.cpp`의 `loadPrices` 함수 전체를 다음으로 바꾼다.
```cpp
std::vector<Item> loadPrices()
{
    std::vector<Item> items;
    std::ifstream in(gameDir() / "data/prices.txt");   // 실행 파일 폴더 기준
    Item item;
    while (in >> item.name >> item.price)
        items.push_back(item);
    return items;
}
```
**교훈:** 작업 디렉터리는 프로그램을 띄운 쪽(IDE, 바로 가기, 다른 프로그램)이 정한다. 실행 파일 옆의 데이터는 실행 파일 위치를 기준으로 절대 경로를 만든다. 파일을 못 열었을 때 빈 목록을 조용히 돌려주지 말고 알린다(숨은 테스트처럼 파일이 아예 없으면 품목 0개가 된다).

## Lab 56 — 버퍼에만 있고 파일에는 아직 없는 내용을 다시 읽음
```
틀린 결과:     일지 창 0줄 (기대 3)
멈춘 위치:     readJournal 호출 줄에서 멈춘 뒤 journal.txt 확인 → 0바이트
진짜 원인:     ofstream이 열려 있는 동안 쓴 내용은 스트림 버퍼에 있고, flush/close 전에는 디스크에 없다
```
**수정:** `record`에서 쓴 뒤 `out_.flush();`. flush는 Journal 안에서 한다. 본문에서 `readJournal` 앞에만 flush·close를 넣으면, 일지를 쓰고 바로 읽는 다른 곳(숨은 케이스)에서 다시 틀린다. `io02_quest_journal.cpp`의 `Journal` 클래스 안에 있는 `record` 멤버 함수 전체를 다음으로 바꾼다.
```cpp
    void record(const std::string& entry)
    {
        out_ << entry << '\n';
        out_.flush();   // 다시 읽는 쪽이 바로 볼 수 있게 버퍼를 파일로 내보낸다
    }
```
**교훈:** 같은 프로그램 안에서 쓰고 다시 읽으면, 읽기 전에 쓴 내용이 디스크에 있는지 보장한다. 12부 랩 96(`std::endl`이 매번 flush해서 느림)과 짝: flush는 필요한 순간에만.

## Lab 57 — `while (!in.eof())`로 마지막 기록이 두 번
```
틀린 결과:     4줄, 마지막에 "도윤 7100" 한 번 더, 총점 32100 (기대 25000)
멈춘 위치:     push_back, 조건 scores.size() == 3
기대값 → 실제값: 네 번째 바퀴는 없어야 함 / 실제 in.fail() == true인데 지난 값으로 push_back
진짜 원인:     eof()는 끝을 넘어 읽으려다 실패한 "뒤에" 켜진다. 마지막 줄바꿈이 남아 한 바퀴 더 돈다
```
**수정:** 읽기에 성공했는지를 반복 조건으로 삼는다. `io03_high_scores.cpp`의 `loadScores` 함수 전체를 다음으로 바꾼다.
```cpp
std::vector<Score> loadScores(const fs::path& file)
{
    std::vector<Score> scores;
    std::ifstream in(file);
    std::string name;
    int points = 0;
    while (in >> name >> points)   // 두 값을 모두 읽었을 때만 담는다
        scores.push_back({ name, points });
    return scores;
}
```
**교훈:** 읽기는 "읽고 → 성공했는지 확인 → 쓴다" 순서. 실패한 `>>`는 변수를 그대로 두므로(이 랩) 지난 값이 한 번 더 들어간다. `pop_back` 땜질은 줄바꿈 없이 끝나는 파일에서 진짜 기록을 지운다.

## Lab 58 — `>>` 뒤의 `getline`이 남은 줄바꿈을 읽음
```
틀린 결과:     첫 대사 "" (빈 줄), 셋째 대사 자리에 둘째 대사, 마지막 대사 없음
멈춘 위치:     push_back, 첫 바퀴 line.size() == 0
진짜 원인:     in >> count는 "3"까지만 읽고 줄바꿈을 남긴다. getline은 그 줄바꿈까지를 빈 줄로 읽는다
```
**수정:** `in >> count;` 다음에 숫자 줄의 나머지(줄바꿈까지)를 버린다. `std::numeric_limits`에는 `<limits>`가 필요하다(`io_kit.h`는 포함하지 않는다). `io04_npc_dialogue.cpp`의 파일 맨 위 `#include`에 다음을 추가한다.
```cpp
#include <limits>
```
같은 파일의 `loadDialogue` 함수 전체를 다음으로 바꾼다.
```cpp
std::vector<std::string> loadDialogue(const fs::path& file)
{
    std::vector<std::string> lines;
    std::ifstream in(file);
    int count = 0;
    in >> count;
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');   // "3" 뒤의 공백과 줄바꿈을 버린다
    for (int i = 0; i < count; ++i)
    {
        std::string line;
        std::getline(in, line);
        lines.push_back(line);
    }
    return lines;
}
```
**교훈:** `>>`(토큰 단위)와 `getline`(줄 단위)을 섞으면 경계의 줄바꿈을 직접 처리해야 한다. `in >> std::ws`도 통과처럼 보이지만 다음 줄 앞의 공백까지 먹어서 들여 쓴 대사를 망친다(숨은 테스트).

## Lab 59 — failbit가 남아 이후 읽기가 모두 무시됨
```
틀린 결과:     몬스터 3종, 보상 0개 (기대 3), 102번 보상 0
멈춘 위치:     in >> header 줄, 조사식 in.fail() == true
진짜 원인:     첫 while이 "보상"을 숫자로 읽으려다 실패 → failbit. 지우지 않아 header와 골드 읽기가 전부 그냥 넘어감
```
**수정:** `std::string header;` 앞에 `in.clear();`를 넣는다. `io05_reward_table.cpp`의 `loadRewards` 함수 전체를 다음으로 바꾼다.
```cpp
RewardTable loadRewards(const fs::path& file)
{
    RewardTable table;
    std::ifstream in(file);

    int id = 0;
    while (in >> id)                  // 숫자가 아닌 "보상"에서 멈춘다
        table.monsterIds.push_back(id);

    in.clear();                       // 예상한 실패로 켜진 failbit를 지운다 ("보상"은 그대로 남는다)
    std::string header;
    in >> header;

    int gold = 0;
    while (in >> gold)
        table.gold.push_back(gold);
    return table;
}
```
**교훈:** 실패 표시는 저절로 꺼지지 않는다. 실패를 "예상한" 지점(섹션 끝)에서는 `clear()`로 지우고 계속 읽는다. `clear()`는 글자를 버리지 않으므로 실패를 일으킨 토큰("보상")은 그대로 다음 읽기에 들어간다.

## Lab 60 — UTF-8 BOM이 첫 키에 붙음
```
틀린 결과:     음량 80 (기본값, 기대 30). 전체 화면(둘째 줄)은 정상
멈춘 위치:     getInt, 조건 key == "volume" → settings의 첫 키가 "volume"처럼 보이지만 size() == 9
진짜 원인:     파일 맨 앞의 BOM(EF BB BF) 3바이트가 첫 줄의 키 앞에 붙어 "\xEF\xBB\xBFvolume"이 됨
```
**수정:** 첫 줄이 `"\xEF\xBB\xBF"`로 시작할 때만 3바이트를 지운다. `io06_settings_bom.cpp`의 `loadSettings` 함수 전체를 다음으로 바꾼다.
```cpp
std::map<std::string, std::string> loadSettings(const fs::path& file)
{
    std::map<std::string, std::string> settings;
    std::ifstream in(file);
    std::string line;
    bool firstLine = true;
    while (std::getline(in, line))
    {
        if (firstLine && line.rfind("\xEF\xBB\xBF", 0) == 0)   // 파일 맨 앞의 UTF-8 BOM만 지운다
            line.erase(0, 3);
        firstLine = false;
        const size_t eq = line.find('=');
        if (eq == std::string::npos)
            continue;
        settings[line.substr(0, eq)] = line.substr(eq + 1);
    }
    return settings;
}
```
**교훈:** "첫 줄만 이상하다"면 파일 앞부분의 보이지 않는 바이트를 의심한다. 디버거가 보여 주는 글자보다 `size()`와 바이트 값을 믿는다. 무조건 3바이트를 버리는 땜질은 BOM 없는 파일의 첫 키를 망친다(숨은 테스트).

## Lab 61 — 저장 도중 실패로 반쯤 쓴 세이브
```
틀린 결과:     저장 실패 뒤 불러온 아이템 3종 (기대 5종)
멈춘 위치:     writeItem의 throw 줄, 멈춘 채로 save.txt 열기 → 이미 지워지고 앞의 몇 줄만 있음
진짜 원인:     trunc로 여는 순간 옛 세이브가 지워지고, 예외로 빠져나갈 때 ofstream 소멸자가 앞부분만 쓴 파일을 닫는다
```
**수정:** 임시 파일에 끝까지 쓰고 닫은 뒤에만 바꿔치기한다. `std::error_code`에는 `<system_error>`가 필요하다. `io07_safe_save.cpp`의 파일 맨 위 `#include`에 다음을 추가한다.
```cpp
#include <system_error>
```
같은 파일의 `saveInventory` 함수 전체를 다음으로 바꾼다.
```cpp
bool saveInventory(const fs::path& file, const std::vector<Item>& items)
{
    fs::path temp = file;
    temp += ".tmp";
    try
    {
        {
            std::ofstream out(temp, std::ios::trunc);
            for (const Item& item : items)
                writeItem(out, item);
            out.close();
            if (!out)
                throw std::runtime_error(i18n::L("쓰기 실패", "書き込み失敗"));
        }
        fs::rename(temp, file);          // 같은 폴더 안에서 바꿔치기
        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << i18n::L("  저장 실패: ", "  保存失敗: ") << e.what() << '\n';
        std::error_code ec;
        fs::remove(temp, ec);            // 실패한 임시 파일 정리
        return false;
    }
}
```
**교훈:** 저장은 "전부 아니면 전무"여야 한다. 새 내용은 임시 파일에, 성공한 뒤에만 `rename`. 임시 파일은 같은 폴더(같은 드라이브)에 둬야 rename이 복사 없이 바꿔치기가 된다. rename 전에 스트림을 닫아야 한다(열린 파일은 OS에 따라 이름을 바꿀 수 없다). 실제 게임에서는 전원이 꺼지는 경우까지 대비해 rename 전에 디스크 동기화(OS API)를 하기도 한다. 이 랩은 표준 C++ 범위에서 예외 경로만 다룬다.

---

# 8부: 직렬화 (JSON/XML)


## Lab 62 — `json::operator[]`가 없는 키를 null로 만듦
```
틀린 결과:     소환 버튼 켬 (기대 끔), 다시 저장한 세이브에 "pet":null 추가
멈춘 위치:     petLabel 전후, 조사식 save.size() 3 → 4
처음 어긋난 줄: if (save["pet"].is_null())
진짜 원인:     const가 아닌 json의 []는 없는 키를 null로 삽입한다 (std::map::operator[]와 같음)
```
**수정:** 없는 키를 만들지 말고 있는지만 확인한다. 읽기만 하는 함수는 `const json&`로 받아 `[]`가 키를 끼워 넣지 못하게 하고, `contains`로 확인한 뒤 `at`으로 꺼낸다. `sz01_pet_slot.cpp`의 `petLabel` 함수 전체를 다음으로 바꾼다.
```cpp
std::string petLabel(const json& save)
{
    if (!save.contains("pet"))
        return i18n::L("(펫 없음)", "(ペットなし)");
    return save.at("pet").at("name").get<std::string>();
}
```
**교훈:** "있는지 확인"과 "꺼내기"를 구분한다. const json에 없는 키로 `[]`를 쓰는 것은 정의되지 않은 동작(Debug에서는 assert)이므로 const로 바꾸기만 해서는 안 되고 `contains`로 확인한다.

## Lab 63 — `value("crit", 1)`이 int로 읽어 1.5를 1로 자름
```
틀린 결과:     단검 치명타 30 (기대 45). 도끼(정수 2)와 지팡이(없음)는 정상
멈춘 위치:     loadWeapon의 return, 조건 w.name == "단검" → w.crit == 1
진짜 원인:     value()의 반환 형식은 기본값 1(int)이 정한다. nlohmann은 실수 → 정수 변환에서 예외 없이 자른다
```
**수정:** 기본값을 `1.0`(double)으로 줘서 반환 형식을 double로 만든다(`j.value<double>("crit", 1)`처럼 형식을 직접 적어도 된다). `sz02_crit_multiplier.cpp`의 `loadWeapon` 함수 전체를 다음으로 바꾼다.
```cpp
Weapon loadWeapon(const json& j)
{
    Weapon w;
    w.name = j.at("name").get<std::string>();
    w.attack = j.at("attack").get<int>();
    w.crit = j.value("crit", 1.0);
    return w;
}
```
**교훈:** 라이브러리의 변환 함수는 "형식을 어디서 정하는지" 확인한다. `get<int>()`도 1.5를 1로 조용히 자른다. 정수로 저장되던 값을 실수로 바꾸는 데이터 패치에서 잘 터진다. 반올림 땜질은 1.75, 2.25에서 다시 틀린다(숨은 테스트).

## Lab 64 — `json`이 키를 정렬해서 순서가 사라짐
```
틀린 결과:     불러온 순서 "번개 얼음창 치유 화염구" (기대 "화염구 얼음창 번개 치유"), 저장 후에도 정렬됨
멈춘 위치:     loadSkillBar의 for, j를 펼쳐 보면 이미 정렬되어 있음
진짜 원인:     nlohmann::json의 객체는 std::map(키 정렬)에 담긴다. JSON 표준도 객체 키 순서에 의미를 두지 않는다
```
**수정:** 키를 넣은 순서대로 간직하는 `ordered_json`을 쓴다. `sz03_skill_bar.cpp`의 `namespace sz3` 안 `using Json = nlohmann::json;` 한 줄을 다음으로 바꾼다(`loadSkillBar`와 `saveSkillBar`가 함께 바뀐다).
```cpp
using Json = nlohmann::ordered_json;
```
**교훈:** 순서가 의미 있는 데이터는 배열로 저장하는 것이 정석이다. 객체 키 순서에 기대면 라이브러리·언어를 바꿀 때마다 깨진다. 한글 키는 UTF-8 바이트순으로 정렬되어 "가나다순처럼 보이지만 아닌" 결과가 나오기도 한다.

## Lab 65 — UTF-8을 바이트로 잘라 `dump`가 예외를 던짐
```
틀린 결과:     "Lv99용감한전사" 저장 실패: [json.exception.type_error.316] incomplete UTF-8 string; last byte: 0x95
멈춘 위치:     catch 블록 → clampName의 return에서 마지막 바이트 확인 (0xED 0x95 = "한"의 앞 2바이트)
진짜 원인:     4 + 3 + 3 = 10바이트 뒤 "한"(3바이트)의 중간인 12바이트에서 잘림
```
**수정:** 자를 자리가 글자 중간(이어지는 바이트 `10xxxxxx`)이면 글자의 첫 바이트까지 물러난 뒤 자른다. 12바이트 이하면 그대로 돌려주는 검사는 남긴다. `sz04_nickname_cut.cpp`의 `clampName` 함수 전체를 다음으로 바꾼다.
```cpp
std::string clampName(const std::string& name)
{
    if (name.size() <= kMaxNameBytes)
        return name;
    size_t cut = kMaxNameBytes;
    // 글자 중간(10xxxxxx)이면 그 글자의 첫 바이트까지 물러난다
    while (cut > 0 && (static_cast<unsigned char>(name[cut]) & 0xC0) == 0x80)
        --cut;
    return name.substr(0, cut);
}
```
**교훈:** 바이트 수 제한은 글자 경계에서 자른다(이어지는 바이트는 `10xxxxxx`). `dump`의 오류 처리기(`error_handler_t::replace/ignore`)로 예외만 막으면 깨진 이름이 화면과 서버에 남는다. CP949로 읽은 문자열을 그대로 넣어도 같은 예외(type_error 316)가 난다. 원인은 "JSON에 UTF-8이 아닌 바이트가 들어갔다"로 같다.

## Lab 66 — 기본 정밀도 6자리로 float 왕복 손실
```
틀린 결과:     저장한 문자열 {"x":1234.57,"y":-87.6543,"z":3.33333} → 불러온 위치가 1234.56995 등으로 달라짐
멈춘 위치:     savePosition의 return (out.str()), 디버거의 p.x 표시(1234.56775)와 비교
진짜 원인:     ostream의 기본 정밀도는 유효 숫자 6자리. float을 정확히 되살리려면 max_digits10(9)자리가 필요
```
**수정:** 저장할 때 정밀도를 `max_digits10`(float은 9자리)로 올린다. (직접 이어 붙이지 말고 json 객체에 넣어 `dump()`로 저장해도 된다. nlohmann은 왕복이 보장되는 자릿수로 쓴다.) `std::numeric_limits`에는 `<limits>`가 필요하다. `sz05_float_position.cpp`의 파일 맨 위 `#include`에 다음을 추가한다.
```cpp
#include <limits>
```
`sz05_float_position.cpp`의 `savePosition` 함수 전체를 다음으로 바꾼다.
```cpp
std::string savePosition(const Vec3& p)
{
    std::ostringstream out;
    out << std::setprecision(std::numeric_limits<float>::max_digits10);   // float을 되살릴 수 있는 9자리
    out << "{\"x\":" << p.x << ",\"y\":" << p.y << ",\"z\":" << p.z << "}";
    return out.str();
}
```
**교훈:** 사람이 읽는 출력과 다시 읽을 저장은 다르다. 저장은 왕복(round-trip)이 보장되는 형식으로. `std::fixed`는 소수점 아래 자릿수라 0.0003141 같은 작은 값을 망친다. 숨은 테스트는 7~8자리로는 부족한 값도 넣었다(MSVC와 clang에서 같은 결과를 확인).

## Lab 67 — 속성을 자식 요소로 읽어 빈 노드의 0
```
틀린 결과:     모든 몬스터 HP 0 (공격력은 정상)
멈춘 위치:     m.hp = ... 줄, 조사식 node.child("hp").empty() == true, node.attribute("hp").value() == "320"
진짜 원인:     hp는 속성(hp="320")인데 child("hp")로 찾음 → 빈 노드 → as_int() 0
```
**수정:** hp는 자식 요소가 아니라 속성으로 읽는다. `sz06_bestiary.cpp`의 `loadBestiary` 함수 전체를 다음으로 바꾼다.
```cpp
std::vector<Monster> loadBestiary(const char* xml)
{
    std::vector<Monster> list;
    pugi::xml_document doc;
    if (!doc.load_string(xml))
        return list;
    for (pugi::xml_node node : doc.child("Bestiary").children("Monster"))
    {
        Monster m;
        m.name = node.attribute("name").as_string();
        m.hp = node.attribute("hp").as_int();
        m.attack = node.child("Attack").text().as_int();
        list.push_back(m);
    }
    return list;
}
```
**교훈:** pugixml은 없는 노드/속성에 예외 대신 빈 객체를 돌려준다. 오타, 대소문자("Attack"/"attack"), 속성/자식 혼동이 모두 조용한 0이 된다. 필수 값은 `if (!attr)`로 확인하고 데이터 오류로 알린다.

## Lab 68 — 이스케이프하지 않고 이어 붙인 XML
```
틀린 결과:     길드 목록 읽기 실패: Error parsing element attribute → 0개 (기대 3)
멈춘 위치:     loadGuilds 실패 쪽, xml.c_str() + result.offset → "방패>..." 부분
진짜 원인:     이름 "<검과 방패>"의 <가 새 태그 시작으로 해석됨
```
**수정:** 이름을 XML에 넣기 전에 특수 문자를 이스케이프한다. 글자 하나씩 바꾸므로 `&`가 다른 치환 결과를 다시 건드리는 일은 없다. `sz07_guild_list.cpp`의 `namespace sz7` 안, `saveGuilds` 함수 바로 위에 다음 함수를 추가한다.
```cpp
std::string escapeXml(const std::string& text)
{
    std::string out;
    out.reserve(text.size());
    for (char c : text)
    {
        switch (c)
        {
        case '&':  out += "&amp;"; break;
        case '<':  out += "&lt;"; break;
        case '>':  out += "&gt;"; break;
        case '"':  out += "&quot;"; break;
        case '\'': out += "&apos;"; break;
        default:   out += c; break;
        }
    }
    return out;
}
```
그리고 `saveGuilds` 함수 전체를 다음으로 바꾼다(`g.name` 대신 `escapeXml(g.name)`을 이어 붙인다).
```cpp
std::string saveGuilds(const std::vector<Guild>& guilds)
{
    std::string xml = "<Guilds>\n";
    for (const Guild& g : guilds)
    {
        xml += "  <Guild members=\"" + std::to_string(g.members) + "\">"
             + "<Name>" + escapeXml(g.name) + "</Name></Guild>\n";
    }
    xml += "</Guilds>\n";
    return xml;
}
```
문자열 이어 붙이기를 아예 버리고 pugixml로 문서를 만드는 방법도 있다(`pugi::xml_document`에 `append_child`/`append_attribute`로 노드를 만들고 `doc.save(std::ostringstream&)`로 문자열을 얻는다. 이스케이프는 pugixml이 한다). 이 경우 `<sstream>`이 필요하고, 파일 맨 위의 "문자열을 이어 붙여" 주석도 맞지 않게 된다.
**교훈:** 문자열 이어 붙이기로 구조화된 형식(XML, JSON, SQL, HTML)을 만들면 이스케이프를 빠뜨린다. 더 무서운 경우는 오류가 없는 쪽이다: 이름 "I &lt;3 PvP"는 "I <3 PvP"로 조용히 바뀐다(숨은 테스트). pugixml은 짝 없는 `&`를 너그럽게 넘겨서 "Tom & Jerry"는 읽히지만, 다른 파서에서는 오류다.
**참고(보안):** 외부에서 받은 XML을 읽을 때는 외부 엔티티(XXE)와 엔티티 확장 폭탄을 조심한다. pugixml은 DTD 안의 엔티티를 처리하지 않아 이 두 공격에 해당하지 않지만, 다른 파서(libxml2, MSXML, .NET의 예전 기본값)는 설정에 따라 파일을 읽거나 메모리를 다 쓸 수 있다.

---

# 9부: 포인터 오용

## Lab 69 — 지역 변수의 주소를 반환
```
조사식:      board[0], board[1], board[2] 가 모두 같은 주소
진짜 원인:   formatEntry가 지역 배열 line의 주소를 반환. 함수가 끝나면 line은 사라지고,
             다음 formatEntry 호출(그리고 다른 함수 호출)이 같은 스택 자리를 다시 쓴다.
```
**수정:** 포인터 대신 값(`std::string`)으로 돌려준다. 반환할 때 `line`의 내용이 `std::string`으로 복사되므로 지역 배열이 사라져도 괜찮다. (`static char line[64]`로 바꾸면 깨지지는 않지만 모든 줄이 마지막 값으로 똑같아진다.) `pt01_ranking.cpp`의 `formatEntry` 함수 전체를 다음으로 바꾼다.
```cpp
std::string formatEntry(int rank, const Player& player)
{
    char line[64];
    std::snprintf(line, sizeof(line), i18n::L("%d위 %s (%d점)", "%d位 %s (%d点)"), rank, player.name.c_str(), player.score);
    return line;   // 여기서 std::string으로 복사된다
}
```
받는 쪽 `buildBoard`도 문자열을 담도록 함수 전체를 다음으로 바꾼다. 랩 함수의 `std::string(board[0])`과 출력 반복문은 그대로 동작한다.
```cpp
std::vector<std::string> buildBoard(const std::vector<Player>& ranking)
{
    std::vector<std::string> board;
    for (size_t i = 0; i < ranking.size(); ++i)
        board.push_back(formatEntry(static_cast<int>(i) + 1, ranking[i]));
    return board;
}
```
**구성별:** Debug와 Release 모두 깨진 글자. MSVC ASan은 기본 설정으로는 잡지 못한다. Clang ASan은 `stack-use-after-scope`로 잡기도 한다(이 저장소에서 확인). 가장 빠른 단서는 컴파일러 경고(MSVC C4172, Clang `-Wreturn-stack-address`, GCC `-Wreturn-local-addr`)였다.
**교훈:** 포인터를 돌려줄 때는 "가리키는 대상이 함수보다 오래 사는가?"를 먼저 묻는다.

## Lab 70 — 해제된 몬스터에서 현상금을 읽음 (use-after-free)
```
이상한 값:     hunter.target->bounty == -286331154 (0xEEEEEEEE = 메모리 검사기가 채운 해제된 힙)
처음 어긋난 줄: field.removeDefeated()  (타깃 몬스터를 delete)
진짜 원인:     hunt가 전장을 정리한 뒤에 hunter.target으로 현상금을 읽음
```
**수정:** 현상금을 먼저 받고, 타깃을 놓은 뒤에 전장을 정리한다. `pt02_bounty.cpp`의 `hunt` 함수 전체를 다음으로 바꾼다.
```cpp
void hunt(Battlefield& field, Hunter& hunter, const std::string& targetName, int damagePerHit)
{
    hunter.target = field.find(targetName);
    while (!attack(hunter, damagePerHit))
    {
    }

    hunter.gold += hunter.target->bounty;   // 현상금을 먼저 받고
    hunter.target = nullptr;
    field.removeDefeated();                 // 그다음에 전장 정리
}
```
더 튼튼하게는 raw 포인터를 들고 있지 말고 ID로 다시 찾거나, 몬스터를 `shared_ptr`로 보관하고 타깃은 `weak_ptr`로 든다.
**구성별:** Debug와 Release는 메모리 검사기가 `0xEE`로 채워 FAIL(골드 -572662308). 검사기가 없는 진짜 프로그램이라면 해제된 메모리에 옛 값이 남아 **PASS**할 수도 있다(검사기를 넣기 전 MSVC Release가 그랬다). ASan은 heap-use-after-free와 함께 읽은 곳(`hunt`), 해제한 곳(`removeDefeated`), 할당한 곳(`spawn`)의 호출 스택을 보여 준다.
**교훈:** Release에서 통과했다고 버그가 없는 게 아니다. 해제된 메모리를 읽는 버그는 알아볼 수 있는 채움 값(검사기의 `0xEE`, MSVC 디버그 힙의 `0xDD`)과 ASan으로 드러낸다.

## Lab 71 — new[]로 만들고 delete로 지움
```
멈춘 곳:     [CRASH] 메모리 검사: new로 받은 주소가 아닌 곳을 해제했다 ← operator delete ← ~Explosion의 delete particles_
관찰:        첫 폭발이 끝날 때 ~Particle()이 16번이 아니라 1번만 불림
             (char*)particles_ - 8 자리에 배열 개수 16이 저장되어 있음 (주요 ABI 공통, 표준이 정한 것은 아님)
진짜 원인:   new Particle[count]로 만든 배열을 delete(배열이 아닌 것)로 지움
```
**수정:** `new[]`로 만든 배열은 `delete[]`로 지운다. `pt03_particles.cpp`의 `Explosion` 클래스 안 소멸자 `~Explosion() { delete particles_; }` 한 줄을 다음으로 바꾼다. 더 좋게는 `std::vector<Particle>`이나 `std::unique_ptr<Particle[]>`로 바꿔 직접 지우지 않는다.
```cpp
    ~Explosion() { delete[] particles_; }
```
**구성별:** Debug와 Release는 메모리 검사기가 그 자리에서 `[CRASH]`. 검사기가 없으면 MSVC Debug는 CRT 힙 어설션, Release는 힙 손상(Windows `0xC0000374`, glibc `free(): invalid pointer`)으로 죽는다. ASan은 "attempting free on address which was not malloc()-ed". 소멸자가 없는 타입(`int` 등)이면 앞의 개수 칸이 없어서 Debug와 Release에서는 아무 증상 없이 지나가기도 한다. 그래도 정의되지 않은 동작이다.
**교훈:** 소멸자를 가진 타입의 배열은 `new[]`가 앞에 개수를 몰래 적어 둔다. 만든 방법과 지우는 방법은 항상 짝을 맞춘다.

## Lab 72 — 포인터 산술 단위 실수
```
조사식:      (const char*)entries - (const char*)header == 16  (헤더 바로 뒤라면 4)
진짜 원인:   header는 PacketHeader*. header + sizeof(PacketHeader) 는 "4바이트 뒤"가 아니라
             "PacketHeader 4개 뒤" = 16바이트 뒤
```
**수정:** 헤더 바로 뒤는 `header + 1`(PacketHeader 하나 뒤)이다. 바이트 단위로 `reinterpret_cast<const LootEntry*>(packet.data() + sizeof(PacketHeader))`라고 써도 된다. `pt04_loot_packet.cpp`의 `totalItems` 함수 전체를 다음으로 바꾼다.
```cpp
int totalItems(const std::vector<uint8_t>& packet)
{
    const PacketHeader* header = reinterpret_cast<const PacketHeader*>(packet.data());
    const LootEntry* entries = reinterpret_cast<const LootEntry*>(header + 1);   // PacketHeader 하나 뒤 = 4바이트 뒤

    int total = 0;
    for (uint32_t i = 0; i < header->entryCount; ++i)
        total += entries[i].count;
    return total;
}
```
**구성별:** Debug와 Release는 패킷 바로 뒤에 메모리 검사기가 둔 경계 바이트 `0xBDBD`(= 48573)를 개수로 읽어 합계가 145722가 된다. 검사기가 없으면 쓰레기 값. ASan은 heap-buffer-overflow(READ of size 2)로 그 줄에서 멈춘다.
**교훈:** 포인터에 더하는 수의 단위는 "가리키는 타입 하나"다. 바이트 단위로 움직이려면 `char*`/`uint8_t*`에서 계산한다.

## Lab 73 — 초기화 안 된 포인터 멤버
```
멈춘 곳:     Hero::attackPower의 power += weapon_->bonus  (읽기 액세스 위반)
지역 변수:   this->weapon_ == 0xa5a5a5a5a5a5a5a5
진짜 원인:   무기를 받지 않는 생성자가 weapon_을 초기화하지 않음.
             if (weapon_ != nullptr) 검사는 0xA5A5... 앞에서는 아무 소용이 없다.
```
**수정:** 멤버 선언에서 초기화해, 무기를 받지 않는 생성자도 `nullptr`에서 시작하게 한다. `pt05_equipment.cpp`의 `Hero` 클래스 `private:` 아래 `const Weapon* weapon_;` 한 줄을 다음으로 바꾼다.
```cpp
    const Weapon* weapon_ = nullptr;
```
**구성별:** 파티는 재사용 메모리(`bughunt::recycledStorage`, `0xA5`로 채워져 있음) 위에 만들어지므로 Debug와 Release 모두 매번 같은 곳에서 죽는다(Windows 액세스 위반, POSIX `SIGSEGV`). 재사용 메모리가 아니었다면 남은 쓰레기에 따라 통과하기도 하고 엉뚱한 공격력이 나오기도 한다. 쓰레기 주소가 어디를 가리키느냐에 따라 stack-buffer-overflow처럼 엉뚱한 이름의 리포트가 나오기도 한다. ASan은 "초기화 안 된 값" 자체를 잡는 도구가 아니다.
**교훈:** x64에서 `0xA5A5...`처럼 정규 주소가 아닌 값을 읽으면 예외 메시지에는 `0xFFFFFFFFFFFFFFFF`로 나올 수 있다. 예외 메시지보다 디버거의 포인터 값을 본다. 랩 12처럼 "멤버를 초기화하지 않음" 정적 분석 경고가 미리 알려 준다.

## Lab 74 — shared_ptr 순환 참조
```
지역 변수:   guild [strong ref=4]  (지역 변수 1 + 길드원 3명의 guild 멤버)
진짜 원인:   Guild → members(shared_ptr) → Member → guild(shared_ptr) → Guild
             서로 붙잡고 있어서 마지막 지역 변수가 사라져도 참조 수가 0이 되지 않는다.
```
**수정:** 길드원은 길드를 소유하지 않고 바라보기만 한다. `pt06_guild.cpp`의 `struct Member` 정의 전체를 다음으로 바꾼다(`join`의 `member->guild = guild;`는 그대로 컴파일된다).
```cpp
struct Member
{
    explicit Member(const std::string& n) : name(n) { ++g_aliveMembers; }
    ~Member() { --g_aliveMembers; }

    std::string name;
    std::weak_ptr<Guild> guild;   // 소유하지 않고 바라보기만 한다
};
```
`weak_ptr`는 바로 `->`로 쓸 수 없으므로 `guildNameOf` 함수 전체를 다음으로 바꾼다.
```cpp
std::string guildNameOf(const Member& member)
{
    const auto guild = member.guild.lock();   // 길드가 아직 살아 있으면 shared_ptr, 아니면 빈 포인터
    return guild ? guild->name : i18n::L("(없음)", "(なし)");
}
```
**구성별:** 세 구성 모두 똑같이 FAIL이다. 크래시도, 어설션도, ASan 리포트도 없다(Windows의 ASan에는 누수 검사기가 없다. Linux·macOS의 ASan은 LeakSanitizer가 프로그램이 끝날 때 누수를 보고할 수 있다). 2부 랩 16처럼 인스턴스 수나 소멸자 중단점으로 확인한다.
**교훈:** 스마트 포인터도 소유 관계에 고리가 생기면 샌다. "누가 누구를 소유하는가"를 한 방향으로 정하고, 거꾸로 가는 쪽은 `weak_ptr`로 둔다.

## Lab 75 — [&] 람다가 끝난 함수의 지역 변수를 참조
```
중단점:      람다 안 hero.attack -= bonus; (Kael 차례)
지역 변수:   bonus = 45   ← 분노를 걸 때 출력은 (+30)
호출 스택:   TurnTimers::advance → 람다. castRage는 이미 끝나서 스택에 없음
조사식:      &bonus 가 castRage 안에서 본 &bonus와 같은 주소 → 끝난 함수의 스택 자리.
             그 자리를 나중에 Doran의 castRage가 다시 써서 Doran의 bonus(45)가 들어 있다
진짜 원인:   [&]가 지역 변수 bonus를 참조로 캡처했는데, 람다는 타이머에 맡겨져 castRage보다 오래 산다
```
**수정:** 함수보다 오래 사는 람다는 지역 변수를 값으로 가져간다. `hero`는 호출한 쪽이 계속 들고 있으므로 참조로 둔다. `pt07_rage_timer.cpp`의 `castRage` 함수 전체를 다음으로 바꾼다(바뀐 곳은 캡처 목록 `[&]` → `[&hero, bonus]`뿐이다).
```cpp
void castRage(TurnTimers& timers, Hero& hero)
{
    const int bonus = rageBonus(hero);
    hero.attack += bonus;
    std::cout << "  " << hero.name << i18n::L(" 분노! 공격력 ", " 怒り! 攻撃力 ") << hero.attack << " (+" << bonus << ")\n";

    timers.after(3, [&hero, bonus] {   // bonus는 값으로 복사, hero는 참조
        hero.attack -= bonus;
        std::cout << "  " << hero.name << i18n::L(" 분노 끝. 공격력 ", " の怒りが切れた。攻撃力 ") << hero.attack << '\n';
    });
}
```
**구성별:** 틀린 값은 컴파일러와 구성마다 다르다(이 저장소에서 Kael의 예: MSVC Debug -124(다시 빌드하면 바뀔 수 있다), MSVC·Clang Release 45, Clang Debug 85, ASan 쓰레기 값). 위 조사식은 Release의 45가 나온 이유다. 기본 ASan은 stack-use-after-return을 잡지 않는다. Clang ASan은 `ASAN_OPTIONS=detect_stack_use_after_return=1`로 실행하면 이 람다 줄에서 `stack-use-after-return`으로 멈춘다(README "람다 캡처와 ASan").
**교훈:** `[&]`는 람다가 **지금 이 자리에서** 실행될 때만 안전하다. 콜백, 타이머, 스레드, 나중에 실행할 목록에 넣는 람다는 캡처 목록을 하나씩 적고, 무엇을 참조로 들고 가는지 확인한다. `hero`를 참조로 가져가는 것도 "Hero가 타이머보다 오래 산다"는 약속에 기댄 것이다.

## 넘침: 스택과 힙의 경계 (랩 76~80)


## Lab 76 — 16 MB 지역 배열로 스택 오버플로
```
틀린 결과:     CRASH, 스택 오버플로 (0xC00000FD / ASan stack-overflow)
멈춘 위치:     countReachable의 첫 줄(또는 스택을 미리 건드리는 도우미 함수). 호출 스택은 lab → countReachable 두어 줄뿐
진짜 원인:     int dist[2048][2048] = 16 MB 지역 변수. 함수에 들어가자마자 스택(Windows 1 MB)을 넘는다. 지도 크기와 상관없다
```
**수정:** 거리표를 지도 크기만큼만 힙에 잡는다. `dist`를 선언하던 줄과 그 아래 `-1`로 채우던 이중 for 문이 `vector` 한 줄로 바뀌고(생성자가 `-1`로 채운다), 나머지 `dist[y][x]` 코드는 그대로 동작한다. `of01_minimap.cpp`의 `countReachable` 함수 전체를 다음으로 바꾼다.
```cpp
int countReachable(const Map& map, int startX, int startY)
{
    // 시작 칸에서 몇 걸음인지 (-1 = 아직 안 감). 지도 크기만큼만 힙에 잡는다
    std::vector<std::vector<int>> dist(static_cast<size_t>(map.height), std::vector<int>(static_cast<size_t>(map.width), -1));

    std::vector<std::pair<int, int>> queue;
    queue.emplace_back(startX, startY);
    dist[startY][startX] = 0;
    int reached = 0;
    for (size_t head = 0; head < queue.size(); ++head)
    {
        const auto [x, y] = queue[head];
        ++reached;
        const int step[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
        for (const auto& s : step)
        {
            const int nx = x + s[0];
            const int ny = y + s[1];
            if (map.walkable(nx, ny) && dist[ny][nx] < 0)
            {
                dist[ny][nx] = dist[y][x] + 1;
                queue.emplace_back(nx, ny);
            }
        }
    }
    return reached;
}
```
이제 쓰지 않는 상수 줄 `constexpr int kMaxSize = 2048;`을 지운다. 파일 맨 위 설명 주석 중 `// 지도는 최대 2048 x 2048칸까지 지원한다. 거리표(dist)는 최대 크기로 한 번에 잡아 둔다.` 줄과 그 아래 일본어 줄 `// [JA] マップは最大 2048 x 2048 マスまで対応しています。距離表(dist)は最大サイズで一度に確保しておきます。`도 맞지 않게 되므로 두 줄 모두 지운다.
(1차원 `std::vector<int> dist(static_cast<size_t>(map.width) * map.height, -1);`로 잡고 `dist[y * map.width + x]`로 써도 된다. 이때는 `dist[...]` 네 곳을 모두 고친다.)
**교훈:** 스택 오버플로가 늘 재귀인 것은 아니다. 호출 스택이 짧은데 스택 오버플로라면 큰 지역 변수를 의심한다. 크기가 실행 중에 정해지거나 수십 KB를 넘는 데이터는 힙(`vector`)에. `static` 배열로 바꾸면 죽지는 않지만 16 MB를 늘 붙잡고 여러 스레드에서 쓸 수 없다.

## Lab 77 — 프레임마다 96 KB 버퍼 × 재귀 160겹
```
틀린 결과:     CRASH, 스택 오버플로. "방 8번 생성"까지 출력되고 죽음 (방 5개 시험 던전은 통과)
멈춘 위치:     buildRooms가 열 겹 남짓 쌓인 호출 스택. 이웃한 두 프레임의 &layout 차이 ≈ 98,000바이트
진짜 원인:     재귀는 160겹에서 끝나지만, 한 겹이 96 KB라 160겹이면 15 MB. 버퍼가 재귀 호출 뒤에도 쓰여 줄일 수 없다
```
**수정:** 버퍼를 힙으로 옮긴다. 첫 줄 `char layout[kLayoutBytes];`가 `std::vector<char>`로 바뀌고, 프레임에는 vector 하나(포인터 세 개)만 남는다. 나머지 `layout[i]` 코드는 그대로다. `of02_dungeon_gen.cpp`의 파일 맨 위 `#include`에 다음을 추가한다.
```cpp
#include <vector>
```
`of02_dungeon_gen.cpp`의 `buildRooms` 함수 전체를 다음으로 바꾼다.
```cpp
int buildRooms(int index, int roomCount, std::uint32_t seed)
{
    std::vector<char> layout(kLayoutBytes);           // 이 방의 배치도 (힙)
    const std::size_t size = 2048 + seed % 2048;      // 실제 방 크기
    std::uint32_t s = seed;
    for (std::size_t i = 0; i < size; ++i)
    {
        s = nextSeed(s);
        layout[i] = (s >> 16) % 64 == 0 ? '$' : '.';
    }
    if (index % 8 == 0)
        std::cout << i18n::L("  방 ", "  部屋") << index << i18n::L("번 생성\n", "番を生成\n") << std::flush;

    int chests = 0;
    if (index + 1 < roomCount)
        chests += buildRooms(index + 1, roomCount, nextSeed(seed ^ 0x9E3779B9u));
    for (std::size_t i = 0; i < size; ++i)
        chests += layout[i] == '$' ? 1 : 0;
    return chests;
}
```
또는 보물 상자를 재귀 호출 전에 세고 버퍼를 일찍 끝내거나, 재귀를 반복문으로 바꾼다.
**교훈:** 스택 사용량은 "프레임 크기 × 깊이"다. 깊이가 데이터(방 수, 트리 높이)에 따라 정해지는 재귀는 스택 한도를 생각해야 한다. 프레임 크기는 구성마다 다르다(Debug는 검사용 공간, ASan은 변수 사이 경계). 그래서 프레임이 작고 깊이가 경계에 걸린 재귀는 Debug에서만, 또는 ASan에서만 죽기도 한다. 직접 보려면 `kLayoutBytes`를 8 KB로 줄이고 방 수를 늘려 Debug와 Release에서 죽는 방 번호를 비교해 보라.

## Lab 78 — `new Frame[n]` 뒤 `<=`로 한 칸 더 쓰기, 해제할 때 드러남
```
틀린 결과:     CRASH, [CRASH] 메모리 검사: 할당한 크기를 넘어서 썼다 (ASan: heap-buffer-overflow)
멈춘 위치:     ~ReplayRecorder의 delete[]. 데이터 중단점(&frames_[ticks_])을 걸면 recordAll의 frames_[t] = ... 에서 t == 600
진짜 원인:     for (t = 0; t <= ticks_; ++t)가 원소 600개 배열의 frames_[600]에 쓴다
```
**수정:** 반복 조건을 `<=`에서 `<`로 바꿔 마지막 칸 `frames_[ticks_ - 1]`까지만 쓴다. `of03_replay_buffer.cpp`의 `ReplayRecorder` 클래스 안 `recordAll` 함수 전체를 다음으로 바꾼다.
```cpp
    void recordAll(std::uint32_t seed)
    {
        for (int t = 0; t < ticks_; ++t)
            frames_[t] = Frame{ t, buttonsAt(seed, t) };
    }
```
**교훈:** 힙 넘침은 쓰는 순간이 아니라 그 자리를 다른 누가 볼 때(해제, 다음 할당, 옆 블록 사용) 드러난다. 크래시 위치와 원인이 멀면 **데이터 중단점**으로 "그 바이트를 누가 바꾸는지"를 묻는다. ASan은 쓰는 순간 잡는다. 랩 72(포인터 산술의 단위, 읽기)와 달리 이 랩은 반복 경계의 한 칸 쓰기다.

## Lab 79 — `memcpy`에 `sizeof(포인터)`
```
틀린 결과:     첫 번째 이름 "Nightingnd_Archer" (기대 "Nightingale"), 두 번째 이름 "Moonshadnd_Archer". 앞 8글자 뒤에 직전 접속자 이름(Silverwind_Archer)의 나머지가 붙음
멈춘 위치:     writeName, 조사식 sizeof(dst) == 8, std::strlen(src) == 11. 메모리 창에서 packet.name 9바이트째부터 앞사람 이름
진짜 원인:     매개변수 char* dst는 배열이 아니라 포인터라 sizeof가 8. 앞 8바이트만 복사되고 널 문자가 없다
```
**수정:** 버퍼 크기를 함께 넘기고, 넘치지 않게 자른 뒤 널 문자를 직접 넣는다. `std::min`에는 `<algorithm>`이 필요하다. `of04_name_packet.cpp`의 파일 맨 위 `#include`에 다음을 추가한다.
```cpp
#include <algorithm>   // std::min
```
`of04_name_packet.cpp`의 `writeName` 함수 전체를 다음으로 바꾼다(매개변수 `cap`이 늘어난다).
```cpp
void writeName(char* dst, std::size_t cap, const char* src)
{
    const std::size_t n = std::min(std::strlen(src), cap - 1);   // 널 문자 자리 1바이트를 남기고 자른다
    std::memcpy(dst, src, n);
    dst[n] = '\0';
}
```
호출하는 `sendLogin` 함수 전체도 크기를 넘기도록 다음으로 바꾼다.
```cpp
std::string sendLogin(NamePacket& packet, const char* name, int level)
{
    writeName(packet.name, sizeof(packet.name), name);   // 여기서 sizeof는 진짜 배열이라 32
    packet.level = level;
    return std::string(packet.name);   // 받는 쪽은 널 문자까지를 이름으로 읽는다
}
```
**교훈:** 배열은 함수 매개변수가 되는 순간 포인터로 바뀐다(배열 붕괴). 크기는 같이 넘기거나 `char (&dst)[N]` 템플릿으로 받는다. C 문자열 복사는 "길이 + 널 문자 1"과 "버퍼 크기 - 1로 자르기"를 둘 다 챙겨야 한다. clang은 `-Wsizeof-pointer-memaccess`로 이미 경고한다. 패킷 밖에서는 `std::string`.

## Lab 80 — 1부터 세는 스킬 번호로 `std::array` 인덱스, 옆 멤버 `gold`를 덮음
```
틀린 결과:     Debug: CRASH, "array subscript out of range" / Release: 2번 스킬 레벨 0, 골드 801 (기대 800)
멈춘 위치:     Debug: learnSkill의 hero.skillLevels[skillId] 줄 (어설션) / Release: hero.gold 데이터 중단점이 같은 줄에서 멈춤
진짜 원인:     화면 번호 1~4를 그대로 인덱스로 씀. skillLevels[4]는 배열 바로 뒤의 gold
```
**수정:** 화면 번호(1~4)를 인덱스(0~3)로 바꿔서 쓴다. `of05_skill_tree.cpp`의 `learnSkill` 함수 전체를 다음으로 바꾼다.
```cpp
bool learnSkill(Hero& hero, int skillId)
{
    if (skillId < 1 || skillId > 4 || hero.gold < kSkillCost)
        return false;
    hero.skillLevels[skillId - 1] += 1;   // 화면 번호 1~4 → 인덱스 0~3
    hero.gold -= kSkillCost;
    return true;
}
```
**교훈:** `operator[]`는 범위를 검사하지 않는다. Debug 구성의 표준 라이브러리 검사는 보험일 뿐이고(Release CRT로 빌드한 ASan에도 없음), 같은 객체 안의 넘침은 ASan도 대개 못 잡는다. 확신이 없는 인덱스에는 `at()`. "어디서 바뀌었는지 모르는 값"에는 데이터 중단점(랩 18).

---

# 10부: 비동기와 동시성

## Lab 81 — 버린 future가 비동기를 순차로 만듦
```
메인 스레드:  startLoading의 for 문 → std::future<void>::~future → 작업이 끝나기를 기다림
작업 스레드:  한 번에 하나만 ChunkDisk::read 안에 있음 (300 ms 기다린 뒤 콜드 읽기)
진짜 원인:    std::async의 반환값을 저장하지 않아 임시 future가 그 줄 끝에서 파괴됨.
              std::launch::async로 만든 future의 소멸자는 작업이 끝날 때까지 기다린다
```
**수정:** `std::async`가 돌려준 future를 `jobs_`에 모아 둔다. 그러면 `finishLoading`이 모아 둔 future를 기다린다. `as01_loading.cpp`의 `startLoading` 함수 전체를 다음으로 바꾼다.
```cpp
void startLoading()
{
    for (int i = 0; i < static_cast<int>(chunks_.size()); ++i)
        jobs_.push_back(std::async(std::launch::async, [this, i] { chunks_[i] = disk_.read(i); }));
}
```
**구성별:** 어느 컴파일러에서나 같다(표준이 정한 동작). C++20부터 `std::async`는 `[[nodiscard]]`라서 경고(MSVC C4858, GCC·Clang `-Wunused-result`)가 이 줄을 가리킨다.
**교훈:** "비동기로 띄웠다"와 "기다리지 않는다"는 다르다. future를 어디에 두고 언제 기다리는지가 코드의 실행 순서를 정한다.

## Lab 82 — 비동기 작업의 예외는 get()에서 나옴
```
예외가 던져질 때 멈춤:  작업 스레드의 craft("Mithril Helm") throw
abort 시점 호출 스택:   메인 스레드 craftAll → std::future<int>::get → 예외 다시 던지기
진짜 원인:             try/catch가 std::async 호출만 감쌌다. 작업 안의 예외는 future에 담겼다가 get()에서 던져진다
```
**수정:** `get()`을 try 안으로 옮긴다. `craftAll`의 첫 반복문은 try 없이 작업만 띄우고, 둘째 반복문이 결과를 꺼내면서 예외를 잡는다. `as02_crafting.cpp`의 `craftAll` 함수 전체를 다음으로 바꾼다.
```cpp
CraftReport craftAll(const std::vector<Recipe>& orders, int oreInStock)
{
    CraftReport report;
    std::vector<std::future<int>> jobs;
    for (const Recipe& recipe : orders)
        jobs.push_back(std::async(std::launch::async, craft, recipe, oreInStock));

    for (std::future<int>& job : jobs)
    {
        try
        {
            report.revenue += job.get();   // 작업 안에서 던진 예외는 여기서 다시 던져진다
        }
        catch (const std::exception& e)
        {
            std::cout << i18n::L("  제작 실패: ", "  製作失敗: ") << e.what() << '\n';
            ++report.failed;
        }
    }
    return report;
}
```
(`std::async` 자체도 스레드를 만들지 못하면 `std::system_error`를 던지므로 첫 반복문의 원래 try/catch를 남겨 두어도 된다.)
**교훈:** 비동기 작업의 예외는 **결과를 꺼내는 곳**에서 처리한다. `get()`은 future 하나에 한 번만 부를 수 있다. 두 번째 호출 전에는 `valid()`가 false이고, 그때 `get()`을 부르는 것은 정의되지 않은 동작이다(구현 대부분은 `std::future_error`를 던진다).

## Lab 83 — 조건 없는 condition_variable 대기
```
모두 중단:  공대원 스레드는 모두 끝남. lab-worker(공대장)는 ReadyCheck::waitForAll의 wait 안
조사식:     readyCount_ == 4, memberCount_ == 4  → 기다릴 이유가 없다
진짜 원인:  notify_one은 공대장이 wait하기 전에 이미 네 번 불렸다. 알림은 저장되지 않으므로 사라졌다
```
**수정:** 조건과 함께 기다린다. 조건이 이미 참이면 기다리지 않고, 가짜로 깨어나도 조건이 거짓이면 다시 기다린다. `as03_raid_ready.cpp`의 `waitForAll` 함수 전체를 다음으로 바꾼다.
```cpp
void waitForAll()
{
    std::unique_lock<std::mutex> lock(mutex_);
    allReady_.wait(lock, [&] { return readyCount_ >= memberCount_; });
}
```
**구성별:** 어느 구성에서나 TIMEOUT. `if (readyCount_ < memberCount_) allReady_.wait(lock);`처럼 한 번만 검사하면 화면은 통과하지만, 공대장이 먼저 기다리는 숨은 케이스에서 첫 알림에 깨어나 3명 중 1명만 준비한 채 입장한다(FAIL).
**교훈:** `condition_variable`은 "무엇인가 바뀌었을 수 있다"는 신호일 뿐이다. 기다리는 **조건**은 mutex로 지키는 변수에 있고, `wait`는 항상 그 조건과 함께 쓴다.

## Lab 84 — 떼어 낸 스레드의 [this]
```
중단점:     board.post(bid_)  (작업 스레드. 호출 스택에 bidAndCloseWindow 없음)
조사식:     bid_ == -286331154 (0xEEEEEEEE = 해제된 메모리)
진짜 원인:  람다가 this를 캡처했는데, AuctionWindow(unique_ptr)는 bidAndCloseWindow가 끝날 때 지워졌다.
            람다는 마감 종이 울린 뒤, 창이 사라진 다음에 실행된다
```
**수정:** 필요한 값만 복사해 간다. `as04_auction_notice.cpp`의 `subscribeClosingNotice` 함수 전체를 다음으로 바꾼다.
```cpp
void subscribeClosingNotice(ClosingBell& bell, NoticeBoard& board)
{
    std::thread([bid = bid_, &bell, &board] {
        bell.waitForRing();
        board.post(bid);
    }).detach();
}
```
(종과 게시판은 서버가 켜져 있는 동안 사는 객체라 참조로 둔다. 그 약속이 없다면 `shared_ptr`로 함께 소유한다.)
스레드를 떼어 내지 않는 설계도 있지만 조건이 붙는다. 창이 `std::jthread`를 소유하고 소멸자에서 기다리게 하면, 이 랩처럼 종이 울리기 전에 창이 닫히는 흐름에서는 소멸자가 종을 기다리며 멈춘다(TIMEOUT). 그 방법은 창이 종이 울린 뒤에 닫힐 때만 맞다. 이 흐름에서는 알림을 서버 쪽 객체에 등록하는 편이 맞다.
**구성별:** Debug와 Release는 메모리 검사기의 `0xEE`로 FAIL. ASan은 작업 스레드에서 heap-use-after-free(해제한 곳은 메인 스레드의 `bidAndCloseWindow`)로 CRASH.
**교훈:** `detach`는 "이 스레드의 수명을 아무도 책임지지 않는다"는 뜻이다. 떼어 낸 스레드에 넘긴 람다는 캡처한 모든 것이 그 스레드보다 오래 사는지 확인한다(9부 랩 75와 같은 실수가 스레드로 옮겨 온 것).

## Lab 85 — atomic 확인 후 행동 레이스
```
조건부 중단점:  --stock_; 에서 stock_ <= 0 일 때 멈춤
스레드 목록:    다섯 구매 스레드가 모두 stock_.load() > 0 검사를 통과해 payment_.charge에서 기다리는 중이었다
진짜 원인:      "남았나 확인 → 결제 → 하나 빼기"가 원자적이지 않다. atomic 하나하나는 안전하지만,
                확인과 빼기 사이에 다른 스레드가 모두 확인을 끝낼 수 있다
```
**수정:** 결제하기 전에 수량을 원자적으로 확보한다. `as05_limited_sale.cpp`의 `LimitedShop::buy` 함수 전체를 다음으로 바꾼다.
```cpp
bool buy(const std::string& player)
{
    int left = stock_.load();
    do
    {
        if (left <= 0)
            return false;
    } while (!stock_.compare_exchange_weak(left, left - 1));

    payment_.charge(player, price_);
    ++sold_;
    return true;
}
```
(`fetch_sub`로 빼고 결과가 0 이하면 되돌리는 방법도 있다. 결제가 실패할 수 있다면 실패했을 때 수량을 돌려놓는다.) `buy` 전체를 `std::mutex`로 감싸도 맞지만, 결제를 기다리는 동안 다른 구매자가 모두 줄을 선다.
**구성별:** 어느 구성에서나 판매 5, 수량 -2. 랩 17과 달리 **데이터 레이스가 아니다.** 모든 접근이 `atomic`이라 ThreadSanitizer도 보고하지 않는다.
**교훈:** 불변식("판매 수 + 남은 수량 = 처음 수량", "수량 ≥ 0")은 그것을 이루는 값 전체를 한 번에 바꾸는 연산으로 지킨다. 변수마다 `atomic`을 붙인다고 불변식이 지켜지지 않는다.

## Lab 86 — join 없이 파괴된 std::thread
```
호출 스택:  std::terminate ← std::thread::~thread ← runMatchmaking의 return result; (둘째 대기열)
진짜 원인:  인원이 모자라 취소하는 경로가 matcher.join() 없이 함수를 빠져나간다.
            joinable한 std::thread의 소멸자는 std::terminate를 부른다
```
**수정:** 취소 경로에서도 기다린다. 결과는 스레드가 끝난 뒤에 고친다(아직 도는 스레드가 `result.matches`를 쓰고 있으므로). `as06_matchmaking.cpp`의 `runMatchmaking` 함수 전체를 다음으로 바꾼다.
```cpp
MatchResult runMatchmaking(const std::vector<std::string>& queue)
{
    MatchResult result;
    std::atomic<bool> cancel{ false };
    std::thread matcher(groupPlayers, std::cref(queue), std::cref(cancel), std::ref(result.matches));

    if (queue.empty() || queue.size() % 4 != 0)
    {
        cancel = true;
        matcher.join();
        result.cancelled = true;
        result.matches = 0;
        return result;
    }

    matcher.join();
    return result;
}
```
`std::jthread`로 바꾸면 모든 빠져나가는 길에서 자동으로 멈춰 달라고 요청하고 join한다. 다만 jthread는 함수가 끝날 때 join하므로, 위처럼 "멈추고 기다린 뒤 결과를 고치기"가 필요하면 `request_stop()`과 `join()`을 직접 부른다.
**교훈:** 스레드 객체도 리소스다. 예외와 이른 `return`을 포함한 모든 길에서 정리되도록 RAII(`std::jthread`)를 쓴다.

## Lab 87 — 코루틴의 참조 매개변수
```
중단점:     talkTo의 첫 줄 (호출 스택: as07_cutscene → Cutscene::step → 코루틴 재개. startVillageScene은 이미 끝남)
조사식:     npc.affinity == -286331154 (0xEEEEEEEE)
진짜 원인:  talkTo는 initial_suspend에서 바로 멈추고, 코루틴 프레임에는 npc의 참조만 저장된다.
            startVillageScene이 끝나면서 Npc(unique_ptr)가 지워진 뒤에 엔진이 코루틴을 처음 실행한다
```
**수정:** 코루틴이 쓸 값은 프레임 안으로 복사해 간다. `npc`를 값으로 받는다(`log`는 호출한 쪽이 컷신보다 오래 들고 있으므로 참조로 둔다). `as07_cutscene.cpp`의 `talkTo` 함수 전체를 다음으로 바꾼다.
```cpp
Cutscene talkTo(Npc npc, DialogueLog& log)
{
    log.affinityShown.push_back(npc.affinity);
    co_await std::suspend_always{};
    co_await std::suspend_always{};
    log.affinityShown.push_back(npc.affinity + 10);   // 선물로 호감도 +10
}
```
**구성별:** Debug와 Release는 메모리 검사기의 `0xEE`로 FAIL. ASan은 heap-use-after-free로 CRASH. MSVC 14.51과 Clang 22에서 확인했다.
**교훈:** 코루틴의 참조 매개변수는 "처음 멈추는 곳까지"만 안전하다. 멈췄다 이어지는 코루틴은 람다를 나중에 실행하는 것과 같은 수명 문제를 가진다.

---

# 11부: 멈춤 - 무한루프와 데드락

## Lab 88 — float 누적과 != 종료 조건
```
모두 중단:     랩 작업 스레드 → ticksUntilReady. 다시 모두 중단할 때마다 ticks가 커져 있음 (무한루프)
조건부 중단점: ticks == 15 일 때 t == 1.50000024, cooldownSeconds == 1.5
진짜 원인:     0.1은 2진수로 정확히 나타낼 수 없어서 0.1f를 15번 더해도 정확히 1.5f가 되지 않는다.
               t != cooldownSeconds 는 영원히 참. (대시의 0.5초는 우연히 딱 맞아서 끝났다)
```
**수정:** 반복 횟수를 정수로 계산한다. `hg01_cooldown.cpp`의 `ticksUntilReady` 함수를 통째로 바꾼다(`ticks` 변수와 반복문은 사라진다).
먼저 같은 파일 맨 위 `#include`에 다음을 추가한다.
```cpp
#include <cmath>
```
`hg01_cooldown.cpp`의 `ticksUntilReady` 함수 전체를 다음으로 바꾼다.
```cpp
int ticksUntilReady(float cooldownSeconds, float tickSeconds)
{
    return static_cast<int>(std::lround(cooldownSeconds / tickSeconds));
}
```
`t < cooldownSeconds`로만 바꾸면 멈추지는 않지만 오차 때문에 한 틱을 더 센다. 예를 들어 1.2초 / 0.05초는 24가 아니라 25가 나온다(채점기의 숨은 케이스가 이것을 잡는다).
**교훈:** 실수는 `==`, `!=`로 비교하지 않는다. "몇 번 반복하는가"는 정수로 센다.

## Lab 89 — unsigned의 s >= 0
```
모두 중단:   s가 12보다 훨씬 큰 수 (수십억). 조건부 중단점 s == 0 에서 한 줄 실행하면 4294967295 (0xFFFFFFFF)
진짜 원인:   unsigned에는 음수가 없어서 s >= 0 이 항상 참. 0에서 --s 하면 4294967295로 돌아간다.
```
**수정:** 비교하기 전에 줄이는 `s-- > 0` 형태로 센다. `hg02_respawn.cpp`의 `countAnnouncements` 함수 전체를 다음으로 바꾼다.
```cpp
int countAnnouncements(unsigned int waitSeconds)
{
    int count = 0;
    for (unsigned int s = waitSeconds + 1; s-- > 0;)
    {
        if (s % 5 == 0 || s <= 3)
            ++count;
    }
    return count;
}
```
(부호 있는 정수로 세어도 된다: `for (int s = static_cast<int>(waitSeconds); s >= 0; --s)`.)
**교훈:** 이 실수를 알려 주는 "식이 항상 true" 경고는 기본 경고 수준에서 보이지 않는 경우가 많다. MSVC C4296은 `/W4`에서도 꺼져 있어 `/w14296`으로 켜고, GCC는 `-Wtype-limits`(`-Wextra`에 포함), Clang은 `-Wtautological-unsigned-zero-compare`로 켠다. 거꾸로 세는 반복에서 `unsigned`/`size_t`를 보면 의심한다.

## Lab 90 — continue가 ++i를 건너뜀
```
모두 중단:   i == 6 (첫 공백), c == ' ', words == { "/party" }. 한 줄씩 계속 실행해도 i가 그대로
진짜 원인:   공백 분기가 i를 늘리지 않고 continue → 같은 공백을 영원히 다시 읽는다
```
**수정:** `continue;` 앞에 `++i;`를 넣는다. `hg03_chat_command.cpp`의 `splitCommand` 함수 전체를 다음으로 바꾼다.
```cpp
std::vector<std::string> splitCommand(const std::string& line)
{
    std::vector<std::string> words;
    std::string current;

    size_t i = 0;
    while (i < line.size())
    {
        const char c = line[i];
        if (c == ' ')
        {
            if (!current.empty())
            {
                words.push_back(current);
                current.clear();
            }
            ++i;
            continue;
        }
        current += c;
        ++i;
    }
    if (!current.empty())
        words.push_back(current);
    return words;
}
```
(`for (char c : line)`처럼 인덱스를 직접 다루지 않는 반복으로 바꿔도 된다.)
**교훈:** `while` 반복의 모든 경로가 루프 변수를 앞으로 움직이는지 확인한다. 다음 문 설정으로 `++i;`를 한 번 실행시켜 보면 가설을 바로 시험할 수 있다.

## Lab 91 — 끼워 넣기 순서가 바뀌어 자기 자신을 가리킴
```
모두 중단:   countStops 안, stops가 수천만, p->name == "Forest"
조사식:      p == p->next  (Forest의 next가 자기 자신)
진짜 원인:   pos->next = wp; 를 먼저 하면 다음 줄 wp->next = pos->next 가 방금 넣은 wp 자신을 읽는다
```
**수정:** 순서를 바꾼다. `hg04_route.cpp`의 `insertAfter` 함수 전체를 다음으로 바꾼다.
```cpp
void insertAfter(Waypoint* pos, Waypoint* wp)
{
    wp->next = pos->next;
    pos->next = wp;
}
```
방어하려면 순회할 때 순환을 검사한다(토끼와 거북이: 한 칸씩 가는 포인터와 두 칸씩 가는 포인터가 만나면 순환).
**교훈:** 연결 리스트에서 무한루프가 나면 조사식에 `p`, `p->next`, `p->next->next`를 넣고 **주소**를 비교한다.

## Lab 92 — 잠금 순서가 엇갈리는 데드락
```
병렬 스택:   trade 스레드 2개가 모두 lock_guard → mutex lock 에서 대기 (CPU 0%)
스레드 A:    seller = Kael. Kael.lock을 가졌고 Mira.lock을 기다림
스레드 B:    seller = Mira. Mira.lock을 가졌고 Kael.lock을 기다림
진짜 원인:   "판매자 먼저, 구매자 나중"이라는 잠금 순서는 거래 방향에 따라 뒤집힌다.
             반대 방향 거래가 동시에 오면 서로 상대가 가진 잠금을 기다린다 (원형 대기).
```
**수정:** 두 mutex를 한꺼번에 잠근다. `std::scoped_lock`은 교착을 피하는 순서로 잠근다. 원래의 `std::lock_guard<std::mutex> sellerLock(seller.lock);`과 `std::lock_guard<std::mutex> buyerLock(buyer.lock);`은 둘 다 없어진다(남겨 두면 같은 mutex를 두 번 잠근다). `hg05_trade.cpp`의 `trade` 함수 전체를 다음으로 바꾼다.
```cpp
void trade(Inventory& seller, Inventory& buyer, const std::string& item, int price)
{
    std::scoped_lock locks(seller.lock, buyer.lock);
    if (!verifyItem(seller, item))
        return;
    if (buyer.gold < price)
        return;

    seller.items.erase(std::find(seller.items.begin(), seller.items.end(), item));
    buyer.items.push_back(item);
    buyer.gold -= price;
    seller.gold += price;
}
```
또는 언제나 같은 기준(플레이어 ID가 작은 쪽, 주소가 작은 쪽)으로 먼저 잠근다. 숨은 케이스의 세 명 원형 거래도 이 방법으로 풀린다.
위조 검사를 잠금 전으로 옮기기만 하면 50ms의 틈이 줄어 대부분 통과하지만, 잠금 순서 문제는 그대로라 아주 가끔만 멈추는 더 찾기 어려운 버그가 된다. 게다가 잠금 없이 검사하면 검사와 거래 사이에 아이템이 바뀔 수 있다.
**교훈:** 데드락은 "누가 무엇을 가졌고 무엇을 기다리는가"를 스레드마다 적어 보면 고리가 보인다. 여러 잠금은 한 번에, 또는 항상 같은 순서로 잡는다.

---

# 12부: 성능

12부는 모두 "결과는 맞는데 일을 너무 많이 한다"입니다. 기록표의 **세는 값**이 수정 전후로 어떻게 바뀌는지 함께 적었습니다.

## Lab 93 — vector 앞에서 하나씩 지우기 (O(n²))
```
세는 값:     원소 이동 31,996,000번 (메시지 8000개) → 16000개면 약 4배
핫 경로:     ChatQueue::drain → vector::erase → Message 이동 대입 / memmove
진짜 원인:   queue_.erase(queue_.begin())는 뒤의 원소를 전부 한 칸씩 당긴다. n개를 이렇게 꺼내면 n²/2번 이동
```
**수정:** 지우지 말고 끝까지 읽은 뒤 한 번에 비운다. `pf01_chat_queue.cpp`의 `ChatQueue::drain` 함수 전체를 다음으로 바꾼다.
```cpp
long long drain()
{
    long long chars = 0;
    for (const Message& m : queue_)
        chars += static_cast<long long>(m.text.size());
    queue_.clear();
    return chars;
}
```
(앞에서 꺼내는 일이 잦다면 `std::deque`의 `pop_front`도 된다. 이동 0번.)
**수정 후:** 원소 이동 0번.
**교훈:** "앞에서 하나씩 지우기"는 vector에서 가장 흔한 O(n²)다. 크기를 두 배로 해서 시간이나 횟수가 네 배가 되면 의심한다.

## Lab 94 — 큰 객체를 값으로 넘기고 값으로 돌기
```
세는 값:     Item 복사 144,000번 = 120개 × 600프레임 × 2
복사 생성자 중단점의 호출 스택:  (1) drawHud → totalWeight 호출(인자 복사)  (2) totalWeight의 for (Item item : items)
진짜 원인:   totalWeight(std::vector<Item> items)가 벡터 전체를 복사하고, 범위 for가 원소를 또 하나씩 복사한다.
             Item마다 200글자 설명 문자열이 있어 복사할 때마다 할당한다
```
**수정:** 매개변수를 `const&`로 받고, 범위 for도 `const&`로 돈다. `pf02_loadout.cpp`의 `totalWeight` 함수 전체를 다음으로 바꾼다.
```cpp
int totalWeight(const std::vector<Item>& items)
{
    int total = 0;
    for (const Item& item : items)
        total += item.weight;
    return total;
}
```
**수정 후:** Item 복사 0번. 둘 중 하나만 고치면 72,000번이 남는다.
**교훈:** 읽기만 하는 큰 인자는 `const&`, 범위 for도 `const auto&`가 기본이다.

## Lab 95 — 루프 안의 s = s + ...
```
세는 값:     할당 약 250MB (결과 58KB). 이벤트를 2배로 하면 약 4배
핫 경로:     exportLog → operator+ → 문자열 복사(memcpy), 할당/해제
진짜 원인:   log = log + "[" + ... 는 매 줄마다 지금까지의 log 전체를 복사한 임시 문자열을 만들고, 이어 붙일 때마다 또 커진다.
             4000번째 줄을 붙일 때는 6만 글자가 여러 번 복사된다
```
**수정:** 기존 문자열 뒤에 붙인다(`+=`는 용량이 모자랄 때만 키운다). 반복문 몸통이 여러 문장이 되므로 중괄호로 묶는다. `pf03_combat_log.cpp`의 `exportLog` 함수 전체를 다음으로 바꾼다.
```cpp
Text exportLog(const std::vector<DamageEvent>& events)
{
    Text log;
    for (const DamageEvent& e : events)
    {
        log += "[";
        log += toText(e.tick);
        log += "] hit ";
        log += toText(e.amount);
        log += "\n";
    }
    return log;
}
```
(`log += "[" + toText(e.tick) + "] hit " + toText(e.amount) + "\n";`처럼 한 줄짜리 임시만 만드는 것도 통과한다. 크기를 안다면 `log.reserve(...)`로 할당을 한 번으로 줄일 수 있다.)
**수정 후:** Release 기준 할당 20번, 약 200KB. Debug는 반복자 디버깅용 보조 할당 때문에 횟수가 더 많지만 결과 길이에 비례한다.
**교훈:** 문자열을 누적할 때 `s = s + x`는 O(n²), `s += x`는 분할 상환 O(n)이다.

## Lab 96 — std::endl은 매번 flush
```
세는 값:     flush 5002번 (오브젝트 5000개 + 머리줄 + 마지막 flush)
FakeLogFile::sync 중단점의 호출 스택:  writeSave의 << std::endl 두 줄
진짜 원인:   std::endl = '\n' + flush. 줄마다 디스크 쓰기를 한다
```
**수정:** 두 `std::endl`을 `'\n'`으로 바꾸고, 마지막 `out.flush()`는 남긴다. `pf04_save_log.cpp`의 `writeSave` 함수 전체를 다음으로 바꾼다.
```cpp
void writeSave(std::ostream& out, const std::vector<WorldObject>& objects)
{
    out << "SAVE v3 " << objects.size() << '\n';
    for (const WorldObject& o : objects)
        out << o.id << ' ' << o.x << ' ' << o.y << '\n';
    out.flush();   // 다 쓴 뒤 한 번 디스크로 내보낸다
}
```
**수정 후:** flush 1번. (마지막 flush까지 지우면 0번이 되어 FAIL. 다 쓴 뒤 한 번은 내보내야 한다.)
**교훈:** 줄바꿈은 `'\n'`, flush는 필요할 때 한 번. 콘솔 출력도 같다.

## Lab 97 — 열 우선으로 도는 행 우선 배열
```
세는 값:     건너뛴 접근 3,999,999번 (거의 모든 접근)
조건부 중단점 (jumps_ < 5):  row가 0, 1, 2...로 바뀌고 col은 그대로 → index가 cols(2000)씩, 8000바이트씩 건너뜀
진짜 원인:   칸은 한 행씩 이어서 놓여 있는데 countFog는 열을 바깥, 행을 안쪽 반복으로 돈다.
             매 접근이 다른 캐시 줄이라 메모리에서 새로 읽어 온다. reveal은 행 우선이라 빠르다
```
**수정:** 반복 순서를 바꾼다(행이 바깥, 열이 안쪽). `pf05_fog_map.cpp`의 `countFog` 함수 전체를 다음으로 바꾼다.
```cpp
FogStats countFog(FogMap& map)
{
    FogStats s{ 0, 0 };
    for (int r = 0; r < map.rows(); ++r)
    {
        for (int c = 0; c < map.cols(); ++c)
        {
            const int v = map.at(r, c);
            s.brightness += v;
            if (v >= 128)
                ++s.revealed;
        }
    }
    return s;
}
```
**수정 후:** 건너뛴 접근 0번.
**교훈:** 2차원 배열은 메모리에 놓인 순서대로 돈다. 프로파일러는 "countFog가 느리다"까지만 알려 주고, 왜 느린지는 같은 일을 하는 reveal과 비교해야 보인다.

## Lab 98 — shared_ptr를 값으로 넘기고 값으로 돌기
```
세는 값:     한 몬스터를 동시에 붙잡은 shared_ptr 최대 3개 (목록 + 범위 for의 m + threatOf의 인자)
조건부 중단점 (use_count() > 1)의 호출 스택:  mostDangerous의 for (MonsterPtr m : monsters), threatOf(MonsterPtr monster)
진짜 원인:   shared_ptr 복사마다 참조 카운트를 원자적으로 올리고 내린다. 3000마리 × 300틱 × 2번
```
**수정:** 두 곳 모두 `const&`로 바꾼다. `pf06_aggro_scan.cpp`의 `threatOf` 함수 전체를 다음으로 바꾼다.
```cpp
int threatOf(const MonsterPtr& monster)
{
    monster->noteHolders();
    return monster->level * 10 - monster->distance;
}
```
같은 파일의 `mostDangerous` 함수 전체를 다음으로 바꾼다.
```cpp
MonsterPtr mostDangerous(const std::vector<MonsterPtr>& monsters)
{
    MonsterPtr best;
    int bestThreat = -1000000;
    for (const MonsterPtr& m : monsters)
    {
        const int threat = threatOf(m);
        if (threat > bestThreat)
        {
            bestThreat = threat;
            best = m;
        }
    }
    return best;
}
```
(`threatOf(const Monster& monster)`처럼 소유권과 상관없는 함수는 객체 참조를 받아도 된다.)
**수정 후:** 최대 1개. 둘 중 하나만 고치면 2개가 남는다. `best = m;`은 가장 위험한 몬스터가 바뀔 때만 복사하므로 그대로 두어도 된다.
**교훈:** 소유권을 넘기거나 나눠 가질 때만 `shared_ptr`를 값으로 넘긴다. 읽기만 하면 `const&`나 객체 참조로 충분하다.
