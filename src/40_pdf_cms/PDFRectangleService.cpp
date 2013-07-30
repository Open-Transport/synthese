
//////////////////////////////////////////////////////////////////////////////////////////
///	PDFRectangleService class implementation.
///	@file PDFRectangleService.cpp
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

#include "PDFRectangleService.hpp"

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
	const string FactorableTemplate<Function,pdf_cms::PDFRectangleService>::FACTORY_KEY = "pdf_rectangle";
	
	namespace pdf_cms
	{
		const string PDFRectangleService::PARAMETER_X1 = "x1";
		const string PDFRectangleService::PARAMETER_Y1 = "y1";
		const string PDFRectangleService::PARAMETER_X2 = "x2";
		const string PDFRectangleService::PARAMETER_Y2 = "y2";
		const string PDFRectangleService::PARAMETER_WIDTH = "width";
		const string PDFRectangleService::PARAMETER_COLOR = "color";
		const string PDFRectangleService::PARAMETER_BACKGROUND_COLOR = "background_color";



		ParametersMap PDFRectangleService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void PDFRectangleService::_setFromParametersMap(const ParametersMap& map)
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

			// Background color
			if(!map.getDefault<string>(PARAMETER_BACKGROUND_COLOR).empty())
			{
				_backgroundColor = util::RGBColor(map.get<string>(PARAMETER_BACKGROUND_COLOR));
			}
		}



		ParametersMap PDFRectangleService::run(
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

			// Change the color
			if(_backgroundColor)
			{
				pdf.setFillColor(
					PDF::RGBColor(*_backgroundColor)
				);
			}

			// Set the width
			if(_width)
			{
				pdf.setLineWidth(
					PDF::GetPixelsFromMM(*_width)
				);
			}

			// Draw the line
			pdf.drawRectangle(
				PDF::GetPixelsFromMM(_x1),
				PDF::GetPixelsFromMM(_y1),
				PDF::GetPixelsFromMM(_x2),
				PDF::GetPixelsFromMM(_y2),
				_color || _width,
				_backgroundColor
			);


			ParametersMap map;
			return map;
		}
		
		
		
		bool PDFRectangleService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PDFRectangleService::getOutputMimeType() const
		{
			return "text/html";
		}



		PDFRectangleService::PDFRectangleService():
			_x1(0),
			_y1(0),
			_x2(0),
			_y2(0)
		{

		}
}	}
