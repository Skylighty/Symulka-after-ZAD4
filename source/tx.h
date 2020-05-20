#ifndef TX_H_
#define TX_H_

#include <cstdint>
#include <queue>
#include "packet.h"
#include "generator.h"

class WirelessNetwork;

class TX {
public:
    TX(int id, std::queue<int> &sq);

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
    void SetNetwork(WirelessNetwork *network) {network_ = network;}
    void SetSeedQueue(std::queue<int> seed_queue) {seeds = seed_queue;}
    Packet *GetCurrentPacket() { return current_packet_; }
    int GetSeed();
    void InitializeGenerators();
    Generator* GeneratorCP;
    Generator* GeneratorCTP;
    Generator* GeneratorCRP;
    Generator* GeneratorCGP;
    Generator* GeneratorR;
    Generator* GeneratorT;
    Generator* GeneratorTER;
private:
    WirelessNetwork *network_;
    std::queue<int> seeds;
    std::queue<Packet *> *buffer_;               //Packet buffer declaration
    int tx_id_;                            //Very own TX's ID
    Packet *current_packet_;                    //Currently serviced packet
};

#endif //TX_H_
