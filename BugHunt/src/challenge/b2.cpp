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
        const std::string& who = authorOr(n, i18n::L("길드 운영 위원회 (자동 공지)", "ギルド運営委員会 (自動お知らせ)"));
        lines.push_back(n.title + " - " + who);
    }
    return lines;
}
}

void chB2()
{
    using namespace blind2;
    const std::vector<Notice> notices = {
        { i18n::L("레이드 일정", "レイド日程"), "Mira" },
        { i18n::L("점검 안내", "メンテナンスのお知らせ"), "" },
        { i18n::L("길드전 신청", "ギルド戦の申し込み"), "Kael" },
    };
    const auto lines = boardLines(notices);
    for (const std::string& line : lines)
        std::cout << "  " << line << '\n';
    expect(i18n::L("둘째 줄", "2行目"), std::string(i18n::L("점검 안내 - 길드 운영 위원회 (자동 공지)", "メンテナンスのお知らせ - ギルド運営委員会 (自動お知らせ)")), lines.at(1));
    expect(i18n::L("셋째 줄", "3行目"), std::string(i18n::L("길드전 신청 - Kael", "ギルド戦の申し込み - Kael")), lines.at(2));
}

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
void chB2_hidden()
{
    using namespace blind2;
    const auto lines = boardLines({ { "a", "" }, { "b", "" }, { "c", "Doran" } });
    expect(i18n::L("첫 줄", "1行目"), std::string(i18n::L("a - 길드 운영 위원회 (자동 공지)", "a - ギルド運営委員会 (自動お知らせ)")), lines.at(0));
    expect(i18n::L("둘째 줄", "2行目"), std::string(i18n::L("b - 길드 운영 위원회 (자동 공지)", "b - ギルド運営委員会 (自動お知らせ)")), lines.at(1));
    expect(i18n::L("셋째 줄", "3行目"), std::string("c - Doran"), lines.at(2));
    const std::string fallback = i18n::L("아무개", "名無し");
    expect(i18n::L("이름 있는 공지", "名前のあるお知らせ"), std::string("Ivo"), authorOr({ "t", "Ivo" }, fallback));
    expect(i18n::L("이름 없는 공지", "名前のないお知らせ"), std::string(i18n::L("아무개", "名無し")), authorOr({ "t", "" }, fallback));

    // 임시 fallback을 넘기고 결과를 붙잡아 둔다. 값을 돌려주는 authorOr라면 수명이 연장되어 안전하다.
    const std::string& kept = authorOr({ "t", "" }, std::string(i18n::L("임시로 만든 기본 작성자 이름", "一時的に作った既定の作成者名")));
    expect(i18n::L("임시 fallback으로 받은 이름", "一時的な fallback で受け取った名前"), std::string(i18n::L("임시로 만든 기본 작성자 이름", "一時的に作った既定の作成者名")), kept);
}
#pragma endregion
