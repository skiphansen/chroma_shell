///
/// @file Pervasive_BWRY_Large.h
/// @brief Driver for BWRY large screens
///
/// @details Project Pervasive Displays Library Suite
///
/// @version 904
///
/// @copyright (c) Pervasive Displays, 2010-2025
/// @copyright All rights reserved
/// @copyright For exclusive use with Pervasive Displays screens
///
/// * Basic edition: for hobbyists and for basic usage
/// @n Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
/// @see https://creativecommons.org/licenses/by-sa/4.0/
///
/// @n Consider the Evaluation or Commercial editions for professionals or organisations and for commercial usage
///
/// * Evaluation edition: for professionals or organisations, evaluation only, no commercial usage
/// @n All rights reserved
///
/// * Commercial edition: for professionals or organisations, commercial usage
/// @n All rights reserved
///
/// * Viewer edition: for professionals or organisations
/// @n All rights reserved
///
/// * Documentation
/// @n All rights reserved
///
/// @see Application notes
/// *
///

#ifndef PROXY
// SDK and configuration
#include "PDLS_Common.h"

#if (PDLS_COMMON_RELEASE < 902)
#error Required PDLS_COMMON_RELEASE 902
#endif // PDLS_COMMON_RELEASE

// Driver
#include "Driver_EPD_Virtual.h"
#else    // PROXY
///
/// @brief Function for screen macros
///
#include <stdint.h>
#include <stddef.h>
#define SCREEN(S, F, D) ((uint32_t)((S & 0x0fff) << 16 | (F & 0xff) << 8 | (D & 0xff)))
#define SIZE_969 969 ///< 9.69"
#define eScreen_EPD_t uint32_t
#define FILM_Q 'Q' ///< Film Q, BWRY, "Spectra 4"
#define DRIVER_B 'B' ///< Driver B
#define FRAMEBUFFER_CONST_TYPE const uint8_t *
#define LOW  0x0
#define HIGH 0x1
///
/// @name Large screens sub-panels selection
/// @note Numbers are sequential and exclusive
/// @{
#define PANEL_CS_MASTER 0x01 ///< Large screens sub-panels: first panel only
#define PANEL_CS_SLAVE 0x02 ///< Large screens sub-panels: second panel only
#define PANEL_CS_BOTH 0x03 ///< Large screens sub-panels: both panels
#define hV_HAL_delayMilliseconds(X) (delay(X))


extern "C" {
   int GetOTP_97_BWRY(uint8_t *OtpBuf);
   void delay(int iTime);
   int EpdBusyWait(int State,int Timeout);
   void SendIndexData(uint8_t Cmd,const uint8_t *pData,uint32_t DataBytes);
   void SendIndexDataM(uint8_t Cmd,const uint8_t *pData,uint32_t DataBytes);
   void SendIndexDataS(uint8_t Cmd,const uint8_t *pData,uint32_t DataBytes);
   int EpdSetEnable(uint8_t Enable);
   int EpdSetReset(uint8_t Reset);
   int EpdSetDC(uint8_t DC);
   int EpdSetCS(uint8_t CS,uint8_t CS1);
}

#endif

#ifndef DRIVER_BWRY_LARGE_RELEASE
///
/// @brief Library release number
///
#define DRIVER_BWRY_LARGE_RELEASE 904

///
/// @name List of supported screens
/// @details Colour black-white-red-yellow "Spectra 4" screens
///
/// @note Normal update
/// @see https://www.pervasivedisplays.com/products/?_sft_etc_itc=itc&_sft_product_colour=black-white-red-yellow
/// @{
///
#define eScreen_EPD_969_QS_0B SCREEN(SIZE_969, FILM_Q, DRIVER_B) ///< reference 970-QS-0B
/// @}

///
/// @name List of features
/// @{
///
#define WITH_COLOURS ///< With colours
#define WITH_COLOURS_BWRY ///< Black-White-Red-Yellow colours
/// @}

