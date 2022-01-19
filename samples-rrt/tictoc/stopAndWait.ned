//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import ned.DatarateChannel;


simple Generador
{
    parameters:
        @display("i=block/process");
        double lambda = default(2);
    gates:
        output out;
}

simple Transceptor
{
    parameters:
        @display("i=block/routing");
        
        double probACKLost = default(0.15);   // another parameter with a default value    
        double probNACKLost = default(0.5);   // another parameter with a default value
        
        double probPacketLost = default(0.5);   // another parameter with a default value
        
        @signal[sentSignal](type="long");
        @statistic[numSent](title="num sent"; source="sentSignal"; record=vector,stats; interpolationmode=none);
        
        @signal[rcvdSignal](type="long");
        @statistic[numRcvd](title="num received"; source="rcvdSignal"; record=vector,stats; interpolationmode=none);
        
        @signal[bufferSignal](type="long");
        @statistic[bufferLength](title="num received"; source="bufferSignal"; record=vector,stats; interpolationmode=none);
        
    gates:
        input inPacket;
        input in;
        output out;
        output outFinal;
}

simple Repartidor
{
 
 	parameters:
    	int maxPuertas = default(3);
    	
        int maxInterfaces = default(3);
        string probs_arriba_str = default("1,3");
        string probs_abajo_str = default("4,4");
        string destinos_str = default("1,2");
        
        @signal[sentSignal](type="long");
        @statistic[numSent](title="num sent"; source="sentSignal"; record=vector,stats; interpolationmode=none);
        
        @signal[rcvdSignal](type="long");
        @statistic[numRcvd](title="num received"; source="rcvdSignal"; record=vector,stats; interpolationmode=none);
        
        @signal[durationSignal](type="double");
        @statistic[durationNetwork](title="time in network"; source="durationSignal"; record=vector,stats; interpolationmode=none);

        @signal[hopSignal](type="long");
        @statistic[hopNum](title="hop number in network"; source="hopSignal"; record=vector,stats; interpolationmode=none);
        
        @signal[sourceUNOSignal](type="double");
        @statistic[sourceUNOnum](title="packets with SOURCE GEN 1"; source="sourceUNOSignal"; record=vector,stats; interpolationmode=none);
         
        @signal[sourceDOSSignal](type="double");
        @statistic[sourceDOSnum](title="packets with SOURCE GEN 2"; source="sourceDOSSignal"; record=vector,stats; interpolationmode=none);
          
        @signal[sourceTRESSignal](type="double");
        @statistic[sourceTRESnum](title="packets with SOURCE GEN 3"; source="sourceTRESSignal"; record=vector,stats; interpolationmode=none);
         
                
    gates:
        input in[3];
        output out[3];
        input inPacket;
     
}

simple Retransmisor
{
     parameters:
        @display("i=block/routing");

    gates:
        input in1;
        output out1; //Al de otro conmutador
        input in2;
        output out2;//Al de conmutador propio
}

module Conmutador
{
	
    gates:
		input in[3];
		output out[3]; 
		input inPacket;
    submodules:
        repartidor: Repartidor{
            
        }
        retransmisor[3]: Retransmisor;
    connections allowunconnected:
        
        in[1] --> retransmisor[1].in2;
        in[2] --> retransmisor[0].in2;
        
        retransmisor[1].out2 --> repartidor.in[1];
        retransmisor[0].out2 --> repartidor.in[2];
        
        repartidor.out[1] --> retransmisor[1].in1;
        repartidor.out[2] --> retransmisor[0].in1;
     
     	retransmisor[1].out1 --> out[1];
     	retransmisor[0].out1 --> out[2];
     	
     	inPacket --> repartidor.inPacket;
}

module ProtocolLink
{
 
 	parameters:
 	    
        @signal[timeSignal](type="double");
        @statistic[durationLink](title="time in network"; source="timeSignal"; record=vector,stats; interpolationmode=none);    
          
 	gates:

 	    input inPacket1;
 	    output outPacket1;
 	    
