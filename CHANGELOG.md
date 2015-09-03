# Change Log

All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

Prior to 3.9.0 no changelog was published with SYNTHESE releases,
therefore this changelog only tracks changes between 3.8.0 and
3.9.0.
To track what happened before 3.8.0, git log is your friend.

## [3.9.0] - 2015-04-27
### Added
- Added the ability to synchronize data packages
(InterSynthese package).
- Added the ability to synchronize incremental data changes
(InterSynthese flux).
- Added core CMS packages (admin, cms, connect, lib, monitoring_api,
onboard, ops, terminus) to source tree.
- A* search algorithm implemented as an alternative shortest road approach.
- Parking friendly criterion in road approach.
- Added robotframework functional tests

### Changed
- Migrated Boost dependency to 1.57 version.
- Packaging now uses CPack to produce distribution independant
tarballs (Boost and Synthese server packaged this way).
- Client IPs are now allowed to change during a same Synthese server session.

### Deprecated
- synthesepy is not used anymore neither in build nor for testing
purpose.

### Removed
- s3-proxy is removed from source tree.

### Fixed
- Unit tests (and related bugs) were all fixed.
- Windows build is back to normal (Microsoft Visual Studio 2013
required).

### Security

## [3.9.1] - 2015-06-08
### Added

### Changed
- tc-74 : Display client IP address in synthese logs

### Deprecated

### Removed

### Fixed
- tc-72 : The DestinationsAdmin page could only display 200 first destinations
- tc-150 : Admin could not display non concurrency rules for commercial lines in subfolders
- #56104 : Fixes on pedestrian journey using AStar
- In RoutePlannerFunction and PTJourneyPlannerService : max depth (number of journeys) is number of connections + 1
- In HeuresFileFormat : bugfix on creation/update of commercial lines, short name is read in itinerai file instead of long name

### Security

## [3.9.2] - 2015-06-23
### Added

### Changed

- Display Request duration, CPU time, HTTP Status and service called in the log

### Deprecated

### Removed

### Fixed

### Security

## [3.9.3] - 2015-06-30
### Added

### Changed

### Deprecated

### Removed

### Fixed

- #57622 : Prevent gzip compression of empty responses

### Security

## [3.9.4] - 2015-09-03
### Added

### Changed

### Deprecated

### Removed

### Fixed

- #58117 : Crashes when generating timetables using 'timetable_build' service

### Security




