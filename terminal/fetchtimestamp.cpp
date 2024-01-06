#include "fetchtimestamp.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

FetchTimestamp::FetchTimestamp() {
}

long long FetchTimestamp::fetchUnixTimestamp() {
    // Set up the URL of a time server that provides Unix timestamp
    std::string url = "http://worldtimeapi.org/api/ip";

    // Open a pipe to the system shell
    FILE* pipe = popen(("curl -s " + url).c_str(), "r");

    if (!pipe) {
        std::cerr << "Error: Unable to open pipe." << std::endl;
        return -1; // Return -1 on failure
    }

    // Read the response from the pipe
    char buffer[128];
    std::ostringstream responseStream;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        responseStream << buffer;
    }

    // Close the pipe
    pclose(pipe);

    // Parse the JSON response to get the Unix timestamp, UTC offset, and daylight saving info
    std::string response = responseStream.str();
    size_t timestampStart = response.find("\"unixtime\":") + 11;
    size_t timestampEnd = response.find(",", timestampStart);
    std::string timestampStr = response.substr(timestampStart, timestampEnd - timestampStart);
    long long timestamp = std::stoll(timestampStr);

    // Extract UTC offset
    size_t offsetStart = response.find("\"utc_offset\":") + 13;
    size_t offsetEnd = response.find(",", offsetStart);
    std::string offsetStr = response.substr(offsetStart, offsetEnd - offsetStart);
    int utcOffset = std::stoi(offsetStr);

    // Extract daylight saving info
    size_t dstStart = response.find("\"dst\":") + 7;
    size_t dstEnd = response.find(",", dstStart);
    std::string dstStr = response.substr(dstStart, dstEnd - dstStart);
    bool isDst = (dstStr == "true");

    // Apply UTC offset to the timestamp
    timestamp += utcOffset * 3600; // Convert offset to seconds

    // Adjust for daylight saving time if applicable
    if (isDst) {
        timestamp += 3600; // Add one hour if daylight saving is in effect
    }

    return timestamp;
}
