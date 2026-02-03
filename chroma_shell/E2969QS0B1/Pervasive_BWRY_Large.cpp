//
// Pervasive_BWRY_Large.cpp
// Class library C++ code
// ----------------------------------
//
// Project Pervasive Displays Library Suite
//
// Copyright (c) Pervasive Displays, 2010-2025
// Licence All rights reserved
//
// See Pervasive_BWRY_Large.h for references
//
// Release 904: Added new driver library
//
// Header
#include "Pervasive_BWRY_Large.h"

#ifdef PROXY
#include "proxy_msgs.h"
#endif

//
// === COG section
//
//
// --- Large screens with Q film
//
void Pervasive_BWRY_Large::COG_reset()
{
    // Application note § 2. Power on COG driver
    b_reset(20, 10, 20, 10, 10); // BWRY specific
    b_waitBusy(); // BWRY specific
}

void Pervasive_BWRY_Large::COG_getDataOTP()
{
#ifndef PROXY
    hV_HAL_SPI_end(); // With unicity check
    hV_HAL_SPI3_begin(); // Define 3-wire SPI pins

    // 1.6 Read OTP memory mapping data
    uint16_t _chipId;
    uint16_t _readBytes = 0;
    u_flagOTP = false;

    _chipId = 0x0d04;
    _readBytes = 112;

    // GPIO
    hV_HAL_GPIO_set(b_pin.panelReset);

    // Check
    hV_HAL_GPIO_clear(b_pin.panelDC); // Command
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    hV_HAL_SPI3_write(0x70);
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
    hV_HAL_delayMilliseconds(8); // Added

    uint16_t ui16 = 0;
    hV_HAL_GPIO_set(b_pin.panelDC); // Data
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    ui16 = hV_HAL_SPI3_read();
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
    ui16 <<= 8;
    hV_HAL_GPIO_set(b_pin.panelDC); // Data
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    ui16 |= hV_HAL_SPI3_read();
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
    Serial.println(ui16, HEX);
    hV_HAL_Serial_crlf();
    if (ui16 == _chipId)
    {
        hV_HAL_log(LEVEL_INFO, "OTP check 1 passed - Chip ID %04x as expected", ui16);
    }
    else
    {
        hV_HAL_Serial_crlf();
        hV_HAL_log(LEVEL_CRITICAL, "OTP check 1 failed - Chip ID 0x%04x, expected 0x%04x", ui16, _chipId);
        hV_HAL_exit(0x01);
    }

    // Read OTP
    uint8_t ui8 = 0;
    uint16_t offset = 0x0000;

    hV_HAL_GPIO_clear(b_pin.panelDC); // Command
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    hV_HAL_SPI3_write(0x90);
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
    hV_HAL_delayMilliseconds(8);

    hV_HAL_GPIO_clear(b_pin.panelDC); // Command
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    hV_HAL_SPI3_write(0xa2);
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

    hV_HAL_GPIO_set(b_pin.panelDC); // Data
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    hV_HAL_SPI3_write(0x00);
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    hV_HAL_SPI3_write(0x15);
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    hV_HAL_SPI3_write(0x00);
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    hV_HAL_SPI3_write(0x00);
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    hV_HAL_SPI3_write(0xe0);
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

    hV_HAL_GPIO_clear(b_pin.panelDC); // Command
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    hV_HAL_SPI3_write(0x92);
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

    hV_HAL_GPIO_set(b_pin.panelDC); // Data
    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    ui8 = hV_HAL_SPI3_read();
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
    // hV_HAL_log(LEVEL_DEBUG, "Dummy read 0x%02x", ui8);

    hV_HAL_GPIO_clear(b_pin.panelCS); // Select
    COG_data[0] = hV_HAL_SPI3_read(); // First byte for check
    hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
    Serial.println(COG_data[0], HEX);
    // Check table start and set bank offset
    if (COG_data[0] == 0xa5)
    {
        offset = 0x00;
    }
    else
    {
        offset = 0x70;
        for (uint16_t index = 1; index < offset; index += 1)
        {
            hV_HAL_GPIO_clear(b_pin.panelCS); // Select
            hV_HAL_SPI3_read(); // Ignore bytes 1..offset
            hV_HAL_GPIO_set(b_pin.panelCS); // Unselect
        }

        hV_HAL_GPIO_clear(b_pin.panelCS); // Select
        COG_data[0] = hV_HAL_SPI3_read(); // First byte for check
        hV_HAL_GPIO_set(b_pin.panelCS); // Unselect

        if (COG_data[0] == 0xa5) // First byte check = 0xa5
        {
            offset = 0x00;
        }
        else
        {
            hV_HAL_Serial_crlf();
            hV_HAL_log(LEVEL_CRITICAL, "OTP check 2 failed - Bank %i, first 0x%02x, expected 0x%02x", 0, COG_data[0], 0xa5);
            hV_HAL_exit(0x01);
        }
    }
    hV_HAL_log(LEVEL_INFO, "OTP check 2 passed - Bank %i, first 0x%02x as expected", (offset > 0x00), COG_data[0]);

    // Populate COG_data
    for (uint16_t index = 1; index < _readBytes; index += 1)
    {
        digitalWrite(b_pin.panelCS, LOW); // Select
        COG_data[index] = hV_HAL_SPI3_read(); // Read OTP
        Serial.println(COG_data[index], HEX);
        digitalWrite(b_pin.panelCS, HIGH); // Unselect
    }

    u_flagOTP = true;

#if (DEBUG_OTP == 1) // Debug COG_data
    debugOTP(COG_data, _readBytes, COG_BWRY_LARGE, SCREEN_DRIVER(u_eScreen_EPD));
#endif // DEBUG_OTP

    hV_HAL_SPI3_end();
#else    // Proxy
    GetOTP_97_BWRY(COG_data);
   u_flagOTP = true;
#endif
}

