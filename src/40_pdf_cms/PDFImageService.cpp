
//////////////////////////////////////////////////////////////////////////////////////////
///	PDFImageService class implementation.
///	@file PDFImageService.cpp
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

#include "PDFImageService.hpp"

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
	const string FactorableTemplate<FunctionWithSite<false>, pdf_cms::PDFImageService>::FACTORY_KEY = "pdf_image";
	
	namespace pdf_cms
	{
		const string PDFImageService::PARAMETER_IMAGE = "image";
		const string PDFImageService::PARAMETER_X = "x";
		const string PDFImageService::PARAMETER_Y = "y";
		const string PDFImageService::PARAMETER_WIDTH = "width";
		const string PDFImageService::PARAMETER_HEIGHT = "height";
		


		ParametersMap PDFImageService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void PDFImageService::_setFromParametersMap(const ParametersMap& map)
		{
			// The image
			_key = map.get<string>(PARAMETER_IMAGE);
			ParametersMap::Trim(_key);
			if(_key[0] >= '0' && _key[0] <= '9')
			{	// Font by ID
				try
				{
					RegistryKeyType pageId(lexical_cast<RegistryKeyType>(_key));
					_image = Env::GetOfficialEnv().get<Webpage>(pageId).get();
				}
				catch(bad_lexical_cast&)
				{
					throw RequestException("Bad cast in image id");
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such image");
				}
			}
			else
			{	// Page by smart URL
				_image = getSite()->getPageBySmartURL(_key);
				if(!_image)
				{
					throw RequestException("No such image");
				}
			}

			// x/y
			_x = map.get<float>(PARAMETER_X);
			_y = map.get<float>(PARAMETER_Y);

			// width/height
			_width = map.getDefault<float>(PARAMETER_WIDTH, 0);
			_height = map.getDefault<float>(PARAMETER_HEIGHT, 0);
		}



		ParametersMap PDFImageService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Get the pdf
			PDF& pdf(PDFCMSModule::GetRequestPDF(request));

			// Get or create image
			HPDF_Image image(pdf.getImage(_key));
			if(!image)
			{
				string content(_image->get<WebpageContent>().getCMSScript().eval());
				image = pdf.embedPNGImage(_key, content);
			}

			// Width and Height
			float width(_width);
			float height(_height);
			if(!width && height)
			{
				HPDF_Point siz(HPDF_Image_GetSize(image));
				width = _height * (siz.x / siz.y);
			}
			else if(!height && width)
			{
				HPDF_Point siz(HPDF_Image_GetSize(image));
				height = _width * (siz.y / siz.x);
			}

			// Draw the image
			pdf.drawImage(
				image,
				PDF::GetPixelsFromMM(_x),
				PDF::GetPixelsFromMM(_y),
				width ? PDF::GetPixelsFromMM(width) : HPDF_Image_GetWidth(image),
				height ? PDF::GetPixelsFromMM(height) : HPDF_Image_GetHeight(image)
			);

			// Return nothing
			ParametersMap map;
			return map;
		}
		
		
		
		bool PDFImageService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PDFImageService::getOutputMimeType() const
		{
			return "text/html";
		}



		PDFImageService::PDFImageService():
			_image(NULL)
		{}
}	}
