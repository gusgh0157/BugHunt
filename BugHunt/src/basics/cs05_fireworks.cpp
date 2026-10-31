#include "../labs.h"
#include <string>

// 불꽃놀이 카운트다운. 2초 간격으로 숫자를 센다.
namespace lab5
{
void countdown(int n, std::string& text)
{
    if (n == 0)
    {
        text += "발사!";
        return;
    }
    text += std::to_string(n) + " ";
    countdown(n - 2, text);
}

void startFireworks(int seconds)
{
    std::string text;
    countdown(seconds, text);
    std::cout << "  " << text << '\n';
}

void fireworksShow()
{
    startFireworks(10);
    startFireworks(6);
    startFireworks(5);
    startFireworks(8);
}
}

void cs05_fireworks()
{
    lab5::fireworksShow();
}
