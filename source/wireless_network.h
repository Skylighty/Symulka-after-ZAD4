#ifndef WIRELESS_NETWORK_H_
#define WIRELESS_NETWORK_H_

#include <vector>
#include "tx.h"
#include "channel.h"
#include "rx.h"
#include "logger.h"

class WirelessNetwork {
public:
    WirelessNetwork(double lambda, std::vector<std::queue<int>> &svector);

    ~WirelessNetwork();

    //void Init();                                              //Method that prepares
    void
    GenerateRXTX();                                           //Method that creates RXs and TXs specified by constant kDeviceCount attribute
    uint32_t GetPacketCount() { return this->packet_count; }  //Returns total count of packets transmitted by system
    void IncPacketCount() { ++packet_count;}
    void IncSuccessPacket(){++successful_packets_;}
    void IncDeadPacket(){++dead_packets_;}
    uint32_t GetSuccessPackets() {return successful_packets_;}
    uint32_t GetDeadPackets() {return dead_packets_;}
    TX *GetTX(int id);                                        //Returns TX of particular device ID
    RX *GetRX(int id);                                        //Same as above, but for RX
    double GetLambda() {return lambda_;}
    Channel *channel_;                                        //Channel declaration (didn't want to make it private, it's much simpler and comfy this way actually)
    int const kMaxRetransmissions = 10;                       //Const representing maximal retransmission number
    int const kDeviceCount_ = 4;                              //Const representing number of RX/TX pairs
    Logger* logger;
private:
    double lambda_;
    std::vector<std::queue<int>> seed_vector_;
    uint32_t packet_count;                                    //Variable that stores total packet count transmitted in system
    uint32_t successful_packets_;
    uint32_t dead_packets_;
    std::vector<TX *> *TXList;                                 //Vector container of all TXs
    std::vector<RX *> *RXList;                                 //Vector container of all RXs

};

#endif //WIRELESS_NETWORK_H_
