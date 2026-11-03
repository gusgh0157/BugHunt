# 해답 (스포일러!)

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
멈춘 함수:  [외부 코드] vector::operator[] → 첫 내 코드: QuickSlotBar::itemAt(slotIndex = 4)
값의 출처:  onNumberKey(key = 4) ← replayInput
진짜 원인:  키 번호(1~4)를 슬롯 인덱스(0~3)로 바꾸지 않음
```
**수정:** `useItem(bar.itemAt(key - 1));`
**교훈:** 외부 코드는 건너뛰고 내 코드 첫 줄부터. 그리고 크래시 전에 나온 "틀린 출력"도 같은 버그의 증상이다.

## Lab 3 — 정수 나눗셈으로 0초가 됨
```
멈춘 함수:  damagePerSecond(seconds = 0)
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
멈춘 함수:  countdown(n = -4xxx) 이 수천 번 반복 → Stack overflow
값의 출처:  n이 홀수 → startFireworks(seconds = 5)
진짜 원인:  5, 3, 1, -1, ... 로 0을 건너뛰어 끝나지 않음
```
**수정:** `if (n <= 0)`.
**교훈:** 스택이 같은 함수로 가득 차면 재귀가 끝나지 않는 것. 매개변수 값의 변화를 보면 이유가 보인다.

## Lab 6 — 없는 키로 map::at
```
멈춘 함수:  [외부 코드] _CxxThrowException ← std::map::at → 첫 내 코드: priceOf(itemId = "mana_poiton")
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
멈춘 함수:  [외부 코드] std::array::operator[] → statsFor(level = 200)  (테이블은 0~50)
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
`i <= scores.size()` → 마지막 반복에서 `scores[5]` 범위 밖 접근. Debug STL이 어설션으로 잡아 준다.
**수정:** `i < scores.size()` 또는 범위 기반 for / `std::accumulate`.
**교훈:** 어설션 창 → 다시 시도 → 호출 스택은 가장 기본적인 추적 루트.

## Lab 10 — 참조 무효화 (vector 재할당)
`add`가 `items_.back()`의 참조를 돌려주는데, 이후 `push_back`에서 capacity가 늘면 원소들이 새 메모리로 옮겨지고 `sword`는 해제된 메모리를 가리킨다(use-after-free).
**수정:** 참조를 오래 들고 있지 말고 이름/인덱스로 다시 찾기, 또는 `reserve`, 또는 `std::deque`/`std::list`처럼 주소가 안정적인 컨테이너.
**교훈:** "참조/포인터/이터레이터를 들고 있는 동안 컨테이너를 바꾸지 말 것."

## Lab 11 — 임시 객체를 가리키는 string_view
`makeTitle`이 만든 `std::string`은 `titleOf`의 return 문이 끝나면 파괴되고, `string_view`는 그 해제된 버퍼를 가리킨다.
**수정:** `std::string`으로 리턴. `string_view`는 소유자가 더 오래 살아 있을 때만.

## Lab 12 — 초기화 안 된 멤버
`int combo_;`에 초기값이 없다. Debug에선 `0xCCCCCCCC`(-858993460)라 처음 4연속 명중이 음수로 기록되어 무시됨. Release에선 우연히 0이라 맞게 나올 수도 있다.
**수정:** `int combo_ = 0;`
**교훈:** Debug/Release 결과가 다르면 미초기화 변수를 1순위로 의심. (코드 분석 경고 C26495)

## Lab 13 — erase 후 이터레이터 무효화
`erase(it)` 후 `it`는 무효인데 `++it`를 한다.
**수정:**
```cpp
for (auto it = enemies.begin(); it != enemies.end(); )
    it = (it->hp <= 0) ? enemies.erase(it) : it + 1;
// 또는 C++20: std::erase_if(enemies, [](const Enemy& e) { return e.hp <= 0; });
```

## Lab 14 — unsigned 언더플로
`20u - 22u` = 4294967294. "남은 칸"이 엄청 커서 무엇이든 들어간다.
**수정:** `if (bag.usedSlots >= bag.maxSlots) return false;`를 먼저 검사하거나 부호 있는 타입으로 계산.

## Lab 15 — Rule of Three 위반 (이중 해제)
`PacketBuffer`는 raw 포인터를 소유하지만 복사 생성자/대입이 기본(얕은 복사)이다. 소멸자를 직접 선언했으므로 이동 생성자는 자동 생성되지 않아 `push_back(PacketBuffer(64))`에서 **복사**가 일어남 → 임시 객체 소멸 시 `data_` 해제 → 벡터 안 객체는 해제된 버퍼를 사용 → 끝에서 또 해제(double free).
**수정(권장, Rule of Zero):** `std::vector<char> data_;` 또는 `std::unique_ptr<char[]>` 사용. 또는 복사 생성자/대입/이동을 직접 구현.

## Lab 16 — 가상 소멸자 누락 (메모리 누수 + UB)
`StatusEffect`의 소멸자가 virtual이 아니라 `unique_ptr<StatusEffect>`가 `delete`할 때 `Burn`의 소멸자가 불리지 않는다 → `damagePerTurn_` 벡터 메모리 누수. (엄밀히는 정의되지 않은 동작)
**수정:** `virtual ~StatusEffect() = default;`
**교훈:** 다형적으로 delete될 기반 클래스는 반드시 virtual 소멸자. (경고 C5205)

## Lab 17 — 데이터 레이스
`gold_` 읽기-수정-쓰기가 원자적이지 않다. 두 스레드가 같은 값을 읽고 각각 +1을 쓰면 하나가 사라진다.
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
**교훈:** 경고 C4554가 이미 알려 주고 있었다. 경고를 0개로 유지하자.

---

# 3부: 흐름 추적 (크래시 없음)

## Lab 21 — 반복문 안에서 초기화되는 누적 변수
```
틀린 결과:     최장 연속 1, 골드 900 (보너스 없음)
처음 어긋난 줄: 둘째 바퀴의 int streak = 0;  (1이었던 값이 0으로 돌아감)
진짜 원인:     streak가 for 안에 선언되어 매 바퀴 새로 만들어짐
```
**수정:** `int streak = 0;`을 `for` 위로 옮긴다. (결석 처리 `streak = 0;`은 그대로 둔다.)
**교훈:** 크래시가 없으면 호출 스택은 쓸모없다. **기대값을 먼저 적고**, F10으로 처음 어긋나는 줄을 찾는다.

## Lab 22 — 계산한 값이 아닌 원본을 반환
```
단계별 값:     기대 70 → 105 → 84 → 126 / 실제 70 → 70 → 56 → 84
처음 어긋난 곳: applyCrit 반환 (자동 창: "applyCrit 반환 70")
진짜 원인:     result에 원본을 복사해 두고 damage만 바꾼 뒤 result를 반환
```
**수정:** `return damage;` (또는 `return isCrit ? damage * (100 + critBonusPercent) / 100 : damage;`로 임시 변수 제거)
**교훈:** 파이프라인은 단계별 기대값을 정해 두고 F10으로 넘기면 범인 함수가 바로 나온다. 함수 안은 F11, 나와서는 **자동 창의 반환값**.

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
**교훈:** 수백 번 중 몇 번만 틀리면 F10으로는 못 찾는다. "틀린 순간"을 **조건식**으로 표현해서 중단점에 넣는다. 합계 검증(불변식)은 그 조건을 만드는 좋은 재료다.

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
