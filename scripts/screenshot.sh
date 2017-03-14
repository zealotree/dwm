#! /bin/bash

SCREENSHOT_DIR="~/Pictures/Screenshots'

if [[ "$1" != "--silent" ]]; then
	notify-send "Screen captured"
fi
scrot -d 2 'Screenshot-%Y-%b-%d-%R-$wx$h.png' -e 'mv $f $SCREENSHOT_DIR'

