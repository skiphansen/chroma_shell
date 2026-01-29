## Chroma shell

Chroma_shell is a Linux command line program that is used to communicate with 
a electronic price tags running custom proxy firmware to facilitate reverse 
engineering.

Two projects which have compatible proxy firmware are:

See https://github.com/skiphansen/dmitrygr-einkTags and https://github.com/skiphansen/Tag_FW_CC13xx

## Requirements

1. The Linux operating system or Linux running in a VM (if it can access the serial port).
2. A 3.3V level compatible serial port.
3. Proxy firmware running on the target Chroma tag.

For non-developers the most useful function provided by chroma_shell is the
ability to backup and restore the tag's EEPROM image.  This is useful because the
"factory" setting stored in the EEPROM such as the tag's serial number
are essential for the tag operation.  If a software bug or user error
causes the information to lost a backup is the easiest way to recover.

A number of other functions are available that assist reverse engineering
efforts. 

Run the help command for a complete list.

## Build Requirements

1. gcc (`sudo apt-get install build-essential` or equivalent)
2. Cmake (`sudo apt-get install cmake` or equivalent)

## Building

Chroma_shell is a C program that uses CMake as the build system.  Cmake
and gcc are the only requirements.

Building the application is a two step process:

1. Configure the build for your particular system ``cmake -B build``.
2. Build it ``cmake --build build``.

For example:
```
skip@Dell-5510:~/esl/chroma_shell/chroma_shell$ cmake -B build
-- The C compiler identification is GNU 13.3.0
-- The CXX compiler identification is GNU 13.3.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (0.4s)
-- Generating done (0.0s)
-- Build files have been written to: /home/skip/esl/chroma_shell/chroma_shell/build
skip@Dell-5510:~/esl/chroma_shell/chroma_shell$ cmake --build build
[  9%] Building C object CMakeFiles/chroma_shell.dir/serial_shell.c.o
[ 18%] Building C object CMakeFiles/chroma_shell.dir/chroma_cmds.c.o
[ 27%] Building C object CMakeFiles/chroma_shell.dir/sfdp-parser.c.o
[ 36%] Building C object CMakeFiles/chroma_shell.dir/home/skip/esl/chroma_shell/common/CobsFraming.c.o
[ 45%] Building C object CMakeFiles/chroma_shell.dir/linenoise/linenoise.c.o
[ 54%] Building C object CMakeFiles/chroma_shell.dir/logging.c.o
[ 63%] Building C object CMakeFiles/chroma_shell.dir/lut_dump.c.o
[ 72%] Building C object CMakeFiles/chroma_shell.dir/jep106.c.o
[ 81%] Building CXX object CMakeFiles/chroma_shell.dir/test_large_bwr/test_large_bwr.cpp.o
[ 90%] Building CXX object CMakeFiles/chroma_shell.dir/test_large_bwr/EPD_Driver.cpp.o
[100%] Linking CXX executable chroma_shell
[100%] Built target chroma_shell
skip@Dell-5510:~/esl/chroma_shell/chroma_shell$
```

By default the serial port connected to the Chroma tag is assumed to be 
/dev/ttyUSB0 is assumed and the baudrate is 1,000,000 baud.  Command line
options can be used to specify a different serial port or baudrate.

Note:  If the default baudrate is changed the proxy firmware must also be
built for that baudrate.  If your serial port doesn't support 1,000,000 baud
the proxy can be build for 115200 baud.

```
skip@Dell-5510:~/esl/chroma_shell/chroma_shell$ ./build/chroma_shell -?
./build/chroma_shell: invalid option -- '?'
Usage: chroma_shell [options]
  options:
        -b<baud rate>   Set serial port baudrate (default 1000000)
        -c<command>     Run specified command and exit
        -d              Debug mode
        -D<path>        Set path to async device (default /dev/ttyUSB0)
        -q              quiet
        -v?             List available verbose display levels
        -v<bitmap>      Set desired display levels (Hex bit map)
```

## Connecting Serial port to Tag

See this OpenEPaperLink [Wiki](https://github.com/OpenEPaperLink/OpenEPaperLink/wiki/Chroma-Series-SubGhz-Tags#debug-serial-port-connections)
for serial port connection information.

## Running

Once the shell has been built it can be run as follows:

```
skip@Dell-5510:~/esl/chroma_shell/chroma_shell$ ./build/chroma_shell
chroma shell v0.01, compiled Jan 29 2026 05:58:02
chroma>help
chroma shell v0.01, compiled Jan 29 2026 05:58:02

       bb_test - Try to display an image using bb_epaper type
    board_type - Display board type
     chip_type - Guess EPD controller type
      dump_lut - Display LUTs extraced from EEPROM
  dump_rf_regs - Display settings of all RF registers
 dump_settings - Display EEPROM settings
          eerd - Read data from EEPROM
          eewr - Write data to EEPROM
         ee_pd - Power up/down EEPROM
     ee_backup - Write EEPROM data to a file
      ee_erase - Erase EEPROM sectors
         ee_id - Display EEPROM manufacture and device IDs
    ee_restore - Read EEPROM data from a file
      epd_test - Send test image to EPD
        get_sn - Read SN from flash or file
          ping - Send a ping
  radio_config - Set radio configuration
         reset - reset device
            rx - Enter RF receive mode
     port_read - Read port X
    port_write - Write port X
          p2wr - Write port 2
          sfdp - Dump SPI flash sfdp into
       set_reg - set chip register device
        sn2mac - Convert a Chroma serial number string to MAC address
            tx - Send text
chroma>
```

The ping command can be used to verify successfully communications between
the host and the tag.

```
chroma>ping
Received ping response
chroma>
```

If "Received ping response" is displayed the proxy firmware, serial port, and 
chroma_shell are communicating successfully.

If you get "Wait4Response: Timeout waiting for NOP response" check your wiring.

The ee_backup and ee_restore commands are used to back backup or restore EEPROM 
contents.

For example:

```
chroma>ee_backup eeprom_backup.bin
EEPROM len 1024K (1048576) bytes
100% complete
Backup took 86 seconds
chroma>ee_restore eeprom_backup.bin
EEPROM len 1024K (1048576) bytes
Erasing chip...Erase flash

100% complete
```

## Flashing Chroma Tag with proxy firmware

Two binaries are provided in `proxy_firmware`, proxy_vXX.bin and proxy_115200_vXX.bin.  

If your serial port supports 1,000,000 baud flash the tag with proxy_vXX.bin 
otherwise flash the tag with proxy_115200_vXX.bin.

If you are flashing a new tag with the proxy for the first time it is advisable
to make a copy of the current firmware before flashing it in case you ever
want to go back.  Fortunately the stock firmware is **NOT** read protected or
we would not be able to do this.

```
skip@Dell-5510:~/esl/chroma_shell/proxy_firmware$ cc-tool -r cc1110.bin
  Programmer: CC Debugger
  Target: CC1110
  Reading flash (32 KB)...
  Completed (16.86 s.)
skip@Dell-5510:~/esl/chroma_shell/proxy_firmware$
```

```
skip@Dell-5510:~/esl/chroma_shell/proxy_firmware$ cc-tool -e -v -w proxy_v01.bin
  Programmer: CC Debugger
  Target: CC1110
  Erasing flash...
  Completed
  Writing flash (11 KB)...
  Completed (7.38 s.)
  Verifying flash...
  Completed (0.56 s.)
skip@Dell-5510:~/esl/chroma_shell/proxy_firmware$
```


