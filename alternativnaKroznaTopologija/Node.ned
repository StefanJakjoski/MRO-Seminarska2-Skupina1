package dn2;

simple Node
{
    parameters:
        int queueLength = default(0);
        int nodeId = default(0);   // index in the ring
        int hopLimit = default(10); // maximum hops for message to avoid infinite loops
        int startId = int(uniform(0,6));
        int destinationId = int(uniform(0,6));	// end node for message, after which it terminates
        double sendInterval @unit(s);
		
	
        bool isPacket = default(true);
        int packetSize = default(4096*16*16);
    gates:
        input in[];
        output out[];
}