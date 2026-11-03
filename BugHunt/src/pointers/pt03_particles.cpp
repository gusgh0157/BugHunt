#include "../labs.h"
#include <cstddef>

// 스킬 이펙트. 폭발 한 번에 파티클 여러 개를 한꺼번에 만들고, 이펙트가 끝나면 한꺼번에 지운다.
// 파티클은 소멸자에서 GPU 버퍼 슬롯을 반납한다. (여기서는 카운터로 흉내 낸다)

// [JA] スキルエフェクトです。1回の爆発でパーティクルをまとめて複数作り、エフェクトが終わるとまとめて消します。
// [JA] パーティクルはデストラクターで GPU バッファーのスロットを返却します。(ここではカウンターで真似しています)
namespace ptr3
{
int g_slotsInUse = 0;

class Particle
{
public:
    Particle() { ++g_slotsInUse; }
    ~Particle() { --g_slotsInUse; }

    void update(float dt) { life_ -= dt; }

private:
    float life_ = 1.0f;
};

class Explosion
{
public:
    explicit Explosion(int count)
        : particles_(new Particle[count]), count_(count)
    {
    }

    ~Explosion() { delete particles_; }

    void update(float dt)
    {
        for (int i = 0; i < count_; ++i)
            particles_[i].update(dt);
    }

private:
    Particle* particles_;
    int count_;
};

void playExplosion(int particleCount)
{
    Explosion boom(particleCount);
    for (int frame = 0; frame < 30; ++frame)
        boom.update(1.0f / 30);
}
}

void pt03_particles()
{
    ptr3::g_slotsInUse = 0;

    ptr3::playExplosion(16);
    std::cout << "  첫 번째 폭발 끝\n";
    ptr3::playExplosion(16);
    std::cout << "  두 번째 폭발 끝\n";

    expect("사용 중인 GPU 슬롯", 0, ptr3::g_slotsInUse);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 디버거로 실행하면 abort()에서 멈춥니다. 콘솔의 [CRASH] 메시지를 읽고, 호출 스택에서 내 코드가 처음 나오는 줄을 찾으세요.
// 크래시 직전 콘솔에 무엇이 나왔나요? 첫 번째 폭발은 끝났나요?

// [JA] デバッガーで実行すると abort() で止まります。コンソールの [CRASH] メッセージを読み、呼び出し履歴で自分のコードが最初に出てくる行を探してください。
// [JA] クラッシュ直前、コンソールには何が出力されましたか? 1回目の爆発は終わりましたか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// ~Particle()에 중단점을 걸고 첫 번째 폭발이 끝날 때 몇 번 걸리는지 세어 보세요. 16번인가요?
// 조사식에 g_slotsInUse를 넣어 두면 편합니다.

// [JA] ~Particle() にブレークポイントを置き、1回目の爆発が終わるときに何回止まるか数えてみてください。16回ですか?
// [JA] ウォッチに g_slotsInUse を入れておくと便利です。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 메모리 보기에 (char*)particles_ - 8 을 넣어 보세요. 배열 바로 앞 8바이트에 어떤 숫자가 들어 있나요?
// new로 만든 것과 new[]로 만든 것은 지우는 방법이 다릅니다.

// [JA] メモリ ウィンドウに (char*)particles_ - 8 を入れてみてください。配列のすぐ前の8バイトにはどんな数が入っていますか?
// [JA] new で作ったものと new[] で作ったものは、消し方が違います。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void pt03_particles_hidden()
{
    ptr3::g_slotsInUse = 0;
    {
        ptr3::Explosion big(40);
        expect("폭발 중 사용 중인 슬롯", 40, ptr3::g_slotsInUse);
    }
    expect("폭발 후 사용 중인 슬롯", 0, ptr3::g_slotsInUse);

    ptr3::playExplosion(3);
    ptr3::playExplosion(1);
    expect("작은 폭발 두 번 후 슬롯", 0, ptr3::g_slotsInUse);
}
#pragma endregion
