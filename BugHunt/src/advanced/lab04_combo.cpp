#include "../labs.h"

namespace
{
class ComboCounter
{
public:
    void hit()
    {
        ++combo_;
        if (combo_ > best_)
            best_ = combo_;
    }

    void miss() { combo_ = 0; }

    int best() const { return best_; }

private:
    int combo_;
    int best_ = 0;
};
}

void lab04_combo()
{
    // H = 명중, M = 빗나감
    const char* log = "HHHHMHHMH";

    ComboCounter counter;
    for (const char* p = log; *p != '\0'; ++p)
    {
        if (*p == 'H')
            counter.hit();
        else
            counter.miss();
    }

    expect("최고 콤보", 4, counter.best());
}
