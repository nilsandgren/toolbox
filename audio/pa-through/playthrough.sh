#!/bin/bash

# Turn microphone loopback on or off
# Requires pactl

if [[ $# -ne 1 ]]; then
  echo ""
  echo "  usage $0 [on|off]"
  echo ""
  exit
fi

if [[ $1 == "on" ]]; then
  pactl load-module module-loopback latency_msec=1
  exit
fi

if [[ $1 == "off" ]]; then
  index=`pactl list short modules | grep loopback | awk '{ print $1 }'`
  pactl unload-module $index
  exit
fi
