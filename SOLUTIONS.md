# 해답 (스포일러!)

1~12부의 해답입니다. 챌린지(C, B)의 해답은 `CHALLENGE_SOLUTIONS.md`에 따로 있습니다.

# 1부: 호출 스택 입문

## Lab 1 — nullptr 역참조
```
멈춘 함수:  printPlayer           이상한 값: player == nullptr
값의 출처:  showProfile ← findPlayer가 못 찾아서 nullptr 반환
진짜 원인:  openFriendList에서 "Doran"을 "Dorna"로 오타
```
**수정:** 오타 수정. 방어도 추가: `if (!player) { std::cout << "  " << name << ": 없는 플레이어\n"; return; }`
**교훈:** 죽은 곳(printPlayer)은 아무 잘못이 없다. nullptr를 **돌려받고 검사하지 않은** 곳과 **잘못된 이름을 넘긴** 곳이 원인.

## Lab 2 — 1부터 세는 키를 0부터 세는 인덱스로 그대로 사용
```
멈춘 함수:  [외부 코드] std::out_of_range 던지기 ← vector::at → 첫 내 코드: QuickSlotBar::itemAt(slotIndex = 4)
값의 출처:  onNumberKey(key = 4) ← replayInput
진짜 원인:  키 번호(1~4)를 슬롯 인덱스(0~3)로 바꾸지 않음
```
**수정:** `useItem(bar.itemAt(key - 1));`
**교훈:** 외부 코드는 건너뛰고 내 코드 첫 줄부터. 그리고 크래시 전에 나온 "틀린 출력"도 같은 버그의 증상이다.

## Lab 3 — 정수 나눗셈으로 0초가 됨
```
멈춘 함수:  damagePerSecond(seconds = 0)이 std::invalid_argument를 던짐 (예외가 던져질 때 멈춤을 켜면 throw 줄에서 멈춘다)
값의 출처:  printBattleSummary: battle = {"Slime", 35, 800}, toSeconds(800) == 0
진짜 원인:  1초 미만 전투가 정수 나눗셈으로 0초가 됨
```
**수정:** 밀리초로 계산: `totalDamage * 1000 / durationMs` (그리고 `durationMs == 0` 검사), 또는 `double`로 계산.
**교훈:** 값의 출처를 여러 프레임 거슬러 올라가야 한다. 0으로 나눈 함수는 받은 값을 썼을 뿐이다.

## Lab 4 — 검사가 한 경로에만 있음
```
멈춘 함수:  leaderOf ← sendRaidReminders  (printPartyBoard가 아님!)
이상한 값:  party.name == "Silent Hollow", members 비어 있음
```
**수정:** `sendRaidReminders`에도 `if (party.members.empty()) continue;`. 더 좋은 방법은 `leaderOf`가 빈 파티를 안전하게 처리하도록 하는 것 (`const std::string*` 반환, `std::optional` 등).
**교훈:** 같은 함수라도 **어느 경로로 불렸는지**는 호출 스택만이 정확히 알려 준다.

## Lab 5 — 종료 조건을 건너뛰는 재귀
```
멈춘 함수:  countdown(n = -수만) 이 수만 번 반복 → 스택 오버플로
값의 출처:  n이 홀수 → startFireworks(seconds = 5)
진짜 원인:  5, 3, 1, -1, ... 로 0을 건너뛰어 끝나지 않음
```
**수정:** `if (n <= 0) return "발사!";`
**교훈:** 스택이 같은 함수로 가득 차면 재귀가 끝나지 않는 것. 매개변수 값의 변화를 보면 이유가 보인다.

## Lab 6 — 없는 키로 map::at
```
멈춘 함수:  [외부 코드] std::out_of_range 던지기 ← std::map::at → 첫 내 코드: priceOf(itemId = "mana_poiton")
값의 출처:  cartTotal ← checkout의 장바구니 목록
진짜 원인:  "mana_potion" 오타
```
**수정:** 오타 수정. 방어: `auto it = kPrices.find(itemId); if (it == kPrices.end()) { /* 경고 후 0 */ }`
**교훈:** 예외도 호출 스택으로 추적한다. throw한 곳 → 처음 나오는 내 코드.

## Lab 7 — 너무 일찍 캡처한 포인터
```
멈춘 함수:  HealthBar::setValue (this == nullptr)
아래 프레임: connectHealthBar의 람다 — 캡처된 bar == nullptr
          ← std::function (외부 코드) ← EventBus::publishHpChanged ← startGame
진짜 원인:  startGame에서 connectHealthBar(game)를 createUi(game)보다 먼저 호출.
           그 순간 game.healthBar.get()은 nullptr였고, 람다는 그 값을 복사해 저장했다.
```
**수정:** `createUi(game);`를 `connectHealthBar(game);` 위로 옮긴다. 또는 람다가 `&game`을 캡처해서 호출 시점에 `game.healthBar`를 확인하게 한다.
**교훈:** 호출 스택은 **지금 어떻게 여기 왔는지**만 보여 준다. "그 값이 **언제** 저장됐는지"는 중단점을 걸고 다시 실행해서 찾는다.

## Lab 8 — 인자 순서가 뒤바뀜
```
멈춘 함수:  [외부 코드] std::out_of_range 던지기 ← std::array::at → statsFor(level = 200)  (테이블은 0~50)
값의 출처:  createMonster(level = 200) ← spawnMonster(name = "Ogre Boss", level = 200, bonusHp = 13)
          ← spawnWave(wave = 3)
진짜 원인:  spawnMonster("Ogre Boss", 200, 10 + wave)
           → 의도는 레벨 10 + wave, 보너스 HP 200
```
**수정:** `spawnMonster("Ogre Boss", 10 + wave, 200);`
**교훈:** 같은 타입(int, int) 매개변수가 나란히 있으면 순서를 바꿔 넣기 쉽다. 강한 타입(`struct Level { int value; };`)이나 지정 초기화로 예방할 수 있다.

---

# 2부: 심화

## Lab 9 — off-by-one
`i <= scores.size()` → 마지막 반복에서 `scores.at(5)` 범위 밖 접근 → `std::out_of_range`.
**수정:** `i < scores.size()` 또는 범위 기반 for / `std::accumulate`.
**교훈:** 예외(또는 어설션)에서 멈춤 → 호출 스택 → 첫 내 코드는 가장 기본적인 추적 루트. `[]` 대신 `at()`을 쓰면 어느 컴파일러, 어느 구성에서나 범위 밖 접근이 예외로 드러난다.

## Lab 10 — 참조 무효화 (vector 재할당)
`add`가 `items_.back()`의 참조를 돌려주는데, 이후 `push_back`에서 capacity가 늘면 원소들이 새 메모리로 옮겨지고 `sword`는 해제된 메모리를 가리킨다(use-after-free).
**수정:** 참조를 오래 들고 있지 말고 이름/인덱스로 다시 찾기, 또는 `reserve`, 또는 `std::deque`/`std::list`처럼 주소가 안정적인 컨테이너.
**교훈:** "참조/포인터/이터레이터를 들고 있는 동안 컨테이너를 바꾸지 말 것."

## Lab 11 — 임시 객체를 가리키는 string_view
`makeTitle`이 만든 `std::string`은 `titleOf`의 return 문이 끝나면 파괴되고, `string_view`는 그 해제된 버퍼를 가리킨다.
**수정:** `std::string`으로 리턴. `string_view`는 소유자가 더 오래 살아 있을 때만.

## Lab 12 — 초기화 안 된 멤버
`int combo_;`에 초기값이 없다. 카운터는 오브젝트 풀(`bughunt::recycledStorage`)의 재사용 메모리 위에 만들어지는데, 이 자리에는 전에 쓰던 값(BugHunt에서는 `0xA5A5A5A5` = -1515870811)이 남아 있어서 처음 4연속 명중이 음수로 기록되어 무시된다. 평범한 지역 변수였다면 컴파일러와 구성에 따라 우연히 0이라 맞게 나올 수도 있다.
**수정:** `int combo_ = 0;`
**교훈:** 생성자가 모든 멤버를 초기화하는지 확인. 값이 실행마다, 구성마다 다르면 미초기화 변수를 1순위로 의심. (컴파일러의 "멤버를 초기화하지 않음" 정적 분석 경고. 예: MSVC 코드 분석 C26495, clang-tidy `cppcoreguidelines-pro-type-member-init`)

## Lab 13 — erase 후 이터레이터 무효화
`erase(it)` 후 `it`는 무효인데 `++it`를 한다.
**수정:**
```cpp
for (auto it = enemies.begin(); it != enemies.end(); )
    it = (it->hp <= 0) ? enemies.erase(it) : it + 1;
// 목록이 std::vector라면 C++20 std::erase_if(enemies, [](const Enemy& e) { return e.hp <= 0; });로 한 줄에 된다.
```

## Lab 14 — unsigned 언더플로
`20u - 22u` = 4294967294. "남은 칸"이 엄청 커서 무엇이든 들어간다.
**수정:** `if (bag.usedSlots >= bag.maxSlots) return false;`를 먼저 검사하거나 부호 있는 타입으로 계산.

## Lab 15 — Rule of Three 위반 (이중 해제)
`PacketBuffer`는 raw 포인터를 소유하지만 복사 생성자/대입이 기본(얕은 복사)이다. 소멸자를 직접 선언했으므로 이동 생성자는 자동 생성되지 않아 `push_back(PacketBuffer(64))`에서 **복사**가 일어남 → 임시 객체 소멸 시 `data_` 해제 → 벡터 안 객체는 해제된 버퍼를 가리킨다 → **두 번째 `push_back`**에서 vector가 재할당하며 옛 원소를 파괴할 때 같은 버퍼를 또 해제(double free). 그 자리에서 메모리 검사기가 `[CRASH] 메모리 검사: 이미 해제한 메모리를 다시 해제했다`로 멈추므로 `expect` 줄까지 가지 못한다.
**수정(권장, Rule of Zero):** `std::vector<char> data_;` 또는 `std::unique_ptr<char[]>` 사용. 또는 복사 생성자/대입/이동을 직접 구현.

