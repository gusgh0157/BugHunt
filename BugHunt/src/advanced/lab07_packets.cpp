#include "../labs.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace
{
class PacketBuffer
{
public:
    explicit PacketBuffer(size_t size)
        : data_(new char[size]), size_(size)
    {
        std::memset(data_, 0, size_);
    }

    ~PacketBuffer() { delete[] data_; }

    void write(const char* text) { std::snprintf(data_, size_, "%s", text); }
    const char* text() const { return data_; }

private:
    char* data_;
    size_t size_;
};
}

void lab07_packets()
{
    std::vector<PacketBuffer> sendQueue;

    sendQueue.push_back(PacketBuffer(64));
    sendQueue.back().write("LOGIN user=kael");

    sendQueue.push_back(PacketBuffer(64));
    sendQueue.back().write("MOVE x=10 y=20");

    expect(i18n::L("첫 번째 패킷", "1番目のパケット"), std::string("LOGIN user=kael"), std::string(sendQueue[0].text()));
    expect(i18n::L("두 번째 패킷", "2番目のパケット"), std::string("MOVE x=10 y=20"), std::string(sendQueue[1].text()));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (편집기의 접기 기능으로 모두 접어 두세요)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(エディターの折りたたみ機能ですべて折りたたんでおいてください)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// 생성자와 소멸자 첫 줄에 중단점을 걸고 this와 data_를 기록해 보세요.
// 생성자는 몇 번, 소멸자는 몇 번 불리나요?

// [JA] コンストラクターとデストラクターの最初の行にブレークポイントを置き、this と data_ を記録してみてください。
// [JA] コンストラクターは何回、デストラクターは何回呼ばれますか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 같은 data_ 주소를 가진 객체가 두 개 생기는 순간은 언제인가요?
// (코드에 보이지 않는 함수가 호출되고 있습니다.)

// [JA] 同じ data_ アドレスを持つオブジェクトが二つできる瞬間はいつですか?
// [JA] (コードに見えない関数が呼ばれています。)
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// "Rule of Three / Five / Zero"를 찾아보세요.

// [JA] "Rule of Three / Five / Zero" を調べてみてください。
#pragma endregion
