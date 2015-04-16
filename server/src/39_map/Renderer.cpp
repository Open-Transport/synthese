#include "Renderer.h"

namespace synthese
{

namespace map
{


#ifdef WIN32
	const std::string Renderer::GHOSTSCRIPT_BIN ("gswin32");
#else
	const std::string Renderer::GHOSTSCRIPT_BIN ("gs");
#endif


Renderer::~Renderer()
{
}


}
}

