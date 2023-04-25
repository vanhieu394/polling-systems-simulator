# Changelog

## [v0.4] - 2023-04-25
 
### Added
- Added the monitor module that can mesure probability of all system states when queue number = 2.
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
- Only dynamic adaptive polling system with exhaused service is included in this version.
- Updated code to be compatible with OMNeT++ version 6.0.1.
- The name of files, classes, variables, functions... are edited to follow the naming convention.
- The code structure is rearranged.

### Known Issues
- The simulation speed is 2x slower than Tan's one.

### Edited: WARNING - The simulation result of this version is NOT correct!!!
