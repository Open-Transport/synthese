////////////////////////////////////////////////////////////////////////////////
/// LineMarkerInterfacePage class header.
///	@file LineMarkerInterfacePage.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_LineMarkerInterfacePage_H__
#define SYNTHESE_LineMarkerInterfacePage_H__

#include "InterfacePage.h"

#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}
	namespace pt
	{
		class CommercialLine;

		/** Line marker.
			@ingroup 15Library refLibrary
			@code line_marker @endcode

			Parameters :
				- opening_html_code : table opening HTML code
				- closing_html_code : table closing HTML code
				- width : width of the marker (pixels)
				- height : height of the marker (pixels)

			Available data :
				- style : line style name
				- image : line picto image url
				- short_name : line short name
				- id : line ID

			Object : CommercialLine
		*/
		class LineMarkerInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage, LineMarkerInterfacePage>
		{
		public:
			static const std::string PARAMETER_OPENING_HTML_CODE;
			static const std::string PARAMETER_CLOSING_HTML_CODE;
			static const std::string PARAMETER_WIDTH;
			static const std::string PARAMETER_HEIGHT;

			static const std::string DATA_STYLE_NAME;
			static const std::string DATA_IMAGE_URL;
			static const std::string DATA_SHORT_NAME;
			static const std::string DATA_ID;

			LineMarkerInterfacePage();

			/** Display of line of schedule sheet.
				@param stream Stream to write on
				@param vars Execution variables
				@param tableOpeningHTML HTML d'ouverture du tableau (RIEN = Pas d'ouverture du tableau)
				@param tableClosingHTML HTML de fermeture du tableau (RIEN = Pas de fermeture du tableau)
				@param pixelWidth Largeur de la case en pixels
				@param pixelHeight Hauteur de la case en pixels
				@param commercialLine Commercial Line to display
				@param request Request
			*/
			void display(
				std::ostream& stream
				, interfaces::VariablesMap& vars
				, const std::string& tableOpeningHTML
				, const std::string& tableClosingHTML
				, int pixelWidth
				, int pixelHeight
				, const pt::CommercialLine& commercialLine
				, const server::Request* request = NULL
			) const;

		};
	}
}

#endif // SYNTHESE_LineMarkerInterfacePage_H__
