
//////////////////////////////////////////////////////////////////////////////////////////
///	PDFTextService class implementation.
///	@file PDFTextService.cpp
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

#include "PDFTextService.hpp"

#include "IConv.hpp"
#include "PDF.hpp"
#include "PDFCMSModule.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,pdf_cms::PDFTextService>::FACTORY_KEY = "pdf_text";
	
	namespace pdf_cms
	{
		const string PDFTextService::PARAMETER_T = "t";
		const string PDFTextService::PARAMETER_X = "x";
		const string PDFTextService::PARAMETER_Y = "y";
		const string PDFTextService::PARAMETER_FONT = "font";
		const string PDFTextService::PARAMETER_SIZE = "size";
		


		ParametersMap PDFTextService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void PDFTextService::_setFromParametersMap(const ParametersMap& map)
		{
			_text = map.get<string>(PARAMETER_T);
			_x = map.get<float>(PARAMETER_X);
			_y = map.get<float>(PARAMETER_Y);

			// Font
			string font(map.getDefault<string>(PARAMETER_FONT));
			if(!font.empty())
			{
				_font = font;
			}

			// Size
			_size = map.getOptional<float>(PARAMETER_SIZE);
		}



		ParametersMap PDFTextService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Get the current PDF object
			PDF& pdf(PDFCMSModule::GetRequestPDF(request));

			// Change the font
			if(_font)
			{
				pdf.setTextFont(*_font);
			}

			// Change the text size
			if(_size)
			{
				pdf.setTextSize(
					PDF::GetPixelsFromMM(*_size)
				);
			}

			// Write the text
			pdf.drawText(
				IConv("UTF-8", "CP1252").convert(_text),
				PDF::GetPixelsFromMM(_x),
				PDF::GetPixelsFromMM(_y)
			);

			// Return informations about the text
			ParametersMap map;
			return map;
		}
		
		
		
		bool PDFTextService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PDFTextService::getOutputMimeType() const
		{
			return "text/html";
		}



		PDFTextService::PDFTextService():
			_x(0),
			_y(0)
		{}
}	}
