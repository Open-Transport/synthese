
//////////////////////////////////////////////////////////////////////////////////////////
///	PDFAddFontService class implementation.
///	@file PDFAddFontService.cpp
///	@author hromain
///	@date 2013
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PDFAddFontService.hpp"

#include "PDF.hpp"
#include "PDFCMSModule.hpp"
#include "Request.h"
#include "RequestException.h"
#include "Webpage.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace cms;
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<FunctionWithSite<false>, pdf_cms::PDFAddFontService>::FACTORY_KEY = "pdf_add_font";
	
	namespace pdf_cms
	{
		const string PDFAddFontService::PARAMETER_FONT = "font";
		


		ParametersMap PDFAddFontService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void PDFAddFontService::_setFromParametersMap(const ParametersMap& map)
		{
			// Font
			string fontStr(map.get<string>(PARAMETER_FONT));
			ParametersMap::Trim(fontStr);
			if(fontStr[0] >= '0' && fontStr[0] <= '9')
			{	// Font by ID
				try
				{
					RegistryKeyType pageId(lexical_cast<RegistryKeyType>(fontStr));
					_font = Env::GetOfficialEnv().get<Webpage>(pageId).get();
				}
				catch(bad_lexical_cast&)
				{
					throw RequestException("Bad cast in font id");
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such font");
				}
			}
			else
			{	// Page by smart URL
				_font = getSite()->getPageBySmartURL(fontStr);
				if(!_font)
				{
					throw RequestException("No such font");
				}
			}
		}



		ParametersMap PDFAddFontService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// PDF
			PDF& pdf(PDFCMSModule::GetRequestPDF(request));

			// Extract font content
			string fontContent(_font->get<WebpageContent>().getCMSScript().eval());
			
			// Add the font to the pdf
			pdf.embedTTFFont(fontContent);

			// Return nothing
			ParametersMap map;
			return map;
		}
		
		
		
		bool PDFAddFontService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PDFAddFontService::getOutputMimeType() const
		{
			return "text/html";
		}



		PDFAddFontService::PDFAddFontService():
			_font(NULL)
		{}
}	}
