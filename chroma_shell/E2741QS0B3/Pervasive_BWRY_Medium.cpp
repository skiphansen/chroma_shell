//
// Pervasive_BWRY_Medium.cpp
// Class library C++ code
// ----------------------------------
//
// Project Pervasive Displays Library Suite
//
// Copyright (c) Pervasive Displays Inc., 2021-2026
// Licence All rights reserved
//
// See Pervasive_BWRY_Medium.h for references
//
// Release 611: Added proxy functions to normal update
// Release 611: Added support for red and yellow colour screens
// Release 614: Improved BWRY examples
// Release 702: Added support for 417-QS-0A
// Release 700: Refactored screen and board functions
// Release 800: Read OTP memory
// Release 801: Improved OTP implementation
// Release 802: Added references to application notes
// Release 802: Refactored CoG functions
// Release 900: Added new driver library
//

// Header
#include "Pervasive_BWRY_Medium.h"
//
// === COG section
//
//
// --- Medium screens with Q film
//
void Pervasive_BWRY_Medium::COG_reset()
{
    // Application note § 2. Power on COG driver
    b_reset(20, 10, 20, 10, 10); // BWRY specific
    b_waitBusy(); // BWRY specific
}

void Pervasive_BWRY_Medium::COG_getDataOTP()
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
    hV_HAL_SPI3_end();

#if (DEBUG_OTP == 1) // Debug COG_data
    debugOTP(COG_data, _readBytes, COG_BWRY_MEDIUM, SCREEN_DRIVER(u_eScreen_EPD));
#endif // DEBUG_OTP
#else    // Proxy
    GetOTP_97_BWRY(COG_data);
   u_flagOTP = true;
#endif
}

void Pervasive_BWRY_Medium::COG_initial()
{
    b_sendCommandData8(0xe6, 0x19); // 25 C temp
    b_sendCommandData8(0xe0, 0x02); //

    switch (u_eScreen_EPD)
    {
        case eScreen_EPD_741_QS_0B:
            b_sendCommand8(0xa5); //
            b_waitBusy();
            break;

        default:

            break;
    }

    // Application note § 3. COG initial
    // Work settings
    b_sendCommandData8(0x01, COG_data[16]); // PWR
    b_sendIndexData(0x00, &COG_data[26], 2); // PSR
    b_sendIndexData(0x61, &COG_data[19], 4); // TRES
//    LOG_REG(0x61, &COG_data[19], 4,"TRES"); // TRES
    b_sendIndexData(0x00, &COG_data[17], 2); // PSR
    b_sendIndexData(0x06, &COG_data[23], 3); // BTST_P
    b_sendIndexData(0x03, &COG_data[30], 3); // PFS
    b_sendCommandData8(0xe7, COG_data[33]); // PFS
    b_sendIndexData(0x65, &COG_data[34], 4); // TRES
    b_sendCommandData8(0x30, COG_data[38]); // PLL
    b_sendCommandData8(0x50, COG_data[39]); // CDI
    b_sendIndexData(0x60, &COG_data[40], 2); // TCON
    b_sendCommandData8(0xe3, COG_data[42]); // PWS
    b_sendCommandData8(0xff, 0xa5); //
    b_sendIndexData(0xef, &COG_data[43], 8); // TRES

    b_sendCommandData8(0xdc, COG_data[59]); // CDI
    b_sendCommandData8(0xdd, COG_data[60]); // CDI
    b_sendCommandData8(0xde, COG_data[61]); // CDI
    b_sendCommandData8(0xe8, COG_data[62]); // CDI
    b_sendCommandData8(0xda, COG_data[63]); // CDI
    b_sendCommandData8(0xff, 0xe3); //
    b_sendCommandData8(0xe9, 0x01); //
}

extern "C" void SendEpdData(uint8_t EpdCmd,uint8_t Flags,const uint8_t *pData, uint32_t DataBytes);

#ifndef PROXY
void Pervasive_BWRY_Medium::COG_sendImageDataNormal(FRAMEBUFFER_CONST_TYPE frame, uint32_t sizeFrame) // First frame, blackBuffer
{
    // Application note § 4. Input image to the EPD
    // b_sendIndexDataSelect(0x10, s_newImage, u_pageColourSize,PANEL_CS_MASTER); // First frame, blackBuffer
    b_sendIndexData(0x10, frame, sizeFrame); // First frame, blackBuffer
}
#else
void Pervasive_BWRY_Medium::COG_sendImageDataNormal(FRAMEBUFFER_CONST_TYPE frame, uint32_t sizeFrame) // First frame, blackBuffer
{
   int y;
   uint8_t Flags = EPD_FLG_DEFAULT;
   int Bytes = 0;

// Send refresh command to master and slave
   SendEpdData(0x10,Flags,NULL,0);  
   Flags &= ~EPD_FLG_CMD;
// master controller handles left side of display, 480 x 800
// 480 pixels/line / (4 pixels/byte) = 120 bytes per line / controller
   int xIncrement = 480 / 4;  // byte address increase for each line of y

   for(y = 0; y < 800; y++) {
   // Send data to master
      SendEpdData(0,Flags,&frame[y * xIncrement],xIncrement);
      Bytes += xIncrement;
   }

   LOG("Sent %d bytes of image data\n",Bytes);

   EpdSetCS(HIGH,HIGH);
}
#endif


