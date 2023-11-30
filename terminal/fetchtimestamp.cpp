#include "fetchtimestamp.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

FetchTimestamp::FetchTimestamp() {
    // Add any initialization code if needed
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

    // Parse the JSON response to get the Unix timestamp
    std::string response = responseStream.str();
    size_t timestampStart = response.find("\"unixtime\":") + 11;
    size_t timestampEnd = response.find(",", timestampStart);
    std::string timestampStr = response.substr(timestampStart, timestampEnd - timestampStart);
    long long timestamp = std::stoll(timestampStr);

    return timestamp;
}
