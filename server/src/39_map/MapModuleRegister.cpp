
#include "MapModule.h"

#include "MapRequest.h"

#include "HtmlMapRenderer.h"
#include "JpegRenderer.h"
#include "MapInfoRenderer.h"
#include "PostscriptRenderer.h"

#include "TestMapAdmin.h"

#include "WFSService.hpp"


#include "MapModule.inc.cpp"

void synthese::map::moduleRegister()
{

	synthese::map::MapModule::integrate();

	synthese::map::HtmlMapRenderer::integrate();
	synthese::map::JpegRenderer::integrate();
	synthese::map::MapInfoRenderer::integrate();
	synthese::map::PostscriptRenderer::integrate();

	synthese::map::MapRequest::integrate();

	synthese::map::TestMapAdmin::integrate();

	synthese::map::WFSService::integrate();
}
