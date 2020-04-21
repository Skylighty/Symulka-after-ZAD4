#ifndef PACKET_H_
#define PACKET_H_
#include "process.h"
#include "logger.h"

class Channel;              //Necessary includes to avoid Constructor replacement 
class WirelessNetwork;

class Packet : public Process //Packet is the process so it inherits from Process
{
public:
  enum class State
  {
    CREATED,
    WAITING,
    SENT,
    IN_CHANNEL,
    RECEIVED,
    SENT_BACK,
    CHECK,
    RETRANSMISSION
  };
  Packet(uint32_t did, WirelessNetwork* network);
  ~Packet();
  void WaitCRP();                              //Waits the CRP time during retransmission
  void WaitCP();                               //Waits CP time during channel checking
  void WaitCTP();                              //Waits CTP time - time of transmission
  void WaitCTIZ();                             //Waits CTIZ time - time of ACK transmission
  void Execute() override;                     //Overrided excecute method for our process
  void Activate(double time) override;         //Overrided activate method which is 
  bool CheckForErrors(Channel* channel);       //Checks if packet run in some errors during transmission
  bool CheckForCollision(Channel* channel);    //Checks if collision occured during transmission

private:
  WirelessNetwork* network_;      //Pointer to wirelessnetwork object, necessary for whole implementation
  Logger* logger_;                //Our logger object
  uint32_t packet_id_;            //Very own ID of the packet
  uint32_t devices_id_;           //ID of devices between which transmission occurs
  uint32_t r_;                    //Retransmission counter
  uint32_t t_;                    //Channel check coutner
  double T_;                      //Random variable T value
  double R_;                      //Random variable R value
  State state_;                   //Enum class object to implement the phases of process
  bool error_;                    //Error occured flag
  bool ack_;                      //ACK flag
  bool active_;                   //Active flag to execute the process
  bool collision_;                //Collision occured flag
  bool retransmission_;           //Obligatory retransmission flag
};

#endif //PACKET_H_
