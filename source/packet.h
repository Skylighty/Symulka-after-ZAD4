#ifndef PACKET_H_
#define PACKET_H_

#include "process.h"
#include "logger.h"


class Channel;              //Necessary includes to avoid Constructor replacement & conflict
class WirelessNetwork;

class Packet : public Process //Packet is the process so it inherits from Process
{
public:
    enum class State {
        CREATED,
        WAITING,
        SENT,
        IN_CHANNEL,
        RECEIVED,
        SENT_BACK,
        CHECK
    };

    Packet(uint32_t did, WirelessNetwork *network, size_t time, Agenda *agenda, Logger *log);

    ~Packet();

    void Execute() override;                     //Overrided excecute method for our process
    
    bool CheckForErrors();                       //Checks if packet run in some errors during transmission
    
    void GenerateNext();                         //Method that generates new packet basing on THIS one
    uint32_t GetPacketId() {return packet_id_;}
    
    //Packet time statistic methods - obligatory times.
    size_t GetDelayTime() {return success_end_time_-appearence_time_;}
    size_t GetBufforLeaveTime() {return buffor_leave_time_-appearence_time_;}
    
    //------------------------==+ STATE SPECIFIC METHODS +==--------------------------
    // I added these methods just to make the code more structured and easier to read
    
    void StateCreated();

    void StateWaiting();

    void StateSent();

    void StateInChannel();
    
    void StateReceived();

    void StateSentBack();

    void StateCheck();
    
    //-------- TIME FUNCTIONS--------------------
    
    //size_t CalculateCPTime() { return cp_time_*t_;}
    void SetCollision(bool collision) { collision_ = collision; }
    
    //----------------- GENERATORS ------------------------------
    
    int GenerateCRP();
    int GenerateCGP();
    int GenerateCP();
    int GenerateCTP();
    void GenerateT();
    void GenerateR();
    
    //--------------------------------------------------------------------------------

    
private:
    State state_;                   //Enum class object to implement the phases of process
    WirelessNetwork *network_;      //Pointer to wirelessnetwork object, necessary for whole implementation
    Logger *logger_;                //Our logger object
    uint32_t packet_id_;            //Very own ID of the packet
    uint32_t devices_id_;           //ID of devices between which transmission occurs
    uint32_t r_;                    //Retransmission counter
    uint32_t t_;                    //Channel check coutner
    uint32_t T_;                    //Random variable T value
    uint32_t R_;                    //Random variable R value
    size_t buffor_leave_time_;      //Time in which packet leaves the buffer
    size_t appearence_time_;        //Time of packet's appearence
    size_t success_end_time_;       //Time of successful end of transmission
    size_t ctp_time_;               //Value of current packet's CTP time
    bool error_;                    //Error occured flag
    bool ack_;                      //ACK flag
    bool active_;
    bool collision_;                //Collision occured flag
    bool retransmission_;           //Obligatory retransmission flag
};

#endif //PACKET_H_
