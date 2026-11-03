#pragma once

// 기본 스탯표 랩의 스탯 접근 함수. 두 번역 단위(lg05_stat_table.cpp, lg05_class_stats.cpp)에 나뉘어 있다.
// [JA] 基本ステータス表のラボのステータス取得関数です。二つの翻訳単位(lg05_stat_table.cpp, lg05_class_stats.cpp)に分かれています。
namespace lang5
{
// 설정 파일에서 읽은 값 (lg05_stat_table.cpp)
int readStat(const char* key, int fallback);

// lg05_stat_table.cpp: 기본값과 보스
int baseHp();
int baseMana();
int bossAttack();
int eliteArmor();

// lg05_class_stats.cpp: 기본값과 직업
int baseAttack();
int baseArmor();
int warriorHp();
int mageMana();
}
