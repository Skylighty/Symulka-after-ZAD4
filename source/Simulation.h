#ifndef SIMULATION_H_
#define SIMULATION_H_

#include "wireless_network.h"

class Simulation {
public:
    
    //Constructor explicit because takes only one parameter
    explicit Simulation(WirelessNetwork *wireless_network, size_t startingtime);

    void Run(int time);

    void DisplayStatistics();
    
    double GetBitrate();
    

private:
    size_t starting_time_;
    size_t clock_ = 0;
    WirelessNetwork *wireless_network_;
};


#endif //#SIMULATION_H_