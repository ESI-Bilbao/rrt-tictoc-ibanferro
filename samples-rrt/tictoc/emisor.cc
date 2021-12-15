/*
 * emisor.cc
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
class Emisor : public cSimpleModule
{
  private:
    simtime_t timeout;  // timeout
    cMessage *timeoutEvent;  // holds pointer to the timeout self-message
    int seq;  // message sequence number
    cPacket *packet;  // message that has to be re-sent on timeout

    cQueue *colaPaquetes; //Se almacenan los paquetes recibidos desde el Generador, que hay que enviar al Receptor
    cQueue *colaEnviando;   /// Se almacenan los paquetes que se han Enviado pero cuyo ACK aun no se ha recibido

  public:
    Emisor();
    virtual ~Emisor();

  protected:
    virtual cPacket *generateNewPacket();
    virtual void sendCopyOf(cPacket *pkt);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Emisor);

Emisor::Emisor()
{
    timeoutEvent = nullptr;
    packet = nullptr;
}

Emisor::~Emisor()
{
    cancelAndDelete(timeoutEvent);
    delete packet;
}

void Emisor::initialize()
{
    // Initialize variables.
    seq = 0;
    timeout = 1.0;
    timeoutEvent = new cMessage("timeoutEvent");


    colaPaquetes = new cQueue("colaPaquetes");
    colaEnviando = new cQueue("colaEnviando");

    EV << "Inicializando el EMISOR\n";


}

void Emisor::handleMessage(cMessage *msg)
{
   // cPacket *pkt=check_and_cast<cPacket *>(msg);

    if (msg == timeoutEvent) {
        // If we receive the timeout event, that means the packet hasn't
        // arrived in time and we have to re-send it.
        EV << "Timeout expired, resending message and restarting timer\n";
        sendCopyOf(packet);

        //cancelEvent(timeoutEvent);
        scheduleAt(simTime()+timeout, timeoutEvent);
    }
    else {  // message arrived
            // Acknowledgement received!

        EV << " Response Received: " << msg->getName() << "\n";

        if( strcmp( msg->getName() , "nack" ) == 0)
        {
            if (uniform(0, 1) < 0.1) // Simula el caso de que el mensaje se haya perdido por el camino
            {
                EV << "\"Losing\" message " << msg << endl;
                bubble("NACK lost");
                delete msg;

            } else{
                EV << " NACK Received: " << msg->getName() << "\n";

                // Also delete the stored message and cancel the timeout event.
                EV << "Timer cancelled.\n";
                cancelEvent(timeoutEvent);

                sendCopyOf(packet);

                cancelEvent(timeoutEvent);
                scheduleAt(simTime()+timeout, timeoutEvent);

            }

        }else if( strcmp( msg->getName() , "ack" ) == 0)
        {
            if (uniform(0, 1) < 0.1) // Simula el caso de que el mensaje se haya perdido por el camino
            {
                EV << "\"Losing\" message " << msg << endl;
                bubble("ACK lost");
                delete msg;

            } else{

                EV << "Received: " << msg->getName() << "\n";
                delete msg;

                // Also delete the stored message and cancel the timeout event.
                EV << "Timer cancelled.\n";
                cancelEvent(timeoutEvent);

                if( colaEnviando->isEmpty() == false )
                {
                    EV << " Removing the Packet from colaEnviando \n";
                    colaEnviando->remove(  check_and_cast<cObject *> (packet)  );
                }

                delete packet;

                if( colaPaquetes->isEmpty() == false )
                {
                    // Ready to send another one.
                    packet = generateNewPacket();

                    sendCopyOf(packet);

                    cancelEvent(timeoutEvent);
                    scheduleAt(simTime()+timeout, timeoutEvent);
                }

            }

        }else{

            EV << "Ha llegado un NUEVO PAQUETE AL EMISOR\n";

            cPacket *pkt = check_and_cast<cPacket *>( msg );

            colaPaquetes->insert( check_and_cast<cObject *> (pkt) );
            EV << "Paquete insertado en la cola colaPaquetes\n";

            if(colaPaquetes->getLength() == 1 && colaEnviando->isEmpty() )
            {

                packet = generateNewPacket();
                colaEnviando->insert( check_and_cast<cObject *> (packet) );
                sendCopyOf(packet);

                cancelEvent(timeoutEvent);
                scheduleAt(simTime()+timeout, timeoutEvent);

            }

        }
    }
}

cPacket *Emisor::generateNewPacket()
{
    // Generate a message with a different name every time.

    /*
    char pktname[20];
    sprintf(pktname, "tic-%d", ++seq);
    cPacket *pkt = new cPacket(pktname,0,960);
    */

    cPacket *pkt = check_and_cast<cPacket *>( colaPaquetes->get(0));
    EV << "Vamos a REMOVER pkt de colaPaquetes\n";

    colaPaquetes->remove( check_and_cast<cObject *> (pkt) );

    /*
    char pktname[20];
    sprintf(pktname, "tic-%d", ++seq);
    cPacket *paquete = new cPacket(pktname,0,960);

    colaPaquetes->insert( check_and_cast<cObject *> (paquete) );
    */

    return pkt;
}

void Emisor::sendCopyOf(cPacket *pkt)
{
    // Duplicate packet and send the copy.
    cPacket *copy = (cPacket *)pkt->dup();
    send(copy, "out");
}
