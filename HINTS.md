# 힌트 (한 단계씩만 열어 보세요)

# 1부: 호출 스택 입문

모든 랩에서 순서는 같습니다: **멈춘 함수 → 이상한 값 → 그 값을 넘겨준 프레임 → 진짜 원인**

## Lab 1 — 친구 프로필
README의 "따라 해 보기"를 그대로 따라 하세요.
<details><summary>힌트 1</summary><code>showProfile</code> 프레임에서 <code>name</code> 값을 <code>g_players</code>에 있는 이름들과 한 글자씩 비교해 보세요.</details>

## Lab 2 — 퀵슬롯
<details><summary>힌트 1</summary>Assertion 창에서 <b>다시 시도(Retry)</b>를 누르세요. 호출 스택 맨 위에 <code>std::vector&lt;...&gt;::operator[]</code>나 <code>[외부 코드]</code>가 보이면, 그 아래에서 <b>lab2::</b>로 시작하는 첫 줄을 찾으세요.</details>
<details><summary>힌트 2</summary><code>itemAt</code> 프레임의 <code>slotIndex</code>, <code>onNumberKey</code> 프레임의 <code>key</code>를 보세요. 벡터에는 원소가 몇 개 있나요?</details>
<details><summary>힌트 3</summary>콘솔 출력도 단서입니다. 1번 키를 눌렀는데 무엇이 나왔나요? 크래시가 없던 줄도 사실 틀렸던 겁니다.</details>

## Lab 3 — 전투 기록 DPS
<details><summary>힌트 1</summary><code>damagePerSecond</code>에서 <code>seconds</code>는 0입니다. 0으로 나누는 코드는 문제가 아닙니다. 왜 0이 들어왔을까요?</details>
<details><summary>힌트 2</summary>한 칸 아래 <code>printBattleSummary</code> 프레임에서 <code>battle</code>을 펼쳐 보세요. 어떤 적이고 <code>durationMs</code>는 얼마인가요?</details>
<details><summary>힌트 3</summary>800 / 1000 은 정수 나눗셈으로 얼마인가요?</details>

## Lab 4 — 파티 게시판
<details><summary>힌트 1</summary><code>leaderOf</code>를 부르는 곳은 세 군데입니다. 그중 어떤 곳에서 불렀는지 호출 스택이 정확히 알려 줍니다.</details>
<details><summary>힌트 2</summary><code>printPartyBoard</code>에는 빈 파티 검사가 있습니다. 호출 스택에 그 함수가 있나요?</details>

## Lab 5 — 불꽃놀이
<details><summary>힌트 1</summary>호출 스택이 <code>lab5::countdown</code>으로 가득 차 있습니다. 맨 위 몇 줄의 매개변수 <code>n</code> 값을 읽어 보세요.</details>
<details><summary>힌트 2</summary><code>n</code>이 홀수인가요, 짝수인가요? 그럼 처음에 어떤 숫자로 시작했을까요? (스택이 너무 깊으면 VS가 맨 아래를 잘라서 보여 줄 수도 있습니다. 이럴 땐 값으로 추리하세요.)</details>
<details><summary>힌트 3</summary>2씩 빼다 보면 <code>n == 0</code>을 영원히 건너뛰는 경우가 있습니다.</details>

## Lab 6 — 상점 계산
<details><summary>힌트 1</summary>디버거가 "처리되지 않은 예외: std::out_of_range"로 멈춥니다. 호출 스택에서 <code>std::map::at</code> 같은 외부 코드 아래의 첫 <b>lab6::</b> 줄을 찾으세요.</details>
<details><summary>힌트 2</summary><code>priceOf</code> 프레임의 <code>itemId</code>를 <code>kPrices</code>의 키와 비교해 보세요.</details>
<details><summary>보너스</summary>Ctrl+Alt+E(예외 설정)에서 "C++ Exceptions"를 체크하면 예외를 잡든 안 잡든 <b>던져지는 순간</b> 멈춥니다.</details>

## Lab 7 — HP 이벤트
<details><summary>힌트 1</summary><code>HealthBar::setValue</code>에서 지역 창의 <code>this</code>를 보세요. 객체가 없는데 멤버 함수가 불렸습니다.</details>
<details><summary>힌트 2</summary>한 칸 아래, 이름이 <code>&lt;lambda_1&gt;::operator()</code>처럼 생긴 줄이 람다입니다. 거기서 캡처된 <code>bar</code> 값을 보세요.</details>
<details><summary>힌트 3</summary>호출 스택은 <b>지금</b>만 보여 줍니다. <code>bar</code>가 언제 nullptr로 저장됐는지는 스택에 없어요. <code>connectHealthBar</code>에 중단점(F9)을 걸고 다시 실행해서 그 순간 <code>game.healthBar</code>를 보세요.</details>
<details><summary>힌트 4</summary><code>startGame</code>에서 함수를 부르는 순서를 보세요.</details>

## Lab 8 — 몬스터 웨이브
<details><summary>힌트 1</summary>Assertion → 다시 시도. <code>statsFor</code> 프레임의 <code>level</code>은 얼마인가요? 테이블 크기는요?</details>
<details><summary>힌트 2</summary>한 칸씩 아래로 내려가면서 <code>level</code>이 어디서 왔는지 보세요: <code>createMonster</code> → <code>spawnMonster</code> → <code>spawnWave</code>.</details>
<details><summary>힌트 3</summary><code>spawnMonster</code>의 매개변수 순서와 <code>spawnWave</code>에서 넘긴 인자 순서를 비교해 보세요.</details>

---

# 2부: 심화

