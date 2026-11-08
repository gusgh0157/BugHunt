# 챌린지 해답 (스포일러!)

챌린지를 직접 풀기 전에는 읽지 마세요. 문제를 어느 부에서 가져왔는지는 맨 아래 표에 있습니다.

---

# 깊은 추적

세 문제 모두 증상이 나는 곳과 원인이 있는 곳 사이에 네 단계 이상이 있고, 증상 바로 근처에 "고치면 화면의 [PASS]가 나오는" 가짜 용의자가 있습니다. 거기를 고치면 채점기의 숨은 케이스가 `증상은 사라졌지만 원인이 남아 있음`으로 FAIL을 줍니다.

## C-1 — 화면 패널이 경매장의 참가자 저장소를 정렬함

```
증상:        낙찰자가 Kael. Mira의 금화는 800 그대로(입찰한 450이 빠지지 않음), Kael은 150(밀려났는데 돌려받지 못함)
1단계:       close()가 bidders_[*leader_]에게 물건을 줌 → leader_가 가리키는 사람이 Kael
2단계:       placeBid(mira, 450)에 조건부 중단점(amount == 450) → bidders_[who].name이 "Kael"
             → 참가 번호 mira(1)가 다른 사람을 가리킨다
3단계:       세 번째 join 뒤 줄에 중단점을 걸고 처음부터 실행. 멈추면 조사식에 house.bidders_[1].name, 데이터 중단점(gdb·lldb의 watchpoint)을 &house.bidders_[1].gold에 걸고 계속
             → 바로 다음 drawFrame 안에서 멈춤. 호출 스택: std::stable_sort ← AuctionHouse::forEachByTier ← BidderPanel::refresh ← drawFrame
4단계:       forEachByTier가 "보여 주기 위해" bidders_ 자체를 등급순으로 정렬한다.
             참가 번호(BidderId = 벡터 위치)와 leader_가 모두 다른 사람을 가리키게 된다
추적점 예:   placeBid 첫 줄에 "{who} {bidders_[who].name} {amount}" → 호출한 쪽이 넘긴 번호와 실제 이름이 어긋나는 것이 한눈에 보인다
```
**가짜 용의자:** `close()`의 수수료 계산(`TODO: 반올림이 맞는지 기획 확인`)과 `placeBid`의 "자기 입찰 올리기" 분기. 둘 다 맞다. 수령액 428도 이미 맞게 나온다.
**증상만 가리는 수정:** 시나리오의 `drawFrame`에서 `bidders.refresh(house)`를 빼면 화면의 검사는 모두 통과하지만, 숨은 케이스가 패널을 그린 뒤 참가 번호를 확인해서 FAIL.
**수정:** 저장소는 그대로 두고 보여 줄 순서만 정렬한다.
```cpp
void AuctionHouse::forEachByTier(const std::function<void(const Bidder&)>& visit)
{
    std::vector<const Bidder*> view;
    for (const Bidder& b : bidders_)
        view.push_back(&b);
    std::stable_sort(view.begin(), view.end(), [](const Bidder* a, const Bidder* b) { return a->tier > b->tier; });
    for (const Bidder* b : view)
        visit(*b);
}
```
**교훈:** 위치(인덱스)를 아이디로 쓰는 컨테이너는 순서를 바꾸는 모든 연산이 아이디를 깨뜨린다. "보여 주는" 함수가 `const`가 아니라면 왜 아닌지 의심한다. 값이 언제 바뀌는지 모를 때는 데이터 중단점이 범인을 바로 알려 준다.

## C-2 — 핸들러가 구독을 끊으면 다음 구독자가 이벤트를 건너뜀

