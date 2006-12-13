
/* petits commentaires au milieu de toute ce bazar : 
 - pour regler les pb de dependences, point de salut il faut 99_synthese de manière à avoir  39_carto -> 30_server et (99_synthese -> 39_carto , 99_synthese -> 30_server). donc on est bons et il faut garder le main (généré ou pas dans 99 synthese)
*/


synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::carto::CartoModule> ("39_carto");

synthese::util::Factory<synthese::carto::Renderer>::integrate<synthese::carto::HtmlMapRenderer>("html");
synthese::util::Factory<synthese::carto::Renderer>::integrate<synthese::carto::JpegRenderer>("jpeg");
synthese::util::Factory<synthese::carto::Renderer>::integrate<synthese::carto::MapInfoRenderer>("mapinfo");
synthese::util::Factory<synthese::carto::Renderer>::integrate<synthese::carto::PostscriptRenderer>("ps");

synthese::util::Factory<synthese::server::Request>::integrate<synthese::carto::MapRequestHandler>("map");


