#ifndef FETCHTIMESTAMP_H
#define FETCHTIMESTAMP_H

#include <string>

class FetchTimestamp {
public:
    FetchTimestamp();

    // Function to fetch Unix timestamp from a time server
    long long fetchUnixTimestamp();

private:
         // Add any private members or helper functions if needed
};

#endif // FETCHTIMESTAMP_H
