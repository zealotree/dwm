#! /bin/bash

# USAGE: vol.sh [inc|dec|mute]
# inc - increase volume
# dec - decrease volume
# mute - mute volume

SINK=1

if [[ $1 == "inc" ]]; then
  pactl set-sink-volume $SINK +5%
elif [[ $1 == "dec" ]]; then
  pactl set-sink-volume $SINK -5%
elif [[ $1 == "mute" ]]; then
  pactl set-sink-mute $SINK toggle
fi

