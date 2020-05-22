#ifndef WIRELESS_NETWORK_H_
#define WIRELESS_NETWORK_H_

#include <vector>
#include <fstream>
#include <string>
#include "tx.h"
#include "channel.h"
#include "rx.h"
#include "logger.h"


class WirelessNetwork {
public:
    WirelessNetwork(double lambda, std::queue<int> &squeue);

    ~WirelessNetwork();
    
    void
    GenerateRXTX();                                             //Method that creates RXs and TXs specified by constant kDeviceCount attribute
    uint32_t GetPacketCount() { return this->packet_count; }    //Returns total count of packets transmitted by system
    
    //Statistic implementation-connected methods.
    //---------------------------------------------------------------------------
    void IncPacketCount() { ++packet_count;}
    void IncSuccessPacket() {++successful_packets_;}
    void IncRejectedPackets() {++rejected_packets_;}
    void IncRetransmissions(int retransmissions) {retransmissions_ += retransmissions;}
    void IncBufferTime(size_t leave_time) {total_buffor_leave_delay_+=leave_time;}
    void IncTotalDelay(size_t time) {total_success_delay_ += time;}
    void SetEndOfStartingPhase(bool state) {starting_phase_end_ = state;}
    double GetAverageRetransmissions();                                               //Returns average retransmissions per packet
    double GetAverageDelay();                                                         //Returns average delay time of packet
    double GetAverageBufforLeaveTime();                                               //Returns average buffer leave time of packet
    double GetAverageBER();                                                           //Returns average BER for all TXs
    double GetMaxBER();                                                               //Returns highest BER of all TXs
    uint32_t GetRejectedPackets() {return rejected_packets_;}                         //Returns all rejected packets
    uint32_t GetSuccessPackets() {return successful_packets_;}                        //Returns all successful packets
    bool CheckIfStartingPhaseEnded() {return starting_phase_end_;}                    //Returns whether starting phase is ended
    //--------------------------------------------------------------------------
    
    //In time statistic-related methods - makes us able to make a time graphs of simulation
    //---------------------------------------------------------------------------
    void AddAverageRetransmissionRatio();
    void AddAverageBERRatio();
    void OutputRetransmissionToFile();
    void OutputAverageBERToFile();
    //---------------------------------------------------------------------------
    
    TX *GetTX(int id);                                        //Returns TX of particular device ID
    RX *GetRX(int id);                                        //Same as above, but for RX
    Channel *channel_;                                        //Channel declaration (didn't want to make it private, it's much simpler and comfy this way actually)
    Logger* logger;                                           //Public logger for all kinds of messages
    double GetLambda() {return lambda_;}                      //Return lambda coefficient
    int const kMaxRetransmissions = 10;                       //Const representing maximal retransmission number
    int const kDeviceCount_ = 4;                              //Const representing number of RX/TX pairs
    
    int const kNecessaryGenerators = 7;
private:
    bool starting_phase_end_;                                 //Flag indicating whether starting phase has ended
    double lambda_;                                           //Lambda coefficient for CGP time exponential generation
    size_t total_buffor_leave_delay_;                         //Total buffer leavet time (for all successful packets)
    size_t total_success_delay_;                              //Total success transmission time (for all successful packets)
    uint32_t packet_count;                                    //Variable that stores total packet count transmitted in system
    uint32_t rejected_packets_;                               //Number of ALL rejected packets
    uint32_t successful_packets_;                             //Number of ALL successfully transmitted packets
    uint32_t retransmissions_;                                //Number of total retransmissions
    std::vector<double>* retransmissions_vector;              //Vector for in-time average retransmissions values
    std::vector<double>* average_ber_vector;                  //Vector for in-time average BER values
    std::vector<TX *> *TXList;                                //Vector container of all TXs
    std::vector<RX *> *RXList;                                //Vector container of all RXs
    std::queue<int> seed_queue_;                              //Queue of all seeds necessary for 10 simulations

};

#endif //WIRELESS_NETWORK_H_
