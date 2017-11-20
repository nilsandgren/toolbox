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
        "/Users/nils/Music/Logic"
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


def hostname():
    """Return the host name of this machine."""
    return socket.gethostname()


def trimmed_hostname(hostname):
    """Return hostname without any domain suffix."""
    return hostname.split('.')[0]


def user():
    """Get the name of the current user."""
    return getpass.getuser()


def get_sources(user, hostname):
    """Get locations to back up based on user name and the hostname."""
    hostname = trimmed_hostname(hostname)
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


def get_destinations(user, hostname):
    """Get the backup storages that are available on this machine."""
    available = []

    print "Backup destinations"
    hostname = trimmed_hostname(hostname)
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
    sources = get_sources(user(), hostname())
    destinations = get_destinations(user(), hostname())

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
