#include "../labs.h"
#include <string>

// 손님 등급에 따라 할인율을 정한다.
//   첫 구매 10% / VIP(멤버십 + 골드 등급 이상) 15% / 멤버십 5% / 5만 원 이상 구매 3%
//   여러 조건에 해당하면 가장 큰 할인 하나만 적용한다.
namespace flow5
{
enum class Grade { Bronze, Silver, Gold, Platinum };

struct Customer
{
    std::string name;
    Grade grade;
    int membershipMonths;
    int orderCount;

    bool isFirstOrder() const { return orderCount == 0; }
    bool isMember() const { return membershipMonths > 0; }
    bool isVip() const { return isMember() && grade >= Grade::Gold; }
};

int discountPercent(const Customer& c, int total)
{
    if (c.isFirstOrder())
        return 10;
    else if (c.isMember())
        return 5;
    else if (c.isVip())
        return 15;
    else if (total >= 50000)
        return 3;
    return 0;
}

int finalPrice(const Customer& c, int total)
{
    return total - total * discountPercent(c, total) / 100;
}
}

void ft05_discount()
{
    using namespace flow5;

    const Customer newbie  = { "Rin",    Grade::Bronze,    0,   0 };
    const Customer regular = { "Tomas",  Grade::Silver,    6,  12 };
    const Customer vip     = { "Selene", Grade::Platinum, 30, 140 };
    const Customer guest   = { "Bram",   Grade::Bronze,    0,   3 };

    expect("Rin (첫 구매, 20000원)",  18000, finalPrice(newbie, 20000));
    expect("Tomas (멤버, 20000원)",   19000, finalPrice(regular, 20000));
    expect("Selene (VIP, 20000원)",   17000, finalPrice(vip, 20000));
    expect("Bram (비회원, 60000원)",  58200, finalPrice(guest, 60000));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// ============================================================================

#pragma region 힌트 1 (먼저 직접 추적해 보고 펼치세요)
// discountPercent 첫 줄에 중단점, 조건: c.name == "Selene"
// 멈추면 조사식 창(Ctrl+Alt+W, 1)에 아래 식을 하나씩 넣어 보세요. 조사식은 멤버 함수 호출도 계산합니다.
//     c.isFirstOrder()
//     c.isMember()
//     c.isVip()
// 그리고 F10으로 진행하며 노란 화살표가 어느 return 으로 가는지 보세요.
#pragma endregion

#pragma region 힌트 2
// isVip()가 true이면서 isMember()가 false인 손님이 있을 수 있나요?
// 그렇다면 else if (c.isVip()) 줄에 도달할 수 있는 손님은 누구일까요?
#pragma endregion

#pragma region 채점용 숨은 테스트 (풀이에는 필요 없습니다. 고치지 마세요)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void ft05_discount_hidden()
{
    using namespace flow5;

    const Customer goldMember  = { "Ivo",   Grade::Gold,     2,  9 };
    const Customer firstTimer  = { "Nell",  Grade::Silver,   0,  0 };
    const Customer bigSpender  = { "Oren",  Grade::Silver,  14, 30 };
    const Customer richGuest   = { "Pia",   Grade::Platinum, 0,  5 };   // 멤버십이 없으니 VIP가 아니다

    expect("Ivo (VIP, 40000원)",        34000, finalPrice(goldMember, 40000));
    expect("Nell (첫 구매, 10000원)",    9000, finalPrice(firstTimer, 10000));
    expect("Oren (멤버, 80000원)",      76000, finalPrice(bigSpender, 80000));
    expect("Pia (비회원, 50000원)",     48500, finalPrice(richGuest, 50000));
}
#pragma endregion