```
증상:        "그림자" 칭호 없음, 최고 무피격 연속 처치 5 (기대 6)
1단계:       TitleSystem::award → tracker_.bestStreak()가 5
2단계:       AchievementTracker 핸들러의 ++current_에 데이터 중단점(또는 추적점 "#{e.seq} {current_}")
             → #2, #3, #5, #6, #7은 지나가는데 #4(세 번째 늑대)만 빠짐
3단계:       EventBus::publish에 조건부 중단점 e.seq == 4, 한 줄씩 따라가며 조사식에 i, subscribers_.size(), subscribers_[i].name
             → i == 1 "의뢰: 늑대 무리 소탕" 핸들러를 부른 뒤 size가 5 → 4, 다음 i == 2는 "전리품"
4단계:       의뢰 핸들러 안 bus_.unsubscribe(sub_)의 호출 스택: HuntQuest 람다 ← EventBus::publish
             → 순회 중에 vector에서 지우면 뒤 원소가 한 칸씩 당겨지고, i는 그대로 증가하므로 "업적 추적"을 건너뛴다
```
**가짜 용의자:** `AchievementTracker`의 "막아 낸 공격(피해 0)도 피격" 처리. #8에서 방패로 막자 연속 처치가 끊기는 것이 수상해 보인다. 하지만 기획서대로다.
**증상만 가리는 수정:**
- 피해 0이면 연속 처치를 끊지 않게 바꾸면 5 + 2 = 7이 되어 화면은 통과. 숨은 케이스가 업적의 처치 수와 기획서 4.2를 확인해서 FAIL.
- 의뢰가 구독을 끊지 않게(`done_`만 세우게) 바꾸면 화면은 통과. 숨은 케이스가 남은 구독자 수를 확인해서 FAIL. 버스는 "핸들러 안에서 unsubscribe해도 된다"고 약속했으므로 고칠 곳은 버스다.
**수정:** 이번 이벤트를 받을 구독자 목록을 먼저 복사해 두고 그 복사본을 돈다.
```cpp
void publish(const Event& e)
{
    const std::vector<Subscriber> snapshot = subscribers_;
    for (const Subscriber& s : snapshot)
        s.handler(e);
}
```
(지우기를 미뤄 두었다가 publish가 끝난 뒤 한꺼번에 지우는 방법도 있다. 이렇게 하면 실행 중인 `std::function`을 덮어쓰는 문제도 함께 사라진다.)
**교훈:** "순회 중 컨테이너 수정"은 이터레이터 대신 인덱스를 써도 사라지지 않는다. 크래시 없이 한 번만 건너뛰므로, 값을 세는 쪽(업적)에서 시작해 이벤트 하나하나를 추적점으로 찍어 보는 것이 가장 빠르다.

## C-3 — 직접 만든 복사 생성자가 새로 추가된 멤버를 빠뜨림

```
증상:        부활 후 한 방 48 (기대 52)
1단계:       DamageMeter::hit → StatSystem::attackOf. 조사식에 hero.prestige(), hero.buffs()
             → 부활 전에는 prestige 2, 부활 후에는 0
2단계:       Aria를 만든 다음 줄에서 멈춰 데이터 중단점을 &aria.prestige_에 걸고 계속 → prestigeUp에서 두 번 멈춘 뒤, respawn 안의 swap에서 멈춤
             호출 스택: swap ← Hero::operator= ← RespawnSystem::respawn
3단계:       swap 직전 other.prestige_가 이미 0. other는 checkpoints_.snapshotOf(...)의 복사본
             → 조사식에 checkpoints.snapshots_ → 저장된 Aria도 prestige_ 0
4단계:       CheckpointSystem::save의 insert_or_assign에 중단점, 함수 안으로 들어가면(step into) Hero(const Hero&)
             → 초기화 목록에 prestige_가 없다. 기본값 0으로 남는다 (펫을 깊은 복사하려고 직접 만든 복사 생성자, prestige_는 시즌 2에서 추가)
```
**가짜 용의자:** `RespawnSystem::respawn`의 `hero.clearBuffs()`. 줄어든 피해(48 → 52, 약 10%)가 사라진 "전투 함성 +10%"와 딱 맞아 보인다. 하지만 기획서 7.1대로 죽으면 버프는 사라져야 하고, 프레스티지 2단계도 +10%라서 우연히 같다.
**증상만 가리는 수정:** `clearBuffs()`를 지우면 0 + 10% = 52가 되어 화면은 통과. 숨은 케이스가 프레스티지 3, 버프 없는 영웅으로 확인해서 FAIL.
**수정:**
```cpp
Hero::Hero(const Hero& other)
    : ...
      buffs_(other.buffs_),
      prestige_(other.prestige_)
{
}
```
**교훈:** 복사 생성자를 직접 만들면 멤버를 추가할 때마다 거기도 고쳐야 한다(`swap`은 고쳤는데 복사 생성자는 빠뜨린 것처럼). 깊은 복사가 필요한 멤버 하나 때문이라면, 그 멤버만 복사 가능한 래퍼로 감싸고 클래스는 기본 복사(= Rule of Zero)를 쓰는 편이 안전하다. 기본값이 있는 멤버는 빠뜨려도 경고가 없다.

---

# 블라인드

## B-1 — 빈 vector에서 `size() - 1`
`shelf.size() - 1`은 `size_t`라서 빈 진열대에서 0 - 1 = 18446744073709551615. 반복이 돌고 `shelf[0]`을 읽는다. 빈 vector는 버퍼가 없어서(data()가 nullptr) 잘못된 주소 접근으로 죽는다(MSVC Debug STL은 그 전에 범위 어설션).
**수정:** `for (size_t i = 0; i + 1 < shelf.size(); ++i)`

## B-2 — 임시 문자열을 가리키는 참조를 돌려줌
`authorOr(n, i18n::L("길드 운영 위원회 (자동 공지)", "ギルド運営委員会 (自動お知らせ)"))`의 두 번째 인자는 그 줄이 끝나면 사라지는 임시 `std::string`이다. 돌려받은 `const std::string&`는 해제된 메모리를 가리킨다. (SSO 15바이트보다 길어서 힙에 있다. 짧은 이름이었다면 우연히 맞게 나왔을 수 있다.)
**수정:** `std::string authorOr(...)`로 값을 돌려준다.

