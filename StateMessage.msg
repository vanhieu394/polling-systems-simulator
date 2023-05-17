// Server and queues send this msg to the monitor to upate system state

enum MsgType {
    SET_SERVER_PHASE = 0;
    SET_N = 1;
    SET_Q = 2;
}

enum ServerPhase {
    VACATION = 0;
    CONNECTION = 1;
    SERVICE = 2;
}

message StateMessage {
    MsgType msgType;			// Msg's type
    ServerPhase serverPhase;	// Server phase
    int queueIndex;				// Queue's index
    int q;						// Queue's flag
    int n;						// Number of packets in queue
}