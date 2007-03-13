synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::map::MapModule> ("39_map");

synthese::util::Factory<synthese::map::Renderer>::integrate<synthese::map::HtmlMapRenderer>("html");
synthese::util::Factory<synthese::map::Renderer>::integrate<synthese::map::JpegRenderer>("jpeg");
synthese::util::Factory<synthese::map::Renderer>::integrate<synthese::map::MapInfoRenderer>("mapinfo");
synthese::util::Factory<synthese::map::Renderer>::integrate<synthese::map::PostscriptRenderer>("ps");

synthese::util::Factory<synthese::server::Request>::integrate<synthese::map::MapRequest> ("map");

