//Dynamic adaptive polling systems with exhaustive service
network DynamicExhaustivePollingSystem
{
    parameters:
        int numQueues;
    submodules:
        generator[numQueues]: Generator {
            @display("p=700,100,c,100");
        }

        dynamicExhaustiveQueue[numQueues]: DynamicExhaustiveQueue {
            @display("p=500,100,c,100");
            ownIndex = index;
        }

        dynamicServer: DynamicServer {
            numQueues = parent.numQueues;
            @display("p=300,100");
        }
        
        sink: Sink {
            @display("p=100,100");
        }
        
        monitor: Monitor {
            numQueues = parent.numQueues;
            @display("p=300,200");
        }

    connections:
        for i=0..numQueues-1 {
            generator[i].out --> dynamicExhaustiveQueue[i].in;
            dynamicExhaustiveQueue[i].server <--> dynamicServer.queue[i];
            dynamicExhaustiveQueue[i].toMonitor --> monitor.fromQueue[i];
        }
        dynamicServer.toMonitor --> monitor.fromServer;
        dynamicServer.toSink --> sink.in;
}
