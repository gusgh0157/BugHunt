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

---

# 4부: 포인터 오용

## Lab 29 — 지역 변수의 주소를 반환
```
조사식:      board[0], board[1], board[2] 가 모두 같은 주소
진짜 원인:   formatEntry가 지역 배열 line의 주소를 반환. 함수가 끝나면 line은 사라지고,
             다음 formatEntry 호출(그리고 다른 함수 호출)이 같은 스택 자리를 다시 쓴다.
```
**수정:** 값으로 돌려준다. `std::string formatEntry(...)`, `std::vector<std::string> buildBoard(...)`. (`static char line[64]`로 바꾸면 깨지지는 않지만 모든 줄이 마지막 값으로 똑같아진다.)
**구성별:** Debug와 Release 모두 깨진 글자. ASan도 기본 설정으로는 잡지 못한다(`/fsanitize-address-use-after-return`과 `ASAN_OPTIONS=detect_stack_use_after_return=1`이 필요). 가장 빠른 단서는 컴파일러 경고 **C4172**였다.
**교훈:** 포인터를 돌려줄 때는 "가리키는 대상이 함수보다 오래 사는가?"를 먼저 묻는다.

## Lab 30 — 해제된 몬스터에서 현상금을 읽음 (use-after-free)
```
이상한 값:     hunter.target->bounty == -572662307 (0xDDDDDDDD = 해제된 힙)
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
**구성별:** Debug는 `0xDD`로 채워져 FAIL. Release는 해제된 메모리에 옛 값이 남아 **PASS**. ASan은 heap-use-after-free와 함께 읽은 곳(`hunt`), 해제한 곳(`removeDefeated`), 할당한 곳(`spawn`)의 호출 스택을 보여 준다.
**교훈:** Release에서 통과했다고 버그가 없는 게 아니다. 해제된 메모리를 읽는 버그는 Debug의 `0xDD`와 ASan으로 드러낸다.

## Lab 31 — new[]로 만들고 delete로 지움
```
멈춘 곳:     어설션 _CrtIsValidHeapPointer(block) ← ~Explosion의 delete particles_
관찰:        첫 폭발이 끝날 때 ~Particle()이 16번이 아니라 1번만 불림
             (char*)particles_ - 8 자리에 배열 개수 16이 저장되어 있음
진짜 원인:   new Particle[count]로 만든 배열을 delete(배열이 아닌 것)로 지움
```
**수정:** `delete[] particles_;` 더 좋게는 `std::vector<Particle>`이나 `std::unique_ptr<Particle[]>`로 바꿔 직접 지우지 않는다.
**구성별:** Debug는 힙 검사 어설션. Release는 힙 손상(`0xC0000374`)으로 죽는다. ASan은 "attempting free on address which was not malloc()-ed". 소멸자가 없는 타입(`int` 등)이면 앞의 개수 칸이 없어서 Debug와 Release에서는 아무 증상 없이 지나가기도 한다. 그래도 정의되지 않은 동작이다.
**교훈:** 소멸자를 가진 타입의 배열은 `new[]`가 앞에 개수를 몰래 적어 둔다. 만든 방법과 지우는 방법은 항상 짝을 맞춘다.

## Lab 32 — 포인터 산술 단위 실수
```
조사식:      (const char*)entries - (const char*)header == 16  (헤더 바로 뒤라면 4)
진짜 원인:   header는 PacketHeader*. header + sizeof(PacketHeader) 는 "4바이트 뒤"가 아니라
             "PacketHeader 4개 뒤" = 16바이트 뒤
```
**수정:** `reinterpret_cast<const LootEntry*>(header + 1)` 또는 바이트 단위로 `reinterpret_cast<const LootEntry*>(packet.data() + sizeof(PacketHeader))`.
**구성별:** Debug는 패킷 바로 뒤의 경계 바이트 `0xFDFD`(= 65021)를 개수로 읽어 합계가 6만대가 된다. Release는 쓰레기 값. ASan은 heap-buffer-overflow(READ of size 2)로 그 줄에서 멈춘다.
**교훈:** 포인터에 더하는 수의 단위는 "가리키는 타입 하나"다. 바이트 단위로 움직이려면 `char*`/`uint8_t*`에서 계산한다.

## Lab 33 — 초기화 안 된 포인터 멤버
```
멈춘 곳:     Hero::attackPower의 power += weapon_->bonus  (읽기 액세스 위반)
지역 창:     this->weapon_ == 0xcccccccccccccccc
진짜 원인:   무기를 받지 않는 생성자가 weapon_을 초기화하지 않음.
             if (weapon_ != nullptr) 검사는 0xCCCC... 앞에서는 아무 소용이 없다.
