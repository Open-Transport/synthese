
//////////////////////////////////////////////////////////////////////////////////////////
///	PDFOpenService class implementation.
///	@file PDFOpenService.cpp
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

#include "PDFOpenService.hpp"

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
	const string FactorableTemplate<Function,pdf_cms::PDFOpenService>::FACTORY_KEY = "pdf_open";
	
	namespace pdf_cms
	{
		const string PDFOpenService::PARAMETER_WIDTH = "width";
		const string PDFOpenService::PARAMETER_HEIGHT = "height";



		ParametersMap PDFOpenService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void PDFOpenService::_setFromParametersMap(const ParametersMap& map)
		{
			// Width
			_width = map.get<float>(PARAMETER_WIDTH);

			// Height
			_height = map.get<float>(PARAMETER_HEIGHT);
		}



		ParametersMap PDFOpenService::run(
			std::ostream& stream,
			const Request& request
		) const {
			// PDF creation
			PDFCMSModule::CreateRequestPDF(request);

			// First page creation
			PDFCMSModule::GetRequestPDF(request).addPage(
				PDF::GetPixelsFromMM(_width),
				PDF::GetPixelsFromMM(_height)
			);

			// Return nothing
			return ParametersMap();
		}
		
		
		
		bool PDFOpenService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PDFOpenService::getOutputMimeType() const
		{
			return "text/html";
		}



		PDFOpenService::PDFOpenService():
			_width(3),
			_height(3)
		{
		}
}	}
