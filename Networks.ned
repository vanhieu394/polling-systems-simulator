// #1 - Adaptive cyclic polling systems with exhaustive service with 2 queues and monitor
network AdaptiveCyclicExhaustivePollingSystemWith2QueuesAndMonitor
{
    parameters:
        int numQueues;
    submodules:
        generator[numQueues]: Generator {
            @display("p=700,100,c,100");
        }

        adaptiveCyclicExhaustiveQueue[numQueues]: AdaptiveCyclicExhaustiveQueue {
            @display("p=500,100,c,100");
            ownIndex = index;
        }

        adaptiveCyclicServer: AdaptiveCyclicServer {
            numQueues = parent.numQueues;
            @display("p=300,100");
        }
        
        sink: Sink {
            @display("p=100,100");
        }
        
        monitorFor2Queues: MonitorFor2Queues {
            numQueues = parent.numQueues;
            @display("p=300,200");
        }

    connections:
        for i=0..numQueues-1 {
            generator[i].out --> adaptiveCyclicExhaustiveQueue[i].in;
            adaptiveCyclicExhaustiveQueue[i].server <--> adaptiveCyclicServer.queue[i];
            adaptiveCyclicExhaustiveQueue[i].toMonitor --> monitorFor2Queues.fromQueue[i];
        }
        adaptiveCyclicServer.toMonitor --> monitorFor2Queues.fromServer;
        adaptiveCyclicServer.toSink --> sink.in;
}

// #2 - Adaptive cyclic polling systems with gated service with 2 queues and monitor
network AdaptiveCyclicGatedPollingSystemWith2QueuesAndMonitor
{
    parameters:
        int numQueues;
    submodules:
        generator[numQueues]: Generator {
            @display("p=700,100,c,100");
        }

        adaptiveCyclicGatedQueue[numQueues]: AdaptiveCyclicGatedQueue {
            @display("p=500,100,c,100");
            ownIndex = index;
        }

        adaptiveCyclicServer: AdaptiveCyclicServer {
            numQueues = parent.numQueues;
            @display("p=300,100");
        }
        
        sink: Sink {
            @display("p=100,100");
        }
        
        monitorFor2Queues: MonitorFor2Queues {
            numQueues = parent.numQueues;
            @display("p=300,200");
        }

    connections:
        for i=0..numQueues-1 {
            generator[i].out --> adaptiveCyclicGatedQueue[i].in;
            adaptiveCyclicGatedQueue[i].server <--> adaptiveCyclicServer.queue[i];
            adaptiveCyclicGatedQueue[i].toMonitor --> monitorFor2Queues.fromQueue[i];
        }
        adaptiveCyclicServer.toMonitor --> monitorFor2Queues.fromServer;
        adaptiveCyclicServer.toSink --> sink.in;
}

// #3 - Cyclic polling systems with exhaustive batch service
network CyclicExhaustiveBatchPollingSystem
{
    parameters:
        int numQueues;
    submodules:
        generator[numQueues]: Generator {
            @display("p=700,100,c,100");
        }

        cyclicExhaustiveBatchQueue[numQueues]: CyclicExhaustiveBatchQueue {
            @display("p=500,100,c,100");
            ownIndex = index;
        }

        cyclicServer: CyclicServer {
            numQueues = parent.numQueues;
            @display("p=300,100");
        }
        
        sink: Sink {
            @display("p=100,100");
        }

    connections:
        for i=0..numQueues-1 {
            generator[i].out --> cyclicExhaustiveBatchQueue[i].in;
            cyclicExhaustiveBatchQueue[i].server <--> cyclicServer.queue[i];
        }
        cyclicServer.toSink --> sink.in;
}

// #4 - Cyclic polling systems with gated batch service
network CyclicGatedBatchPollingSystem
{
    parameters:
        int numQueues;
    submodules:
        generator[numQueues]: Generator {
            @display("p=700,100,c,100");
        }

        cyclicGatedBatchQueue[numQueues]: CyclicGatedBatchQueue {
            @display("p=500,100,c,100");
            ownIndex = index;
        }

        cyclicServer: CyclicServer {
            numQueues = parent.numQueues;
            @display("p=300,100");
        }
        
        sink: Sink {
            @display("p=100,100");
        }

    connections:
        for i=0..numQueues-1 {
            generator[i].out --> cyclicGatedBatchQueue[i].in;
            cyclicGatedBatchQueue[i].server <--> cyclicServer.queue[i];
        }
        cyclicServer.toSink --> sink.in;
}