void Pervasive_BWRY_Medium::COG_update()
{

    b_sendCommand8(0x04); // Power on
    b_waitBusy();

    b_sendCommandData8(0x12, 0x00); // Display Refresh
    b_waitBusy();
}

void Pervasive_BWRY_Medium::COG_stopDCDC()
{
    // Application note § 5. Turn-off DC/DC
    b_sendCommandData8(0x02, 0x00); // Turn off DC/DC
    b_waitBusy();

    uint8_t COG_temp[3] = {0x07, 0x2b, 0x01};
    b_sendIndexData(0x00, &COG_temp[0], 3); // PSR

    b_sendCommandData8(0xff, 0xa5); // Turn off DC/DC

    hV_HAL_delayMilliseconds(400);
    uint8_t COG_temp2[2] = {0xa0, 0x1e};
    b_sendIndexData(0xee, &COG_temp2[0], 2); // PSR

    hV_HAL_delayMilliseconds(4);
    uint8_t COG_temp3[2] = {0x00, 0x00};
    b_sendIndexData(0xee, &COG_temp3[0], 2); // PSR

    hV_HAL_delayMilliseconds(3);
    b_sendCommandData8(0xff, 0xe3); // Turn off DC/DC
    hV_HAL_delayMilliseconds(6);
}
//
// --- End of Medium screens with Q film
//
/// @endcond
//
// === End of COG section
//

#ifndef PROXY
Pervasive_BWRY_Medium::Pervasive_BWRY_Medium(eScreen_EPD_t eScreen_EPD,pins_t board)
{
    d_COG = COG_BWRY_MEDIUM;
    u_eScreen_EPD = eScreen_EPD;
    b_pin = board;
}
#else
Pervasive_BWRY_Medium::Pervasive_BWRY_Medium(eScreen_EPD_t eScreen_EPD)
{
    u_eScreen_EPD = eScreen_EPD;
}
#endif

void Pervasive_BWRY_Medium::begin()
{
#ifndef PROXY
    b_begin(b_pin, FAMILY_MEDIUM, b_delayCS);
#endif
    b_resume(); // GPIO

    COG_reset(); // Reset
    COG_getDataOTP(); // 3-wire SPI read OTP memory
}

#ifndef PROXY
STRING_CONST_TYPE Pervasive_BWRY_Medium::reference()
{
    return formatString("%s v%i.%i.%i", DRIVER_EPD_VARIANT, DRIVER_EPD_RELEASE / 100, (DRIVER_EPD_RELEASE / 10) % 10, DRIVER_EPD_RELEASE % 10);
}
#endif

void Pervasive_BWRY_Medium::updateNormal(FRAMEBUFFER_CONST_TYPE frame, uint32_t sizeFrame)
{
    b_resume(); // GPIO
    COG_reset(); // Reset

    if (u_flagOTP == false)
    {
        COG_getDataOTP(); // 3-wire SPI read OTP memory
        COG_reset(); // Reset
    }

    // Start SPI
#ifndef PROXY
    hV_HAL_SPI_begin(16000000); // Fast 16 MHz, with unicity check
#endif

    COG_initial(); // Initialise
    COG_sendImageDataNormal(frame, sizeFrame);

    COG_update(); // Update
    COG_stopDCDC(); // Power DC/DC off
}

#ifdef PROXY
void Pervasive_BWRY_Medium::b_sendCommandData8(uint8_t command, uint8_t data)
{
   SendIndexDataM(command,&data,1);
}

void Pervasive_BWRY_Medium::b_sendIndexData(uint8_t index, const uint8_t * data, uint32_t size)
{
   SendIndexDataM(index,data,size);
}

void Pervasive_BWRY_Medium::b_reset(uint32_t ms1, uint32_t ms2, uint32_t ms3, uint32_t ms4, uint32_t ms5)
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

void Pervasive_BWRY_Medium::b_sendCommand8(uint8_t command)
{
    SendIndexData(command,NULL,0);
}

void Pervasive_BWRY_Medium::b_waitBusy(bool state)
{
   EpdBusyWait(state ? 1 : 0,0);
}

void Pervasive_BWRY_Medium::b_resume()
{
}

#endif
