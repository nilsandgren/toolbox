#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <libgen.h>
#include <ifaddrs.h>

#include <iostream>
#include <vector>


//
// A tool for printing network I/O information based on /proc/net/dev.
//
//  user@host:~# ./nw eths0 eths1 ethm0
//  -----------------------------------------------------------------------------
//  |  eths0                  | eths1                  | ethm0                  |
//  |  Mbit/s                 | Mbit/s                 | Mbit/s                 |
//  |        rx         tx    |       rx         tx    |       rx         tx    |
//  -----------------------------------------------------------------------------
//  |      0.01       0.00    |     0.00       0.00    |     0.00       0.00    |
//  |      0.00       0.00    |     0.00       0.00    |     0.00       0.00    |
//  |      0.00       0.00    |     0.00       0.00    |     0.02       0.00    |
//  |      0.01       0.00    |     0.71      30.67    |     0.00       0.00    |
//  |      0.00       0.00    |     2.31     123.18    |     0.01       0.00    |
//  |      0.00       0.00    |     1.94     123.20    |     0.00       0.00    |
//  |      0.01       0.00    |     1.90     123.18    |     0.01       0.00    |
//  |      0.00       0.00    |     1.86     123.20    |     0.00       0.00    |
//  |      0.00       0.00    |     1.84     123.19    |     0.00       0.00    |
//  |      0.01       0.00    |     1.95     123.20    |     0.01       0.00    |
//  -----------------------------------------------------------------------------
//

#define COLUMNS 22

enum Direction
{
    kTX, // Transmit
    kRX  // Receive
};

enum BandwidthUnit
{
    kBits,
    kKiloBits,  // 1000
    kMegaBits,  // 1000000
    kGigaBits   // 1000000000
};

std::string toString(BandwidthUnit unit)
{
    switch(unit)
    {
        case kBits: return "bit/s";
        case kKiloBits: return "kbit/s";
        case kMegaBits: return "Mbit/s";
        case kGigaBits: return "Gbit/s";
    }
}

const int32_t kRunForever = -1;
const int32_t kDefaultMaxInterfaces = 4;

// Represents a network interface
class Interface
{
    public:
        ~Interface();
        static Interface * create(const std::string & name);

        // Update interface statistics
        void update();

        // Print text in a fixed-width column
        void printColumn(const std::string& text) const;

        // Print title line for this interface
        void printTitle() const;

        // Print traffic direction title line for this interface
        void printDirectionTitle() const;

        // Print the bandwidth unit
        void printBandwidthUnit() const;

        // Print current interface statistics
        void print() const;

        // Get the interface name
        std::string getName() const;

    protected:
        Interface(const std::string & name);

    private:
        std::string mName;

        int64_t mTXBytes;
        int64_t mTXBitsPerSecond;

        int64_t mRXBytes;
        int64_t mRXBitsPerSecond;

        double mLastUpdateTime;
};

// Runtime configuration
class Configuration
{
    public:
        Configuration()
            : mPollInterval(1.0)
            , mTitleInterval(20)
            , mBandwidthUnit(kMegaBits)
            , mIterationLimit(kRunForever)
            , mListAllInterfaces(false)
            , mGraphMax(0)
        {
        }

        // How often to update interface statistics
        double mPollInterval;

        // Print interface title every X times
        int32_t mTitleInterval;

        // The unit used for printing the bandwidth
        BandwidthUnit mBandwidthUnit;

        // Run at most X iterations
        int32_t mIterationLimit;

        // List all interfaces
        bool mListAllInterfaces;

        // Max rate, in current unit, for graph mode
        int64_t mGraphMax;
} gOptions;

// Read /proc/net/dev into a buffer
static int64_t
readProcNetFile(char * buffer, size_t bufferSize)
{
    FILE * netFile = fopen("/proc/net/dev", "r");
    int netFileSize = 0;
    if (!netFile)
    {
        std::cerr << "Could not open /proc/net/dev" << std::endl;
        return 0;
    }
    netFileSize = fread(buffer, 1, bufferSize, netFile);
    fclose(netFile);
    return netFileSize;
}


