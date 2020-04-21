#ifndef PROCESS_H_
#define PROCESS_H_

class Process
{
public:
  virtual void Execute() {};
  virtual void Activate(double time) {};
  double time_;
  bool terminated_;
  int phase_;
};

#endif //PROCESS_H_
