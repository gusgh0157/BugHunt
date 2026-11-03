#include "../labs.h"
#include <string>
#include <vector>

// 챌린지 B-2

// [JA] チャレンジ B-2
namespace blind2
{
struct Notice
{
    std::string title;
    std::string author;   // 비어 있으면 시스템 공지
};

const std::string& authorOr(const Notice& n, const std::string& fallback)
{
    return n.author.empty() ? fallback : n.author;
}

std::vector<std::string> boardLines(const std::vector<Notice>& notices)
{
    std::vector<std::string> lines;
    for (const Notice& n : notices)
    {
        const std::string& who = authorOr(n, "길드 운영 위원회 (자동 공지)");
        lines.push_back(n.title + " - " + who);
    }
    return lines;
}
}

void chB2()
{
    using namespace blind2;
    const std::vector<Notice> notices = {
        { "레이드 일정", "Mira" },
        { "점검 안내", "" },
        { "길드전 신청", "Kael" },
    };
    const auto lines = boardLines(notices);
    for (const std::string& line : lines)
        std::cout << "  " << line << '\n';
    expect("둘째 줄", std::string("점검 안내 - 길드 운영 위원회 (자동 공지)"), lines.at(1));
    expect("셋째 줄", std::string("길드전 신청 - Kael"), lines.at(2));
}

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
void chB2_hidden()
{
    using namespace blind2;
    const auto lines = boardLines({ { "a", "" }, { "b", "" }, { "c", "Doran" } });
    expect("첫 줄", std::string("a - 길드 운영 위원회 (자동 공지)"), lines.at(0));
    expect("둘째 줄", std::string("b - 길드 운영 위원회 (자동 공지)"), lines.at(1));
    expect("셋째 줄", std::string("c - Doran"), lines.at(2));
    const std::string fallback = "아무개";
    expect("이름 있는 공지", std::string("Ivo"), authorOr({ "t", "Ivo" }, fallback));
    expect("이름 없는 공지", std::string("아무개"), authorOr({ "t", "" }, fallback));

    // 임시 fallback을 넘기고 결과를 붙잡아 둔다. 값을 돌려주는 authorOr라면 수명이 연장되어 안전하다.
    const std::string& kept = authorOr({ "t", "" }, std::string("임시로 만든 기본 작성자 이름"));
    expect("임시 fallback으로 받은 이름", std::string("임시로 만든 기본 작성자 이름"), kept);
}
#pragma endregion
