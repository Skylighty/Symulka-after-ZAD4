#ifndef PROCESS_H_
#define PROCESS_H_

#include <functional>
#include <vector>
#include <queue>

class WirelessNetwork;
//Abstract class to inherit from.
//Allows us to make process agenda in main simulation file.

class Process {
public:
    typedef std::priority_queue<Process *, std::vector < Process * >, std::function<bool(Process *, Process *)>> Agenda;

    virtual void Execute() = 0;              //Execute method to overwrite
    Process(size_t time, WirelessNetwork* wireless_network, Agenda* agenda);
    virtual ~Process();
    void Activate(size_t time, bool relative = true);  //Activate time management method to overwrite
    bool IsTerminated() { return terminated_; }

    void SetTerminated(bool state) { terminated_ = state; }

    size_t GetTime() { return time_; }

protected:
    size_t time_;                           //Process lifetime variable
    bool terminated_ = false;                       //Process terminated flag
    WirelessNetwork* wireless_network_ = nullptr;
    Agenda *agenda_ = nullptr;
};

#endif //PROCESS_H_
