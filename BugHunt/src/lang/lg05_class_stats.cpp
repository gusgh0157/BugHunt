#include "lg05_stats.h"

// 직업별 스탯. 기본값 일부는 lg05_stat_table.cpp에 있다.
// [JA] 職業別のステータスです。基本値の一部は lg05_stat_table.cpp にあります。
namespace lang5
{
int g_baseAttack = readStat("base_attack", 12);
int g_baseArmor = readStat("base_armor", 7);

int g_warriorHp = baseHp() * 3;      // 전사는 기본 체력의 3배
int g_mageMana = baseMana() * 5;     // 마법사는 기본 마나의 5배

int baseAttack() { return g_baseAttack; }
int baseArmor() { return g_baseArmor; }
int warriorHp() { return g_warriorHp; }
int mageMana() { return g_mageMana; }
}
