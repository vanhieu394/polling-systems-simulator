simple AdaptiveCyclicGatedQueue
{
    parameters:
        int ownIndex;					// Queue's index        
        volatile double serviceTime;	// Service time of the server in this queue
        @display("i=block/queue");
    gates:
        input in;						// From the generator
        inout server;					// From and to the server
        output toMonitor;				// To the monitor
}