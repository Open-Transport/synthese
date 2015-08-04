# SYNTHESE

SYNTHESE is a public transportation server.

It provides network modeling, passenger information, DRT reservation,
CMS, real time data updating, and operations optimization. Detailed
description of SYNTHESE capabilities are available
[here](https://extranet.rcsmobility.com/projects/synthese/wiki/Home).

* `server`: contains eveything needed to build SYNTHESE server binary itself.
* `packages`: contains CMS applications commonly deployed into SYNTHESE.
* `robot`: contains SYNTHESE high level functional tests.
* `legacy`: mostly deprecated stuff.

More specific instructions may be given in each subfolder.

### Developer

SYNTHESE is coded in C++ using the Boost library. Developer
documentation is available
[here](https://extranet.rcsmobility.com/projects/synthese/wiki/Developer_documentation).

### Download

Public releases and snapshot builds can be found [here](http://download.rcsmobility.com/synthese3/release/)

### Configuration management
* A modification should be done in a feature branch
* A "merge commit" is done from the feature branch to master and should refer to a [ticket in our bug tracker](http://youtrack.rcsmobility.com/dashboard)
* A minimum set of work should be shown for the feature (automatic test, robot test or manual process described in the youtrack ticket)
* A pushed commit should not break the build process (compilation, link) or [automatic tests](http://assurancetourix4.rcsmobility.com:8080/job/master-synthese3-tests-Linux-x86_64/)
* If a regression is discovered on a feature, it will be reverted until a better solution is obtained