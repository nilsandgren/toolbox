#!/usr/bin/python3

import json
import sys


def main():
    if len(sys.argv) == 1:
        f = sys.stdin
    else:
        f = open(sys.argv[1])
    json.dump(json.load(f), sys.stdout, indent=2, sort_keys=True)
    print()

    return 0


if __name__ == '__main__':
    main()
