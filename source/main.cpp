#include "Simulation.h"
#include "wireless_network.h"
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <memory>


//Function for loading seeds from "seeds.txt" to queue.
void LoadSeeds(std::queue<int> &sque)
{
  std::string line;
  std::fstream seeds;
  seeds.open("seeds.txt");
  if (seeds.is_open())
  {
    while(std::getline(seeds,line))
    {
      sque.push(std::stoi(line));
    }

  }
}

//Function for generating seeds into "seeds.txt" file.
void GenerateSeeds(Generator* gen)
{
  long long seed_iterator = 0;
  long long end_seeds = 29000000;
  long long seed = 0;
  std::ofstream seeds;
  seeds.open("seeds.txt");
  while (seed_iterator < end_seeds)
  {
    seed = gen->Rand(0, 2145483647);
    if (seed_iterator % 100000 == 0)
    {
      seeds << seed << "\n";
    }
    ++seed_iterator;
  }
  seeds.close();
}

int main(int argc, char *argv[])
{
    //Generator object for generating starting seeds
    Generator* start_gen = new Generator(12554568);
    GenerateSeeds(start_gen);
    
    //A Logger object for statistics output.
    Logger* log = new Logger();
    
    //Declaring simulation times - simulating 60 seconds = 1 minute for each simulation
    size_t simulation_time = 0;
    size_t starting_time = 0;
    size_t sim_time_seconds = simulation_time/1000;
    std::cout << "\nProsze podac czas zakonczenia symulacji (wpisz liczbe) : ";
    std::cin >> simulation_time;
    std::cout << "Prosze podac czas zakonczenia fazy poczatkowej (wpisz liczbe) : ";
    std::cin >> starting_time;
    std:: cout << std::endl;
    
    //Prepare seeds for generator
    std::queue<int> seeds_queue;
    LoadSeeds(seeds_queue);
    
    //Lambda declaration
    double L = 0.05;
  
    WirelessNetwork *wireless_network = new WirelessNetwork(L, seeds_queue);
    wireless_network->GenerateRXTX();
    Simulation* simulation = new Simulation(wireless_network, starting_time);
    simulation->Run(simulation_time);
    //Simulations being made
    //Wireless Network uses "global" queue, so it has to be remade after every iteration
    //and same goes for Simulation object because it bases on WirelessNetwork object
    /*for (int i = 0; i < 10; ++i)
    {
      WirelessNetwork *wireless_network = new WirelessNetwork(L, seeds_queue);
      wireless_network->GenerateRXTX();
      Simulation* simulation = new Simulation(wireless_network, starting_time);
      simulation->Run(simulation_time);
    }*/
}