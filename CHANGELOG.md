# Changelog

## [v0.6.4] - 2023-05-17

### Fixed
- Corrected some spelling mistakes: switchingTime -> switchoverTime, restTime -> vacationTime, interArrivalTime -> interarrivalTime
- In the server module: rename serviceEvent to checkQueueFlagEvent.
- In the monitor module: rename C1_1_1_n1_n2[i][j] to C1_1_1_i_j in the output file for convenience in looking up results.
- Minor fixes in some events.

### Note
- If you want to measure probability of all states, uncomment the corresponding code segment in the monitor.cc file.

## [v0.6.3] - 2023-05-05

### Fixed
- Spelling correction in all files: exhaused -> exhaustive

### Note
- If you want to measure probability of all states, uncomment the corresponding code segment in the monitor.cc file.

## [v0.6.2] - 2023-05-03

### Fixed
- Fixed the order of system state variables in the monitor.cc when printed out to the screen, from (serverPhase, n1, n2, q1, q2) to (serverPhase, q1, q2, n1, n2).

### Note
- If you want to measure probability of all states, uncomment the corresponding code segment in the monitor.cc file.

## [v0.6.1] - 2023-05-02

### Changed
- Commented out the code segment in the monitor module for measuring probabilities of states with all variables: (serverPhase, q1, q2, n1, n2) to significantly increase simulation speed (up to 4x). Other than that, everything is the same with the v0.6 program version.

## [v0.6] - 2023-05-02

### Added
- Added queue index and values of n1, n2 to the scalar output in the queue and monitor module.

## [v0.5] - 2023-05-01

### Added
- Measured the probabilities of all states in the 2-queue system, instead of just certain states, with the trade-off of simulation speed.
 
### Changed
- Changed the order of variables to describe the 2-queue system states from (serverPhase, n1, n2, q1, q2) to (serverPhase, q1, q2, n1, n2).
- Renamed variable "i" in state message and monitor to queueIndex to avoid confusion with the iterator when running for loop.
- Rearranged code structure of the monitor module.
- Optimized code.

## [v0.4] - 2023-04-25
 
### Added
- Added the monitor module that can measure probability of all system states when queue number = 2.
- Added new message type (StateMessage) to transfer state info from server and queues to the monitor.
 
### Changed
- Updated server and queue module code.
- Renamed scalar result file in the omnetpp.ini.

## [v0.3] - 2023-04-19
 
### Fixed
- Removed the redundant first value of the cycleTime and sojTime in queue module.

## [v0.2] - 2023-04-18
 
### Fixed
- This version's simulation result is now correct.
- Increased the simulation speed.

### Changed
- Removed redundant variables.

### Known Issues
- In the queue module, cycleTime and sojTime contain a redundant first value.
 
## [v0.1] - 2023-03-24
Compared to Tan's Polling_systems_v5.5.1:
 
### Added
- Added the feature of measuring packet waiting time in the queue module.
 
### Changed
- Only dynamic adaptive polling system with exhaustive service is included in this version.
- Updated code to be compatible with OMNeT++ version 6.0.1.
- The name of files, classes, variables, functions... are edited to follow the naming convention.
- The code structure is rearranged.

### Known Issues
- The simulation speed is 2x slower than Tan's one.

### Edited: WARNING - The simulation result of this version is NOT correct!!!
