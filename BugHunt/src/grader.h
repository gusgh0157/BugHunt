#pragma once
#include <string>
#include <vector>

// 채점기 (BugHunt.exe --grade). 랩마다 자기 자신을 자식 프로세스로 다시 띄워
// 종료 코드와 시간으로 PASS / FAIL / TIMEOUT / CRASH를 판정한다.
// [JA] 採点器 (BugHunt.exe --grade)。ラボごとに自分自身を子プロセスとして再起動し、
// [JA] 終了コードと時間で PASS / FAIL / TIMEOUT / CRASH を判定します。
struct GradeTarget
{
    int id;              // grade-report.json의 id. 챌린지는 C-n = 100 + n, B-n = 200 + n [JA] grade-report.json の id。チャレンジは C-n = 100 + n、B-n = 200 + n
    std::string label;   // 표에 보이는 번호 ("7", "C-1") [JA] 表に表示される番号 ("7", "C-1")
    const char* title;
    int part;
    bool hasHiddenCase;
};

// 표를 콘솔에 출력하고 grade-report.json을 현재 폴더에 쓴다. 만점이면 0을 돌려준다.
// [JA] 表をコンソールに出力し、grade-report.json を現在のフォルダーに書き出します。満点なら 0 を返します。
int runGrader(const std::vector<GradeTarget>& targets, int totalCount, const char* buildName);

// 자식 프로세스(--grade-child)용: 크래시나 어설션이 대화 상자를 띄우지 않고 바로 끝나게 한다.
// [JA] 子プロセス(--grade-child)用: クラッシュやアサーションがダイアログを出さずにすぐ終了するようにします。
void prepareGradeChild();