## Lab 16 — 가상 소멸자 누락 (메모리 누수 + UB)
`StatusEffect`의 소멸자가 virtual이 아니라 `unique_ptr<StatusEffect>`가 `delete`할 때 `Burn`의 소멸자가 불리지 않는다 → 멤버 `schedule_`(`DamageSchedule`)도 파괴되지 않아 전투 1000번 뒤 1000개가 살아 남는다(그 안의 벡터 메모리도 누수). 엄밀히는 정의되지 않은 동작이다.
**추적:** `DamageSchedule`의 소멸자에 중단점을 걸면 한 번도 멈추지 않는다. 생성자에는 멈춘다. → 누가 지워야 했나? `unique_ptr`의 `delete`가 `StatusEffect`의 소멸자만 부른다.
**수정:** `virtual ~StatusEffect() = default;`
**교훈:** 다형적으로 delete될 기반 클래스는 반드시 virtual 소멸자. (경고: MSVC C5205(표준 헤더 안 템플릿이라 `/external:templates-`가 있어야 보인다. 이 저장소의 CMake는 켜 둔다), Clang `-Wall`의 `-Wdelete-abstract-non-virtual-dtor`. GCC는 시스템 헤더 안이라 보이지 않을 수 있다)

## Lab 17 — 데이터 레이스
`gold_` 읽기-수정-쓰기가 원자적이지 않다. 두 스레드가 같은 값을 읽고 각각 +1을 쓰면 하나가 사라진다. 읽기와 쓰기 사이의 `std::this_thread::sleep_for(1ms)`는 그 틈을 넓혀 어느 컴파일러에서나 증상이 드러나게 할 뿐, 원인이 아니다(지워도 레이스는 남는다).
**수정:** `std::atomic<int> gold_{0};` + `gold_ += amount;` 또는 `std::mutex`로 보호.

## Lab 18 — 구조체 내부 버퍼 오버플로
`char name[8]`에 `"Sir Kael"`(8글자 + NUL = 9바이트)을 `memcpy` → 바로 뒤의 `hp` 첫 바이트를 0으로 덮음 → 100(0x64)이 0이 됨. `"Sir Doran"`은 10바이트라 `hp` 하위 2바이트를 `'n'`, `0`으로 덮어 120 → 110.
**수정:** `name`을 `std::string`으로 바꾸거나, 길이를 제한해서 복사 (`std::snprintf(c.name, sizeof(c.name), "%s", newName)`).
**교훈:** "누가 이 값을 바꿨지?"는 데이터 중단점으로 바로 답이 나온다.

## Lab 19 — 순환 데이터로 인한 무한 재귀 (스택 오버플로)
테이블에 `village_intro: forge_sword` 오타가 있어 `forge_sword → meet_blacksmith → village_intro → forge_sword ...` 순환.
**수정:** 데이터를 `village_intro:`로 고치고, 코드도 방어: 방문 집합(`std::set<std::string> visited`)으로 순환 감지 및 중복 제거.
**교훈:** 호출 스택이 같은 패턴으로 반복되면 재귀 종료 조건/데이터 순환을 의심.

## Lab 20 — 연산자 우선순위
`==`가 `&`보다 먼저 계산되어 `flags & (status == status)` = `flags & 1` → 항상 STUN 비트만 검사.
**수정:** `return (flags & status) == status;`
**교훈:** 컴파일러 경고(MSVC C4554, GCC·Clang `-Wparentheses`)가 이미 알려 주고 있었다. 경고를 0개로 유지하자.

---

# 3부: 흐름 추적 (크래시 없음)

## Lab 21 — 반복문 안에서 초기화되는 누적 변수
```
틀린 결과:     최장 연속 1, 골드 900 (보너스 없음)
처음 어긋난 줄: 둘째 바퀴의 int streak = 0;  (1이었던 값이 0으로 돌아감)
진짜 원인:     streak가 for 안에 선언되어 매 바퀴 새로 만들어짐
```
**수정:** `int streak = 0;`을 `for` 위로 옮긴다. (결석 처리 `streak = 0;`은 그대로 둔다.)
**교훈:** 크래시가 없으면 호출 스택은 쓸모없다. **기대값을 먼저 적고**, 한 줄씩 실행하며 처음 어긋나는 줄을 찾는다.

## Lab 22 — 계산한 값이 아닌 원본을 반환
```
단계별 값:     기대 70 → 105 → 84 → 126 / 실제 70 → 70 → 56 → 84
처음 어긋난 곳: applyCrit 반환 (함수에서 나온 직후 디버거가 보여 주는 반환값: "applyCrit 반환 70")
진짜 원인:     result에 원본을 복사해 두고 damage만 바꾼 뒤 result를 반환
```
**수정:** `return damage;` (또는 `return isCrit ? damage * (100 + critBonusPercent) / 100 : damage;`로 임시 변수 제거)
**교훈:** 파이프라인은 단계별 기대값을 정해 두고 한 줄씩 넘기면 범인 함수가 바로 나온다. 함수 안으로는 step into, 나와서는 **디버거가 보여 주는 반환값**(Visual Studio 자동 창, gdb `finish`).

## Lab 23 — 보호막이 깨질 때 남은 피해가 사라짐
```
틀린 결과:     받은 피해 160, 흡수 70 + HP 감소 87 = 157 (3이 사라짐)
멈춘 위치:     조건부 중단점 (before.shield - tank.shield) + (before.hp - tank.hp) != damage
처음 어긋난 곳: 보호막이 damage보다 적게 남은 틱 (예: 보호막 6에 피해 7)
진짜 원인:     takeDamage가 보호막을 0으로 깎고 바로 return → 넘친 피해가 HP로 안 감
```
**수정:**
```cpp
const int absorbed = std::min(unit.shield, damage);
unit.shield -= absorbed;
unit.hp -= damage - absorbed;
```
**교훈:** 수백 번 중 몇 번만 틀리면 한 줄씩 실행해서는 못 찾는다. "틀린 순간"을 **조건식**으로 표현해서 중단점에 넣는다. 합계 검증(불변식)은 그 조건을 만드는 좋은 재료다.

## Lab 24 — 이벤트 구독 순서
```
추적점 출력 (5번째 늑대):  퀘스트 검사: 카운터 값 4  →  카운터: Wolf, 증가 전 4
진짜 원인:     Game::setup에서 퀘스트를 카운터보다 먼저 구독 → 퀘스트가 갱신 전 값을 읽음
```
**수정:** 카운터를 먼저 구독한다. 더 튼튼한 방법은 퀘스트가 다른 구독자의 상태에 기대지 않고 직접 세거나, 카운터가 "갱신 후" 이벤트를 따로 내보내는 것.
**교훈:** "누가 먼저 실행되는가" 문제는 멈추면 오히려 헷갈린다. **추적점**으로 멈추지 않고 순서를 기록하자.

## Lab 25 — else if 순서 (넓은 조건이 좁은 조건을 가림)
```
조사식 (Selene):  c.isFirstOrder() false / c.isMember() true / c.isVip() true
화살표:          else if (c.isMember()) → return 5
진짜 원인:       VIP는 항상 멤버이므로 isVip() 분기에 영원히 도달하지 못함
```
**수정:** 좁은 조건(VIP)을 넓은 조건(멤버)보다 먼저 검사. 규칙이 "가장 큰 할인 하나"이므로 순서에 의존하지 않게 `std::max`로 해당하는 할인 중 최댓값을 고르는 편이 더 안전하다.
**교훈:** 분기 버그는 조사식에 **조건 하나하나를 따로 넣어 계산**해 보면 한눈에 보인다.

## Lab 26 — auto가 참조를 버리고 복사본을 만듦
```
조사식:      &item != &backpack.get("Iron Sword")
진짜 원인:   auto item = backpack.get(...)  → get()은 Equipment&를 주지만 auto는 값으로 받아 복사
             복사본을 수리하고, 복사본으로 "수리 완료"를 출력함
```
**수정:** `auto& item = backpack.get(itemName);`
**교훈:** "고쳤다고 출력되는데 반영이 안 된다"면 **같은 객체인지부터** 의심하라. 주소 비교나 개체 ID로 확인한다.