///
/// @brief Driver variant
///
#define DRIVER_EPD_RELEASE DRIVER_BWRY_LARGE_RELEASE
#define DRIVER_EPD_VARIANT "BWRY large"

///
/// @brief BWRY large screens class
///
#ifndef PROXY
class Pervasive_BWRY_Large : public Driver_EPD_Virtual
{
#else
class Pervasive_BWRY_Large
{
#endif
  public:

    ///
    /// @brief Constructor
    /// @param eScreen_EPD
    /// @param board
    ///
    Pervasive_BWRY_Large(eScreen_EPD_t eScreen_EPD);
#ifndef PROXY
                         ,pins_t board
                         );
#endif

    /// @name General
    /// @{

    ///
    /// @brief Initialisation
    /// @details Initialise the board and read OTP
    ///
    void begin();

    ///
    /// @brief Driver reference
    ///
    /// @return STRING_CONST_TYPE scope and release number
    /// @note Example `BWRY large v9.0.0`
    ///
#ifndef PROXY
    STRING_CONST_TYPE reference();
#endif
    ///
    /// @brief Normal update
    ///
    /// @param masterframe master half of next image
    /// @param slaveframe slave half of next image
    /// @param sizeFrame size of the frame
    ///
    void updateNormal(FRAMEBUFFER_CONST_TYPE masterframe, FRAMEBUFFER_CONST_TYPE slaveframe, uint32_t sizeFrame);

    /// @}

  protected:

  private:

    // Variables and functions specific to the screen
    uint8_t COG_data[112]; // OTP

    void COG_reset();
    void COG_getDataOTP();
    void COG_initial();
    void COG_sendImageDataNormal(FRAMEBUFFER_CONST_TYPE masterFrame, FRAMEBUFFER_CONST_TYPE slaveFrame, uint32_t sizeFrame);
    void COG_update();
    void COG_stopDCDC();
#ifdef PROXY
    bool u_flagOTP = false; // true = OTP read
    eScreen_EPD_t u_eScreen_EPD;
    ///
    /// @brief Send a command and one byte of data to selected half of large screen
    /// @param command command
    /// @param data uint8_t data
    /// @param select default = PANEL_CS_BOTH, otherwise PANEL_CS_MASTER or PANEL_CS_SLAVE
    /// @note panelDC is kept high, to be changed manually after
    ///
    void b_sendCommandDataSelect8(uint8_t command, uint8_t data, uint8_t select);

    ///
    /// @brief General reset
    /// @param ms1 delay for power stabilisation, ms
    /// @param ms2 delay after RESET HIGH, ms
    /// @param ms3 delay after RESET LOW, ms
    /// @param ms4 delay after RESET HIGH, ms
    /// @param ms5 delay after CS_PIN and CSS_PIN HIGH, ms
    ///
    void b_reset(uint32_t ms1, uint32_t ms2, uint32_t ms3, uint32_t ms4, uint32_t ms5);

    ///
    /// @brief Wait for ready
    /// @details Wait for panelBusy signal to reach state
    /// @note Signal is busy until reaching state
    /// @param state to reach HIGH = default, LOW
    ///
    void b_waitBusy(bool state = HIGH);

    ///
    /// @brief Send a command
    /// @param command command
    /// @note panelDC is kept high, to be changed manually after
    ///
    void b_sendCommand8(uint8_t command);

    ///
    /// @brief Send data through SPI to selected half of large screen
    /// @param index register
    /// @param data data
    /// @param size number of bytes
    /// @param select default = PANEL_CS_BOTH, otherwise PANEL_CS_MASTER or PANEL_CS_SLAVE
    /// @note Valid only for 9.7 and 12.20" screens
    ///
    void b_sendIndexDataSelect(uint8_t index, const uint8_t * data, uint32_t size, uint8_t select = PANEL_CS_BOTH);

    ///
    /// @brief Resume GPIOs
    /// @details Turn on and configure all GPIOs
    ///
    void b_resume();
#endif

};

#endif // DRIVER_BWRY_LARGE_RELEASE

