
#include "PDFCMSModule.hpp"

#include "PDFAddFontService.hpp"
#include "PDFCloseService.hpp"
#include "PDFImageService.hpp"
#include "PDFOpenService.hpp"
#include "PDFTextService.hpp"
#include "PDFLineService.hpp"
#include "PDFRectangleService.hpp"

#include "PDFCMSModule.inc.cpp"

void synthese::pdf_cms::moduleRegister()
{
	synthese::pdf_cms::PDFCMSModule::integrate();

	synthese::pdf_cms::PDFAddFontService::integrate();
	synthese::pdf_cms::PDFCloseService::integrate();
	synthese::pdf_cms::PDFImageService::integrate();
	synthese::pdf_cms::PDFOpenService::integrate();
	synthese::pdf_cms::PDFTextService::integrate();
	synthese::pdf_cms::PDFLineService::integrate();
	synthese::pdf_cms::PDFRectangleService::integrate();
}
