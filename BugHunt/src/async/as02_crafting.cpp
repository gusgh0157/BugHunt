#include "../labs.h"
#include <future>
#include <stdexcept>
#include <string>
#include <vector>

// 대장간 일괄 제작. 주문서의 장비를 비동기 작업으로 동시에 만든다.
// 재료가 모자란 주문은 실패로 기록하고 건너뛰어야 한다. 한 주문 때문에 대장간 전체가 멈추면 안 된다.
namespace async2
{
struct Recipe
{
    std::string item;
    int ore;    // 필요한 광석
    int price;  // 판매가
};

// 장비 하나를 만든다. 광석이 모자라면 예외를 던진다.
int craft(const Recipe& recipe, int oreInStock)
{
    if (recipe.ore > oreInStock)
        throw std::runtime_error(recipe.item + ": 광석이 모자람");
    return recipe.price;
}

struct CraftReport
{
    int revenue = 0;
    int failed = 0;
};

CraftReport craftAll(const std::vector<Recipe>& orders, int oreInStock)
{
    CraftReport report;
    std::vector<std::future<int>> jobs;
    for (const Recipe& recipe : orders)
    {
        try
        {
            jobs.push_back(std::async(std::launch::async, craft, recipe, oreInStock));
        }
        catch (const std::exception& e)
        {
            std::cout << "  제작 실패: " << e.what() << '\n';
            ++report.failed;
        }
    }

    for (std::future<int>& job : jobs)
        report.revenue += job.get();
    return report;
}
}

void as02_crafting()
{
    using namespace async2;
    const std::vector<Recipe> orders = {
        { "Iron Sword", 3, 120 },
        { "Iron Shield", 4, 150 },
        { "Mithril Helm", 12, 900 },   // 광석 10개로는 못 만든다
        { "Iron Boots", 2, 80 },
    };

    const CraftReport report = craftAll(orders, 10);
    expect("판매 수익 (120 + 150 + 80)", 350, report.revenue);
    expect("실패한 주문 수", 1, report.failed);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// 디버거에서 "예외가 던져질 때 멈춤"을 켜고 실행해 보세요. 어느 스레드에서, 어느 함수가 던지나요?
// 그 스레드는 craftAll의 try 블록 안에 있나요? 스레드 목록에서 메인 스레드가 그 순간 어디에 있는지도 보세요.
#pragma endregion

#pragma region 힌트 2
// 계속 실행하면 프로그램이 abort로 끝납니다. 이번에는 메인 스레드의 호출 스택을 보세요.
// 예외가 메인 스레드로 다시 튀어나온 곳은 std::async를 부른 줄인가요, 아니면 다른 줄인가요?
#pragma endregion

#pragma region 힌트 3
// 비동기 작업이 던진 예외는 future에 담겨 있다가, get()을 부를 때 그 자리에서 다시 던져집니다.
// try/catch는 예외가 실제로 나오는 곳을 감싸야 합니다. (get()은 future 하나에 한 번만 부를 수 있습니다. valid()로 확인)
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void as02_crafting_hidden()
{
    using namespace async2;
    const std::vector<Recipe> orders = {
        { "Dragon Lance", 30, 5000 },
        { "Bronze Ring", 1, 40 },
        { "Steel Axe", 6, 260 },
        { "Rune Staff", 25, 3000 },
        { "Leather Cap", 0, 15 },
    };
    const CraftReport report = craftAll(orders, 8);
    expect("판매 수익 (40 + 260 + 15)", 315, report.revenue);
    expect("실패한 주문 수", 2, report.failed);
}
#pragma endregion
