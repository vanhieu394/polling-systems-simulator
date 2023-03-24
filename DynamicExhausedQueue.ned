simple DynamicExhausedQueue
{
    parameters:
        volatile double serviceTime;	// Service time of the server in this queue
        int ownIndex;					// Queue's index
        @display("i=block/queue");
    gates:
        input in;						// From the generator
        inout out;						// To the server
}