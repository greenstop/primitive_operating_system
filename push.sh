#!/bin/bash

rsync -razvP --delete --include="kernel.asm" --include="bootload.asm" --include="kernel.c" --include="remove.sh" --include="make.sh" --include="lib.asm" --include="shell.c" --exclude="*" /Users/Jason/Documents/Forum/Ampitheater/OS/toemail/ProjectE/ jason@10.2.15.90:/home/jason/Documents/OSBochs/toemail/ProjectE/
