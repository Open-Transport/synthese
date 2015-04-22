# Change Log

All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

Sorry for this change log to start from version 3.9.0 ; there was no
proper tracking of major changes from one version to another so far.
To track what happened before 3.9.0, git log is your friend.

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






