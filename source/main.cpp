#include "Simulation.h"
#include "wireless_network.h"
#include <iostream>
#include <string>
#include <memory>

void Mode(Simulation* sim){
  std::cout << "How long period of time do you want to simulate? (Input number) : ";
  std::string time;
  getline(std::cin, time);
  int simtime = std::stoi(time);
  std::cout << "Choose mode : \n" << "[1] Run continuously \n" << "[2] Run step by step\n" << "Your choice : ";
  char choice = getchar();
  if (choice == '1')
    sim->Run(simtime);
  else
    sim->RunAsSteps(simtime);
}

int main(int argc, char *argv[]) {
    WirelessNetwork *wireless_network = new WirelessNetwork();
    Simulation* simulation = new Simulation(wireless_network);
    Mode(simulation);
    //simulation->Run(10000);
}