## Lab 9 — 점수 평균
<details><summary>힌트 1</summary>어설션 창에서 "다시 시도"를 누르면 디버거가 멈춥니다. 호출 스택에서 <b>내 코드</b>가 나오는 첫 프레임을 더블클릭하세요.</details>
<details><summary>힌트 2</summary>그 순간 <code>i</code>와 <code>scores.size()</code>의 값은?</details>

## Lab 10 — 인벤토리 보상
<details><summary>힌트 1</summary><code>sword</code>를 받은 직후 조사식에 <code>&sword</code>와 <code>&amp;inv.items_[0]</code>를 넣고, F10으로 <code>add</code>를 하나씩 진행하며 두 주소를 비교하세요.</details>
<details><summary>힌트 2</summary><code>std::vector</code>의 <code>capacity()</code>가 언제 바뀌나요? 바뀌면 원소들은 어디에 있게 되나요?</details>
<details><summary>힌트 3</summary>ASan 구성으로 실행해 보세요. 리포트의 "freed by thread T0 here" 스택을 읽어 보세요.</details>

## Lab 11 — 플레이어 칭호
<details><summary>힌트 1</summary><code>titleOf</code>가 리턴하는 <code>string_view</code>가 가리키는 문자열은 누가 소유하나요?</details>
<details><summary>힌트 2</summary>그 문자열의 수명은 어디서 끝나나요? ASan으로 확인해 보세요.</details>

## Lab 12 — 콤보 카운터
<details><summary>힌트 1</summary><code>ComboCounter counter;</code> 다음 줄에 중단점을 걸고 지역 변수 창에서 멤버 값을 보세요.</details>
<details><summary>힌트 2</summary>-858993460을 16진수로 보면?  README의 매직 넘버 표를 보세요.</details>

## Lab 13 — 죽은 적 제거
<details><summary>힌트 1</summary>어설션 메시지를 그대로 읽어 보세요. "invalidated"가 무슨 뜻일까요?</details>
<details><summary>힌트 2</summary><code>erase</code>의 리턴값은 무엇인가요? (cppreference 확인)</details>

## Lab 14 — 배낭 용량
<details><summary>힌트 1</summary><code>canPickUp</code> 안에서 <code>freeSlots</code>의 값을 조사식으로 보세요.</details>
<details><summary>힌트 2</summary>unsigned에서 20 - 22 는?</details>

## Lab 15 — 패킷 버퍼
<details><summary>힌트 1</summary>생성자와 소멸자 첫 줄에 중단점을 걸고 <code>this</code>와 <code>data_</code>를 기록해 보세요. 생성자는 몇 번, 소멸자는 몇 번 불리나요?</details>
<details><summary>힌트 2</summary>같은 <code>data_</code> 주소를 가진 객체가 두 개 생기는 순간은 언제인가요? (보이지 않는 함수가 호출되고 있습니다)</details>
<details><summary>힌트 3</summary>"Rule of Three / Five / Zero"를 찾아보세요.</details>

## Lab 16 — 상태 이상 효과
<details><summary>힌트 1</summary><code>Burn</code>에 소멸자를 하나 직접 만들고 중단점을 걸어 보세요. 불리나요?</details>
<details><summary>힌트 2</summary><code>unique_ptr&lt;StatusEffect&gt;</code>는 무슨 타입의 포인터로 <code>delete</code>를 할까요? 빌드 경고도 다시 읽어 보세요.</details>

## Lab 17 — 길드 은행
<details><summary>힌트 1</summary>여러 번 실행해 보세요. 결과가 매번 다르면 무엇을 의심해야 할까요?</details>
<details><summary>힌트 2</summary><code>deposit</code> 안에 중단점을 걸고 스레드 창을 보세요. 두 스레드가 같은 <code>current</code> 값을 읽는 상황을 상상해 보세요.</details>
<details><summary>참고</summary>Release에서는 최적화 때문에 우연히 맞게 나올 수도 있습니다. 그래도 버그입니다.</details>

## Lab 18 — 파티 칭호 수여
<details><summary>힌트 1</summary><code>levelUpAll(party);</code> 첫 호출에 중단점 → 조사식에 <code>&amp;party[0].hp</code> → 데이터 중단점(값 변경 시 중단) 설정 → F5.</details>
<details><summary>힌트 2</summary>멈춘 곳의 호출 스택을 보세요. hp를 쓰는 코드가 아닌데 hp가 바뀌었다면?</details>
<details><summary>힌트 3</summary>메모리 창에 <code>&amp;party[0]</code>를 넣고 <code>name</code>과 <code>hp</code>가 메모리에서 어떻게 붙어 있는지 보세요. "Sir Kael"은 몇 바이트인가요?</details>
<details><summary>참고</summary>이 버그는 ASan도 /RTC도 못 잡습니다(구조체 내부 오버플로). 데이터 중단점이 답입니다.</details>

## Lab 19 — 퀘스트 선행 조건
<details><summary>힌트 1</summary>호출 스택이 같은 함수로 가득 차 있을 겁니다. 몇 개 프레임을 더블클릭해서 <code>questId</code>를 확인하세요.</details>
<details><summary>힌트 2</summary><code>questId</code> 값이 어떤 순서로 반복되나요? 데이터 테이블과 비교해 보세요.</details>

## Lab 20 — 상태 플래그
<details><summary>힌트 1</summary>빌드 출력의 경고를 읽어 보세요. 경고는 공짜 디버거입니다.</details>
<details><summary>힌트 2</summary><code>&amp;</code>와 <code>==</code> 중 무엇이 먼저 계산되나요?</details>
