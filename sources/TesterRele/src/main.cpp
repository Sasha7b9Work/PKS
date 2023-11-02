#include "defines.h"
#include "TesterRele.h"
#include "Modem/MQTT/MQTT.h"


int main()
{
    TesterRele::Init();

    while(1)
    {
        TesterRele::Update();
    }
}


bool MQTT::InStateRunning()
{
    return false;
}
