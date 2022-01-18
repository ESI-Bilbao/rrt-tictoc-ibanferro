/*
 * transceptor.cc
 *
 *  Created on: Dec 22, 2021
 *      Author: ?
 */





#include <stdio.h>
#include <string.h>
#include <omnetpp.h>


using namespace omnetpp;
using namespace std;

/**
 * In the previous model we just created another packet if we needed to
 * retransmit. This is OK because the packet didn't contain much, but
 * in real life it's usually more practical to keep a copy of the original
 * packet so that we can re-send it without the need to build it again.
 */
class Transceptor : public cSimpleModule
{
  private:
    simtime_t timeout;  // timeout
    cMessage *timeoutEvent;  // holds pointer to the timeout self-message
    int seq;  // message sequence number
    cPacket *packet;  // message that has to be re-sent on timeout

    cQueue *colaPaquetes; //Se almacenan los paquetes recibidos desde el Generador, que hay que enviar al Receptor
    cQueue *colaEnviando;   /// Se almacenan los paquetes que se han Enviado pero cuyo ACK aun no se ha recibido
    cQueue *timePaquetes;

    cPacket *nack_packet;   //Paquete que se envia para mandar un NACK
    cPacket *ack_packet;    //Paquete que se envia para mandar un ACK

    cPacket *time_packet;

    char pktname[20];
    string timeString;

    double probACKLost;
    double probNACKLost;
    double probPacketLost;


    simsignal_t sentSignal;
    simsignal_t rcvdSignal;

    simsignal_t bufferSignal;
    simsignal_t timeSignal;

    long numSent;
    long numRcvd;

    long bufferLength;
    double timePacket;

    simtime_t currentTime;

  public:
    Transceptor();
    virtual ~Transceptor();

  protected:
    virtual cPacket *generateNewPacket();
    virtual void sendCopyOf(cPacket *pkt);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Transceptor);

Transceptor::Transceptor()
{
    timeoutEvent = nullptr;
    packet = nullptr;
}

Transceptor::~Transceptor()
{
    cancelAndDelete(timeoutEvent);
    delete packet;
}

void Transceptor::initialize()
{
    // Initialize variables.

    //EMISOR
    seq = 0;
    timeout = 1.0;
    timeoutEvent = new cMessage("timeoutEvent");

    colaPaquetes = new cQueue("colaPaquetes");
    colaEnviando = new cQueue("colaEnviando");


    timePaquetes = new cQueue("timePaquetes");

    EV << "Inicializando el Transceptor\n";
    probACKLost = par("probACKLost").doubleValue();
    probNACKLost = par("probNACKLost").doubleValue();

    //RECEPTOR

    probPacketLost = par("probPacketLost").doubleValue();

    sentSignal = registerSignal("sentSignal");
    rcvdSignal = registerSignal("rcvdSignal");
    bufferSignal = registerSignal("bufferSignal");
    timeSignal = registerSignal("timeSignal");

    numSent = 0;
    numRcvd = 0;
    bufferLength = 0;
    timePacket = 0;


}

