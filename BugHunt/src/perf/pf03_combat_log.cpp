#include "../labs.h"
#include "perf_kit.h"
#include <string>
#include <vector>

// 전투 기록 내보내기. 레이드가 끝나면 모든 피해 이벤트를 한 줄씩 이어 붙여 텍스트로 만든다.
// 긴 레이드일수록 "내보내기" 버튼을 누른 뒤 한참 멈춘다. 결과 텍스트는 맞다.
namespace perf3
{
using Text = std::basic_string<char, std::char_traits<char>, perf::CountingAllocator<char>>;

struct DamageEvent
{
    int tick;
    int amount;
};

Text toText(int value)
{
    const std::string s = std::to_string(value);
    return Text(s.begin(), s.end());
}

Text exportLog(const std::vector<DamageEvent>& events)
{
    Text log;
    for (const DamageEvent& e : events)
        log = log + "[" + toText(e.tick) + "] hit " + toText(e.amount) + "\n";
    return log;
}

std::vector<DamageEvent> makeRaid(int count)
{
    std::vector<DamageEvent> events;
    for (int i = 0; i < count; ++i)
        events.push_back({ i * 3, 100 + (i * 37) % 900 });
    return events;
}
}

void pf03_combat_log()
{
    const auto events = perf3::makeRaid(4000);

    perf::resetAllocCounters();
    const perf::Stopwatch sw;
    const perf3::Text log = perf3::exportLog(events);
    std::cout << "  이벤트 " << events.size() << "개 내보내기: " << sw.ms() << " ms, 할당 " << perf::g_allocCount
              << "번, " << perf::g_allocBytes / 1024 << " KB (결과 " << log.size() / 1024 << " KB)\n";

    expect("결과 길이", static_cast<size_t>(60294), log.size());
    expect("할당한 바이트가 결과 길이의 16배 이하인가? (결과 길이에 비례하는가)", true, perf::g_allocBytes <= 16LL * static_cast<long long>(log.size()));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// Release + 성능 프로파일러(Alt+F2). CPU 사용량의 핫 경로에 memcpy/memmove나 할당 함수가 크게 보이나요?
// 이벤트 수를 2배로 늘리면 걸린 시간과 할당 바이트는 각각 몇 배가 되나요?
#pragma endregion

#pragma region 힌트 2
// log = log + ... 한 줄이 실행될 때 만들어지는 임시 문자열은 몇 개이고, 각각 얼마나 긴가요?
// 4000번째 줄을 붙일 때 복사되는 글자 수는?
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void pf03_combat_log_hidden()
{
    const auto events = perf3::makeRaid(1500);
    perf::resetAllocCounters();
    const perf3::Text log = perf3::exportLog(events);
    expect("결과 길이", static_cast<size_t>(22128), log.size());
    expect("첫 줄", std::string("[0] hit 100\n"), std::string(log.substr(0, 12)));
    expect("할당 바이트가 결과의 16배 이하인가?", true, perf::g_allocBytes <= 16LL * static_cast<long long>(log.size()));
}
#pragma endregion
