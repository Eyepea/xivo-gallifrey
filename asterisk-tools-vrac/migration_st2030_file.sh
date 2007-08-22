#!/bin/bash

cd /tftpboot

for a in $(ls *.txt);
do
        file=$(echo $a.new)
        echo "Migration $a"
        echo "[sip]" > $file
        cat $a | grep -iE '(DisplayName1|TEL1Number|regid1|regpwd1)' >> $file

        echo "" >> $file

        echo "[sys]" >> $file
        cat $a | grep -iE '(config_sn)' >> $file

        rm $a
        mv $file $a
done

for a in ST*.inf
do
        rm -f $a
        ln -s /tftpboot/Thomson/ST2030S_common $a
done
