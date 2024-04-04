#!/usr/bin/python3

from argparse import ArgumentParser
from time import strftime, gmtime
import os
import sys


DESC = "Recursively loop through the input folder.\n" \
       "For each input file, create a folder within output_path named " \
       "'year/month' based on the file's modification time " \
       "(e.g. 2012/02), then store a symlink to the input file in " \
       "this output folder.\n" \
       "The result is a hierarchy of year/month folders each containing " \
       "links to the files from that month."


def make_link_tree(input_path, output_path):
    """Make the year/month output folder structure and store links in it."""

    if not os.path.isdir(input_path):
        print(f"{input_path} is not a directory")
        return False

    if os.path.isfile(output_path):
        print(f"{output_path} exists and is a file")
        return False

    counter = 0;
    for subdir, dirs, files in os.walk(input_path):
        for input_file in files:

            # Figure out the year and the month for this file
            ifile_path = os.path.join(subdir, input_file)
            mtime = os.stat(ifile_path).st_mtime
            year = strftime('%Y', gmtime(int(mtime)))
            month = strftime('%m', gmtime(int(mtime)))
            output_dir = os.path.join(output_path, year, month)

            # Change the '.' in mtime to '_' before use in the link name
            mtime = str(mtime).replace(".", "_")
            # Prepend time to link name for easy sorting
            symlink_path = os.path.join(output_dir, f"{mtime}_{input_file}")

            # Make output directory
            os.makedirs(output_dir, exist_ok=True)
            # Link to the original file
            os.symlink(ifile_path, symlink_path)

            counter += 1
            if counter % 1000 == 0:
                print(f"{counter} files sorted into {output_path}")

    if counter == 0:
        print("No files were sorted")
    else:
        print(f"{counter} files sorted into {output_path}")


def main():
    parser = ArgumentParser(usage="%(prog)s [options]", description=DESC)

    parser.add_argument(
        "-i", "--input-path", required=True,
        help="Path to input directory")

    parser.add_argument(
        "-o", "--output-path", required=True,
        help="Path to output directory")

    opts = parser.parse_args()

    input_path = os.path.normpath(opts.input_path)
    output_path = os.path.normpath(opts.output_path)
    if input_path in output_path:
        sys.exit("The output path should not contain the input path")

    make_link_tree(input_path, output_path)


if __name__ == '__main__':
    main()