// Parse TX/RX information from /proc/net/dev content buffer
static int64_t
pollBytes(const std::string & interfaceName,
          Direction direction)
{
    const size_t kNetFileBufferSize = 10000;
    char netFileContent[kNetFileBufferSize];
    memset(netFileContent, 0, kNetFileBufferSize);
    if (!readProcNetFile(netFileContent, kNetFileBufferSize))
    {
        return 0;
    }

    char * interfaceLine = strstr(netFileContent, interfaceName.c_str());
    if (interfaceLine)
    {
        char device[1024];
        unsigned long int bytes = 0;
        if (kTX == direction)
        {
            // 10th element
            sscanf(interfaceLine,
                   "%s %*u %*u %*u %*u %*u %*u %*u %*u %lu",
                   device, &bytes);
        }
        else
        {
            // 2nd element
            sscanf(interfaceLine,
                   "%s %lu %*u %*u %*u %*u %*u %*u %*u %*u",
                   device, &bytes);
        }
        return bytes;
    }
    return 0;
}

// Print title line for the interfaces
static void
printInterfaceTitles(const std::vector<Interface*> interfaces)
{
    const int kTotalWidth = interfaces.size() * (COLUMNS + 3) + 2;
    std::cout << std::string(kTotalWidth, '-') << std::endl;

    std::cout << "| ";
    for (const auto & interface : interfaces)
    {
        std::cout << " ";
        interface->printTitle();
        std::cout << " |";
    }
    std::cout << std::endl;

    std::cout << "| ";
    for (const auto & interface : interfaces)
    {
        std::cout << " ";
        interface->printBandwidthUnit();
        std::cout << " |";
    }
    std::cout << std::endl;

    std::cout << "| ";
    for (const auto & interface : interfaces)
    {
        std::cout << " ";
        interface->printDirectionTitle();
        std::cout << " |";
    }
    std::cout << std::endl;

    std::cout << std::string(kTotalWidth, '-') << std::endl;
}

// Use getifaddrs to get interface names
static void
autoSetupInterfaces(std::vector<Interface*> & interfaces,
                    bool listAllInterfaces)
{
    struct ifaddrs *addrs,*tmp;

    int numInterfaces = 0;
    int numSkippedInterfaces = 0;

    getifaddrs(&addrs);
    tmp = addrs;

    while (tmp)
    {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET)
        {
            if (listAllInterfaces || numInterfaces < kDefaultMaxInterfaces)
            {
                Interface * interface = Interface::create(tmp->ifa_name);
                interfaces.push_back(interface);
                numInterfaces++;
            }
            else
            {
                numSkippedInterfaces++;
            }
        }
        tmp = tmp->ifa_next;
    }
    freeifaddrs(addrs);
    if (numSkippedInterfaces)
    {
        std::cerr << "Skipped " << numSkippedInterfaces << " interfaces. "
                  << "Run with -a to list all." << std::endl;
    }
}

static void
printHelp(char * argv[])
{
    std::string s1 = "  ";
    std::string s2 = "     ";
    std::string s3 = "           ";
    std::string name = basename(argv[0]);

    std::cerr << std::endl;
    std::cerr << s1 << "NAME" << std::endl;
    std::cerr << s2 << name << " - Network interface monitor" << std::endl;
    std::cerr << std::endl;
    std::cerr << s1 << "SYNOPSIS" << std::endl;
    std::cerr << s2 << name << " [OPTIONS] [INTERFACE] ...";
    std::cerr << std::endl;
    std::cerr << std::endl;
    std::cerr << s1 << "DESCRIPTION" << std::endl;
    std::cerr << s2 << "Display network traffic based on /proc/net/dev." << std::endl;
    std::cerr << std::endl;
    std::cerr << s2 << "-h, --help" << std::endl;
    std::cerr << s3 << "print this help" << std::endl;
    std::cerr << std::endl;
    std::cerr << s2 << "-i <x>" << std::endl;
    std::cerr << s3 << "time (in seconds) between printouts";
    std::cerr << std::endl;
    std::cerr << s3 << "e.g. -i 0.5" << std::endl;
    std::cerr << std::endl;
    std::cerr << s2 << "-n <x>" << std::endl;
    std::cerr << s3 << "exit after <x> iterations";
    std::cerr << std::endl;
    std::cerr << s3 << "e.g. -n 10" << std::endl;
    std::cerr << std::endl;
    std::cerr << s2 << "-t <x>" << std::endl;
    std::cerr << s3 << "print title line every <x> printout";
    std::cerr << std::endl;
    std::cerr << s3 << "e.g. -t 50" << std::endl;
    std::cerr << std::endl;
    std::cerr << s2 << "-u g|m|k|b" << std::endl;
    std::cerr << s3 << "unit of the printed traffic" << std::endl;
    std::cerr << s3 << "e.g. -u k" << std::endl;
    std::cerr << std::endl;
    std::cerr << s3 << "  g: Gbit/sec" << std::endl;
    std::cerr << s3 << "  m: Mbit/sec" << std::endl;
    std::cerr << s3 << "  k: kbit/sec" << std::endl;
    std::cerr << s3 << "  b: bit/sec" << std::endl;
    std::cerr << std::endl;
    std::cerr << s2 << "-g <x>" << std::endl;
    std::cerr << s3 << "graph: set the maximum rate to <x> in current" << std::endl;
    std::cerr << s3 << "unit and print rate with a number of bars" << std::endl;
    std::cerr << std::endl;
    std::cerr << s3 << "e.g. -g 100" << std::endl;
    std::cerr << std::endl;
    std::cerr << s2 << "-a, --all" << std::endl;
    std::cerr << s3 << "list all interface." << std::endl;
    std::cerr << s3 << "by default " << name << " will only list the first "
                    << kDefaultMaxInterfaces << " interfaces " << std::endl;
    std::cerr << s3 << "when run without a list of interface names." << std::endl;
    std::cerr << std::endl;
    std::cerr << s1 << "EXAMPLES" << std::endl;
    std::cerr << s2 << "Display eth0 traffic with two seconds interval.";
    std::cerr << std::endl;
    std::cerr << s3 << name << " -i 2 eth0" << std::endl;
    std::cerr << std::endl;
    std::cerr << s2 << "Display traffic for all interfaces in kbit/sec.";
    std::cerr << std::endl;
    std::cerr << s3 << name << " -a -u k" << std::endl;
    std::cerr << std::endl;
    std::cerr << s1 << "AUTHOR" << std::endl;
    std::cerr << s2 << "Written by Nils Andgren, 2013." << std::endl;
    std::cerr << std::endl;
}

