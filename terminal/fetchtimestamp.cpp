#include "fetchtimestamp.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <QDebug>

FetchTimestamp::FetchTimestamp() {
}

std::pair<long long, int> FetchTimestamp::fetchUnixTimestampAndOffset() {
    // Set up the URL of a time server that provides Unix timestamp
    std::string url = "http://worldtimeapi.org/api/ip";

    // Open a pipe to the system shell
    FILE* pipe = popen(("curl -s " + url).c_str(), "r");

    if (!pipe) {
        std::cerr << "Error: Unable to open pipe." << std::endl;
        return std::make_pair(-1, 0); // Return an error code
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
    size_t offsetStart = response.find("\"utc_offset\":\"") + 16;
    size_t offsetEnd = response.find("\"", offsetStart);
    std::string offsetStr = response.substr(offsetStart, offsetEnd - offsetStart);

    qDebug() << "Offset start:" << offsetStart;
    qDebug() << "Offset end:" << offsetEnd;
    qDebug() << "Extracted UTC offset string:" << QString::fromStdString(offsetStr);

    // Convert offset string to integer
    int utcOffset = 0;
    if (!offsetStr.empty()) {
        // Find the position of the colon
        size_t colonPos = offsetStr.find(':');
        if (colonPos != std::string::npos) {
            // Extract the numeric part before the colon
            std::string numericPart = offsetStr.substr(0, colonPos);
            utcOffset = std::stoi(numericPart);
            qDebug() << "Converted UTC offset:" << utcOffset;
        } else {
            // No colon found, try converting the entire string
            utcOffset = std::stoi(offsetStr);
            qDebug() << "Converted UTC offset:" << utcOffset;
        }
    } else {
        qDebug() << "Empty UTC offset string.";
    }

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

    return std::make_pair(timestamp, utcOffset);
}
