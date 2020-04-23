#include "wireless_network.h"
#include <chrono>
#include <thread>

void waiter(int time_in_secs)
{
  std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(time_in_secs));
}

int main(char** args[]) 
{
  //|=================================WARNING==================================|
  //|This was implemented just for checking if the Execute() method works fine.|
  //|==========================================================================|

  std::queue<Process*>* Agenda = new std::queue<Process*>;
  WirelessNetwork* wnet = new WirelessNetwork();
  wnet->GeneratePacket(1, wnet);
  waiter(1);
  wnet->GeneratePacket(2, wnet);
  waiter(1);
  wnet->GeneratePacket(3, wnet);
  waiter(1);
  wnet->GeneratePacket(4, wnet);
  waiter(1);
  while (true)
  {
    //After transmitting the first packet exception will occur.
    //That's because removing packets from list not implemented yet
    //So program appeals to a place in the memory that doesn't exists anymore
    
    wnet->PacketList->at(0)->Execute();
    waiter(1);
    wnet->PacketList->at(1)->Execute();
    waiter(1);
    wnet->PacketList->at(2)->Execute();
    waiter(1);
    wnet->PacketList->at(3)->Execute();
    waiter(1);
  }
  return 0;
}