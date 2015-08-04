# README #

SYNTHESE is a public transportation server.

It provides network modelling, passenger information, DRT reservation,
CMS, real time data updating, and operations optimization.

* `server` : contains eveything needed to build SYNTHESE server binary itself.
* `packages` : contains CMS applications commonly deployed into SYNTHESE.
* `robot` : contains SYNTHESE high level functional tests.
* `legacy` : mostly deprecated stuff.

More specific instructions may be given in each subfolder.

Detailed description of SYNTHESE capabilities :  
https://extranet.rcsmobility.com/projects/synthese/wiki/Home

Developer documentation :  
https://extranet.rcsmobility.com/projects/synthese/wiki/Developer_documentation

Public releases and snapshot builds :
http://download.rcsmobility.com/

### Configuration management
* A modification should be done in a feature branch
* A "merge commit" is done from the feature branch to master and should refer to a youtrack ticket : http://youtrack.rcsmobility.com/dashboard
* A minimum set of work should be shown for the feature (automatic test, robot test or manual process described in the youtrack ticket)
* A pushed commit should not break build process (compilation, link) or automatic tests : http://assurancetourix4.rcsmobility.com:8080/job/master-synthese3-tests-Linux-x86_64/
* If a regression is discovered on a feature, it will be reverted until a better solution is obtained