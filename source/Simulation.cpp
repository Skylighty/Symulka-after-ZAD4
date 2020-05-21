#include "wireless_network.h"
#include "Simulation.h"
#include "process.h"
#include "packet.h"
#include <iostream>

Simulation::Simulation(WirelessNetwork *wireless_network, size_t startingtime) {
    wireless_network_ = wireless_network;
    starting_time_ = startingtime;
}

//Function that returns packets per seconds in system.
double Simulation::GetBitrate()
{
  return wireless_network_->GetSuccessPackets()/(clock_/1000);
}

void Simulation::DisplayStatistics()
{
  wireless_network_->logger->Info("------------------------------------------------------------------------------\n");
  for (int i = 0; i < wireless_network_->kDeviceCount_; i++)
  {
    wireless_network_->logger->Info("TX of ID : " + std::to_string(wireless_network_->GetTX(i+1)->GetTXSuccessPackets()) + " success\n");
    wireless_network_->logger->Info("TX of ID : " + std::to_string(wireless_network_->GetTX(i+1)->GetTXRejectedPackets()) + " rejected\n");
  }
  wireless_network_->logger->Info("Total success : " + std::to_string(wireless_network_->GetSuccessPackets()) + "\n");
  wireless_network_->logger->Info("Total rejected : " + std::to_string(wireless_network_->GetRejectedPackets()) + "\n");
  wireless_network_->logger->Info("------------------------------------------------------------------------------\n");
  wireless_network_->logger->Info("Statistics:\n");
  wireless_network_->logger->Info("Srednia usredniona stopa bledu  = " + std::to_string(wireless_network_->GetAverageBER()) + "\n");
  wireless_network_->logger->Info("Maksymalna stopa bledu = " + std::to_string(wireless_network_->GetMaxBER()) + "\n");
  wireless_network_->logger->Info("Srednia liczba retransmisji pakietow = " + std::to_string(wireless_network_->GetAverageRetransmissions()) + "\n");
  wireless_network_->logger->Info("Przeplywnosc systemu = " + std::to_string(GetBitrate()) + "p/sek.\n");
  wireless_network_->logger->Info("Srednie opoznienie pakietu = " + std::to_string(wireless_network_->GetAverageDelay()) + "ms\n");
  wireless_network_->logger->Info("Sredni czas oczekiwanie na opuszczenie bufora = " + std::to_string(wireless_network_->GetAverageBufforLeaveTime()) + "ms\n");
}

void Simulation::Run(int time)
{
  //Simulation starts at clock = 0
  clock_ = 0;
    std::cout << " >>> Started simulation with method M4 - Process interaction <<< \n";
    
  //LAMBDA Function declaration for segregation of priority queue - our agenda
  auto cmp = [](Process *left, Process *right) { return left->GetTime() > right->GetTime(); };
  
  //Agenda initialization
  Packet::Agenda agenda(cmp);
  
  //Generating start packet for every 1 of RXTX pairs
  for (int i = 1; i < (wireless_network_->kDeviceCount_+1); ++i)
  {
    auto new_packet = new Packet(i, wireless_network_, clock_, &agenda, wireless_network_->logger);
    new_packet->Activate(0);
  }
  
  
  //=======================================================
  //====================- MAIN LOOP -======================
  //=======================================================
  //Loop basically runs untill simulation time is lesser than the time we want to simulte, conversion to size_t btw.
  while (clock_ < static_cast<size_t>(time))
  {
    //Check if starting phase is ended
    if (clock_ > starting_time_)
    {
      wireless_network_->SetEndOfStartingPhase(true);
    }
      
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
    //=======================================================
    //=====================- END LOOP -======================
    //=======================================================
  
    //Display statistics of simulation.
    DisplayStatistics();
}



