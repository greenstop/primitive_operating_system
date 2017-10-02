#!/bin/bash

rsync -azvP --delete --include="shell" --include="floppya.img" --exclude=* jason@10.2.15.90:/home/jason/Documents/OSBochs/toemail/ProjectE/ /Users/Jason/Documents/Forum/Ampitheater/OS/toemail/ProjectE/
