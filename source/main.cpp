#include "Simulation.h"
#include "wireless_network.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <string>

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

void LoadSeeds(std::vector<std::queue<int>*>* seed_vector)
{
  //Creating 4 queues for our 4 TXs
  for (int i = 0; i < 4; ++i)
  {
    std::queue<int>* newq = new std::queue<int>;
    seed_vector->push_back(newq);
  }
  
  //Loading mandatory seeds for our TXs
  std::string line;
  std::fstream seeds;
  seeds.open("./seeds.txt");
  if (seeds.is_open())
  {
    for (int i = 0; i < 4; ++i)
    {
      for (int j = 0; j < 7; ++j)
      {
        seed_vector->at(i)->push(std::stoi(line));
      }
    }
    seeds.close();
  } else std::cout<< "Could not find file 'seeds.txt'." << std::endl;
  
}

int main(int argc, char *argv[]) {
    double L = 0.001; //Starting L parameter equal to 0.1
    std::vector<std::queue<int>*>* seed_vector = new std::vector<std::queue<int>*>;
    LoadSeeds(seed_vector);
    WirelessNetwork *wireless_network = new WirelessNetwork(L,seed_vector);
    Simulation* simulation = new Simulation(wireless_network);
    for (int i = 0; i < 2; i++) //Will iterate up to 1;
    {
      //Here 10 simulations will run for different lambdas
      wireless_network = new WirelessNetwork(L,seed_vector);
      wireless_network->GenerateRXTX();   //Must be implemented here to avoid constructor conflicts
      simulation = new Simulation(wireless_network);
      simulation->Run(10000);
      //delete wireless_network;
      //delete simulation;
      L += 0.1;
      if (L >= 1) //L = 1 protection;
        L = 0.99;
    }
    //Mode(simulation);
    //simulation->Run(10000);
}