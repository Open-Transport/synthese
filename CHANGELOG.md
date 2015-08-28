# Change Log

All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

Test

## [3.10.0] - 2015-04-27
### Added
- server command line parameter sql_trace added to trace execution of SQL statements (only implemented for SQLite)
	
### Changed

### Deprecated

### Removed

### Fixed
- tc-72 : The DestinationsAdmin page could only display 200 first destinations
- tc-150 : Admin could not display non concurrency rules for commercial lines in subfolders
- In RoutePlannerFunction and PTJourneyPlannerService : max depth (number of journeys) is number of connections + 1
- In HeuresFileFormat : bugfix on creation/update of commercial lines, short name is read in itinerai file instead of long name

### Security






