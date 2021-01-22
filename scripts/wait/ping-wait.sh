#!/bin/bash

# Loop until a host is pingable

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <host>"
fi

TARGET=$1

while true; do
    ping -w 1 -c 1 $TARGET > /dev/null
    if [ $? -eq 0 ]; then
        echo "$TARGET is up"
        exit
    fi
    echo "Waiting for $TARGET"
    sleep 2
done
