#include <string>
#include <vector>
#include "fireboltdemo.h"

class DisplayDemo : public IFireboltDemo 
{
public:
    DisplayDemo();
    ~DisplayDemo() = default;
    void runOption(const int index);
};
