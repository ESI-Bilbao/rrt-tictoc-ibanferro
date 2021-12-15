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



simple TicSaW
{
    parameters:
        @display("i=block/routing");
    gates:
        input in;
        output out;
}

simple TocSaW
{
    parameters:
        @display("i=block/process");
    gates:
        input in;
        output out;
}

//
// Same as Tictoc8.
//
network TictocSaW
{
    
    
    types:
        channel C extends ned.DatarateChannel {
            datarate = 1Mbps;
        }
    submodules:
        tic: TicSaW {
            parameters:
                @display("i=,cyan");
        }
        toc: TocSaW {
            parameters:
                @display("i=,gold");
        }
    connections:
        tic.out --> C --> toc.in;
        tic.in <-- C <-- toc.out;
        
}

