/*
	demoImageData.h
  DEMO set of Image Data
*/

// Load Normal Update Images
#if (SCRN==970)   //9.70"
#include "Image_970_Masterfm_01.c"
#include "Image_970_Slavefm_01.c"
#define frameSize	80640
#define Masterfm1        (uint8_t *)&Image_970_Masterfm_01
#define Slavefm1         (uint8_t *)&Image_970_Slavefm_01
#endif