 	    input inPacket2;
 	    output outPacket2;

    submodules:
        
        transceptorEmisor: Transceptor {
            parameters:
                @display("i=,cyan;p=152.8,60.48");
        }
        transceptorReceptor: Transceptor {
            parameters:
                @display("i=,gold;p=352.8,60.48");
        }
    connections:

        inPacket1 --> transceptorEmisor.inPacket;
        inPacket2 --> transceptorReceptor.inPacket;
        
        transceptorEmisor.out --> DatarateChannel {  datarate = default( 9600000000bps); per = default(0.001); } --> transceptorReceptor.in;
        transceptorReceptor.out --> DatarateChannel {  datarate = default( 9600000000bps); per = default(0.001); } --> transceptorEmisor.in;
        
        transceptorReceptor.outFinal --> outPacket1;
        transceptorEmisor.outFinal --> outPacket2;
           
}

//
// Same as Tictoc8.
//
network myNetStopAndWait
{

    @display("bgb=338.08002,288.64");

    submodules:
    generador1: Generador {
    	parameters:
        	@display("i=,gold;p=22.8,60.48");
        	
    }
    generador2: Generador {
    	parameters:
        	@display("i=,gold;p=92.8,40.48");
        	
    }
    generador3: Generador {
    	parameters:
        	@display("i=,gold;p=62.8,260.48");
        	
    }

    conmutador1: Conmutador {
    	parameters:
            @display("i=,gold;p=22.8,160.48");
            repartidor.probs_arriba_str = default("1,3");
            repartidor.probs_abajo_str = default("4,4");
            repartidor.destinos_str = default("1,2");
        	
    }

    conmutador2: Conmutador {
    	parameters:
        	@display("i=,gold;p=222.8,60.48");
            repartidor.probs_arriba_str = default("1,2");
            repartidor.probs_abajo_str = default("3,3");
            repartidor.destinos_str = default("1,2");
    }

    conmutador3: Conmutador {
    	parameters:
        	@display("i=,gold;p=422.8,60.48");
            repartidor.probs_arriba_str = default("1,1");
            repartidor.probs_abajo_str = default("2,2");
            repartidor.destinos_str = default("1,2");
    }

    conmutador4: Conmutador {
    	parameters:
        	@display("i=,gold;p=422.8,260.48");
        
    }

    conmutador5: Conmutador {
    	parameters:
        	@display("i=,gold;p=222.8,260.48");
            repartidor.probs_arriba_str = default("1,1");
            repartidor.probs_abajo_str = default("2,2");
            repartidor.destinos_str = default("1,2");
    }

    protocolLink1[2]: ProtocolLink {
    	parameters:
        	@display("i=,cyan");
        
    }

    protocolLink2[2]: ProtocolLink {
    	parameters:
        	@display("i=,cyan");
    }

    protocolLink5[2]: ProtocolLink {
    	parameters:
        	@display("i=,cyan");
    }

    connections allowunconnected:
        generador1.out --> conmutador1.inPacket;
        generador2.out --> conmutador2.inPacket;
        generador3.out --> conmutador5.inPacket;
        
        conmutador1.out[1] --> protocolLink1[0].inPacket1;
        conmutador1.out[2] --> protocolLink1[1].inPacket1;
        
		protocolLink1[0].outPacket1 --> conmutador2.in[1];
		protocolLink1[1].outPacket1 --> conmutador5.in[1];

        conmutador2.out[1] --> protocolLink2[0].inPacket1;
        conmutador2.out[2] --> protocolLink2[1].inPacket1;
        
		protocolLink2[0].outPacket1 --> conmutador3.in[1];
		protocolLink2[1].outPacket1 --> conmutador4.in[1];

        conmutador5.out[1] --> protocolLink5[0].inPacket1;
        conmutador5.out[2] --> protocolLink5[1].inPacket1;
        
		protocolLink5[0].outPacket1 --> conmutador2.in[2];
		protocolLink5[1].outPacket1 --> conmutador4.in[2];
        
}
