#/bin/bash
# dump all eeprom images

#set -x

dumpfile=lut_dumps/all_luts.lst

eeproms=`find eeprom_backups -name "*eeprom.bin"`

rm $dumpfile
for file in $eeproms; do
    args="-c dump_lut $file"
    ./chroma_shell/build/chroma_shell "$args" >> $dumpfile
done

