#!/bin/bash

# Loop until curling provided URL gives HTTP 200 OK

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <URL>"
fi

TARGET=$1

while true; do
    RESPONSE=`curl -m 5 -s -w "%{http_code}\\n" -o /dev/null $TARGET`

    if [ $RESPONSE = "200"  ]; then
        echo "$TARGET is available"
        exit
    fi
    echo "Waiting for $TARGET"
    sleep 30
done