void Pervasive_BWRY_Large::COG_initial()
{
    b_sendCommandDataSelect8(0xe6, 0x19, PANEL_CS_BOTH); // 25 C temp
    b_sendCommandDataSelect8(0xe0, 0x02, PANEL_CS_BOTH); //

    switch (u_eScreen_EPD)
    {
        case eScreen_EPD_969_QS_0B:
            b_sendCommand8(0xa5); //
            b_waitBusy();
            break;

        default:

            break;
    }

    // Application note § 3. COG initial
    // Work settings
    b_sendCommandDataSelect8(0x01, COG_data[16], PANEL_CS_BOTH); // PWR
    b_sendIndexDataSelect(0x00, &COG_data[26], 3, PANEL_CS_BOTH); // PSR
    b_sendIndexDataSelect(0x61, &COG_data[19], 4, PANEL_CS_BOTH); // TRES
    b_sendIndexDataSelect(0x61, &COG_data[19], 4, PANEL_CS_BOTH); // TRES
    uint8_t temphold[3] = {COG_data[17], COG_data[18], COG_data[29]};
    b_sendIndexDataSelect(0x00, &temphold[0], 3, PANEL_CS_BOTH); // PSR
    b_sendIndexDataSelect(0x06, &COG_data[23], 3, PANEL_CS_BOTH); // BTST_P
    b_sendIndexDataSelect(0x03, &COG_data[30], 3, PANEL_CS_BOTH); // PFS
    b_sendCommandDataSelect8(0xe7, COG_data[33], PANEL_CS_BOTH); // PFS
    b_sendIndexDataSelect(0x65, &COG_data[34], 4, PANEL_CS_BOTH); // TRES
    b_sendCommandDataSelect8(0x30, COG_data[38], PANEL_CS_BOTH); // PLL
    b_sendCommandDataSelect8(0x50, COG_data[39], PANEL_CS_BOTH); // CDI
    b_sendIndexDataSelect(0x60, &COG_data[40], 2, PANEL_CS_BOTH); // TCON
    b_sendCommandDataSelect8(0xe3, COG_data[42], PANEL_CS_BOTH); // PWS
    b_sendCommandDataSelect8(0xff, 0xa5, PANEL_CS_BOTH); //
    b_sendIndexDataSelect(0xef, &COG_data[43], 8, PANEL_CS_BOTH); // TRES

    b_sendCommandDataSelect8(0xdc, COG_data[59], PANEL_CS_BOTH); // CDI
    b_sendCommandDataSelect8(0xdd, COG_data[60], PANEL_CS_BOTH); // CDI
    b_sendCommandDataSelect8(0xde, COG_data[61], PANEL_CS_BOTH); // CDI
    b_sendCommandDataSelect8(0xe8, COG_data[62], PANEL_CS_BOTH); // CDI
    b_sendCommandDataSelect8(0xda, COG_data[63], PANEL_CS_BOTH); // CDI
    b_sendCommandDataSelect8(0xff, 0xe3, PANEL_CS_BOTH); //
    b_sendCommandDataSelect8(0xe9, 0x01, PANEL_CS_BOTH); //
}

