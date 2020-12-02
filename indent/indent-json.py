#!/usr/bin/python3

import json
import sys


def main():
    if len(sys.argv) != 2:
        print("usage: %s <json file>" % sys.argv[0])
        return 1
    
    with open(sys.argv[1]) as f:
        json.dump(json.load(f), sys.stdout, indent=2, sort_keys=True)

    return 0


if __name__ == '__main__':
    main()