## Lab 27 — || 단락 평가로 뒤쪽 검사가 실행되지 않음
```
중단점:      checkKill 걸림, checkGold·checkLevel 안 걸림
진짜 원인:   checkKillAchievements(p)가 true → || 뒤는 계산하지 않음 → 베테랑 업적 검사 자체가 실행 안 됨
```
**수정:** 부수 효과가 있는 호출은 각각 실행한 뒤 합친다.
```cpp
const bool kill  = checkKillAchievements(p);
const bool gold  = checkGoldAchievements(p);
const bool level = checkLevelAchievements(p);
if (kill || gold || level) ...
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
**수정 (택1):**
- `World`도 `const Settings& settings_;`로 보관 (설정 변경이 즉시 반영됨, 대신 수명 관리 주의)
- 설정을 다 읽은 뒤에 `World`를 만든다 (초기화 순서를 명확히)
- `spawnWave(const Settings&)`처럼 필요할 때 인자로 받는다

**교훈:** 버그가 "어떤 입력에서" 나는 게 아니라 "어떤 **순서**로" 나는 경우, 시작부터 따라가며 **두 사본을 나란히** 조사식에 띄워 언제 갈라지는지 본다.

## 연산자 실수 (랩 29~32)


## Lab 29 — 후위 증가의 값은 올리기 전 값
```
틀린 결과:     새봄이 1007번 (도윤과 같은 번호). lastId는 1010으로 맞지만 실제로 준 번호는 1007~1009
멈춘 위치:     issueId의 return 줄, 첫 join
기대값 → 실제값: lastId 1007 → 새 번호 1008 기대 / 실제 반환 1007
진짜 원인:     return guild.lastId++; 는 올리기 전 값(마지막으로 이미 준 번호)을 돌려준다
```
**수정:** `return ++guild.lastId;`
**교훈:** `x++`는 "x를 올린다" + "식의 값은 옛 x"라는 두 가지 일을 한다. 변수의 뜻("마지막으로 준 번호" vs "다음에 줄 번호")을 먼저 정하고 전위/후위를 고른다. `if (hits++ == 3)`도 같은 실수로, 네 번째 타격에서야 참이 된다.

## Lab 30 — 매크로가 부작용 있는 인자를 두 번 평가
```
틀린 결과:     화면 콤보 2 4 6 0 2 4 (기대 1 2 3 0 1 2), 최고 콤보 6
멈춘 위치:     c.current 데이터 중단점 → 공격 한 번에 두 번 멈춤
처음 어긋난 줄: c.best = BUGHUNT_MAX(c.best, ++c.current);
진짜 원인:     ((a) > (b) ? (a) : (b))로 펼쳐져 조건에서 한 번, 고른 쪽에서 또 한 번 ++c.current
```
**수정:**
```cpp
++c.current;
c.best = std::max(c.best, c.current);
```
**교훈:** 매크로는 함수가 아니라 글자 치환이다. 인자에 `++`, `--`, 함수 호출을 넣지 않는다. 가능하면 `std::max`나 `inline`/`constexpr` 함수를 쓴다. (`?:`는 조건을 먼저 계산하고 고른 쪽만 계산하도록 순서가 정해져 있어서, 이 결과는 어느 컴파일러에서나 같다.)

## Lab 31 — `&&` 자리에 비트 AND `&`
```
틀린 결과:     보람(열쇠 2, 횃불 1) 입장 불가, 들어간 인원 1
멈춘 위치:     canEnterCave, 조건 a.name == "보람"
기대값 → 실제값: a.keys & a.torches → 기대 참 / 실제 0 (2 = 10, 1 = 01)
진짜 원인:     정수 두 개를 &로 묶어 공통 비트가 없으면 0(거짓)
```
**수정:** `return a.keys > 0 && a.torches > 0;`
**교훈:** `&`는 정수의 비트끼리, `&&`는 참/거짓끼리 계산한다. 둘 다 1일 때만 우연히 같은 답이 나와서 처음 테스트(1, 1)를 통과한다. 개수를 참/거짓으로 쓸 때는 `> 0`으로 뜻을 적어 준다.

## Lab 32 — `||` 자리에 `|` → 단락 평가가 사라짐
```
틀린 결과:     연 문 4 (기대 5), 남은 따개 0 (기대 1)
멈춘 위치:     useLockpick의 --t.lockpicks 줄, 호출 스택에서 openDoor의 door 확인
처음 어긋난 곳: "감옥" 문 (열쇠가 있는데 따개가 줄어듦)
진짜 원인:     hasKey(...) | useLockpick(...) 는 앞이 true여도 뒤를 호출한다
```
**수정:** `return hasKey(t, door) || useLockpick(t);`
**교훈:** `||`/`&&`만 단락 평가를 한다. 뒤쪽 피연산자에 부작용(소모, 저장, 로그)이 있으면 `|`/`&`는 그 부작용을 항상 일으킨다. 랩 27과 짝: 27은 단락 평가 때문에 **필요한 호출이 빠졌고**, 32는 단락 평가가 없어서 **불필요한 호출이 일어났다**. `p == nullptr | p->hp <= 0`처럼 앞의 검사가 뒤를 지켜 주는 식에서는 `|`가 null 역참조 크래시가 된다.

---

# 4부: 컨테이너

## Lab 33 — map::operator[]가 없는 키를 만듦
```
관찰:        openShop 전 bag.size() == 3, 후 == 6 ("Mana Potion", "Fire Scroll", "Rope"가 개수 0으로 생김)
진짜 원인:   countOf의 bag[item]은 키가 없으면 기본값(0)으로 원소를 새로 만든다
```
**수정:** 읽기만 할 때는 `find`(또는 C++20 `contains`)를 쓰고 `const Bag&`로 받는다.
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
**수정:** rehash에도 변하지 않는 것으로 기억한다. 표준은 rehash가 이터레이터를 무효화해도 **원소에 대한 참조와 포인터는 유지**된다고 보장한다.
```cpp
void select(size_t row) { selected_ = &*std::next(board_.begin(), row); }
const std::string& selectedName() const { return selected_->first; }
std::string confirmInvite()
{
    auto it = board_.find(selected_->first);
    ...
}
Board::value_type* selected_ = nullptr;
```
키(이름)를 복사해 두고 `find`해도 된다.
**교훈:** `unordered_map`의 순회 순서는 약속이 아니다. 오래 들고 있을 것은 키나 원소 포인터로.

## Lab 35 — 비교자가 strict weak ordering을 어김
```
멈춘 곳:     [CRASH] 정렬 검사: 비교 함수가 엄격한 약순서를 어겼다 ← bughunt::checkedOrder ← std::sort
조사식:      comp(Kael, Doran) == true, comp(Doran, Kael) == true  (둘 다 1500점)
진짜 원인:   >= 는 같은 값에 대해 true를 돌려준다. "a가 b보다 앞서고 b도 a보다 앞선다"는 모순
```
**수정:** `return a.score > b.score;`
**구성별:** 랩은 비교자를 `bughunt::checkedOrder`로 감싸서 어느 컴파일러, 어느 구성에서나 같은 자리에서 멈추게 했다. 검사가 없으면 표준 라이브러리마다 다르다. MSVC의 Debug STL은 `invalid comparator` 어설션을 내고, libstdc++는 `-D_GLIBCXX_DEBUG`, libc++는 하드닝 모드에서만 검사한다. 검사 없는 Release에서는 원소가 적으면 우연히 맞게 정렬되고, 같은 값이 아주 많으면 정렬이 배열 밖까지 읽어 나가 죽기도 한다.
**교훈:** 비교자는 "엄격하게 앞서는가"를 답한다. 같으면 false.

## Lab 36 — remove_if만 하고 erase를 빼먹음
```
관찰:        호출 뒤에도 size 6. 앞 3칸은 남길 장비, 뒤 3칸은 옮겨지고 남은 찌꺼기 (remove_if가 원소를 **이동**하므로 이름은 비고 내구도만 남음)
경고:        반환값을 버리고 있다 (MSVC C4858, GCC·Clang -Wunused-result. remove_if는 [[nodiscard]])
```
**수정:** C++20 `std::erase_if(bag, pred);` 또는 erase-remove 관용구 `bag.erase(std::remove_if(bag.begin(), bag.end(), pred), bag.end());`
**교훈:** 알고리즘은 이터레이터만 받으므로 컨테이너의 크기를 바꿀 수 없다. 경고를 0개로 유지하면 이 버그는 빌드할 때 보인다.

## Lab 37 — vector<bool>의 프록시 참조
```
지역 변수:   wasOpen의 형식이 bool이 아니라 비트 프록시 (MSVC std::_Vb_reference, libstdc++ std::_Bit_reference, libc++ std::__bit_reference)
             토글한 다음 줄에서 wasOpen도 같이 바뀜
진짜 원인:   vector<bool>은 비트로 압축되어 있어 operator[]가 bool&를 줄 수 없고, 비트를 가리키는 프록시 객체를 준다.
             auto는 그 프록시를 복사하므로 "이전 값"이 아니라 "지금 값"을 계속 가리킨다
```
**수정:** `const bool wasOpen = doorsOpen[door];` (타입을 적어서 값으로 꺼낸다). 비트 압축이 필요 없다면 `std::vector<char>`나 `std::vector<std::uint8_t>`도 좋다.
**교훈:** `auto x = v[i];`가 복사라는 믿음은 `vector<bool>`에서 깨진다. 디버거에서 변수의 형식을 보자(gdb `ptype`, lldb `frame variable -T`).

## Lab 38 — c_str() 포인터 수명과 SSO
```
조사식:      (void*)heroes[i].name.c_str() 가 grantTitle 뒤에 바뀜 (Doran, Selene), Kael은 그대로
Kael:        "Kael the Brave" 14글자 → 여전히 객체 안 버퍼(SSO) → 같은 주소라 우연히 맞음
Doran:       21글자 → 객체 안 버퍼에서 힙으로 이사. 이름표는 객체 안의 옛 버퍼(지금은 힙 포인터가 들어 있음)를 읽어 깨진 글자
Selene:      원래 힙(18글자, MSVC STL에서 용량 31) → 37글자로 재할당. 이름표는 해제된 힙을 읽음 (메모리 검사기의 0xEE 글자)
```
**수정:** 이름표가 문자열 버퍼가 아니라 문자열을 가진 객체를 가리키게 한다.
```cpp
explicit Nameplate(const Hero& hero) : hero_(&hero) {}
const char* text() const { return hero_->name.c_str(); }
const Hero* hero_;
```
**구성별:** 위 글자 수 경계는 MSVC STL과 libstdc++(객체 안 버퍼 15글자) 기준이다. 힙 용량도 구현마다 다르다(18글자일 때 MSVC STL 31, libstdc++ 18). libc++(macOS 기본)는 22글자까지 객체 안에 두므로 Doran도 우연히 맞을 수 있고, Selene도 처음에는 객체 안에 있다가 Doran처럼 힙으로 이사한다. ASan은 Selene만 잡는다(heap-use-after-free, MSVC STL·libstdc++ 기준). Doran은 아직 살아 있는 `Hero` 객체 안의 메모리를 읽으므로 ASan이 보기에는 정상 접근이다.
**교훈:** `c_str()`/`data()`/`string_view`는 문자열이 바뀌면 무효가 될 수 있다. 짧은 문자열은 SSO 때문에 "되는 것처럼" 보여서 더 늦게 발견된다.

## Lab 39 — set 안에 있는 원소의 정렬 기준을 바꿈
```
조사식:      season.ranked_ 순서가 Mira(1450), Selene, Kael, Doran(1700) → 점수 순이 아님
진짜 원인:   recordMatch가 set에 들어 있는 Player의 score를 직접 바꿈. set은 넣을 때만 자리를 정하므로 트리가 정렬 불변식을 잃는다.
             그 뒤의 find/erase는 엉뚱한 가지로 내려가 원소를 못 찾을 수 있다
