#include "Pervasive_BWRY_Large.h"

// DEMO Image Set <Comment out when in User Mode>
// Screen Size: 970
#define SCRN 970
#include "demoImageData.h"

#if 0
Pervasive_BWRY_Large myDriver(eScreen_EPD_969_QS_0B, boardRaspberryPiPico_RP2040);

void setup()
{
    hV_HAL_begin();

    myDriver.begin();

    myDriver.updateNormal(Masterfm1, Slavefm1, frameSize);

    hV_HAL_exit();
}

void loop()
{
    hV_HAL_delayMilliseconds(1000);
}
#endif

extern "C" {

Pervasive_BWRY_Large myDriver(eScreen_EPD_969_QS_0B);

extern "C" void EpdTestBWRY_9_7(char *CmdLine)
{
   myDriver.begin();

   myDriver.updateNormal(Masterfm1, Slavefm1, frameSize);
}

}
