#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "Pervasive_BWRY_Medium.h"

// DEMO Image Set <Comment out when in User Mode>
// Screen Size: 581, 741
#define SCRN 741
#include "demoImageData.h"

//Pervasive_BWRY_Medium myDriver(eScreen_EPD_581_QS_0B, boardRaspberryPiPico_RP2040);
static Pervasive_BWRY_Medium myDriver(eScreen_EPD_741_QS_0B);

extern "C" void EpdTestBWRY_7_4(char *CmdLine)
{
   Pervasive_BWRY_Medium epdtest(eScreen_EPD_741_QS_0B);
   epdtest.updateNormal(BW_monoBuffer, frameSize);
}
