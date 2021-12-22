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
    gates:
        output out;
}

simple Emisor
{
    parameters:
        @display("i=block/routing");
        
        double probACKLost = default(0.15);   // another parameter with a default value    
        double probNACKLost = default(0.5);   // another parameter with a default value
    gates:
        input in;
        input inGenerador;
        output out;
}

simple Receptor
{
    parameters:
        @display("i=block/process");
        double probPacketLost = default(0.2);   // another parameter with a default value
    gates:
        input in;
        output out;
}

//
// Same as Tictoc8.
//
network myNetStopAndWait
{
    @display("bgb=318.08002,108.64");
    submodules:
        generador: Generador {
            parameters:
                @display("i=,gold;p=22.8,60.48");
        }
        emisor: Emisor {
            parameters:
                @display("i=,cyan;p=152.8,60.48");
        }
        receptor: Receptor {
            parameters:
                @display("i=,gold;p=269.36002,60.48");
        }
    connections:
        generador.out --> emisor.inGenerador;
        receptor.out --> DatarateChannel {  delay = default(0.1s); datarate = default(9600000bps); per = default(0.1); } --> emisor.in;
        emisor.out --> DatarateChannel {  delay = default(0.1s); datarate = default(9600000bps); per = default(0.1); } --> receptor.in;
}
