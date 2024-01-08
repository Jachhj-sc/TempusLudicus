#ifndef FETCHTIMESTAMP_H
#define FETCHTIMESTAMP_H

#include <string>
#include <utility> // Add this include for std::pair

class FetchTimestamp {
public:
    FetchTimestamp();

    // Function to fetch Unix timestamp and UTC offset from a time server
    std::pair<long long, int> fetchUnixTimestampAndOffset();

private:
         // Add any private members or helper functions if needed
};

#endif // FETCHTIMESTAMP_H
