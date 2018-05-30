"""Backup utility script using rsync."""

import getpass
import itertools
import os
import socket
import subprocess


# Source locations per user@host
locations = {
    "nilsa@blackout": [
        "/home/nilsa/Documents",
        "/home/nilsa/test",
        "/home/nilsa/bin",
    ],
    "nilsa@macbook-pro": [
        "/Users/nilsa/Documents",
        "/Users/nilsa/Music/Logic"
    ],
    "nilsa@le-imaque": [
        "/Users/nilsa/Documents",
        "/Users/nilsa/Downloads",
        "/Users/nilsa/Movies",
        "/Users/nilsa/Music",
        "/Users/nilsa/Projects"
    ],
    "softis@softis": [
        "/Users/softis/Movies/hemmavideo",
        "/Users/softis/Documents",
        "/Users/softis/Pictures/Photos Library.photoslibrary"
    ]
}


# The list of known storages
storages = [
    "/Volumes/Storbiten",
    "/Volumes/MonoLiten"
]


def get_hostname():
    """Return hostname without any domain suffix."""
    hostname = socket.gethostname()
    return hostname.split('.')[0]


def get_username():
    """Get the name of the current user."""
    return getpass.getuser()


def get_sources():
    """Get locations to back up based on user name and the hostname."""
    hostname = get_hostname()
    user = get_username()
    key = "{0}@{1}".format(user, hostname)

    try:
        match = locations[key]
        print "Locations to back up for {0}".format(key)
        for location in match:
            print "  {0}".format(location)
        print

        return match

    except KeyError:
        print "No source locations for {0}".format(key)
        print
        return None


def get_destinations():
    """Get the backup storages that are available on this machine."""
    available = []

    print "Backup destinations"
    hostname = get_hostname()
    user = get_username()
    for storage in storages:
        if os.path.isdir(storage):
            print "  Storage {0} is available".format(storage)
            leaf_dir = "{0}_{1}".format(hostname, user)
            path = os.path.join(storage, leaf_dir)
            available.append(path)

    if not available:
        print "  No known storages available"
    print

    return available


def main():
    """Main entry point."""

    # Get sources and destinations
    sources = get_sources()
    destinations = get_destinations()

    if not sources or not destinations:
        print "Exiting"
        print
        return

    # Store all sources in all destinations
    for source, destination in itertools.product(sources, destinations):
        # Call rsync to store the backup
        print "  Backing up {0} to {1}". format(source, destination)
        cmd = ['/usr/bin/rsync', '-av', source, destination]
        subprocess.Popen(cmd).communicate()

    print
    print "All done"
    print


if __name__ == "__main__":
    main()
