#include "wireless_network.h"
#include <chrono>
#include <thread>

int main(char** args[]) 
{
  WirelessNetwork* wnet = new WirelessNetwork();
  while (true)
  {
    wnet->GeneratePacket(1,wnet);
    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(3));
    wnet->GeneratePacket(2, wnet);
    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(3));
    wnet->GeneratePacket(3, wnet);
    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(3));
    wnet->WaitCGPk();
  }
  return 0;
}