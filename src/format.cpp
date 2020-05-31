#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hours, minutes;
    string hString, mString, sString;
    if(seconds >= 60) {
        if(seconds >= 3600) {
            hours = seconds / 3600;
            seconds = seconds % 3600;
        }
        minutes = seconds / 60;
        seconds = seconds % 60;
    }
    /*This is a better format, but gives an invalid_length error
    hString = std::to_string(hours);
    hString = string(2 - hString.length(), '0') + hString;
    mString = std::to_string(minutes);
    mString = string(2 - mString.length(), '0') + mString;
    sString =  std::to_string(seconds);
    sString = string(2 - sString.length(), '0') + sString;
    return hString + ":" + mString + ":" + sString;*/

    return std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds);
}   