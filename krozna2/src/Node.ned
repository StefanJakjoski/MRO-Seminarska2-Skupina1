package ringtopology;

simple Node
{
    parameters:
        int nodeId = default(0);   // index in the ring
        int hopLimit = default(10); // maximum hops for message to avoid infinite loops
        int startId = default(0);
        int destinationId;	// end node for message, after which it terminates
        double sendInterval @unit(s);

        bool isPacket = default(true);
        int packetSize = default(4096*16);
    gates:
        input in[];
        output out[];
}