```
**수정:** 꺼내고, 바꾸고, 다시 넣는다.
```cpp
void recordMatch(Player* p, int points)
{
    ranked_.erase(p);
    p->score += points;
    ranked_.insert(p);
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
**수정:**
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
**수정:**
```cpp
long long total = static_cast<long long>(current) + static_cast<long long>(baseExp) * multiplier;
if (total > kMaxExp)
    total = kMaxExp;
return static_cast<int>(total);
```
**교훈:** 부호 있는 정수의 오버플로는 정의되지 않은 동작이다. 대개 음수로 돌아가지만 컴파일러는 "넘치지 않는다"고 가정해도 되므로, `if (a + b < a)` 같은 "넘쳤는지 보는" 검사는 Release에서 통째로 지워지기도 한다(이 저장소의 랩은 그런 검사를 쓰지 않아 구성마다 결과가 같다). 검사는 넘치기 **전에**: 더 넓은 형식으로 계산하거나 `std::numeric_limits<int>::max() - current < gain`처럼 남은 여유를 본다. 넘친 뒤 `total < 0`을 보는 땜질은 곱셈이 넘쳐 양수로 돌아오는 경우(숨은 테스트)를 놓친다.

## Lab 42 — 축소 변환 두 곳 (`double`→`int`, `int`→`short`)
```
틀린 결과:     물약 1개 은화 114 (기대 115), 300개 묶음 -31336 (기대 34500)
멈춘 위치:     toSilver의 return 줄, 조사식 gold * 100 = 114.99999999999999 / sellBundle에서 toSilver(unitGold) * count와 record.totalSilver
진짜 원인:     1.15는 2진수로 정확히 나타낼 수 없어 1.1499...로 저장되고, double→int는 버림. 총액 필드가 short라 34200이 잘림
```
**수정:** `return static_cast<int>(std::lround(gold * 100));`, `SaleRecord::totalSilver`를 `int`로
**교훈:** `=`와 `return`의 축소 변환은 경고만 나거나(MSVC C4244) 아무 말이 없다. 중괄호 초기화(`int silver{ gold * 100 };`)는 축소 변환을 컴파일 오류로 만들어 두 곳을 한 번에 찾아 준다. 돈은 처음부터 가장 작은 단위의 정수로 들고 다닌다. `+ 1`로 맞춘 땜질은 2.5골드(정확히 250)에서 틀린다.

## Lab 43 — `std::min<std::size_t>`가 음수를 아주 큰 양수로
```
틀린 결과:     궁수 진(평균보다 12 높음) 보너스 20 (기대 0)
멈춘 위치:     levelBonus, 조건부 중단점 myLevel > average. average - myLevel = -12, size_t로는 18446744073709551604
진짜 원인:     컴파일 오류를 피하려고 std::min의 형식을 std::size_t로 정해 두어 음수가 부호 없는 수로 변환됨
```
**수정:** `return std::clamp(average - myLevel, 0, static_cast<int>(maxBonus));`
**교훈:** 부호 있는 값과 없는 값이 만나면 부호 있는 쪽이 부호 없는 형식으로 바뀐다(`-1 < v.size()`가 false인 것과 같은 규칙). 형식이 안 맞아 나는 컴파일 오류를 캐스트로 누르면 그 오류가 알려 주던 문제가 숨는다. 섞어 비교해야 하면 C++20 `std::cmp_less` 등을 쓴다.

## Lab 44 — 두 파일의 전역 변수가 서로를 씀 (초기화 순서)
```
틀린 결과:     전사 체력 0 (기대 300). 파일 순서를 바꿔 빌드하면 보스 공격력 0
멈춘 위치:     g_warriorHp 초기화 줄 중단점, 호출 스택에 main이 없음. 그때 g_baseHp == 0
진짜 원인:     번역 단위 사이의 전역 초기화 순서는 정해져 있지 않다(보통 링크 순서). 두 파일이 서로의 값을 쓰므로 어느 쪽이 먼저든 하나는 0을 읽는다
```
**수정:** 전역 변수 여덟 개를 함수 지역 static으로.
```cpp
int baseAttack() { static const int value = readStat("base_attack", 12); return value; }
int bossAttack() { static const int value = baseAttack() * 4; return value; }
// 나머지 여섯 개도 같은 모양
```
**교훈:** 다른 파일의 전역 객체에 기대는 전역 초기화는 순서에 맡긴 도박이다. 함수 안의 static은 처음 불릴 때 만들어지고(C++11부터 스레드 안전) 의존하는 쪽이 부르는 순간 준비된다. 컴파일할 때 정할 수 있는 값이면 `constexpr`/`constinit`이 가장 좋다. 이 랩은 두 방향으로 의존하게 만들어, 링크 순서가 바뀌어도 버그 상태가 늘 FAIL이다. 화면의 두 값만 상수로 박는 땜질은 숨은 테스트의 다른 두 값에서 걸린다.

## Lab 45 — 헤더의 `static` 변수가 파일마다 따로
```
틀린 결과:     저장 횟수 0, 마지막 슬롯 "" (기대 3, "auto-용의 둥지")
멈춘 위치:     lg06_autosave.cpp의 ++g_saveStats.count와 showSaveScreen에서 &g_saveStats가 서로 다름
진짜 원인:     네임스페이스 범위의 static은 내부 링크. 헤더를 포함한 .cpp마다 별개의 g_saveStats가 생긴다
```
**수정:** 헤더에서 `inline SaveStats g_saveStats;` (C++17 inline 변수). 또는 헤더에 `extern` 선언, .cpp 한 곳에 정의.
**교훈:** 헤더에 정의를 둘 때 `static`은 "하나만"이 아니라 "파일마다 하나씩"이다. 컴파일러도 링커도 경고하지 않는다. 헤더의 `const`/`constexpr` 변수도 기본으로 내부 링크라 값은 같지만 주소는 다르다.

## Lab 46 — 예외 경로에서만 새는 텍스처
```
틀린 결과:     남은 텍스처 2개 (기대 0)
멈춘 위치:     Texture 생성자/소멸자 중단점: 생성 6번, 소멸 4번. 남은 둘은 "저주받은 성"
진짜 원인:     loadDungeon이 텍스처를 new한 뒤 countRooms가 던지면, 원시 포인터 floor/walls만 사라지고 Texture는 남는다
```
**수정:**
```cpp
auto floor = std::make_unique<Texture>(file.name + "/floor");
auto walls = std::make_unique<Texture>(file.name + "/walls");
const int rooms = countRooms(file.layout);
return new Dungeon{ file.name, floor.release(), walls.release(), rooms };
// 더 좋게는 Dungeon의 멤버도 unique_ptr, loadDungeon은 std::unique_ptr<Dungeon>을 돌려준다
```
**교훈:** 예외가 나면 그 사이 함수들의 남은 줄은 실행되지 않고, 소멸자가 불리는 것은 스택의 객체뿐이다. 자원은 만들자마자 소유 객체(RAII)에 맡긴다. 정상 경로만 보면 new/delete 짝이 맞아 보여서 찾기 어려운 누수다.

## Lab 47 — 소멸자에서 던져 `std::terminate`
```
틀린 결과:     CRASH, abort() (저장 실패 메시지가 나오지 않음)
멈춘 위치:     예외가 던져진 ~SaveFile. 호출 스택 아래에 saveGame의 catch가 아니라 std::terminate
진짜 원인:     C++11부터 소멸자는 암시적 noexcept. noexcept 함수 밖으로 나가려는 예외는 catch를 찾지 않고 terminate
```
**수정:** 실패할 수 있는 쓰기를 `close()`로 꺼낸다.
```cpp
void close()
{
    if (buffer_.size() > freeBytes_)
        throw SaveError("디스크 공간이 모자란다");
    g_disk[slot_] = buffer_;
    closed_ = true;
}
~SaveFile()
{
    if (!closed_)
    {
        try { close(); } catch (...) {}
    }
}
// saveGame: try 블록 마지막 줄에서 file.close();
```
**교훈:** 소멸자는 실패를 알릴 길이 없는 곳이다. 실패할 수 있는 정리는 명시적인 함수(`close`, `commit`)로 꺼내고, 소멸자는 삼킨다. `noexcept(false)`로 바꾸는 땜질은 다른 예외로 스택을 푸는 중에 소멸자가 또 던지면 여전히 terminate다(숨은 테스트). MSVC는 C4297, clang은 `-Wexceptions`로 이미 경고한다. 함수에 `noexcept`를 붙여 두고 안에서 던지는 경우도 같은 결과다.

## Lab 48 — 범위 for는 범위를 처음 한 번만 평가한다
```
틀린 결과:     이번 차례 완료 1 (기대 3), 일지 3개 (기대 5)
멈춘 위치:     endTurn의 for. 반복은 두 번, 그 사이 log.size()는 3으로 늘었다. &q는 일지 안이 아니라 entries()가 돌려준 사본 안
진짜 원인:     범위 for는 log.entries()를 한 번 평가하고 그 begin/end를 잡아 둔다. 반복 중에 일지에 붙은 퀘스트는 사본에도, 잡아 둔 끝 안에도 없다
```
**수정:**
```cpp
for (std::size_t i = 0; i < log.size(); ++i)
{
    const Quest q = log.at(i);   // 복사로 받는다 (complete 안의 push_back이 재할당해도 안전)
    if (q.done || achieved.count(q.goal) == 0)
        continue;
    log.complete(q.name, book);
    ++completed;
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
**수정:** `int damage(const Target& t) const override { ... }`
**교훈:** 재정의하려는 함수에는 항상 `override`. 붙이는 순간 `const` 누락, 매개변수 형식 차이(`int` vs `long`, `const&` vs 값), 기반 쪽 이름 변경을 컴파일러가 오류로 알려 준다. clang은 `-Wall`에서 `-Woverloaded-virtual` 경고를 내지만, MSVC는 기본 경고 수준(/W3)에서 아무 말이 없다.

## Lab 50 — `vector<Monster>`에 담아 Boss가 잘림 (슬라이싱)
```
틀린 결과:     전리품 3개, 황금 왕관 없음 (기대 4개)
멈춘 위치:     Boss::loot 중단점이 걸리지 않음 → collectLoot에서 wave[2]를 펼치면 형식이 Monster, rare_ 없음
진짜 원인:     wave.push_back(Boss(...))가 Boss의 Monster 부분만 복사해 담는다
```
**수정:**
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
// collectLoot도 같은 형식을 받고 m->loot()
```
**교훈:** 다형 객체는 포인터(소유는 `unique_ptr`)로 담는다. 값으로 받는 함수 매개변수(`void f(Monster m)`)도 똑같이 자른다. 다형 기반 클래스의 복사를 `= delete`로 막아 두면 이 실수가 컴파일 오류가 된다.

## Lab 51 — 생성자 안의 가상 호출
```
틀린 결과:     저격 탑 사거리 3 (기대 8), 거리 7을 못 쏨
멈춘 위치:     Tower::baseRange 중단점 → 호출 스택에 Tower::Tower ← SniperTower::SniperTower
진짜 원인:     기반 생성자가 도는 동안 객체의 동적 형식은 아직 Tower. 가상 호출도 Tower 버전으로 간다
```
**수정:** 사거리를 필요할 때 묻는다.
```cpp
int range() const { return baseRange(); }
bool canHit(int distance) const { return distance <= range(); }
```
(또는 `Tower(int range)` 생성자로 파생 클래스가 값을 넘긴다)
**교훈:** 생성자와 소멸자 안에서 가상 함수는 "지금 만들고 있는(부수고 있는) 클래스"의 버전이 불린다. 이것은 표준이 정한 동작이라 어느 컴파일러에서나 같다. 소멸자에서 **순수** 가상 함수를 부르는 경우는 정의되지 않은 동작이라 이 랩에서 뺐다(아래 미포함).

## Lab 52 — 가상 함수의 기본 인자는 정적 형식을 따름
```
틀린 결과:     사제 기본 치유 15 (기대 30), 파티 총 25 (기대 40). 직접 priest.heal()은 30
멈춘 위치:     Priest::heal 안, castDefaultHeal에서 들어왔을 때 amount == 10
진짜 원인:     어느 heal이 불릴지는 동적 형식(Priest)으로, 기본 인자는 정적 형식(Healer)으로 정해진다
```
**수정:** 기본값도 가상 함수로 묻는다.
```cpp
// Healer
virtual int defaultAmount() const { return 10; }
// Priest: return 25;  Druid: return 18;
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
**수정:** `EventMailbox`에 `using Mailbox::give;` (그리고 `Item`의 생성자에 `explicit`)
**교훈:** 이름 찾기가 오버로드 결정보다 먼저다. 파생 클래스에서 같은 이름을 하나라도 선언하면 기반의 같은 이름은 모두 안 보인다. 보이는 후보가 하나뿐이고 암시적 변환이 가능하면 오류 없이 엉뚱한 함수가 불린다. 숫자 하나로 만드는 생성자에는 `explicit`.

## Lab 54 — `dynamic_cast` 결과의 nullptr 확인 누락
```
틀린 결과:     크래시 (액세스 위반 / SIGSEGV)
멈춘 위치:     Mage::restoreMana, this == nullptr → 호출 스택 한 칸 위 drinkManaPotions: unit은 Warrior, mage는 nullptr
진짜 원인:     포인터에 대한 dynamic_cast는 형식이 맞지 않으면 nullptr를 돌려준다. 확인 없이 호출
```
**수정:**
```cpp
if (Mage* mage = dynamic_cast<Mage*>(unit.get()))
{
    mage->restoreMana(amount);
    ++used;
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
**수정:** `std::ifstream in(gameDir() / "data/prices.txt");`
**교훈:** 작업 디렉터리는 프로그램을 띄운 쪽(IDE, 바로 가기, 다른 프로그램)이 정한다. 실행 파일 옆의 데이터는 실행 파일 위치를 기준으로 절대 경로를 만든다. 파일을 못 열었을 때 빈 목록을 조용히 돌려주지 말고 알린다(숨은 테스트처럼 파일이 아예 없으면 품목 0개가 된다).

## Lab 56 — 버퍼에만 있고 파일에는 아직 없는 내용을 다시 읽음
```
틀린 결과:     일지 창 0줄 (기대 3)
멈춘 위치:     readJournal 호출 줄에서 멈춘 뒤 journal.txt 확인 → 0바이트
진짜 원인:     ofstream이 열려 있는 동안 쓴 내용은 스트림 버퍼에 있고, flush/close 전에는 디스크에 없다
```
**수정:** `record`에서 쓴 뒤 `out_.flush();` (또는 읽기 전에 flush/close)
**교훈:** 같은 프로그램 안에서 쓰고 다시 읽으면, 읽기 전에 쓴 내용이 디스크에 있는지 보장한다. 12부 랩 96(`std::endl`이 매번 flush해서 느림)과 짝: flush는 필요한 순간에만.

## Lab 57 — `while (!in.eof())`로 마지막 기록이 두 번
```
틀린 결과:     4줄, 마지막에 "도윤 7100" 한 번 더, 총점 32100 (기대 25000)
멈춘 위치:     push_back, 조건 scores.size() == 3
기대값 → 실제값: 네 번째 바퀴는 없어야 함 / 실제 in.fail() == true인데 지난 값으로 push_back
진짜 원인:     eof()는 끝을 넘어 읽으려다 실패한 "뒤에" 켜진다. 마지막 줄바꿈이 남아 한 바퀴 더 돈다
```
**수정:** `while (in >> name >> points) scores.push_back({ name, points });`
**교훈:** 읽기는 "읽고 → 성공했는지 확인 → 쓴다" 순서. 실패한 `>>`는 변수를 그대로 두므로(이 랩) 지난 값이 한 번 더 들어간다. `pop_back` 땜질은 줄바꿈 없이 끝나는 파일에서 진짜 기록을 지운다.

## Lab 58 — `>>` 뒤의 `getline`이 남은 줄바꿈을 읽음
```
틀린 결과:     첫 대사 "" (빈 줄), 셋째 대사 자리에 둘째 대사, 마지막 대사 없음
멈춘 위치:     push_back, 첫 바퀴 line.size() == 0
진짜 원인:     in >> count는 "3"까지만 읽고 줄바꿈을 남긴다. getline은 그 줄바꿈까지를 빈 줄로 읽는다
```
**수정:** `in >> count;` 다음에 `in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');` (`<limits>`)
**교훈:** `>>`(토큰 단위)와 `getline`(줄 단위)을 섞으면 경계의 줄바꿈을 직접 처리해야 한다. `in >> std::ws`도 통과처럼 보이지만 다음 줄 앞의 공백까지 먹어서 들여 쓴 대사를 망친다(숨은 테스트).

## Lab 59 — failbit가 남아 이후 읽기가 모두 무시됨
```
틀린 결과:     몬스터 3종, 보상 0개 (기대 3), 102번 보상 0
멈춘 위치:     in >> header 줄, 조사식 in.fail() == true
진짜 원인:     첫 while이 "보상"을 숫자로 읽으려다 실패 → failbit. 지우지 않아 header와 골드 읽기가 전부 그냥 넘어감
```
**수정:** `std::string header;` 앞에 `in.clear();`
**교훈:** 실패 표시는 저절로 꺼지지 않는다. 실패를 "예상한" 지점(섹션 끝)에서는 `clear()`로 지우고 계속 읽는다. `clear()`는 글자를 버리지 않으므로 실패를 일으킨 토큰("보상")은 그대로 다음 읽기에 들어간다.

## Lab 60 — UTF-8 BOM이 첫 키에 붙음
```
틀린 결과:     음량 80 (기본값, 기대 30). 전체 화면(둘째 줄)은 정상
멈춘 위치:     getInt, 조건 key == "volume" → settings의 첫 키가 "volume"처럼 보이지만 size() == 9
진짜 원인:     파일 맨 앞의 BOM(EF BB BF) 3바이트가 첫 줄의 키 앞에 붙어 "\xEF\xBB\xBFvolume"이 됨
```
**수정:** 첫 줄이 `"\xEF\xBB\xBF"`로 시작할 때만 3바이트를 지운다.
```cpp
bool firstLine = true;
while (std::getline(in, line))
{
    if (firstLine && line.rfind("\xEF\xBB\xBF", 0) == 0)
        line.erase(0, 3);
    firstLine = false;
    ...
```
**교훈:** "첫 줄만 이상하다"면 파일 앞부분의 보이지 않는 바이트를 의심한다. 디버거가 보여 주는 글자보다 `size()`와 바이트 값을 믿는다. 무조건 3바이트를 버리는 땜질은 BOM 없는 파일의 첫 키를 망친다(숨은 테스트).

## Lab 61 — 저장 도중 실패로 반쯤 쓴 세이브
```
틀린 결과:     저장 실패 뒤 불러온 아이템 3종 (기대 5종)
멈춘 위치:     writeItem의 throw 줄, 멈춘 채로 save.txt 열기 → 이미 지워지고 앞의 몇 줄만 있음
진짜 원인:     trunc로 여는 순간 옛 세이브가 지워지고, 예외로 빠져나갈 때 ofstream 소멸자가 앞부분만 쓴 파일을 닫는다
```
**수정:** 임시 파일에 끝까지 쓰고 닫은 뒤에만 바꿔치기한다.
```cpp
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
            throw std::runtime_error("쓰기 실패");
    }
    fs::rename(temp, file);          // 같은 폴더 안에서 바꿔치기
    return true;
}
catch (const std::exception& e)
{
    std::cout << "  저장 실패: " << e.what() << '\n';
    std::error_code ec;
    fs::remove(temp, ec);            // 실패한 임시 파일 정리
    return false;
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
**수정:** `if (!save.contains("pet"))` (읽기만 하는 함수는 `const json&`로 받고 `contains`/`find`/`at`)
**교훈:** "있는지 확인"과 "꺼내기"를 구분한다. const json에 없는 키로 `[]`를 쓰는 것은 정의되지 않은 동작(Debug에서는 assert)이므로 const로 바꾸기만 해서는 안 되고 `contains`로 확인한다.

## Lab 63 — `value("crit", 1)`이 int로 읽어 1.5를 1로 자름
```
틀린 결과:     단검 치명타 30 (기대 45). 도끼(정수 2)와 지팡이(없음)는 정상
멈춘 위치:     loadWeapon의 return, 조건 w.name == "단검" → w.crit == 1
진짜 원인:     value()의 반환 형식은 기본값 1(int)이 정한다. nlohmann은 실수 → 정수 변환에서 예외 없이 자른다
```
**수정:** `w.crit = j.value("crit", 1.0);` (또는 `j.value<double>("crit", 1)`)
**교훈:** 라이브러리의 변환 함수는 "형식을 어디서 정하는지" 확인한다. `get<int>()`도 1.5를 1로 조용히 자른다. 정수로 저장되던 값을 실수로 바꾸는 데이터 패치에서 잘 터진다. 반올림 땜질은 1.75, 2.25에서 다시 틀린다(숨은 테스트).

## Lab 64 — `json`이 키를 정렬해서 순서가 사라짐
```
틀린 결과:     불러온 순서 "번개 얼음창 치유 화염구" (기대 "화염구 얼음창 번개 치유"), 저장 후에도 정렬됨
멈춘 위치:     loadSkillBar의 for, j를 펼쳐 보면 이미 정렬되어 있음
진짜 원인:     nlohmann::json의 객체는 std::map(키 정렬)에 담긴다. JSON 표준도 객체 키 순서에 의미를 두지 않는다
```
**수정:** `using Json = nlohmann::ordered_json;` (읽기와 저장이 함께 바뀜)
**교훈:** 순서가 의미 있는 데이터는 배열로 저장하는 것이 정석이다. 객체 키 순서에 기대면 라이브러리·언어를 바꿀 때마다 깨진다. 한글 키는 UTF-8 바이트순으로 정렬되어 "가나다순처럼 보이지만 아닌" 결과가 나오기도 한다.

## Lab 65 — UTF-8을 바이트로 잘라 `dump`가 예외를 던짐
```
틀린 결과:     "Lv99용감한전사" 저장 실패: [json.exception.type_error.316] incomplete UTF-8 string; last byte: 0x95
멈춘 위치:     catch 블록 → clampName의 return에서 마지막 바이트 확인 (0xED 0x95 = "한"의 앞 2바이트)
진짜 원인:     4 + 3 + 3 = 10바이트 뒤 "한"(3바이트)의 중간인 12바이트에서 잘림
```
**수정:**
```cpp
size_t cut = kMaxNameBytes;
while (cut > 0 && (static_cast<unsigned char>(name[cut]) & 0xC0) == 0x80)
    --cut;
return name.substr(0, cut);
```
**교훈:** 바이트 수 제한은 글자 경계에서 자른다(이어지는 바이트는 `10xxxxxx`). `dump`의 오류 처리기(`error_handler_t::replace/ignore`)로 예외만 막으면 깨진 이름이 화면과 서버에 남는다. CP949로 읽은 문자열을 그대로 넣어도 같은 예외(type_error 316)가 난다. 원인은 "JSON에 UTF-8이 아닌 바이트가 들어갔다"로 같다.

## Lab 66 — 기본 정밀도 6자리로 float 왕복 손실
```
틀린 결과:     저장한 문자열 {"x":1234.57,"y":-87.6543,"z":3.33333} → 불러온 위치가 1234.56995 등으로 달라짐
멈춘 위치:     savePosition의 return (out.str()), 디버거의 p.x 표시(1234.56775)와 비교
진짜 원인:     ostream의 기본 정밀도는 유효 숫자 6자리. float을 정확히 되살리려면 max_digits10(9)자리가 필요
```
**수정:** `out << std::setprecision(std::numeric_limits<float>::max_digits10);` (`<limits>`) 또는 json 객체에 넣어 `dump()`
**교훈:** 사람이 읽는 출력과 다시 읽을 저장은 다르다. 저장은 왕복(round-trip)이 보장되는 형식으로. `std::fixed`는 소수점 아래 자릿수라 0.0003141 같은 작은 값을 망친다. 숨은 테스트는 7~8자리로는 부족한 값도 넣었다(MSVC와 clang에서 같은 결과를 확인).

## Lab 67 — 속성을 자식 요소로 읽어 빈 노드의 0
```
틀린 결과:     모든 몬스터 HP 0 (공격력은 정상)
멈춘 위치:     m.hp = ... 줄, 조사식 node.child("hp").empty() == true, node.attribute("hp").value() == "320"
진짜 원인:     hp는 속성(hp="320")인데 child("hp")로 찾음 → 빈 노드 → as_int() 0
```
**수정:** `m.hp = node.attribute("hp").as_int();`
**교훈:** pugixml은 없는 노드/속성에 예외 대신 빈 객체를 돌려준다. 오타, 대소문자("Attack"/"attack"), 속성/자식 혼동이 모두 조용한 0이 된다. 필수 값은 `if (!attr)`로 확인하고 데이터 오류로 알린다.

## Lab 68 — 이스케이프하지 않고 이어 붙인 XML
```
틀린 결과:     길드 목록 읽기 실패: Error parsing element attribute → 0개 (기대 3)
멈춘 위치:     loadGuilds 실패 쪽, xml.c_str() + result.offset → "방패>..." 부분
진짜 원인:     이름 "<검과 방패>"의 <가 새 태그 시작으로 해석됨
```
**수정:** 이름을 넣기 전에 `&` → `&amp;`(가장 먼저), `<` → `&lt;`, `>` → `&gt;`, `"` → `&quot;`, `'` → `&apos;` (또는 pugixml로 문서를 만들어 `doc.save()`)
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
**수정:** 값으로 돌려준다. `std::string formatEntry(...)`, `std::vector<std::string> buildBoard(...)`. (`static char line[64]`로 바꾸면 깨지지는 않지만 모든 줄이 마지막 값으로 똑같아진다.)
**구성별:** Debug와 Release 모두 깨진 글자. MSVC ASan은 기본 설정으로는 잡지 못한다. Clang ASan은 `stack-use-after-scope`로 잡기도 한다(이 저장소에서 확인). 가장 빠른 단서는 컴파일러 경고(MSVC C4172, Clang `-Wreturn-stack-address`, GCC `-Wreturn-local-addr`)였다.
**교훈:** 포인터를 돌려줄 때는 "가리키는 대상이 함수보다 오래 사는가?"를 먼저 묻는다.

## Lab 70 — 해제된 몬스터에서 현상금을 읽음 (use-after-free)
```
이상한 값:     hunter.target->bounty == -286331154 (0xEEEEEEEE = 메모리 검사기가 채운 해제된 힙)
처음 어긋난 줄: field.removeDefeated()  (타깃 몬스터를 delete)
진짜 원인:     hunt가 전장을 정리한 뒤에 hunter.target으로 현상금을 읽음
```
**수정:** 현상금을 먼저 받고 정리한다.
```cpp
hunter.gold += hunter.target->bounty;
hunter.target = nullptr;
field.removeDefeated();
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
**수정:** `delete[] particles_;` 더 좋게는 `std::vector<Particle>`이나 `std::unique_ptr<Particle[]>`로 바꿔 직접 지우지 않는다.
**구성별:** Debug와 Release는 메모리 검사기가 그 자리에서 `[CRASH]`. 검사기가 없으면 MSVC Debug는 CRT 힙 어설션, Release는 힙 손상(Windows `0xC0000374`, glibc `free(): invalid pointer`)으로 죽는다. ASan은 "attempting free on address which was not malloc()-ed". 소멸자가 없는 타입(`int` 등)이면 앞의 개수 칸이 없어서 Debug와 Release에서는 아무 증상 없이 지나가기도 한다. 그래도 정의되지 않은 동작이다.
**교훈:** 소멸자를 가진 타입의 배열은 `new[]`가 앞에 개수를 몰래 적어 둔다. 만든 방법과 지우는 방법은 항상 짝을 맞춘다.

## Lab 72 — 포인터 산술 단위 실수
```
조사식:      (const char*)entries - (const char*)header == 16  (헤더 바로 뒤라면 4)
진짜 원인:   header는 PacketHeader*. header + sizeof(PacketHeader) 는 "4바이트 뒤"가 아니라
             "PacketHeader 4개 뒤" = 16바이트 뒤
```
**수정:** `reinterpret_cast<const LootEntry*>(header + 1)` 또는 바이트 단위로 `reinterpret_cast<const LootEntry*>(packet.data() + sizeof(PacketHeader))`.
**구성별:** Debug와 Release는 패킷 바로 뒤에 메모리 검사기가 둔 경계 바이트 `0xBDBD`(= 48573)를 개수로 읽어 합계가 145722가 된다. 검사기가 없으면 쓰레기 값. ASan은 heap-buffer-overflow(READ of size 2)로 그 줄에서 멈춘다.
**교훈:** 포인터에 더하는 수의 단위는 "가리키는 타입 하나"다. 바이트 단위로 움직이려면 `char*`/`uint8_t*`에서 계산한다.

## Lab 73 — 초기화 안 된 포인터 멤버
```
멈춘 곳:     Hero::attackPower의 power += weapon_->bonus  (읽기 액세스 위반)
지역 변수:   this->weapon_ == 0xa5a5a5a5a5a5a5a5
진짜 원인:   무기를 받지 않는 생성자가 weapon_을 초기화하지 않음.
             if (weapon_ != nullptr) 검사는 0xA5A5... 앞에서는 아무 소용이 없다.
```
**수정:** 멤버 선언에서 초기화한다. `const Weapon* weapon_ = nullptr;`
**구성별:** 파티는 재사용 메모리(`bughunt::recycledStorage`, `0xA5`로 채워져 있음) 위에 만들어지므로 Debug와 Release 모두 매번 같은 곳에서 죽는다(Windows 액세스 위반, POSIX `SIGSEGV`). 재사용 메모리가 아니었다면 남은 쓰레기에 따라 통과하기도 하고 엉뚱한 공격력이 나오기도 한다. 쓰레기 주소가 어디를 가리키느냐에 따라 stack-buffer-overflow처럼 엉뚱한 이름의 리포트가 나오기도 한다. ASan은 "초기화 안 된 값" 자체를 잡는 도구가 아니다.
**교훈:** x64에서 `0xA5A5...`처럼 정규 주소가 아닌 값을 읽으면 예외 메시지에는 `0xFFFFFFFFFFFFFFFF`로 나올 수 있다. 예외 메시지보다 디버거의 포인터 값을 본다. 랩 12처럼 "멤버를 초기화하지 않음" 정적 분석 경고가 미리 알려 준다.

## Lab 74 — shared_ptr 순환 참조
```
지역 변수:   guild [strong ref=4]  (지역 변수 1 + 길드원 3명의 guild 멤버)
진짜 원인:   Guild → members(shared_ptr) → Member → guild(shared_ptr) → Guild
             서로 붙잡고 있어서 마지막 지역 변수가 사라져도 참조 수가 0이 되지 않는다.
```
**수정:** 길드원은 길드를 소유하지 않고 바라보기만 한다.
```cpp
std::weak_ptr<Guild> guild;
...
std::string guildNameOf(const Member& member)
{
    const auto guild = member.guild.lock();
    return guild ? guild->name : "(없음)";
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
**수정:** 함수보다 오래 사는 람다는 지역 변수를 값으로 가져간다. `hero`는 호출한 쪽이 계속 들고 있으므로 참조로 둔다.
```cpp
timers.after(3, [&hero, bonus] {
    hero.attack -= bonus;
    ...
});
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
**수정:** `std::vector<std::vector<int>> dist(map.height, std::vector<int>(map.width, -1));` (또는 1차원 `std::vector<int>`에 `y * width + x`)
**교훈:** 스택 오버플로가 늘 재귀인 것은 아니다. 호출 스택이 짧은데 스택 오버플로라면 큰 지역 변수를 의심한다. 크기가 실행 중에 정해지거나 수십 KB를 넘는 데이터는 힙(`vector`)에. `static` 배열로 바꾸면 죽지는 않지만 16 MB를 늘 붙잡고 여러 스레드에서 쓸 수 없다.

## Lab 77 — 프레임마다 96 KB 버퍼 × 재귀 160겹
```
틀린 결과:     CRASH, 스택 오버플로. "방 8번 생성"까지 출력되고 죽음 (방 5개 시험 던전은 통과)
멈춘 위치:     buildRooms가 열 겹 남짓 쌓인 호출 스택. 이웃한 두 프레임의 &layout 차이 ≈ 98,000바이트
진짜 원인:     재귀는 160겹에서 끝나지만, 한 겹이 96 KB라 160겹이면 15 MB. 버퍼가 재귀 호출 뒤에도 쓰여 줄일 수 없다
```
**수정:** `std::vector<char> layout(kLayoutBytes);` (프레임에는 vector 하나만 남는다). 또는 보물 상자를 재귀 전에 세고 버퍼를 일찍 끝내거나, 재귀를 반복문으로.
**교훈:** 스택 사용량은 "프레임 크기 × 깊이"다. 깊이가 데이터(방 수, 트리 높이)에 따라 정해지는 재귀는 스택 한도를 생각해야 한다. 프레임 크기는 구성마다 다르다(Debug는 검사용 공간, ASan은 변수 사이 경계). 그래서 프레임이 작고 깊이가 경계에 걸린 재귀는 Debug에서만, 또는 ASan에서만 죽기도 한다. 직접 보려면 `kLayoutBytes`를 8 KB로 줄이고 방 수를 늘려 Debug와 Release에서 죽는 방 번호를 비교해 보라.

## Lab 78 — `new Frame[n]` 뒤 `<=`로 한 칸 더 쓰기, 해제할 때 드러남
```
틀린 결과:     CRASH, [CRASH] 메모리 검사: 할당한 크기를 넘어서 썼다 (ASan: heap-buffer-overflow)
멈춘 위치:     ~ReplayRecorder의 delete[]. 데이터 중단점(&frames_[ticks_])을 걸면 recordAll의 frames_[t] = ... 에서 t == 600
진짜 원인:     for (t = 0; t <= ticks_; ++t)가 원소 600개 배열의 frames_[600]에 쓴다
```
**수정:** `for (int t = 0; t < ticks_; ++t)`
**교훈:** 힙 넘침은 쓰는 순간이 아니라 그 자리를 다른 누가 볼 때(해제, 다음 할당, 옆 블록 사용) 드러난다. 크래시 위치와 원인이 멀면 **데이터 중단점**으로 "그 바이트를 누가 바꾸는지"를 묻는다. ASan은 쓰는 순간 잡는다. 랩 72(포인터 산술의 단위, 읽기)와 달리 이 랩은 반복 경계의 한 칸 쓰기다.

## Lab 79 — `memcpy`에 `sizeof(포인터)`
```
틀린 결과:     첫 번째 이름 "Nighting" (기대 "Nightingale"), 두 번째 이름 "Moonshadingale" 같은 이어 붙음
멈춘 위치:     writeName, 조사식 sizeof(dst) == 8, std::strlen(src) == 11. 메모리 창에서 packet.name 9바이트째부터 앞사람 이름
진짜 원인:     매개변수 char* dst는 배열이 아니라 포인터라 sizeof가 8. 앞 8바이트만 복사되고 널 문자가 없다
```
**수정:**
```cpp
void writeName(char* dst, std::size_t cap, const char* src)
{
    const std::size_t n = std::min(std::strlen(src), cap - 1);
    std::memcpy(dst, src, n);
    dst[n] = '\0';
}
// 호출: writeName(packet.name, sizeof(packet.name), name);
```
**교훈:** 배열은 함수 매개변수가 되는 순간 포인터로 바뀐다(배열 붕괴). 크기는 같이 넘기거나 `char (&dst)[N]` 템플릿으로 받는다. C 문자열 복사는 "길이 + 널 문자 1"과 "버퍼 크기 - 1로 자르기"를 둘 다 챙겨야 한다. clang은 `-Wsizeof-pointer-memaccess`로 이미 경고한다. 패킷 밖에서는 `std::string`.

## Lab 80 — 1부터 세는 스킬 번호로 `std::array` 인덱스, 옆 멤버 `gold`를 덮음
```
틀린 결과:     Debug: CRASH, "array subscript out of range" / Release: 2번 스킬 레벨 0, 골드 801 (기대 800)
멈춘 위치:     Debug: learnSkill의 hero.skillLevels[skillId] 줄 (어설션) / Release: hero.gold 데이터 중단점이 같은 줄에서 멈춤
진짜 원인:     화면 번호 1~4를 그대로 인덱스로 씀. skillLevels[4]는 배열 바로 뒤의 gold
```
**수정:** `hero.skillLevels[skillId - 1] += 1;`
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
**수정:** `jobs_.push_back(std::async(std::launch::async, [this, i] { chunks_[i] = disk_.read(i); }));` 그러면 `finishLoading`이 모아 둔 future를 기다린다.
**구성별:** 어느 컴파일러에서나 같다(표준이 정한 동작). C++20부터 `std::async`는 `[[nodiscard]]`라서 경고(MSVC C4858, GCC·Clang `-Wunused-result`)가 이 줄을 가리킨다.
**교훈:** "비동기로 띄웠다"와 "기다리지 않는다"는 다르다. future를 어디에 두고 언제 기다리는지가 코드의 실행 순서를 정한다.

## Lab 82 — 비동기 작업의 예외는 get()에서 나옴
```
예외가 던져질 때 멈춤:  작업 스레드의 craft("Mithril Helm") throw
abort 시점 호출 스택:   메인 스레드 craftAll → std::future<int>::get → 예외 다시 던지기
진짜 원인:             try/catch가 std::async 호출만 감쌌다. 작업 안의 예외는 future에 담겼다가 get()에서 던져진다
```
**수정:** `get()`을 try 안으로 옮긴다.
```cpp
for (std::future<int>& job : jobs)
{
    try { report.revenue += job.get(); }
    catch (const std::exception& e) { std::cout << "  제작 실패: " << e.what() << '\n'; ++report.failed; }
}
```
(`std::async` 자체도 스레드를 만들지 못하면 `std::system_error`를 던지므로 원래 try를 남겨 두어도 된다.)
**교훈:** 비동기 작업의 예외는 **결과를 꺼내는 곳**에서 처리한다. `get()`은 future 하나에 한 번만 부를 수 있다. 두 번째 호출 전에는 `valid()`가 false이고, 그때 `get()`을 부르는 것은 정의되지 않은 동작이다(구현 대부분은 `std::future_error`를 던진다).

## Lab 83 — 조건 없는 condition_variable 대기
```
모두 중단:  공대원 스레드는 모두 끝남. lab-worker(공대장)는 ReadyCheck::waitForAll의 wait 안
조사식:     readyCount_ == 4, memberCount_ == 4  → 기다릴 이유가 없다
진짜 원인:  notify_one은 공대장이 wait하기 전에 이미 네 번 불렸다. 알림은 저장되지 않으므로 사라졌다
```
**수정:** `allReady_.wait(lock, [&] { return readyCount_ >= memberCount_; });`
조건이 이미 참이면 기다리지 않고, 가짜로 깨어나도 조건이 거짓이면 다시 기다린다.
**구성별:** 어느 구성에서나 TIMEOUT. `if (readyCount_ < memberCount_) allReady_.wait(lock);`처럼 한 번만 검사하면 화면은 통과하지만, 공대장이 먼저 기다리는 숨은 케이스에서 첫 알림에 깨어나 3명 중 1명만 준비한 채 입장한다(FAIL).
**교훈:** `condition_variable`은 "무엇인가 바뀌었을 수 있다"는 신호일 뿐이다. 기다리는 **조건**은 mutex로 지키는 변수에 있고, `wait`는 항상 그 조건과 함께 쓴다.

## Lab 84 — 떼어 낸 스레드의 [this]
```
중단점:     board.post(bid_)  (작업 스레드. 호출 스택에 bidAndCloseWindow 없음)
조사식:     bid_ == -286331154 (0xEEEEEEEE = 해제된 메모리)
진짜 원인:  람다가 this를 캡처했는데, AuctionWindow(unique_ptr)는 bidAndCloseWindow가 끝날 때 지워졌다.
            람다는 마감 종이 울린 뒤, 창이 사라진 다음에 실행된다
```
**수정:** 필요한 값만 복사해 간다. `std::thread([bid = bid_, &bell, &board] { bell.waitForRing(); board.post(bid); }).detach();`
(종과 게시판은 서버가 켜져 있는 동안 사는 객체라 참조로 둔다. 그 약속이 없다면 `shared_ptr`로 함께 소유한다.)
더 튼튼한 설계는 스레드를 떼어 내지 않는 것이다. 창이 `std::jthread`를 소유하고 소멸자에서 멈춰 달라고 요청하고 기다리거나, 알림을 서버 쪽 객체에 등록한다.
**구성별:** Debug와 Release는 메모리 검사기의 `0xEE`로 FAIL. ASan은 작업 스레드에서 heap-use-after-free(해제한 곳은 메인 스레드의 `bidAndCloseWindow`)로 CRASH.
**교훈:** `detach`는 "이 스레드의 수명을 아무도 책임지지 않는다"는 뜻이다. 떼어 낸 스레드에 넘긴 람다는 캡처한 모든 것이 그 스레드보다 오래 사는지 확인한다(9부 랩 75와 같은 실수가 스레드로 옮겨 온 것).

## Lab 85 — atomic 확인 후 행동 레이스
```
조건부 중단점:  --stock_; 에서 stock_ <= 0 일 때 멈춤
스레드 목록:    다섯 구매 스레드가 모두 stock_.load() > 0 검사를 통과해 payment_.charge에서 기다리는 중이었다
진짜 원인:      "남았나 확인 → 결제 → 하나 빼기"가 원자적이지 않다. atomic 하나하나는 안전하지만,
                확인과 빼기 사이에 다른 스레드가 모두 확인을 끝낼 수 있다
```
**수정:** 결제하기 전에 수량을 원자적으로 확보한다.
```cpp
int left = stock_.load();
do
{
    if (left <= 0)
        return false;
} while (!stock_.compare_exchange_weak(left, left - 1));
payment_.charge(player, price_);
++sold_;
return true;
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
**수정:** 취소 경로에서도 기다린다. 결과는 스레드가 끝난 뒤에 고친다(아직 도는 스레드가 `result.matches`를 쓰고 있으므로).
```cpp
cancel = true;
matcher.join();
result.cancelled = true;
result.matches = 0;
return result;
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
**수정:** 코루틴이 쓸 값은 프레임 안으로 복사해 간다. `Cutscene talkTo(Npc npc, DialogueLog& log)` (`log`는 호출한 쪽이 컷신보다 오래 들고 있으므로 참조로 둔다.)
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
**수정:** 반복 횟수를 정수로 계산한다.
```cpp
return static_cast<int>(std::lround(cooldownSeconds / tickSeconds));   // #include <cmath>
```
`t < cooldownSeconds`로만 바꾸면 멈추지는 않지만 오차 때문에 한 틱을 더 센다. 예를 들어 1.2초 / 0.05초는 24가 아니라 25가 나온다(채점기의 숨은 케이스가 이것을 잡는다).
**교훈:** 실수는 `==`, `!=`로 비교하지 않는다. "몇 번 반복하는가"는 정수로 센다.

## Lab 89 — unsigned의 s >= 0
```
모두 중단:   s가 12보다 훨씬 큰 수 (수십억). 조건부 중단점 s == 0 에서 한 줄 실행하면 4294967295 (0xFFFFFFFF)
진짜 원인:   unsigned에는 음수가 없어서 s >= 0 이 항상 참. 0에서 --s 하면 4294967295로 돌아간다.
```
**수정:** `for (unsigned int s = waitSeconds + 1; s-- > 0;)` 또는 부호 있는 정수로 센다: `for (int s = static_cast<int>(waitSeconds); s >= 0; --s)`.
**교훈:** 이 실수를 알려 주는 "식이 항상 true" 경고는 기본 경고 수준에서 보이지 않는 경우가 많다. MSVC C4296은 `/W4`에서도 꺼져 있어 `/w14296`으로 켜고, GCC는 `-Wtype-limits`(`-Wextra`에 포함), Clang은 `-Wtautological-unsigned-zero-compare`로 켠다. 거꾸로 세는 반복에서 `unsigned`/`size_t`를 보면 의심한다.

## Lab 90 — continue가 ++i를 건너뜀
```
모두 중단:   i == 6 (첫 공백), c == ' ', words == { "/party" }. 한 줄씩 계속 실행해도 i가 그대로
진짜 원인:   공백 분기가 i를 늘리지 않고 continue → 같은 공백을 영원히 다시 읽는다
```
**수정:** `continue;` 앞에 `++i;`. 또는 `for (char c : line)`처럼 인덱스를 직접 다루지 않는 반복으로 바꾼다.
**교훈:** `while` 반복의 모든 경로가 루프 변수를 앞으로 움직이는지 확인한다. 다음 문 설정으로 `++i;`를 한 번 실행시켜 보면 가설을 바로 시험할 수 있다.

## Lab 91 — 끼워 넣기 순서가 바뀌어 자기 자신을 가리킴
```
모두 중단:   countStops 안, stops가 수천만, p->name == "Forest"
조사식:      p == p->next  (Forest의 next가 자기 자신)
진짜 원인:   pos->next = wp; 를 먼저 하면 다음 줄 wp->next = pos->next 가 방금 넣은 wp 자신을 읽는다
```
**수정:** 순서를 바꾼다.
```cpp
wp->next = pos->next;
pos->next = wp;
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
**수정:** 두 mutex를 한꺼번에 잠근다. `std::scoped_lock`은 교착을 피하는 순서로 잠근다.
```cpp
std::scoped_lock locks(seller.lock, buyer.lock);
if (!verifyItem(seller, item))
    return;
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
**수정:** 지우지 말고 끝까지 읽은 뒤 한 번에 비운다.
```cpp
for (const Message& m : queue_)
    chars += static_cast<long long>(m.text.size());
queue_.clear();
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
**수정:**
```cpp
int totalWeight(const std::vector<Item>& items)
...
for (const Item& item : items)
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
**수정:** 기존 문자열 뒤에 붙인다(`+=`는 용량이 모자랄 때만 키운다).
```cpp
log += "[";
log += toText(e.tick);
log += "] hit ";
log += toText(e.amount);
log += "\n";
```
(`log += "[" + toText(e.tick) + ... ;`처럼 한 줄짜리 임시만 만드는 것도 통과한다. 크기를 안다면 `log.reserve(...)`로 할당을 한 번으로 줄일 수 있다.)
**수정 후:** Release 기준 할당 20번, 약 200KB. Debug는 반복자 디버깅용 보조 할당 때문에 횟수가 더 많지만 결과 길이에 비례한다.
**교훈:** 문자열을 누적할 때 `s = s + x`는 O(n²), `s += x`는 분할 상환 O(n)이다.

## Lab 96 — std::endl은 매번 flush
```
세는 값:     flush 5002번 (오브젝트 5000개 + 머리줄 + 마지막 flush)
FakeLogFile::sync 중단점의 호출 스택:  writeSave의 << std::endl 두 줄
진짜 원인:   std::endl = '\n' + flush. 줄마다 디스크 쓰기를 한다
```
**수정:**
```cpp
out << "SAVE v3 " << objects.size() << '\n';
...
out << o.id << ' ' << o.x << ' ' << o.y << '\n';
```
마지막의 `out.flush()`는 남긴다.
**수정 후:** flush 1번. (마지막 flush까지 지우면 0번이 되어 FAIL. 다 쓴 뒤 한 번은 내보내야 한다.)
**교훈:** 줄바꿈은 `'\n'`, flush는 필요할 때 한 번. 콘솔 출력도 같다.

## Lab 97 — 열 우선으로 도는 행 우선 배열
```
세는 값:     건너뛴 접근 3,999,999번 (거의 모든 접근)
조건부 중단점 (jumps_ < 5):  row가 0, 1, 2...로 바뀌고 col은 그대로 → index가 cols(2000)씩, 8000바이트씩 건너뜀
진짜 원인:   칸은 한 행씩 이어서 놓여 있는데 countFog는 열을 바깥, 행을 안쪽 반복으로 돈다.
             매 접근이 다른 캐시 줄이라 메모리에서 새로 읽어 온다. reveal은 행 우선이라 빠르다
```
**수정:** 반복 순서를 바꾼다.
```cpp
for (int r = 0; r < map.rows(); ++r)
{
    for (int c = 0; c < map.cols(); ++c)
```
**수정 후:** 건너뛴 접근 0번.
**교훈:** 2차원 배열은 메모리에 놓인 순서대로 돈다. 프로파일러는 "countFog가 느리다"까지만 알려 주고, 왜 느린지는 같은 일을 하는 reveal과 비교해야 보인다.

## Lab 98 — shared_ptr를 값으로 넘기고 값으로 돌기
```
세는 값:     한 몬스터를 동시에 붙잡은 shared_ptr 최대 3개 (목록 + 범위 for의 m + threatOf의 인자)
조건부 중단점 (use_count() > 1)의 호출 스택:  mostDangerous의 for (MonsterPtr m : monsters), threatOf(MonsterPtr monster)
진짜 원인:   shared_ptr 복사마다 참조 카운트를 원자적으로 올리고 내린다. 3000마리 × 300틱 × 2번
```
**수정:**
```cpp
int threatOf(const MonsterPtr& monster)
...
for (const MonsterPtr& m : monsters)
```
(`threatOf(const Monster& monster)`처럼 소유권과 상관없는 함수는 객체 참조를 받아도 된다.)
**수정 후:** 최대 1개. 둘 중 하나만 고치면 2개가 남는다. `best = m;`은 가장 위험한 몬스터가 바뀔 때만 복사하므로 그대로 두어도 된다.
**교훈:** 소유권을 넘기거나 나눠 가질 때만 `shared_ptr`를 값으로 넘긴다. 읽기만 하면 `const&`나 객체 참조로 충분하다.
