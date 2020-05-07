#include "Simulation.h"
#include "wireless_network.h"
#include <memory>

int main(int argc, char *argv[]) {
    WirelessNetwork *wireless_network = new WirelessNetwork();
    Simulation* simulation = new Simulation(wireless_network);
    simulation->Run(10000);
}