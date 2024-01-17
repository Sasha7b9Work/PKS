#include "defines.h"
#include "TesterRele.h"


int main()
{
    TesterRele::Init();

    while(1)
    {
        TesterRele::Update();
    }
}
