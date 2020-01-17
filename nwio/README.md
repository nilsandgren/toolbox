# nwio (nw)
A little tool for monitoring network IO based on /proc/net/dev on Linux
machines.

## Building
./build.sh

## Usage
`./nw -h` gives the following output:


    NAME
       nw - Network interface monitor

    SYNOPSIS
       nw [OPTIONS] [INTERFACE] ...

    DESCRIPTION
       Display network traffic based on /proc/net/dev.

       -h, --help
             print this help

       -i <x>
             time (in seconds) between printouts
             e.g. -i 0.5

       -n <x>
             exit after <x> iterations
             e.g. -n 10

       -t <x>
             print title line every <x> printout
             e.g. -t 50

       -u g|m|k|b
             unit of the printed traffic
             e.g. -u k

               g: Gbit/sec
               m: Mbit/sec
               k: kbit/sec
               b: bit/sec

       -g <x>
             graph: set the maximum rate to <x> in current
             unit and print rate with a number of bars

             e.g. -g 100


       -a, --all
             list all interface.
             by default nw will only list the first 4 interfaces
             when run without a list of interface names.

    EXAMPLES
       Display eth0 traffic with two seconds interval.
             nw -i 2 eth0

       Display traffic for all interfaces in kbit/sec.
             nw -a -u k

    AUTHOR
       Written by Nils Andgren, 2013.



## Example with output
Monitor the interfaces eth0 and lo. Use kbits/sec as the bandwidth
unit and print the output every two seconds:

    user@host# nw -u k -i 2 eths0 lo
    ----------------------------------------------------
    |  eth0                   | lo                     |
    |  kbit/s                 | kbit/s                 |
    |         rx         tx   |        rx         tx   |
    ----------------------------------------------------
    |    1298.21       0.00   |      0.00       0.00   |
    |    1292.37       0.00   |      0.00       0.00   |
    |    1302.47       0.00   |      0.00       0.00   |
    |    1292.38       0.00   |      0.00       0.00   |

