simple CyclicExhaustiveBatchQueueWithMonitor
{
    parameters:
        int ownIndex;					// Queue's index
        double queueCapacity;			// Maximum space of the queue
        volatile double serviceTime;	// Service time of the server in this queue
        int batchSize;					// Batch size of this queue
        @display("i=block/queue");
        @display("q=buffer");
        
    gates:
        input in;						// From the generator
        inout server;					// From and to the server
        output toMonitor;				// To the monitor
}