void Transceptor::handleMessage(cMessage *msg)
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
            if (uniform(0, 1) < probNACKLost )
            {
                EV << "\"Losing\" NACK message " << msg << endl;
                bubble("NACK lost");
                delete msg;

            } else{

                if(check_and_cast<cPacket *>(msg)->hasBitError())
                {
                    EV << "NACK message HAS BIT ERROR";
                    delete msg;

                }else{

                    EV << " NACK Received: " << msg->getName() << "\n";

                    // Also delete the stored message and cancel the timeout event.
                    EV << "Timer cancelled.\n";
                    cancelEvent(timeoutEvent);

                    sendCopyOf(packet);

                    cancelEvent(timeoutEvent);
                    scheduleAt(simTime()+timeout, timeoutEvent);
                }

            }

        }else if( strcmp( msg->getName() , "ack" ) == 0)
        {
            if ( uniform(0, 1) < probACKLost ) // Simula el caso de que el mensaje se haya perdido por el camino
            {
                EV << "\"Losing\" ACK message " << msg << endl;
                bubble("ACK lost");
                delete msg;

            } else{



                if(check_and_cast<cPacket *>(msg)->hasBitError())
                {
                    EV << "NACK message HAS BIT ERROR";
                    delete msg;

                }else{

                    currentTime = simTime();

                    time_packet =  check_and_cast<cPacket *>( timePaquetes->get(0) ) ;
                    timeString = string(time_packet->getName());

                    timePacket = ( stod( simTime().str() ) ) - stod(timeString);

                    EV << "TimePacket "<< timePacket << "\n";

                    numSent++;
                    EV << "AUmentamos numero de mensajes enviados"<< numSent <<"\n";

                    // send a signal
                    emit(sentSignal, numSent);
                    EV << "TimeString packet WAS INSERTED IN "<< timeString << "\n";

                    emit(timeSignal, timePacket);

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

            }

        }else{

            EV << msg->getName();

            cGate* arrivalGate = msg->getArrivalGate();

            //if( strncmp( msg->getName() , "GEN" ,3 ) == 0)
            if( arrivalGate == gate("inPacket") )
            {

                currentTime = simTime();

                EV << "Ha llegado un NUEVO PAQUETE AL Transceptor desde el INJECTOR\n";
                EV << "Arrival Time of the Packet is "<< currentTime;

                numRcvd++;

                cPacket *pkt = check_and_cast<cPacket *>( msg );

                EV <<  "\n"<< pkt->getArrivalModule()->getParentModule()->getName() <<"AAA\n";
                EV <<  "\n"<< pkt->getArrivalModule()->getParentModule()->getId()<<"AAA\n";
                EV << "Aumentamos numero de mensajes Recibidos NUMRCVD "<< numRcvd <<"\n";

                // receive a signal
                emit(rcvdSignal, numRcvd);


                colaPaquetes->insert( check_and_cast<cObject *> (pkt) );
                EV << "Paquete insertado en la cola colaPaquetes\n";

                timeString = currentTime.str() ;

                time_packet = new cPacket(timeString.data());

                timePaquetes->insert( check_and_cast<cObject *> (time_packet) );

                bufferLength = (long)colaPaquetes->getLength();
                //bufferLength++;
                EV << "Aumentamos Longitud de la COLA DE PAQUETES"<< bufferLength <<"\n";

                // receive a signal
                emit(bufferSignal, bufferLength);

                timeString = currentTime.str();

                EV << "TimeString "<< timeString << "\n";

                if(colaPaquetes->getLength() == 1 && colaEnviando->isEmpty() )
                {

                    packet = generateNewPacket();
                    colaEnviando->insert( check_and_cast<cObject *> (packet) );
                    sendCopyOf(packet);

                    cancelEvent(timeoutEvent);
                    scheduleAt(simTime()+timeout, timeoutEvent);

                }
            }else{


                cPacket *pktRec=check_and_cast<cPacket *>(msg);

                if (uniform(0, 1) < probPacketLost ) // Simula el caso de que el mensaje se haya perdido por el camino
                {
                    EV << "\"Losing\" message " << msg << endl;
                    bubble("Packet lost in Channel");
                    delete msg;
                }
                else { // Simula el caso en el que el mensaje ha llegado a su destino

                    EV << "RECEPTOR\n";
                    EV << "En el RECEPTOR se ha RECIBIDO el PAQUETE "+std::to_string(pktRec->getBitLength())+"\n";

                    if (pktRec->hasBitError()) //Comprueba si el ,ensaje que ha llegado tiene bits ERRONEOS
                    {
                        EV << msg << " received with ERROR, sending back an NACK message.\n";
                        delete msg;

                        nack_packet = new cPacket("nack");
                        nack_packet->setBitLength(48); //Los Bits de un mensaje NACK/ACK
                        send( nack_packet, "out"); //Se envia un NACK
                    }
                    else{   // Simula el caso en que el mensaje ha llegado y NO tiene ERRORES
                        EV << msg << " received, sending back an ACK.\n";
                        send( msg, "outFinal"); //Se envia un ACK


                        ack_packet = new cPacket("ack");
                        ack_packet->setBitLength(48); //Los Bits de un mensaje NACK/ACK
                        send( ack_packet, "out"); //Se envia un ACK

                        //delete msg;
                    }
                }

           }

        }
    }
}

cPacket *Transceptor::generateNewPacket()
{
    // Generate a message with a different name every time.

    cPacket *pkt = check_and_cast<cPacket *>( colaPaquetes->get(0));
    EV << "Vamos a REMOVER pkt de colaPaquetes\n";

    colaPaquetes->remove( check_and_cast<cObject *> (pkt) );


    bufferLength = (long)colaPaquetes->getLength();
    //bufferLength--;
    EV << "Reducimos Longitud de la COLA DE PAQUETES"<< bufferLength <<"\n";

    // receive a signal
    emit(bufferSignal, bufferLength);

    return pkt;
}

void Transceptor::sendCopyOf(cPacket *pkt)
{
    // Duplicate packet and send the copy.
    cPacket *copy = (cPacket *)pkt->dup();
    send(copy, "out");
}


