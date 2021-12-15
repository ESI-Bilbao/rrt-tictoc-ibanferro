/*
 * receptor.cc
 *
 *  Created on: Dec 11, 2021
 *      Author: ?
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

/**
 * In the previous model we just created another packet if we needed to
 * retransmit. This is OK because the packet didn't contain much, but
 * in real life it's usually more practical to keep a copy of the original
 * packet so that we can re-send it without the need to build it again.
 */

/**
 * Sends back an acknowledgement -- or not.
 */

class Receptor : public cSimpleModule
{
  private:
    cPacket *nack_packet;   //Paquete que se envia para mandar un NACK
    cPacket *ack_packet;    //Paquete que se envia para mandar un ACK

  protected:
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Receptor);

void Receptor::handleMessage(cMessage *msg)
{
    cPacket *pkt=check_and_cast<cPacket *>(msg);

    if (uniform(0, 1) < 0.1) // Simula el caso de que el mensaje se haya perdido por el camino
    {
        EV << "\"Losing\" message " << msg << endl;
        bubble("message lost");
        delete msg;
    }
    else { // Simula el caso en el que el mensaje ha llegado a su destino

        EV << "RECEPTOR\n";
        EV << "En el RECEPTOR se ha RECIBIDO el PAQUETE "+std::to_string(pkt->getBitLength())+"\n";

        if (pkt[0].hasBitError()) //Comprueba si el ,ensaje que ha llegado tiene bits ERRONEOS
        {
            EV << msg << " received with ERROR, sending back an NACK message.\n";
            delete msg;

            nack_packet = new cPacket("nack");
            nack_packet->setBitLength(48); //Los Bits de un mensaje NACK/ACK
            send( nack_packet, "out"); //Se envia un NACK
        }
        else{   // Simula el caso en que el mensaje ha llegado y NO tiene ERRORES
            EV << msg << " received, sending back an ACK.\n";
            delete msg;

            ack_packet = new cPacket("ack");
            ack_packet->setBitLength(48); //Los Bits de un mensaje NACK/ACK
            send( ack_packet, "out"); //Se envia un ACK

        }
    }
}
