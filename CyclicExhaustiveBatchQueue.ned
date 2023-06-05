simple CyclicExhaustiveBatchQueue
{
    parameters:
        volatile double serviceTime;	// Service time of the server in this queue
        int batchSize;					// Batch size of this queue
        int ownIndex;					// Queue's index
        @display("i=block/queue");
        @display("q=buffer");
        
    gates:
        input in;						// From the generator
        inout server;					// From and to the server
}