extern "C" void SendEpdData(uint8_t EpdCmd,uint8_t Flags,const uint8_t *pData, uint32_t DataBytes);

void Pervasive_BWRY_Large::COG_sendImageDataNormal(FRAMEBUFFER_CONST_TYPE masterFrame, FRAMEBUFFER_CONST_TYPE slaveFrame, uint32_t sizeFrame) // First frame, blackBuffer
{
   int y;
// Test for sending one line at a time

   uint8_t Flags = EPD_FLG_DEFAULT;
   Flags |= EPD_FLG_CS1;
//    Flags &= ~EPD_FLG_END_XFER;

// Send refresh command to master and slave
   SendEpdData(0x10,Flags,NULL,0);  
   Flags &= ~EPD_FLG_CMD;
// master controller handles left side of display, 480 x 672
// slave controller handles right side of display, 480 x 672
// 480 pixels/line / (4 pixels/byte) = 120 bytes per line / controller
   int xIncrement = 480 / 4;  // byte address increase for each line of y

   for(y = 0; y < 672; y++) {
      Flags &= ~EPD_FLG_CS1;
      Flags |= EPD_FLG_CS;
   // Send data to master
      SendEpdData(0,Flags,&masterFrame[y * xIncrement],xIncrement);

      Flags &= ~EPD_FLG_CS;
      Flags |= EPD_FLG_CS1;
      SendEpdData(0,Flags,&slaveFrame[y * xIncrement],xIncrement);
   }

   EpdSetCS(HIGH,HIGH);
}

void Pervasive_BWRY_Large::COG_update()
{

    b_sendCommand8(0x04); // Power on
    b_waitBusy();

    b_sendCommandDataSelect8(0x12, 0x00, PANEL_CS_BOTH); // Display Refresh
    b_waitBusy();
}

void Pervasive_BWRY_Large::COG_stopDCDC()
{
    // Application note § 5. Turn-off DC/DC
    b_sendCommandDataSelect8(0x02, 0x00, PANEL_CS_BOTH); // Turn off DC/DC
    b_waitBusy();

    uint8_t COG_temp[3] = {0x07, 0x2b, 0x01};
    b_sendIndexDataSelect(0x00, &COG_temp[0], 3, PANEL_CS_BOTH); // PSR

    hV_HAL_delayMilliseconds(400);

    b_sendCommandDataSelect8(0xff, 0xa5, PANEL_CS_BOTH); // Turn off DC/DC
    uint8_t COG_temp2[2] = {0xa0, 0x1e};
    b_sendIndexDataSelect(0xee, &COG_temp2[0], 2, PANEL_CS_BOTH); // PSR

    hV_HAL_delayMilliseconds(4);
    uint8_t COG_temp3[2] = {0x00, 0x00};
    b_sendIndexDataSelect(0xee, &COG_temp3[0], 2, PANEL_CS_BOTH); // PSR

    hV_HAL_delayMilliseconds(3);
    b_sendCommandDataSelect8(0xff, 0xe3, PANEL_CS_BOTH); // Turn off DC/DC
    hV_HAL_delayMilliseconds(6);
}
//
// --- End of Large screens with Q film
//
/// @endcond
//
// === End of COG section
//

