#include <stdio.h>
#include <string.h>
#include <cstdlib>
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

// There are 2-bit data per pixel to define 4 colors.

#define COLOR_BLACK  0
#define COLOR_WHITE  1
#define COLOR_YELLOW 2
#define COLOR_RED    3

/* 
   0,0                0,479            0,959
      +----------------+----------------+
      |                |                |
      |                |                |
      |                |                |
      |                |                |
  Y   |     Master     |   Slave        | Physical
      |                |                |
      |                |                |
      |                |                |
      |                |                |
671,0 +----------------+----------------+ 671,959
                       X
 
 4 pixels per byte in the X direction
*/
extern "C" void EpdTestBWRY_9_7(char *CmdLine)
{
   uint8_t *Master = NULL;
   uint8_t *Slave = NULL;
   unsigned int PlaneSize = sizeof(Image_970_Masterfm_01);
   do {
      int TestType = 0;
      sscanf(CmdLine,"%d",&TestType);
      if(TestType <= 0 || TestType > 3) {
         if(TestType != 0) {
            printf("Invalid test type\n");
         }
         printf("select test type:\n");
         printf("  1 - draw test pattern\n");
         printf("  2 - draw bounding box\n");
         printf("  3 - display test image\n");
         break;
      }

      if((Master = (uint8_t *) malloc(PlaneSize)) == NULL || 
         (Slave = (uint8_t *) malloc(PlaneSize)) == NULL)
      {
         printf("malloc failed\n");
         break;
      }
      if(TestType == 3) {
         printf("displaying test image \n");
         memcpy(Master,Image_970_Masterfm_01,PlaneSize);
         memcpy(Slave,Image_970_Slavefm_01,PlaneSize);
      }
      else {
         memset(Master,0x55,PlaneSize);
         memset(Slave,0x55,PlaneSize);
      }

      if(TestType == 1) {
         printf("drawing test pattern\n");
      // draw horizontal black line across middle of display
         int xIncrement = (960 / 4) / 2;  // byte address increase for each line of y
         int yOffset = xIncrement * (672 / 2);
         int y;

         memset(&Master[yOffset],0,(960/4) / 2);
         memset(&Slave[yOffset],0,(960/4) / 2);
      // draw double width vertical red line across middle of display
         for(y = 0; y < 672/2; y++) {
         // last bit byte of master image
            Master[(xIncrement * y) + xIncrement - 1] &= 0xfc;
            Master[(xIncrement * y) + xIncrement - 1] |= 0x03; // red

         // first bit byte of slave image
            Slave[(xIncrement * y)] &= 0x3f;
            Slave[(xIncrement * y)] |= 0xc0; // red
         }

         for(; y < 672; y++) {
         // last bit byte of master image
            Master[(xIncrement * y) + xIncrement - 1] &= 0xfc;
            Master[(xIncrement * y) + xIncrement - 1] |= 0x02; // yellow

         // first bit byte of slave image
            Slave[(xIncrement * y)] &= 0x3f;
            Slave[(xIncrement * y)] |= 0x80; // yellow
         }

      // draw short arrow pointing to the upper left hand corner
         uint8_t Mask = 0xc0;
         uint8_t Value = 0x00;   // black
         uint8_t x = 1;
         int Offset = 0;
         printf("\n");
         for(int y = 1; y < 8; y++) {
          // draw left edge
            int Adr = y * xIncrement;
            Master[Adr] &= ~Mask;
            Master[Adr] |= Value;

          // draw diagonal
            Adr += Offset;
            Master[Adr] &= ~(Mask >> (x * 2));
            Master[Adr] |= (Value >> (x++ * 2));
            if(x == 4) {
               x = 0;
               Offset++;
            }
         }
      // draw top edge
         Master[0] = 0;
         Master[1] = 0;
      }
      else if(TestType == 2) {
         int y;

         printf("drawing bounding box\n");
      // draw horizontal black line across the top of the display
         int xIncrement = (960 / 4) / 2;  // byte address increase for each line of y

         memset(&Master[0],0,(960/4) / 2);
         memset(&Slave[0],0,(960/4) / 2);

      // draw horizontal black line across the bottom of the display
         int yOffset = xIncrement * 671;
         memset(&Master[yOffset],0,(960/4) / 2);
         memset(&Slave[yOffset],0,(960/4) / 2);

      // draw black line on right and left hand sides of the display
         for(y = 0; y < 672; y++) {
            Master[xIncrement * y] &= 0x3f;
            Slave[(xIncrement * y) + (479/4)] &= 0xfc;
         }
      }

      myDriver.begin();
      myDriver.updateNormal(Master,Slave,frameSize);
   } while(false);

   if(Master != NULL) {
      free(Master);
   }

   if(Slave != NULL) {
      free(Slave);
   }
}

// extern "C"
} 

