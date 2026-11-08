#include "../labs.h"
#include "c3_hero.h"
#include "c3_systems.h"

// 챌린지 C-3
// 성소에서 저장하고, 보스에게 쓰러진 뒤 성소에서 되살아나 다시 싸운다.

// [JA] チャレンジ C-3
// [JA] 聖所でセーブし、ボスに倒されたあと聖所で復活して再び戦います。
void chC3_shrine()
{
    using namespace chal3;

    Hero aria("Aria", 1, 40);
    aria.prestigeUp();          // 시즌 1, 2 만렙 달성
    aria.prestigeUp();
    aria.adoptPet("Fox", 6);
    aria.addBuff({ i18n::L("전투 함성", "雄叫び"), 10 });

    CheckpointSystem checkpoints;
    RespawnSystem respawns(checkpoints);
    DamageMeter meter;

    checkpoints.save(aria);     // 보스방 앞 성소
    meter.hit(aria, i18n::L("문지기", "門番"));
    const int beforeDeath = meter.lastHit();

    aria.setHp(0);              // 보스의 광역기
    respawns.respawn(aria);
    meter.hit(aria, i18n::L("문지기", "門番"));
    const int afterRespawn = meter.lastHit();

    for (const std::string& line : meter.log())
        std::cout << "  " << line << '\n';
    std::cout << i18n::L("  부활 후 체력 ", "  復活後のHP ") << aria.hp() << "/" << aria.maxHp() << i18n::L(", 버프 ", "、バフ ") << aria.buffs().size() << i18n::L("개\n", "個\n");

    expect("쓰러지기 전 한 방 (전투 함성 포함)", 57, beforeDeath);
    expect("부활 후 체력 (절반)", 50, aria.hp());
    expect("부활 후 한 방 (버프는 사라졌으므로 전투 함성 없이)", 52, afterRespawn);
}

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.

// [JA] 採点器(--grade)が、画面には表示されない別のデータで同じコードをもう一度検査します。
void chC3_shrine_hidden()
{
    using namespace chal3;

    Hero brom("Brom", 1, 60);
    brom.prestigeUp();
    brom.prestigeUp();
    brom.prestigeUp();
    CheckpointSystem checkpoints;
    RespawnSystem respawns(checkpoints);

    checkpoints.save(brom);
    expect("성소에 저장된 프레스티지", 3, checkpoints.snapshotOf("Brom").prestige());
    brom.addBuff({ i18n::L("광폭화", "狂暴化"), 25 });
    brom.setHp(0);
    respawns.respawn(brom);
    expect("부활 후 프레스티지", 3, brom.prestige());
    expect("부활 후 버프는 사라져야 한다 (기획서 7.1)", static_cast<size_t>(0), brom.buffs().size());
    expect("부활 후 공격력 (62 x 115%)", 71, StatSystem::attackOf(brom));

    const Hero copy = brom;
    expect("복사본의 프레스티지", 3, copy.prestige());
    Hero other("Other", 1, 10);
    other.adoptPet("Owl", 2);
    other = brom;
    expect("대입한 뒤의 프레스티지", 3, other.prestige());
    expect("대입하면 펫도 대입한 쪽을 따른다 (Brom은 펫 없음)", true, other.pet() == nullptr);
}
#pragma endregion
