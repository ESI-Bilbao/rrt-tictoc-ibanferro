/*
 * repartidor.cc
 *
 *  Created on: Jan 2, 2022
 *      Author: ?
 */




#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>


using namespace omnetpp;
using namespace std;


class Repartidor : public cSimpleModule
{
  private:

    int maxPuertas;
    int puertaOut;
    int seq;  // message sequence number

    double numRandom;
    char pktname[20];
    char outputName[20];

    string probs_arriba_str;
    string probs_abajo_str;
    string destinos_str;

    int probs_arriba_num[2];
    int probs_abajo_num[2];
    int destinos_num[2];

    float probs[2];

  protected:
    virtual void initialize() override;
    virtual void sendCopyOf(cPacket *pkt);
    virtual void convertStrtoArr(string str, int* arr);
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Repartidor);

void Repartidor::initialize()
{

    seq = 1;
    maxPuertas = par("maxPuertas").intValue();

    probs_arriba_str = par("probs_arriba_str").stringValue();
    probs_abajo_str = par("probs_abajo_str").stringValue();
    destinos_str = par("destinos_str").stringValue();

    EV << "INICIALIZANDO el REPARTIIDOR\n";

    EV << "El valor de probs_arriba_str es "<< probs_arriba_str<<"\n";
    convertStrtoArr(probs_arriba_str, probs_arriba_num );
    EV << "El valor de probs_arriba_num es "<< probs_arriba_num[0]<<"\n";

    EV << "El valor de probs_abajo_str es "<< probs_abajo_str<<"\n";
    convertStrtoArr(probs_abajo_str, probs_abajo_num );
    EV << "El valor de probs_abajo_num es "<< probs_abajo_num[0]<<"\n";

    EV << "El valor de destinos_str es "<< destinos_str<<"\n";
    convertStrtoArr(destinos_str, destinos_num );
    EV << "El valor de destinos_num es "<< destinos_num[0]<<"\n";

    int len = sizeof(probs)/sizeof(int);

    for ( int i = 0; i < len ; i++)
    {
        float probActual =  ( (float) probs_arriba_num[i]) / ( (float) probs_abajo_num[i]);

        EV << "Una probabilidad ARRIBA: " << probs_arriba_num[i]  << endl;
        EV << "Una probabilidad ABAJO: " << probs_abajo_num[i]  << endl;
        EV << "Una probabilidad: " << probActual << endl;

        probs[i] = probActual;

    }

    EV << " PROBABILIDADES "<< probs[0]<<"\n";
    EV << "PROBABILIDADES SIZE LENGTH "<< sizeof(probs)/sizeof(int) <<"\n";


}

void Repartidor::handleMessage(cMessage *msg)
{
    cPacket *pkt=check_and_cast<cPacket *>(msg);


        EV << "Repartidor\n";
        EV << "PRUEBA DE A VER SI SE LEE EL PARAMETRO STRING\n";
        EV << "El valor de probs_arriba_str es "<< probs_abajo_str<<"\n";

        EV << "El valor de probs_arriba_num es "<< probs_abajo_num[0]<<"\n";

        if( strncmp( pkt->getName() , "GEN" ,3 ) == 0)
        {
            EV << "Ha llegado un NUEVO PAQUETE AL Transceptor desde el INJECTOR\n";

            sprintf(pktname, "tic-%d", ++seq);
            pkt->setName( pktname );


        }else{

            EV << "Ha llegado un NUEVO PAQUETE AL Transceptor desde otro Conmutador\n";

        }

        sendCopyOf(pkt);

}


void Repartidor::sendCopyOf(cPacket *pkt)
{
    int maxPuertas = gateSize("out");
    float numeroAleatorio = uniform(0, 1);

    int len = sizeof(probs)/sizeof(int);

    int cont;
    float probFloat = (float) probs[0];
    float sumatorioProbs;

    if(numeroAleatorio < probFloat)
    {
        puertaOut = destinos_num[0];
    }else{

        puertaOut = destinos_num[1];

    }

    EV << "PUERTA OUT " << puertaOut << "\n";


    cMessage* msg=check_and_cast<cMessage *>(pkt);

    cGate* arrivalGate = msg->getArrivalGate();

    if( arrivalGate->getVectorSize() > 0 )
    {
        int arrivalGateIndex = arrivalGate->getIndex();

        if(arrivalGateIndex == -1)
        {
            EV << "Error in the ARRIVAL GATE INDEX\n";
        }

        while(puertaOut == arrivalGateIndex)
        {
            numeroAleatorio = uniform(0, 1);

            if(numeroAleatorio < probFloat)
            {
                puertaOut = destinos_num[0];
            }else{

                puertaOut = destinos_num[1];

            }

            EV << "PUERTA OUT " << puertaOut << "\n";

        }

        EV << "Forwarding message " << msg << " on port out[" << puertaOut << "]\n";
        send(msg, "out", puertaOut);
    }else{
        EV << "The output gates vector is 0 or -1";
    }
}


// Function to convert a string to
// integer array
void Repartidor::convertStrtoArr(string str, int* arr)
{

    int contador = 0;
    std::istringstream ss(str);

    std::string token;

    while(std::getline(ss, token, ',')) {
        EV<< "TOKEN "<< token << '\n';
        EV<< "TOKEN NUMBER "<< stoi(token) << '\n';

        arr[contador] = stoi(token);
        EV<< "ARR CONTADOR NUMBER "<< arr[contador]<< '\n';
        contador++;
    }


}
