#ifndef PACKET_H_
#define PACKET_H_
#include "process.h"
#include "logger.h"

class Channel;
class WirelessNetwork;

class Packet : public Process
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
  void WaitCRP();
  void WaitCP();
  void WaitCTP();
  void WaitCTIZ();
  void Execute() override;
  void Activate(double time) override;
  bool CheckForErrors(Channel* channel);
  bool CheckForCollision(Channel* channel);

private:
  WirelessNetwork* network_;
  Logger* logger_;
  uint32_t packet_id_;
  uint32_t devices_id_;
  uint32_t r_;
  uint32_t t_;
  double T_;
  double R_;
  State state_;
  bool error_;
  bool ack_;
  bool active_;
  bool collision_;
  bool retransmission_;
};

#endif //PACKET_H_
