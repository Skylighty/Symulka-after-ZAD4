#ifndef RX_H_
#define RX_H_

#include <cstdint>
#include "packet.h"

class WirelessNetwork;

class RX {
public:
    RX(int id);

    ~RX();

    uint32_t
    GetRXID() { return rx_id_; };                          //Method allowing us to get the RX's very own ID
    void SetRXPacket(
            Packet *packet) { current_packet_ = packet; }; //Method to set currently transmitted packet as currently serviced by RX
private:
    uint32_t rx_id_;    //RX very own ID
    Packet *current_packet_;  //Packet, currently serviced by RX;
};

#endif //RX_H_