## B-3 — `uint8_t` 반복 변수
`x`가 255에서 0으로 돌아가므로 폭이 257 이상이면 `x + 1 < count`가 영원히 참이다.
**수정:** `for (int x = 0; x + 1 < count; ++x)`

## B-4 — 멤버 대신 매개변수에 대입
생성자 안의 `maxHp = maxHp;`는 매개변수에 자기 자신을 대입한다(멤버는 `maxHp_`). 멤버 `maxHp_`는 초기화되지 않아 vector 안의 몬스터는 메모리 검사기가 채운 `0xA5A5A5A5`(-1515870811)가 된다. MSVC는 자기 대입에 경고를 내지 않는다(Clang은 `-Wself-assign`으로 단서를 준다. GCC는 경고가 없다). 디버거에서 `this->maxHp_`를 보면 바로 보인다.
**수정:** `maxHp_ = maxHp;` (가능하면 초기화 목록에서 `maxHp_(maxHp)`)

## B-5 — 범위 for 안에서 같은 vector에 `push_back`
첫 `push_back`에서 재할당이 일어나면 범위 for의 이터레이터가 옛 버퍼를 가리킨다. 그 뒤로는 해제된 메모리를 읽는다.
**수정:** 원래 크기만큼 인덱스로 돌고, 원소는 복사해 둔다.
```cpp
const size_t original = wave.size();
for (size_t w = 0; w < original; ++w)
{
    const Unit u = wave[w];
    for (int i = 0; i < u.summons; ++i)
        wave.push_back({ u.name + i18n::L("의 부하", "の手下"), 0 });
}
```

## B-6 — 중복 제거를 선형 검색으로 (O(n²))
접속 기록마다 지금까지 모은 목록 전체와 비교한다. 비교 약 6천만 번.
**수정:** 본 적 있는지를 `std::set`(또는 해시를 정의한 `unordered_set`)으로 확인한다. 처음 나온 순서는 결과 vector가 지킨다.
```cpp
std::set<std::pair<int, int>> seen;
for (const PlayerId& p : logins)
{
    if (seen.insert({ p.server, p.number }).second)
        unique.push_back(p);
}
```

## B-7 — `std::accumulate`의 초깃값이 `int`
초깃값 `0`의 타입이 누적 타입이 된다. 더할 때마다 `int`로 잘려 2.5 + 3.75 + 1.25 + 2.5가 8이 된다. MSVC는 경고 C4244로 `<numeric>` 안을 가리킨다(이 저장소의 빌드에서 나온다). GCC·Clang은 경고가 시스템 헤더 안에서 나므로 기본 설정에서는 보이지 않을 수 있다.
**수정:** `std::accumulate(prices.begin(), prices.end(), 0.0)`

## B-8 — `[this]` 람다를 등록한 뒤 객체가 옮겨짐
포탑을 `emplace_back`할 때마다 vector가 재할당되면 이전 포탑들은 새 주소로 옮겨진다. 경보에 등록한 람다는 옛 주소의 `this`를 들고 있다.
**수정:** 미리 `turrets.reserve(names.size())`로 옮겨지지 않게 하거나, 모두 세운 뒤에 연결하거나, 뒤에만 추가할 때 주소가 변하지 않는 `std::deque`에 둔다(`push_back`만 하면 원소가 옮겨지지 않는다). `std::list`나 `unique_ptr`의 vector도 주소는 안정적이지만, 그러면 숨은 테스트의 `turrets.at(0)` 같은 접근 방식도 함께 바꿔야 한다.

---

# 출처

| 문제 | 가져온 부 | 종류 |
|---|---|---|
| C-1 | 4부 컨테이너 + 3부 흐름 추적 | 컨테이너(위치를 아이디로 쓰기), 숨은 부작용 |
| C-2 | 2부 심화(랩 13) + 3부 흐름 추적(랩 24) | 컨테이너(순회 중 수정), 흐름과 순서 |
| C-3 | 2부 심화(랩 12, 15) + 3부 흐름 추적 | 객체 생성과 소멸 |
| B-1 | 2부(랩 14) + 4부 | 수치(unsigned), 컨테이너 |
| B-2 | 2부(랩 11) + 9부 | 포인터와 수명 |
| B-3 | 11부 | 멈춤(무한루프), 수치 |
| B-4 | 2부(랩 12) | 객체 생성과 소멸 |
| B-5 | 2부(랩 10, 13) | 컨테이너, 포인터와 수명 |
| B-6 | 12부(랩 93) | 성능 |
| B-7 | 1부(랩 3) | 수치 |
| B-8 | 9부(랩 75) + 2부(랩 10) | 람다 캡처, 포인터와 수명 |