```
**수정:** 멤버 선언에서 초기화한다. `const Weapon* weapon_ = nullptr;`
**구성별:** Debug는 `/RTC1`이 스택을 `0xCC`로 채워서 매번 같은 곳에서 죽는다. Release는 그 자리에 남은 쓰레기에 따라 통과하기도 하고 엉뚱한 공격력이 나오기도 한다. ASan 구성은 `/RTC`가 꺼져 있어 역시 쓰레기 값이다. 쓰레기 주소가 어디를 가리키느냐에 따라 stack-buffer-overflow처럼 엉뚱한 이름의 리포트가 나오기도 한다. ASan은 "초기화 안 된 값" 자체를 잡는 도구가 아니다.
**교훈:** x64에서 `0xCCCC...` 같은 주소를 읽으면 예외 메시지에는 `0xFFFFFFFFFFFFFFFF`로 나올 수 있다. 예외 메시지보다 지역 창의 포인터 값을 본다. 랩 12처럼 코드 분석 경고 C26495가 미리 알려 준다.

## Lab 34 — shared_ptr 순환 참조
```
지역 창:     guild [strong ref=4]  (지역 변수 1 + 길드원 3명의 guild 멤버)
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
**구성별:** 세 구성 모두 똑같이 FAIL이다. 크래시도, 어설션도, ASan 리포트도 없다(Windows의 ASan에는 누수 검사기가 없다). 2부 랩 16처럼 CRT 누수 측정이나 소멸자 중단점으로 확인한다.
**교훈:** 스마트 포인터도 소유 관계에 고리가 생기면 샌다. "누가 누구를 소유하는가"를 한 방향으로 정하고, 거꾸로 가는 쪽은 `weak_ptr`로 둔다.

---

# 5부: 멈춤 - 무한루프와 데드락

## Lab 35 — float 누적과 != 종료 조건
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

## Lab 36 — unsigned의 s >= 0
```
모두 중단:   s가 12보다 훨씬 큰 수 (수십억). 조건부 중단점 s == 0 에서 F10을 하면 4294967295 (0xFFFFFFFF)
진짜 원인:   unsigned에는 음수가 없어서 s >= 0 이 항상 참. 0에서 --s 하면 4294967295로 돌아간다.
```
**수정:** `for (unsigned int s = waitSeconds + 1; s-- > 0;)` 또는 부호 있는 정수로 센다: `for (int s = static_cast<int>(waitSeconds); s >= 0; --s)`.
**교훈:** 이 실수를 알려 주는 경고 C4296("식이 항상 true")은 `/W4`에서도 꺼져 있다. `/w14296`이나 `/Wall`로 켜야 보인다. 거꾸로 세는 반복에서 `unsigned`/`size_t`를 보면 의심한다.

## Lab 37 — continue가 ++i를 건너뜀
```
모두 중단:   i == 6 (첫 공백), c == ' ', words == { "/party" }. F10을 계속 눌러도 i가 그대로
진짜 원인:   공백 분기가 i를 늘리지 않고 continue → 같은 공백을 영원히 다시 읽는다
```
**수정:** `continue;` 앞에 `++i;`. 또는 `for (char c : line)`처럼 인덱스를 직접 다루지 않는 반복으로 바꾼다.
**교훈:** `while` 반복의 모든 경로가 루프 변수를 앞으로 움직이는지 확인한다. 다음 문 설정으로 `++i;`를 한 번 실행시켜 보면 가설을 바로 시험할 수 있다.

## Lab 38 — 끼워 넣기 순서가 바뀌어 자기 자신을 가리킴
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

## Lab 39 — 잠금 순서가 엇갈리는 데드락
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

# 6부: 컨테이너

## Lab 40 — map::operator[]가 없는 키를 만듦
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

