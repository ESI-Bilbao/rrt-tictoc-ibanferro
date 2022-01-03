/*
 * retransmisor.cc
 *
 *  Created on: Jan 2, 2022
 *      Author: ?
 */





#include <stdio.h>
#include <string.h>
#include <omnetpp.h>


using namespace omnetpp;



class Retransmisor : public cSimpleModule
{

  protected:
    virtual void initialize() override;
    virtual void sendCopyOfToNode(cPacket *pkt);
    virtual void sendCopyOfToOther(cPacket *pkt);
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Retransmisor);

void Retransmisor::initialize()
{

}

void Retransmisor::handleMessage(cMessage *msg)
{
    cPacket *pkt=check_and_cast<cPacket *>(msg);

        EV << "Retransmisor\n";

        cGate *arrivalGate = msg->getArrivalGate();
        if (arrivalGate == gate("in1")) {
            // do stuff
            EV << "Mandando paquete al Otro Nodo\n";

            sendCopyOfToOther(pkt);
        } else {
            // or y'know
            EV << "Mandando paquete al Repartidor\n";

            sendCopyOfToNode(pkt);
        }


}


void Retransmisor::sendCopyOfToNode(cPacket *pkt)
{

    cPacket *copy = (cPacket *)pkt->dup();

    send(copy, "out2"); //Al propio nodo
}


void Retransmisor::sendCopyOfToOther(cPacket *pkt)
{

    cPacket *copy = (cPacket *)pkt->dup();

    send(copy, "out1"); //AL de otro conmutador
}









