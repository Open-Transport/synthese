
// ---- 
#include "39_carto/CartoModule.h"
#include "70_server/Server.h"


/* petits commentaires au milieu de toute ce bazar : 
 - pour regler les pb de dependences, point de salut il faut 99_synthese de manière à avoir  39_carto -> 70_server et (99_synthese -> 39_carto , 99_synthese -> 70_server). donc on est bons et il faut garder le main (généré ou pas dans 99 synthese)
 - faudra virer la variable de retour de la factory
*/



std::string toto = synthese::util::Factory<synthese::server::ModuleClass>::integrate<synthese::server::CartoModule> ("39_carto");

// ----

