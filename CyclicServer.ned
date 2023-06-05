simple CyclicServer
{
    parameters:
        string switchoverTime;			// Switching time of the queues
        int numQueues;					// The number of the queues
        @display("i=block/server");
    gates:
        inout queue[numQueues];			// To the queues
        output toSink;					// To the sink
}