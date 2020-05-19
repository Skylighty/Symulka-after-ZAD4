#ifndef PACKET_H_
#define PACKET_H_

#include "process.h"
#include "logger.h"
#include "generator.h"

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
    //TODO - Implement random variables nad RNG
    int GenerateR();                          //Generates R random variable depending on "r" counter value
    int GenerateT();                          //Generates T random variable depending on "t" channel checks counter
    int GenerateCP();
    int GenerateCTP();
    int GenerateCRP();
    static bool CheckForErrors();                       //Checks if packet run in some errors during transmission
    bool CheckForCollision();                    //Checks if collision occured during transmission
    void GenerateNext();                         //Method that generates new packet basing on THIS one
    uint32_t GetPacketId() {return packet_id_;}
    
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
    
    //--------------------------------------------------------------------------------

    
private:
    int ctp_time_;
    State state_;                   //Enum class object to implement the phases of process
    WirelessNetwork *network_;      //Pointer to wirelessnetwork object, necessary for whole implementation
    Logger *logger_;                //Our logger object
    uint32_t packet_id_;            //Very own ID of the packet
    uint32_t devices_id_;           //ID of devices between which transmission occurs
    uint32_t r_;                    //Retransmission counter
    uint32_t t_;                    //Channel check coutner
    double T_;                      //Random variable T value
    double R_;                      //Random variable R value
/*    size_t cp_time_;                //Value of current packet's CP time
    size_t crp_time_;               //Value of current packet's CRP time
    size_t ctp_time_;               //Value of current packet's CTP time
    size_t cgp_time_;               //Value of current packet's CGP time*/
    bool error_;                    //Error occured flag
    bool ack_;                      //ACK flag
    bool active_;
    bool collision_;                //Collision occured flag
    bool retransmission_;           //Obligatory retransmission flag
};

#endif //PACKET_H_
