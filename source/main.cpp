#include "Simulation.h"
#include "wireless_network.h"
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
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

void LoadSeeds(std::vector<std::queue<int>> &svector)
{
  std::string line;
  std::fstream seeds;
  seeds.open("./seeds.txt");
  if (seeds.is_open())
  {
    for(int i = 0; i < 4; ++i)
    {
      std::queue<int> sq;
      for(int j = 0; j < 7; ++j)
      {
        std::getline(seeds, line);
        sq.push(stoi(line));
      }
      svector.push_back(sq);
      while (!sq.empty())
        sq.pop();
    }
  }
}

int main(int argc, char *argv[]) {
    //Prepare seeds for generators
  std::vector<std::queue<int>> seeds_vector;
    LoadSeeds(seeds_vector);
    double L = 0.001;
    WirelessNetwork *wireless_network = new WirelessNetwork(L, seeds_vector);
    wireless_network->GenerateRXTX();
    Simulation* simulation = new Simulation(wireless_network);
    simulation->Run(100000);
    //simulation->Run(10000);
}