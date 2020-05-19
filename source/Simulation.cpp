#include "wireless_network.h"
#include "Simulation.h"
#include "process.h"
#include "packet.h"
#include <iostream>

Simulation::Simulation(WirelessNetwork *wireless_network) {
    wireless_network_ = wireless_network;
    
}

void Simulation::Run(int time)
{
  //Simulation starts at clock = 0
  clock_ = 0;
    std::cout << " >>> Started simulation with method M4 - Process interaction <<< \n";
    
  //LAMBDA Function declaration for segregation of priority queue - our agenda
  auto cmp = [](Process *left, Process *right) { return left->GetTime() > right->GetTime(); };
  
    Packet::Agenda agenda(cmp); //Agenda being made
    //Generating start packet for every 1 of RXTX pairs
    for (int i = 1; i < (wireless_network_->kDeviceCount_+1); ++i) {
        auto new_packet = new Packet(i, wireless_network_, clock_, &agenda, wireless_network_->logger);
        new_packet->Activate(0);
    }
    //=======================================================
    //====================- MAIN LOOP -======================
    //=======================================================
    //Loop basically runs untill simulation time is lesser than the time we want to simulte, conversion to size_t btw.
    
    while (clock_ < static_cast<size_t>(time))
    {
      //Declaration to access a process to put down from agenda and execute loop for
      Process* process = agenda.top();
      
      //Taking process down from agenda
      agenda.pop();
      
      //Clock update
      clock_ = process->GetTime();
      
      //Simulation time put to screen
      wireless_network_->logger->Info("Simulation time : " + std::to_string(clock_));
      
      //Execution of a current process state
      process->Execute();
      
      //If transmission ended delete the process and dont put it back to agenda :)
      if (process->IsTerminated())
      {
        delete process;
      }
    }
    wireless_network_->logger->Info("\nSuccessfully transmitted packets : " + std::to_string(wireless_network_->GetSuccessPackets()));
    wireless_network_->logger->Info("Packets which transmission has been failed : " + std::to_string(wireless_network_->GetDeadPackets()));
}

void Simulation::RunAsSteps(int time) {
  //Simulation starts at clock = 0
  clock_ = 0;
  std::cout << " >>> Started simulation with method M4 - Process interaction <<< \n";
  auto cmp = [](Process *left, Process *right) { return left->GetTime() > right->GetTime(); }; //Lambda for agenda
  Process::Agenda agenda(cmp);
  for (int i = 1; i < (wireless_network_->kDeviceCount_+1); i++) {
    auto new_packet = new Packet(i, wireless_network_, clock_, &agenda, wireless_network_->logger);
    new_packet->Activate(0);
  }
  while (clock_ < static_cast<size_t>(time))
  {
    Process* process = agenda.top();
    agenda.pop();
    clock_ = process->GetTime();
    std::cout << "Simulation Time : " << clock_ << "\n";
    std::cout << "Press ENTER to continue....";
    //------- IMPLEMENTATION OF STEP RUNNING-------------
    getchar();
    process->Execute();
    if (process->IsTerminated())
      delete process;
  }
  std::cout << "Successfully transmitted packets : " + std::to_string(wireless_network_->GetSuccessPackets()) << std::endl;
  std::cout << "Packets which transmission has been failed : " + std::to_string(wireless_network_->GetDeadPackets()) << std::endl;
}



