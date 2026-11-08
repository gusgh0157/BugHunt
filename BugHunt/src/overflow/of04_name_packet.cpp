#include "../labs.h"
#include <cstddef>
#include <cstring>
#include <string>

// 접속한 플레이어의 이름을 이름 패킷(고정 크기 char 배열)에 담아 보낸다.
// 패킷 버퍼는 매번 새로 만들지 않고 하나를 재사용한다. 이름은 널 문자('\0')로 끝나는 C 문자열이다.

// [JA] 接続したプレイヤーの名前を名前パケット(固定サイズの char 配列)に入れて送ります。
// [JA] パケットバッファーは毎回新しく作らず、一つを使い回します。名前はヌル文字('\0')で終わる C 文字列です。
namespace ovf4
{
struct NamePacket
{
    char name[32];   // 널 문자 포함 최대 32바이트
    int level;
};

// 이름을 패킷 버퍼에 복사한다
void writeName(char* dst, const char* src)
{
    std::memcpy(dst, src, sizeof(dst));
}

std::string sendLogin(NamePacket& packet, const char* name, int level)
{
    writeName(packet.name, name);
    packet.level = level;
    return std::string(packet.name);   // 받는 쪽은 널 문자까지를 이름으로 읽는다
}
}

void of04_name_packet()
{
    using namespace ovf4;

    NamePacket packet{};
    const char previous[] = "Silverwind_Archer";   // 직전 접속자의 이름이 버퍼에 남아 있다
    std::memcpy(packet.name, previous, sizeof(previous));
    const std::string first = sendLogin(packet, "Nightingale", 31);
    const std::string second = sendLogin(packet, "Moonshadow", 47);
    std::cout << i18n::L("  받은 이름: \"", "  受け取った名前: \"") << first << "\", \"" << second << "\"\n";

    expect("첫 번째 이름", std::string("Nightingale"), first);
    expect("두 번째 이름", std::string("Moonshadow"), second);
    expect("두 번째 레벨", 47, packet.level);
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// writeName에서 멈추고 조사식에 sizeof(dst)와 std::strlen(src)를 넣어 보세요.
// 두 번째 호출이 끝난 뒤 메모리 창에서 packet.name을 32바이트 보세요. 어디까지가 두 번째 이름이고, 그 뒤의 글자는 어디서 왔나요?

// [JA] writeName で止めて、ウォッチに sizeof(dst) と std::strlen(src) を入れてみてください。
// [JA] 2回目の呼び出しが終わった後、メモリ ウィンドウで packet.name を32バイト見てください。どこまでが2番目の名前で、その後ろの文字はどこから来たのでしょうか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 함수 매개변수로 받은 배열은 포인터로 바뀝니다(배열 붕괴). 그래서 sizeof(dst)는 배열 크기(32)가 아니라 포인터 크기(64비트에서 8)입니다.
// 이름의 앞 8바이트만 복사되고 널 문자는 복사되지 않습니다. 버퍼를 재사용하므로 9번째 바이트부터는 앞사람 이름이 그대로 남아 이어 붙습니다.
// 크기를 strlen(src)로만 고치면 널 문자 자리(+1)가 빠져 같은 일이 생기고, 버퍼보다 긴 이름이면 level까지 덮어씁니다.

// [JA] 関数の引数として受け取った配列はポインターに変わります(配列の減衰)。そのため sizeof(dst) は配列の大きさ(32)ではなく、ポインターの大きさ(64ビットでは8)です。
// [JA] 名前の先頭8バイトだけがコピーされ、ヌル文字はコピーされません。バッファーを使い回しているので、9バイト目からは前の人の名前がそのまま残ってつながります。
// [JA] サイズを strlen(src) だけに直すと、ヌル文字の分(+1)が抜けて同じことが起こり、バッファーより長い名前なら level まで上書きします。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 버퍼 크기를 함께 넘기고, 넘치지 않게 자른 뒤 널 문자를 직접 넣으세요.
//   void writeName(char* dst, std::size_t cap, const char* src) { std::size_t n = std::min(std::strlen(src), cap - 1); std::memcpy(dst, src, n); dst[n] = '\0'; }
//   호출: writeName(packet.name, sizeof(packet.name), name);  (여기서 sizeof는 진짜 배열이라 32)
// 템플릿 매개변수 char (&dst)[N]으로 받으면 크기를 잃지 않습니다. 패킷 밖에서는 std::string을 쓰세요.

// [JA] バッファーの大きさを一緒に渡し、あふれないように切り詰めてから、ヌル文字を自分で入れてください。
// [JA]   void writeName(char* dst, std::size_t cap, const char* src) { std::size_t n = std::min(std::strlen(src), cap - 1); std::memcpy(dst, src, n); dst[n] = '\0'; }
// [JA]   呼び出し: writeName(packet.name, sizeof(packet.name), name);  (ここの sizeof は本物の配列なので32)
// [JA] テンプレート引数 char (&dst)[N] で受け取れば大きさを失いません。パケットの外では std::string を使ってください。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void of04_name_packet_hidden()
{
    using namespace ovf4;
    NamePacket packet{};
    std::memset(packet.name, 'x', sizeof(packet.name));
    expect("숨은 접속: 긴 이름 다음 짧은 이름 (1)", std::string("Thunderclap_Berserker"), sendLogin(packet, "Thunderclap_Berserker", 12));
    expect("숨은 접속: 긴 이름 다음 짧은 이름 (2)", std::string("Rosemary"), sendLogin(packet, "Rosemary", 9));
    expect("숨은 접속: 버퍼보다 긴 이름은 잘린다", std::string("Archmage_of_the_Eternal_Twiligh"),
           sendLogin(packet, "Archmage_of_the_Eternal_Twilight_Tower", 99));
    expect("숨은 접속: 긴 이름 뒤 레벨", 99, packet.level);
    expect("숨은 접속: 한글 이름", std::string(i18n::L("그림자칼날", "シャドウブレード")), sendLogin(packet, i18n::L("그림자칼날", "シャドウブレード"), 5));
}
#pragma endregion