#ifndef PROXY
Pervasive_BWRY_Large::Pervasive_BWRY_Large(eScreen_EPD_t eScreen_EPD, pins_t board)
{
    d_COG = COG_BWRY_LARGE;
    u_eScreen_EPD = eScreen_EPD;
    b_pin = board;
}
#else
Pervasive_BWRY_Large::Pervasive_BWRY_Large(eScreen_EPD_t eScreen_EPD)
{
}
#endif

void Pervasive_BWRY_Large::begin()
{
#ifndef PROXY
    b_begin(b_pin, FAMILY_LARGE, b_delayCS);
    b_resume(); // GPIO
#endif

    COG_reset(); // Reset
    COG_getDataOTP(); // 3-wire SPI read OTP memory
}

#ifndef PROXY
STRING_CONST_TYPE Pervasive_BWRY_Large::reference()
{
    return formatString("%s v%i.%i.%i", DRIVER_EPD_VARIANT, DRIVER_EPD_RELEASE / 100, (DRIVER_EPD_RELEASE / 10) % 10, DRIVER_EPD_RELEASE % 10);
}
#endif

void Pervasive_BWRY_Large::updateNormal(FRAMEBUFFER_CONST_TYPE masterframe, FRAMEBUFFER_CONST_TYPE slaveframe, uint32_t sizeFrame)
{
    b_resume(); // GPIO
    COG_reset(); // Reset

    if (u_flagOTP == false)
    {
        COG_getDataOTP(); // 3-wire SPI read OTP memory
        COG_reset(); // Reset
    }

#ifndef PROXY
    // Start SPI
    hV_HAL_SPI_begin(16000000); // Fast 16 MHz, with unicity check
#endif

    COG_initial(); // Initialise
    COG_sendImageDataNormal(masterframe, slaveframe, sizeFrame);

    COG_update(); // Update
    COG_stopDCDC(); // Power DC/DC off
}

#ifdef PROXY
void Pervasive_BWRY_Large::b_sendCommandDataSelect8(uint8_t command, uint8_t data, uint8_t select)
{
   switch(select) {
      case PANEL_CS_MASTER:
         SendIndexDataM(command,&data,1);
         break;

      case PANEL_CS_SLAVE:
         SendIndexDataS(command,&data,1);
         break;

      case PANEL_CS_BOTH:
         SendIndexData(command,&data,1);
         break;


   }
}

void Pervasive_BWRY_Large::b_sendIndexDataSelect(uint8_t index, const uint8_t * data, uint32_t size, uint8_t select)
{
   switch(select) {
      case PANEL_CS_MASTER:
         SendIndexDataM(index,data,size);
         break;

      case PANEL_CS_SLAVE:
         SendIndexDataS(index,data,size);
         break;

      case PANEL_CS_BOTH:
         SendIndexData(index,data,size);
         break;
   }
}

void Pervasive_BWRY_Large::b_reset(uint32_t ms1, uint32_t ms2, uint32_t ms3, uint32_t ms4, uint32_t ms5)
{
   EpdSetEnable(LOW);   // turn on power
    delay(ms1); // Wait for power stabilisation
    EpdSetReset(HIGH);
    delay(ms2);
    EpdSetReset(LOW);
    delay(ms3);
    EpdSetReset(HIGH);
    delay(ms4);
    EpdSetCS(HIGH,HIGH);
    delay(ms5);
}

void Pervasive_BWRY_Large::b_sendCommand8(uint8_t command)
{
    SendIndexData(command,NULL,0);
}

void Pervasive_BWRY_Large::b_waitBusy(bool state)
{
   EpdBusyWait(state ? 1 : 0,0);
}

void Pervasive_BWRY_Large::b_resume()
{
}

#endif
