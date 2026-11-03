#include "lg06_save_stats.h"

// 자동 저장: 실제로는 파일에 쓰지만 이 랩에서는 통계만 갱신한다.
// [JA] オートセーブ: 実際にはファイルに書き込みますが、このラボでは統計だけを更新します。
namespace lang6
{
void autosave(const std::string& slot)
{
    ++g_saveStats.count;
    g_saveStats.lastSlot = slot;
}
}
