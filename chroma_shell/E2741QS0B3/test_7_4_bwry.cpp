#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "Pervasive_BWRY_Medium.h"

// DEMO Image Set <Comment out when in User Mode>
// Screen Size: 581, 741
#define SCRN 741
#include "demoImageData.h"

//Pervasive_BWRY_Medium myDriver(eScreen_EPD_581_QS_0B, boardRaspberryPiPico_RP2040);
static Pervasive_BWRY_Medium 
myDriver(eScreen_EPD_741_QS_0B);

/*
   0,0                0,479
      +----------------+
      |                |
      |                |
      |                |
      |                |
  Y   |     Master     |
      |                |
      |                |
      |                |
      |                |
799,0 +----------------+
             X
 
 4 pixels per byte in the X direction
*/

extern "C" void EpdTestBWRY_7_4(char *CmdLine)
{
   uint8_t *Frame = NULL;
   Pervasive_BWRY_Medium epdtest(eScreen_EPD_741_QS_0B);

   do {
      printf("PDI 7.4\" BWRY tests\n");
      int TestType = 0;
      sscanf(CmdLine,"%d",&TestType);
      if(TestType <= 0 || TestType > 3) {
         if(TestType != 0) {
            printf("Invalid test type\n");
         }
         printf("select test type:\n");
         printf("  1 - draw bounding box\n");
         printf("  2 - display test image\n");
         printf("  3 - clear screen\n");
         break;
      }

      if(TestType == 2) {
         printf("displaying test image \n");
         epdtest.updateNormal(BW_monoBuffer, frameSize);
      }
      else if(TestType == 1 || TestType == 3) {
         int y;
         if((Frame = (uint8_t *) malloc(frameSize)) == NULL) {
            printf("malloc failed\n");
            break;
         }

      // draw horizontal black line across the top of the display
         int xIncrement = 480 / 4;  // byte address increase for each line of y
// 0x00 = all black
// 0x55 = all white
// 0xaa = all yellow
// 0xff = all red
         memset(Frame,0x55,frameSize);
         if(TestType == 1) {
            printf("drawing bounding box\n");
         // draw horizontal black line across the top
            memset(Frame,0,480/4);
         // draw horizontal black line across the bottom of the display
            int yOffset = xIncrement * 799;
            memset(&Frame[yOffset],0,480/4);

         // draw black line on right and left hand sides of the display
            for(y = 0; y < 800; y++) {
               Frame[xIncrement * y] &= 0x3f;
               Frame[(xIncrement * y) + (479/4)] &= 0xfc;
            }
         }
         else {
            printf("clearing screen\n");
         }
         epdtest.updateNormal(Frame,frameSize);
      }
   } while(false);

   if(Frame != NULL) {
      free(Frame);
   }
}
