# http-tail
Parse HTTP access log and print statistics.

## Usage

This is the output of `http-tail -h`:

    NAME
        http-tail - HTTP stats from access log

    OPTIONS
        -h
            Print help and exit

        -f
            Follow mode (default)
              Tail stdin and use wall-clock as time.
              tail -F access.log | http-tail

        -r <logfile>
            Replay mode
              Read dates from log. Process one second of log per second.
              http-tail -r access.log

        -s <logfile>
            Scan mode
              Read dates from log. Process as fast as possible.
              http-tail -s access.log

    Log line format (defined in http-tail.cpp):
        Date.........: field 4
        Status.......: field 9
        Byte count...: field 10
        Date formats.: "[%d/%b/%Y:%T" or "[%Y-%m-%dT%T"


## Example output

Tailing HTTP access log in real time:

    $ tail -F access.log | ./http-tail
    info: tailing stdin in real time
    info: using interval of 1.00 sec
    .-------.-------.-------.-------.-------.-------.-------.-------------.
    |   0xx |   1xx |   2xx |   3xx |   4xx |   5xx |   all |        rate |
    '-------'-------'-------'-------'-------'-------'-------'-------------'
    |     0 |     0 |   166 |     0 |     5 |     0 |   171 | 419.58 Mbps |
    |     0 |     0 |   300 |     0 |     7 |     0 |   307 | 593.12 Mbps |
    |     0 |     0 |   263 |     0 |     8 |     0 |   271 | 469.04 Mbps |
    |     0 |     0 |   328 |     0 |     8 |     0 |   336 | 646.20 Mbps |
    |     0 |     0 |   371 |     0 |     7 |     0 |   378 |   1.23 Gbps |
    |     0 |     0 |   328 |     0 |     9 |     0 |   337 | 782.16 Mbps |
    ...


Replaying contents of access.log:

    $ ./http-tail -r access.log
    info: replaying /var/log/edgeware/ew-repackager/access.log in real time, reading time from log.
    info: using interval of 1.00 sec
    .-------.-------.-------.-------.-------.-------.-------.-------------.
    |   0xx |   1xx |   2xx |   3xx |   4xx |   5xx |   all |        rate |
    '-------'-------'-------'-------'-------'-------'-------'-------------'
    |     0 |     0 |    83 |     0 |     0 |     0 |    83 |   1.18 Gbps |
    |     0 |     0 |   896 |     0 |     0 |     0 |   896 |  14.58 Gbps |
    |     0 |     0 |     7 |     0 |     0 |     0 |     7 | 115.39 Mbps |
    |     0 |     0 |  1041 |     0 |     0 |     0 |  1041 |  17.08 Gbps |
    |     0 |     0 |  1003 |     0 |     0 |     0 |  1003 |  16.62 Gbps |
    |     0 |     0 |     6 |     0 |     0 |     0 |     6 | 104.93 Mbps |
    ...


