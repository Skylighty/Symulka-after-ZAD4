#ifndef SIMULATION_H_
#define SIMULATION_H_

#include "wireless_network.h"

class Simulation {
public:
    
    //Constructor explicit because takes only one parameter
    explicit Simulation(WirelessNetwork *wireless_network);

    void Run(int time);

    void RunAsSteps(int time);

private:
    size_t clock_ = 0;
    WirelessNetwork *wireless_network_;
};


#endif //#SIMULATION_H_