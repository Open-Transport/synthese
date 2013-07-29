
//////////////////////////////////////////////////////////////////////////////////////////
///	PDFLineService class implementation.
///	@file PDFLineService.cpp
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

#include "PDFLineService.hpp"

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
	const string FactorableTemplate<Function,pdf_cms::PDFLineService>::FACTORY_KEY = "pdf_line";
	
	namespace pdf_cms
	{
		const string PDFLineService::PARAMETER_X1 = "x1";
		const string PDFLineService::PARAMETER_Y1 = "y1";
		const string PDFLineService::PARAMETER_X2 = "x2";
		const string PDFLineService::PARAMETER_Y2 = "y2";
		const string PDFLineService::PARAMETER_WIDTH = "width";
		const string PDFLineService::PARAMETER_COLOR = "color";



		ParametersMap PDFLineService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void PDFLineService::_setFromParametersMap(const ParametersMap& map)
		{
			_x1 = map.get<float>(PARAMETER_X1);
			_y1 = map.get<float>(PARAMETER_Y1);
			_x2 = map.get<float>(PARAMETER_X2);
			_y2 = map.get<float>(PARAMETER_Y2);

			// Width
			_width = map.getOptional<float>(PARAMETER_WIDTH);

			// Color
			if(!map.getDefault<string>(PARAMETER_COLOR).empty())
			{
				_color = util::RGBColor(map.get<string>(PARAMETER_COLOR));
			}
		}



		ParametersMap PDFLineService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Get the current PDF object
			PDF& pdf(PDFCMSModule::GetRequestPDF(request));

			// Change the color
			if(_color)
			{
				pdf.setStrokeColor(
					PDF::RGBColor(*_color)
				);
			}

			// Set the width
			if(_width)
			{
				pdf.setLineWidth(
					*_width
				);
			}

			// Draw the line
			pdf.drawLine(
				_x1,
				_y1,
				_x2,
				_y2
			);


			ParametersMap map;
			return map;
		}
		
		
		
		bool PDFLineService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PDFLineService::getOutputMimeType() const
		{
			return "text/html";
		}



		PDFLineService::PDFLineService():
			_x1(0),
			_y1(0),
			_x2(0),
			_y2(0)
		{

		}
}	}
