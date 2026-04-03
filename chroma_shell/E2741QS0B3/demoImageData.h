/*
   demoImageData.h
  DEMO set of Image Data
*/

// Load Normal Update Images
#if(SCRN == 581)
#include "581E5SE_testimage.c"
#define frameSize 46080
#define BW_monoBuffer        (uint8_t *) & testImage
#elif (SCRN==741)   //7.41"
#include "741E5SE_testimage.c"
#define BW_monoBuffer        (uint8_t *) & testImage
#define frameSize 96000
#endif