## Lab 41 — rehash 뒤에 "몇 번째 줄"이 다른 사람을 가리킴
```
조사식:      registerWave(..., 60) 전 bucket_count 8 → 후 512, selectedName()이 Mira → Evening36
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

## Lab 42 — 비교자가 strict weak ordering을 어김
```
어설션:      xutility : invalid comparator
조사식:      comp(Kael, Doran) == true, comp(Doran, Kael) == true  (둘 다 1500점)
진짜 원인:   >= 는 같은 값에 대해 true를 돌려준다. "a가 b보다 앞서고 b도 a보다 앞선다"는 모순
```
**수정:** `return a.score > b.score;`
**구성별:** Debug와 ASan(Debug 라이브러리 사용)은 어설션으로 알려 준다. Release는 검사가 없어서 원소가 적으면 우연히 맞게 정렬되고, 같은 값이 아주 많으면 정렬이 배열 밖까지 읽어 나가 죽기도 한다.
**교훈:** 비교자는 "엄격하게 앞서는가"를 답한다. 같으면 false.

## Lab 43 — remove_if만 하고 erase를 빼먹음
```
관찰:        호출 뒤에도 size 6. 앞 3칸은 남길 장비, 뒤 3칸은 옮겨지고 남은 찌꺼기 (Silver Ring이 두 번 보이기도 함)
경고:        C4858 반환값을 버리고 있습니다 (remove/remove_if는 erase가 필요)
```
**수정:** C++20 `std::erase_if(bag, pred);` 또는 erase-remove 관용구 `bag.erase(std::remove_if(bag.begin(), bag.end(), pred), bag.end());`
**교훈:** 알고리즘은 이터레이터만 받으므로 컨테이너의 크기를 바꿀 수 없다. 경고를 0개로 유지하면 이 버그는 빌드할 때 보인다.

## Lab 44 — vector<bool>의 프록시 참조
```
지역 창:     wasOpen의 형식이 bool이 아니라 std::_Vb_reference<...>
             토글한 다음 줄에서 wasOpen도 같이 바뀜
진짜 원인:   vector<bool>은 비트로 압축되어 있어 operator[]가 bool&를 줄 수 없고, 비트를 가리키는 프록시 객체를 준다.
             auto는 그 프록시를 복사하므로 "이전 값"이 아니라 "지금 값"을 계속 가리킨다
```
**수정:** `const bool wasOpen = doorsOpen[door];` (타입을 적어서 값으로 꺼낸다). 비트 압축이 필요 없다면 `std::vector<char>`나 `std::vector<std::uint8_t>`도 좋다.
**교훈:** `auto x = v[i];`가 복사라는 믿음은 `vector<bool>`에서 깨진다. 지역 창의 형식 열을 보자.

## Lab 45 — c_str() 포인터 수명과 SSO
```
조사식:      (void*)heroes[i].name.c_str() 가 grantTitle 뒤에 바뀜 (Doran, Selene), Kael은 그대로
Kael:        "Kael the Brave" 14글자 → 여전히 객체 안 버퍼(SSO) → 같은 주소라 우연히 맞음
Doran:       21글자 → 객체 안 버퍼에서 힙으로 이사. 이름표는 객체 안의 옛 버퍼(지금은 힙 포인터가 들어 있음)를 읽어 깨진 글자
Selene:      원래 힙(18글자, 용량 31) → 37글자로 재할당. 이름표는 해제된 힙을 읽음 (Debug에서 0xDD 계열 글자)
```
**수정:** 이름표가 문자열 버퍼가 아니라 문자열을 가진 객체를 가리키게 한다.
```cpp
explicit Nameplate(const Hero& hero) : hero_(&hero) {}
const char* text() const { return hero_->name.c_str(); }
const Hero* hero_;
```
**구성별:** ASan은 Selene만 잡는다(heap-use-after-free). Doran은 아직 살아 있는 `Hero` 객체 안의 메모리를 읽으므로 ASan이 보기에는 정상 접근이다.
**교훈:** `c_str()`/`data()`/`string_view`는 문자열이 바뀌면 무효가 될 수 있다. 짧은 문자열은 SSO 때문에 "되는 것처럼" 보여서 더 늦게 발견된다.

## Lab 46 — set 안에 있는 원소의 정렬 기준을 바꿈
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
