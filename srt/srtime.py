#!/usr/bin/python3.6

"""srtime - SubRip Time - Change timing in SRT files"""


from argparse import ArgumentParser
import sys


def main():
    """Parse command line and start processing."""
    parser = ArgumentParser(usage="%(prog)s [options]",
                            description="Change SRT timing")

    parser.add_argument(
        "-i", "--input-path", required=True,
        help="Path to input SRT file")

    parser.add_argument(
        "-r", "--reset", required=False, type=str,
        help="Absolute shift to time X, e.g. 00:00:05,200. The first cue in "
              "the output will be set to this time before any other changes "
              "are applied.")

    parser.add_argument(
        "-s", "--shift", required=False, type=float, default=0.0,
        help="Relative shift X seconds, e.g. -1.5. All cues are shifted by "
             "this amount. Relative shift is applied after absolute shift.")

    parser.add_argument(
        "-v", "--velocity", required=False, type=float, default=1.0,
        help="Multiply cue times with a velocity factor, e.g. 1.2. All cue "
             "times are multiplied by this factor after shifting.")

    parser.add_argument(
        "-n", "--renumber", required=False, type=int, default=1,
        help="Renumber cues starting at this number if there are cue numbers "
             "in the input.")

    opts = parser.parse_args()
    if opts.reset:
        opts.reset = get_sec(opts.reset)
    process(opts)


def get_sec(time_str):
    """Return seconds from hh:mm:ss,xxx."""
    h, m, s = time_str.split(':')
    s = s.replace(',', '.')
    return int(h) * 3600 + int(m) * 60 + float(s)


def read_times(line):
    """Parse time xx:xx:xx,xxx --> yy:yy:yy,yyy and return as seconds."""
    start_string = line.split()[0]
    stop_string = line.split()[2]
    return get_sec(start_string), get_sec(stop_string)


def write_times(start, stop):
    """Write start and stop seconds as xx:xx:xx,xxx --> yy:yy:yy,yyy."""
    def hms(secs):
        mm, ss = divmod(secs, 60)
        hh, mm = divmod(mm, 60)
        result = "%02d:%02d:%06.3f" % (hh, mm, ss)
        return result.replace('.', ',')

    return "%s --> %s\r\n" % (hms(start), hms(stop))


def process_file(srt_file, opts):
    """Process cue timestamps in srt_file. Print result to stdout."""
    first_cue = True
    new_cue = True
    for line in srt_file:

        if new_cue and opts.renumber and line.strip().isdigit():
            new_cue = False
            line = "%s\r\n" % opts.renumber
            opts.renumber += 1

        elif "-->" in line:
            new_cue = False
            start, stop = read_times(line)

            if first_cue:
                first_cue = False
                # Translate a reset to a shift
                if opts.reset:
                    opts.shift += opts.reset - start

            if opts.shift:
                start += opts.shift
                stop += opts.shift

            if opts.velocity:
                start *= opts.velocity
                stop *= opts.velocity

            line = write_times(start, stop)

        elif line.strip() == "":
            new_cue = True

        try:
            sys.stdout.write(line)
        except IOError:
            return


def process(opts):
    """Open input file and process contents."""
    with open(opts.input_path, encoding="ISO-8859-1") as srt:
        process_file(srt, opts)


if __name__ == '__main__':
    """Main entry point."""
    main()
