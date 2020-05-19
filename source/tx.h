#ifndef TX_H_
#define TX_H_

#include <cstdint>
#include <queue>
#include "packet.h"
#include "generator.h"

class WirelessNetwork;

class TX {
public:
    TX(int id, std::queue<int>* slist);

    ~TX();

    void PushToBuffer(Packet *packet) {
        this->buffer_->push(packet);
    }      //Method that allows to push packet to TX's buffer
    void
    PopFromBuffer() { buffer_->pop(); }                          //Method that allows to remove longest waiting packet from buffer
    Packet *
    PacketFromBuffer() { return buffer_->front(); }           //Method that gives us the access to longest waiting packet in buffer
    void SetTXPacket(Packet *packet) { current_packet_ = packet; }    //Sets packet as currently serviced by TX
    bool BufferEmpty() { return buffer_->empty(); }                   //Returns whether buffer is empty or not
    int GetTXID() { return tx_id_; }                             //Returns very own TX's ID
    int GetSeed();
    Packet *GetCurrentPacket() { return current_packet_; }
    Generator *Tgenerator;          //Random generator for T variable
    Generator *Rgenerator;          //Random generator for R variable
    Generator *CPgenerator;         //Random generator for CP time
    Generator *CRPgenerator;        //Random generator for CRP time
    Generator *CGPgenerator;        //Random generator for CGP time
    Generator *CTPgenerator;        //Random generator for CTP time
private:
    std::queue<int>* seed_list_ = nullptr;
    std::queue<Packet *> *buffer_;               //Packet buffer declaration
    int tx_id_;                            //Very own TX's ID
    Packet *current_packet_;                    //Currently serviced packet
};

#endif //TX_H_