// Create network interfaces objects from command line arguments or
// using getifaddrs if no interface names have been given.
void
setup(std::vector<Interface*> & interfaces,
      int argc, char * argv[])
{
    std::vector<Interface*> allInterfaces;
    autoSetupInterfaces(allInterfaces, true);

    for (int i = 1; i < argc; i++)
    {
        // Help text
        if (strcmp(argv[i], "-h") == 0 ||
            strcmp(argv[i], "--help") == 0)
        {
            printHelp(argv);
            exit(0);
        }
        // Poll interval
        else if (strcmp(argv[i], "-i") == 0 && i < argc-1)
        {
            i++;
            gOptions.mPollInterval = atof(argv[i]);
        }
        // Title interval
        else if (strcmp(argv[i], "-t") == 0 && i < argc-1)
        {
            i++;
            gOptions.mTitleInterval = atoi(argv[i]);
        }
        // Bandwidth unit
        else if (strcmp(argv[i], "-u") == 0 && i < argc-1)
        {
            i++;
            if (tolower(argv[i][0]) == 'g')
            {
                gOptions.mBandwidthUnit = kGigaBits;
            }
            else if (tolower(argv[i][0]) == 'm')
            {
                gOptions.mBandwidthUnit = kMegaBits;
            }
            else if (tolower(argv[i][0]) == 'k')
            {
                gOptions.mBandwidthUnit = kKiloBits;
            }
            else if (tolower(argv[i][0]) == 'b')
            {
                gOptions.mBandwidthUnit = kBits;
            }
            else
            {
                gOptions.mBandwidthUnit = kMegaBits;
            }
        }
        // Limit the number of iterations
        else if (strcmp(argv[i], "-n") == 0 && i < argc-1)
        {
            i++;
            gOptions.mIterationLimit = atof(argv[i]);
        }
        // Graph mode (print bars) max level
        else if (strcmp(argv[i], "-g") == 0 && i < argc-1)
        {
            i++;
            int64_t value = atol(argv[i]);
            if (value > 0)
            {
                gOptions.mGraphMax = value;
            }
        }
        if (strcmp(argv[i], "-a") == 0 ||
            strcmp(argv[i], "--all") == 0)
        {
            gOptions.mListAllInterfaces = true;
        }
        else
        {
            // If not, then it might be an interface name
            bool isInterface = false;
            for (const auto & interface : allInterfaces)
            {
                if (interface->getName() == argv[i])
                {
                    isInterface = true;
                    break;
                }
            }
            if (isInterface)
            {
                Interface * interface = Interface::create(argv[i]);
                if (interface)
                {
                    interfaces.push_back(interface);
                }
            }
            // If not, skip it
        }
    }
    if (interfaces.empty())
    {
        autoSetupInterfaces(interfaces, gOptions.mListAllInterfaces);
    }

    for (auto & interface : allInterfaces)
    {
        delete interface;
    }
}


