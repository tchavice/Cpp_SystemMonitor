#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    long currentTotal = LinuxParser::Jiffies();
    long currentActive = LinuxParser::ActiveJiffies();
    float utilization = (currentActive - prevActiveJiffie_)/(currentTotal - prevJiffie_);
    prevJiffie_ = currentTotal;
    prevActiveJiffie_ = currentActive;
    return utilization;
}