simple AdaptiveCyclicServer
{
    parameters:
        string switchoverTime;			// Switching time of the queues
        volatile double vacationTime;	// Rest time if all queues are empty in the same cycle
        int numQueues;					// The number of the queues
        @display("i=block/server");
    gates:
        inout queue[numQueues];			// To the queues
        output toMonitor;				// To the monitor
        output toSink;					// To the sink
}