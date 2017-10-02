#!/bin/bash

#Remove this huge file
rm bochsout.txt

./push.sh
ssh jason@10.2.15.90 'bash -s' <remoteCmd.sh
./pull.sh

#Copy message.txt into floppya.img
#dd if=message.txt  of=floppya.img bs=512 count=1 seek=30 conv=notrunc

#copy map and dir
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc

#compile loadFile
gcc -o loadFile loadFile.c
./loadFile message.txt
./loadFile shell
./loadFile tstprg
./loadFile tstpr2
./loadFile myTestPrg
./loadFile phello

bochs -qf opsys.bxrc 

