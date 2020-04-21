#ifndef PROCESS_H_
#define PROCESS_H_

//Abstract class to inherit from.
//Allows us to make process agenda in main simulation file.

class Process
{
public:
  virtual void Execute() {};              //Execute method to overwrite
  virtual void Activate(double time) {};  //Activate time management method to overwrite
  double time_;                           //Process lifetime variable
  bool terminated_;                       //Process terminated flag
  int phase_;                             //Not particularly necessary - let's leave it here for now.
};

#endif //PROCESS_H_
