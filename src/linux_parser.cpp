#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel >> version;
  }
  return version;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memTotal, memFree, memUtilized;
  string line, label, value; 
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> label >> value;
      if(label == "MemTotal:") {
        memTotal = std::stof(value);
      } 
      else if(label == "MemFree:") {
        memFree = std::stof(value);
      }
    }
  }
  memUtilized = (memTotal - memFree) / memTotal;
  return memUtilized; 
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, uptime;
  long uptimeVal;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    uptimeVal = std::stol(uptime);
  }
  return uptimeVal; 
}

// NOT USED: Read and return the number of jiffies for the system
// Uptime is used in place of total jiffies, per project forum
//long LinuxParser::Jiffies() { return 0; }

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  long utime,stime;
  string val;
  int utime_idx = 14; //utime and stime elements 14 and 15 per proc man page
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    for(int i = 0; i < utime_idx; i++){ //increment 14 elements into line
      linestream >> val; //parse as string to handle unknown types
    }
    utime = stol(val);
    linestream >> stime; //extract 15th element
  }
  return utime + stime; //excludes child processes
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long user, nice, system;
  string line, cpu;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system; //parse as string, long, long, long
  }
  return user + nice + system;
 }

// NOT USED: Read and return the number of idle jiffies for the system
//long LinuxParser::IdleJiffies() { return 0; }

//ADDED: Return CPU Utilization for single process
float LinuxParser::CpuUtilization(int pid) {
  long activeSecs, upSecs;
  int clock = sysconf(_SC_CLK_TCK);
  activeSecs = LinuxParser::ActiveJiffies(pid) / clock; //convert jiffies to sec
  upSecs = LinuxParser::UpTime(pid);
  if(upSecs == 0) upSecs = 1; //check for divide by zero errors
  return ((float)activeSecs / upSecs); //force floating point division
}

// DONE: Read and return CPU utilization
// This function is not used, not sure what was intended in the template
vector<string> LinuxParser::CpuUtilization() { 
  string cpuPercent;
  vector<string> cpuVect;
  vector<int> pidlist = LinuxParser::Pids();
  for(int pid : pidlist) {
    cpuPercent = to_string(LinuxParser::CpuUtilization(pid));
    cpuVect.push_back(cpuPercent);
  }
  return cpuVect; 
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key;
  int val = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> val; // parse as string, int
      if(key == "processes") {
        return val;
      }
    }
  }
  return val; //return 0 if no processes element is populated
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key;
  int val = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> val;
      if(key == "procs_running") {
        return val;
      }
    }
  }
  return val; //return 0 if no procs_running element is present
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
  }
  return line; 
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line, key, memString;
  int val; 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> val;  //parse as string, int
      if(key == "VmSize:"){
        val = val / 1000; //convert kB to mB
        return to_string(val);
      }
    }
  }
  return string(); //if VmSize line does not exist, return empty string
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line, key, uname, x;
  int val, uid; 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> val; //parse as string, int
      if(key == "Uid:"){
        break;
      }
    }
  }
  stream.close();
  std::ifstream stream2(kPasswordPath); //open a second stream to fix redundancy error
  if(stream2.is_open()){
    while(std::getline(stream2, line)) {
      uname = ""; //clear uname
      std::replace(line.begin(), line.end(), ':', ' '); //delimit with white space
      std::istringstream linestream(line);
      linestream >> uname >> x >> uid; //parse as string, string, int
      if(uid == val) return uname;
    }
  }
  return string();
}

// NOT USED: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
//string LinuxParser::Uid (int pid[[maybe_unused]]) { return string(); }

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime (int pid) { 
  string line;
  int starttime_idx = 22; //start time is 22d element per proc man page
  long clock = sysconf(_SC_CLK_TCK);
  long uptime;
  string val;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    for(int i = 0; i < starttime_idx; i++){ //extract 22d element
      linestream >> val; //parse as string to handle unknown types
    }
    uptime = LinuxParser::UpTime() - stol(val)/clock; //convert to no seconds up
  }
  return uptime; 
}