package ringtopology.simulations;

import ringtopology.Node;

//to delete if not working
import ned.DatarateChannel;
//end to delete if not working

network circularTopology
{
    parameters:
        int numNodes = default(6);   // global number of nodes
        double datarate @unit(bps) = default(10Mbps);

    submodules:
        node[numNodes]: Node {
            @display("p=100,100");  // temporary, will update dynamically
        }

    connections allowunconnected:
        for i=0..numNodes-1 {
            node[i].out++ --> DatarateChannel { datarate; delay = 1ms; } --> node[(i+1) % numNodes].in++;
        }
}
