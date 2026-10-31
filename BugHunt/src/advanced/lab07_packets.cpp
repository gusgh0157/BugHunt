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

    expect("첫 번째 패킷", std::string("LOGIN user=kael"), std::string(sendQueue[0].text()));
    expect("두 번째 패킷", std::string("MOVE x=10 y=20"), std::string(sendQueue[1].text()));
}
