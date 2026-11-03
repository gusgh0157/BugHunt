#include "../labs.h"
#include <cstdint>
#include <cstring>
#include <vector>

// 서버가 보낸 전리품 패킷을 읽는다.
// 패킷 = [헤더: 항목 수 4바이트] [항목 1] [항목 2] ... (항목 하나 = 아이템 ID 2바이트 + 개수 2바이트)

// [JA] サーバーが送った戦利品パケットを読み取ります。
// [JA] パケット = [ヘッダー: 項目数 4バイト] [項目 1] [項目 2] ... (項目一つ = アイテム ID 2バイト + 個数 2バイト)
namespace ptr4
{
struct PacketHeader
{
    uint32_t entryCount;
};

struct LootEntry
{
    uint16_t itemId;
    uint16_t count;
};

// 서버 쪽 코드: 항목들을 패킷 바이트로 만든다.
std::vector<uint8_t> buildPacket(const std::vector<LootEntry>& entries)
{
    const PacketHeader header = { static_cast<uint32_t>(entries.size()) };
    std::vector<uint8_t> bytes(sizeof(PacketHeader) + entries.size() * sizeof(LootEntry));
    std::memcpy(bytes.data(), &header, sizeof(header));
    std::memcpy(bytes.data() + sizeof(header), entries.data(), entries.size() * sizeof(LootEntry));
    return bytes;
}

// 클라이언트 쪽 코드: 패킷에서 받은 아이템 개수를 모두 더한다.
int totalItems(const std::vector<uint8_t>& packet)
{
    const PacketHeader* header = reinterpret_cast<const PacketHeader*>(packet.data());
    const LootEntry* entries = reinterpret_cast<const LootEntry*>(header + sizeof(PacketHeader));

    int total = 0;
    for (uint32_t i = 0; i < header->entryCount; ++i)
        total += entries[i].count;
    return total;
}
}

void pt04_loot_packet()
{
    // 포션 3, 화살 20, 금화 50, 열쇠 1, 보석 2
    const std::vector<ptr4::LootEntry> loot = { { 101, 3 }, { 205, 20 }, { 900, 50 }, { 777, 1 }, { 310, 2 } };
    const std::vector<uint8_t> packet = ptr4::buildPacket(loot);

    std::cout << "  패킷 크기: " << packet.size() << "바이트\n";
    expect("받은 아이템 개수 합계", 76, ptr4::totalItems(packet));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// totalItems의 for 줄에 중단점을 걸고 조사식에 entries[0], entries[1]을 넣어 보세요.
// 첫 항목은 { 101, 3 } 이어야 합니다. 맞나요?

// [JA] totalItems の for の行にブレークポイントを置き、ウォッチに entries[0], entries[1] を入れてみてください。
// [JA] 最初の項目は { 101, 3 } でなければなりません。合っていますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 조사식에 (const char*)entries - (const char*)header 를 넣어 보세요. 헤더 바로 뒤라면 몇 바이트여야 하나요?
// 메모리 보기에 packet.data() 를 넣고, 4바이트마다 무엇이 있는지 보면서 비교해 보세요.

// [JA] ウォッチに (const char*)entries - (const char*)header を入れてみてください。ヘッダーのすぐ後ろなら何バイトであるべきでしょうか?
// [JA] メモリ ウィンドウに packet.data() を入れ、4バイトごとに何があるかを見ながら比べてみてください。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 포인터에 정수 n을 더하면 n "바이트"가 아니라 n "개(가리키는 타입 크기 x n)" 만큼 이동합니다.
// header는 무슨 타입의 포인터인가요? ASan 구성으로 실행하면 이 버그가 어떻게 보이는지도 확인해 보세요.

// [JA] ポインターに整数 n を足すと、n「バイト」ではなく n「個(指す型のサイズ x n)」分だけ移動します。
// [JA] header は何の型のポインターでしょうか? ASan 構成で実行すると、このバグがどう見えるかも確認してみてください。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(--grade)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
void pt04_loot_packet_hidden()
{
    const std::vector<ptr4::LootEntry> loot = { { 12, 7 }, { 13, 11 }, { 14, 100 }, { 15, 4 }, { 16, 9 }, { 17, 30 }, { 18, 2 } };
    expect("항목 7개 합계", 163, ptr4::totalItems(ptr4::buildPacket(loot)));

    const std::vector<ptr4::LootEntry> single = { { 500, 42 } };
    expect("항목 1개 합계", 42, ptr4::totalItems(ptr4::buildPacket(single)));
}
#pragma endregion
