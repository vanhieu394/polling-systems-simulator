simple MonitorFor3BatchQueues
{
    parameters:
        int numQueues;					// The number of the queues
        @display("i=old/monitor");
    gates:
		input fromServer;
		input fromQueue[numQueues];                    
}
