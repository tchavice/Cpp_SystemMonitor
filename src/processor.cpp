#include "processor.h"
#include "linux_parser.h"
#include <unistd.h>

// DONE: Return the aggregate CPU utilization
// ADDED: calculate CPU utilization dynamically
float Processor::Utilization() { 
    long currentTotal = LinuxParser::UpTime();
    long currentActive = LinuxParser::ActiveJiffies();
    long clock = sysconf(_SC_CLK_TCK);
    float utilization = ((float)(currentActive - prevActive_)/clock)/(currentTotal - prevTotal_);
    prevTotal_ = currentTotal; //store current values in Processor struct
    prevActive_ = currentActive;
    return utilization;
}