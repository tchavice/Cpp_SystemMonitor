#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // DONE: Declare any necessary private members
 private:
 long prevTotal_ = 0;
 long prevActive_ = 0;
};

#endif