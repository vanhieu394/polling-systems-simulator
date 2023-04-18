# Changelog
 
## [v0.2] - 2023-04-18
 
### Added
 
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

### Fixed


### Known Issues
- The simulation speed is 2x slower than Tan's one.

### Edited: WARNING - The simulation result of this version is NOT correct!!!
