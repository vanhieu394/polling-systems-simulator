//Dynamic adaptive polling systems with exhaused service
network DynamicExhausedPollingSystem
{
    parameters:
        int numQueues;
    submodules:
        generator[numQueues]: Generator {
            @display("p=700,100,c,100");
        }

        dynamicExhausedQueue[numQueues]: DynamicExhausedQueue {
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
            generator[i].out --> dynamicExhausedQueue[i].in;
            dynamicExhausedQueue[i].server <--> dynamicServer.queue[i];
            dynamicExhausedQueue[i].toMonitor --> monitor.fromQueue[i];
        }
        dynamicServer.toMonitor --> monitor.fromServer;
        dynamicServer.toSink --> sink.in;
}