int
main(int argc, char * argv[])
{
    std::vector<Interface*> interfaces;

    setup(interfaces, argc, argv);

    if (!interfaces.size())
    {
        std::cerr << "No network interfaces found" << std::endl;
        return 0;
    }

    int64_t counter = 0;
    bool runForever = gOptions.mIterationLimit == kRunForever;
    while (counter < gOptions.mIterationLimit || runForever)
    {
        if (gOptions.mTitleInterval && counter % gOptions.mTitleInterval == 0)
        {
            printInterfaceTitles(interfaces);
        }
        counter++;

        usleep(1000000 * gOptions.mPollInterval);
        std::cout << "| ";
        for (const auto & interface : interfaces)
        {
            std::cout << " ";
            interface->update();
            interface->print();
            std::cout << " |";
        }
        std::cout << std::endl;
    }

    for (auto & interface : interfaces)
    {
        delete interface;
    }
    return 0;
}

///////////////////////////////////////////
//
// class Interface implementation
//
Interface * Interface::create(const std::string & name)
{
    return new Interface(name);
}

Interface::Interface(const std::string & name)
    : mName(name)
    , mTXBytes(0)
    , mTXBitsPerSecond(0)
    , mRXBytes(0)
    , mRXBitsPerSecond(0)
    , mLastUpdateTime(-1)
{
    update();
}

Interface::~Interface()
{
}

void Interface::update()
{
    struct timeval timeStruct;
    gettimeofday(&timeStruct, NULL);
    double now = ((timeStruct.tv_sec * 1000000.0) + timeStruct.tv_usec) / 1000000.0;

    int64_t txBytes = pollBytes(mName, kTX);
    int64_t rxBytes = pollBytes(mName, kRX);

    if (mLastUpdateTime > 0)
    {
        double seconds = now - mLastUpdateTime;

        int64_t bits = (txBytes - mTXBytes) * 8;
        mTXBitsPerSecond = bits / seconds;

        bits = (rxBytes - mRXBytes) * 8;
        mRXBitsPerSecond = bits / seconds;
    }
    mTXBytes = txBytes;
    mRXBytes = rxBytes;
    mLastUpdateTime = now;
}

void Interface::printColumn(const std::string& text) const
{
    // print text in a fixed-width column
    char line[COLUMNS + 1];
    memset(line, 0x20, COLUMNS);
    line[COLUMNS] = '\0';
    snprintf(line, COLUMNS, "%s", text.c_str());
    // overwrite null termination
    line[strlen(line)] = ' ';
    fprintf(stdout, "%s", line);
}

void Interface::printTitle() const
{
    // print title that has a fixed width
    printColumn(mName);
}

void Interface::printBandwidthUnit() const
{
    printColumn(toString(gOptions.mBandwidthUnit));
}

void Interface::printDirectionTitle() const
{
    printColumn("       rx         tx");
}

void Interface::print() const
{
    // print an interface statistics line that has a fixed width
    char line[COLUMNS + 1];
    memset(line, 0x20, COLUMNS);
    line[COLUMNS] = '\0';
    double denominator = 1000000;
    switch (gOptions.mBandwidthUnit)
    {
        case kGigaBits: denominator = 1000000000.0; break;
        case kMegaBits: denominator = 1000000.0; break;
        case kKiloBits: denominator = 1000.0; break;
        case kBits: denominator = 1.0; break;
    }

    double rxRate = mRXBitsPerSecond/denominator;
    double txRate = mTXBitsPerSecond/denominator;
    if (gOptions.mGraphMax)
    {
        // Print a number of bars, signifying fraction of max rate
        double rxFraction = std::min(rxRate/gOptions.mGraphMax, 1.0);
        double txFraction = std::min(txRate/gOptions.mGraphMax, 1.0);
        int maxBars = 10;
        int rxBars = maxBars * rxFraction;
        int txBars = maxBars * txFraction;
        for (int i = 0; i < rxBars; i++) printf("|");
        for (int i = 0; i < maxBars - rxBars; i++) printf(" ");
        printf("  ");
        for (int i = 0; i < txBars; i++) printf("|");
        for (int i = 0; i < maxBars - txBars; i++) printf(" ");
    }
    else
    {
        // Print numerical value
        snprintf(line, COLUMNS, "%9.2f  %9.2f", rxRate, txRate);
        line[strlen(line)] = ' ';
        fprintf(stdout, "%s", line);
    }
}

std::string Interface::getName() const
{
    return mName;
}
