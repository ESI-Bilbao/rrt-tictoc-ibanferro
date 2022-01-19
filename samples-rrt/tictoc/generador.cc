/*
 * generador.cc
 *
 *  Created on: Dec 12, 2021
 *      Author: ?
 */




#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "ibanPaquete_m.h"

using namespace omnetpp;
using namespace std;

/**
 * In the previous model we just created another packet if we needed to
 * retransmit. This is OK because the packet didn't contain much, but
 * in real life it's usually more practical to keep a copy of the original
 * packet so that we can re-send it without the need to build it again.
 */
class Generador : public cSimpleModule
{
  private:
    simtime_t timeout;  // timeout
    cMessage *timeoutEvent;  // holds pointer to the timeout self-message
    int seq;  // message sequence number
    cPacket *packet;  // message that has to be re-sent on timeout

    cExponential *longExponential;
    cExponential *timeExponential;


    int lonMedia = 512;
    //float timeMedia = 0.5;
    double lambda;

  public:
    Generador();
    virtual ~Generador();

  protected:
    virtual cPacket *generateNewPacket();
    virtual void sendCopyOf(cPacket *pkt);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Generador);

Generador::Generador()
{
    timeoutEvent = nullptr;
    packet = nullptr;
}

Generador::~Generador()
{
    cancelAndDelete(timeoutEvent);
    delete packet;
}

void Generador::initialize()
{
    // Initialize variables.
    seq = 0;
    timeout = 1.0;
    timeoutEvent = new cMessage("timeoutEvent");

    longExponential = new cExponential();
    timeExponential = new cExponential();

    lambda = par("lambda").doubleValue();

    cPacket *packet = generateNewPacket();

    int longitud = lonMedia*longExponential->draw();
    float tiempo = (1/lambda) *longExponential->draw();

    EV << "Longitud del PAQUETE "+ std::to_string(longitud)  +"\n";
    EV << "Tiempo del PAQUETE "+ std::to_string(tiempo)  +"\n";

    packet->setBitLength(longitud);

    scheduleAt(simTime() + tiempo, packet);
}

void Generador::handleMessage(cMessage *msg)
{
   // cPacket *pkt=check_and_cast<cPacket *>(msg);

    cPacket *packet = generateNewPacket();

    int longitud = lonMedia*longExponential->draw();
    float tiempo = (1/lambda) *longExponential->draw();



    EV << "Longitud del PAQUETE "+ std::to_string(longitud)  +"\n";
    EV << "Tiempo del PAQUETE "+ std::to_string(tiempo)  +"\n";

    packet->setBitLength(longitud);

    sendCopyOf(packet);
    scheduleAt(simTime() + tiempo, packet);

}
string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}
cPacket *Generador::generateNewPacket()
{
    // Generate a message with a different name every time.

    char pktname[20];

    string strModuleName;

    strModuleName.assign(getName());

    string strModuleNum = strModuleName.substr( strModuleName.length() - 1 );

    EV << "GENERADOR NUMERO " << strModuleNum << endl;

    sprintf(pktname, "GEN-%d-tic-%d", stoi(strModuleNum) , ++seq);

    IbanPaquete *ibpkt = new IbanPaquete(pktname,0);

    ibpkt->setInitTime( (stod( simTime().str()) ) );

    ibpkt->setBitLength(3200);
    ibpkt->setSrc( stoi(strModuleNum) );

    EV << " GENERADOR Initial Time" << ibpkt->getInitTime() << endl;

    cPacket *pkt = check_and_cast<cPacket *>(ibpkt);



    return pkt;
}

void Generador::sendCopyOf(cPacket *pkt)
{
    // Duplicate packet and send the copy.
    cPacket *copy = (cPacket *)pkt->dup();
    send(copy, "out");
}



