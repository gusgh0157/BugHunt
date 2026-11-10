#pragma once
#include <string>

// 저장 통계. 자동 저장(lg06_autosave.cpp)이 갱신하고 저장 화면(lg06_save_screen.cpp)이 보여 준다.
// [JA] セーブの統計です。オートセーブ(lg06_autosave.cpp)が更新し、セーブ画面(lg06_save_screen.cpp)が表示します。
namespace lang6
{
struct SaveStats
{
    int count = 0;              // 저장한 횟수 [JA] セーブした回数
    std::string lastSlot;       // 마지막으로 저장한 슬롯 [JA] 最後にセーブしたスロット
};

// 게임 전체에서 하나만 있는 저장 통계
// [JA] ゲーム全体で一つだけのセーブ統計
static SaveStats g_saveStats;

// 자동 저장 (lg06_autosave.cpp)
// [JA] オートセーブ (lg06_autosave.cpp)
void autosave(const std::string& slot);
}
