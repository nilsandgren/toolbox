#!/bin/bash

# Forcefully free up disk space consumed by docker
# Run at your own risk


echo 
echo "  Warning!"
echo 
echo "    Containers, images and data in /var/lib/docker will be erased."
echo 
echo -n "    Do you want to continue? [y/n]: "

read input

if [ $input = "y" ]; then
    echo

    echo "    Removing docker containers"
    docker rm -f $(docker ps -a -q)

    echo
    echo "    Removing docker images"
    docker rmi -f $(docker images -a -q)

    echo
    echo "    Stopping docker service"
    sudo service docker stop

    echo
    echo "    Erasing data in /var/lib/docker/"
    sudo rm -rf /var/lib/docker/aufs
    sudo rm -f /var/lib/docker/linkgraph.db

    echo
    echo "    Starting docker service"
    sudo service docker start
    echo
else
    echo
    echo "    Maybe later"
    echo
fi
