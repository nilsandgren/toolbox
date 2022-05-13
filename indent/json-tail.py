#!/usr/bin/python3

"""Tail stdin where each line is a JSON object. Output indented version."""

import json
import sys


def main():
    for line in sys.stdin:
        if line[0] == '{':  # Be kind to the parser
            try:
                j = json.loads(line)
                json.dump(j, sys.stdout, indent=2, sort_keys=True)
                print()  # dump does not add newline
                continue
            except Exception:
                pass
        # Pass-through if line does not seem to be JSON
        print(line, end='')

    return 0


if __name__ == '__main__':
    main()
