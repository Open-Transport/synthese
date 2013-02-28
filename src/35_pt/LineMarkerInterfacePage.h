////////////////////////////////////////////////////////////////////////////////
/// LineMarkerInterfacePage class header.
///	@file LineMarkerInterfacePage.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_LineMarkerInterfacePage_H__
#define SYNTHESE_LineMarkerInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"
#include "ParametersMap.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace server
	{
		class Request;
	}

	namespace pt
	{
		class CommercialLine;

		/** JourneyPattern marker.
			@ingroup m35Pages refPages
			@code line_marker @endcode

			Available data :
				- roid : line ID
				- style : line CSS style name
				- image : line picto image url
				- short_name : line short name
				- name : JourneyPattern name
				- color : JourneyPattern color
				- rank : Item rank if the line belongs to a list
				- rank_is_odd : Item rank is odd
				- network_id : Network id
				- network_name : Network name

		*/
		class LineMarkerInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage, LineMarkerInterfacePage>
		{
		public:
			static const std::string DATA_STYLE_NAME;
			static const std::string DATA_IMAGE_URL;
			static const std::string DATA_SHORT_NAME;
			static const std::string DATA_NAME;
			static const std::string DATA_COLOR;
			static const std::string DATA_RANK;
			static const std::string DATA_RANK_IS_ODD;
			static const std::string DATA_NETWORK_ID;
			static const std::string DATA_NETWORK_NAME;


			LineMarkerInterfacePage();

			/** Display of line of schedule sheet.
				@param stream Stream to write on
				@param vars Execution variables
				@param tableOpeningHTML HTML d'ouverture du tableau (RIEN = Pas d'ouverture du tableau)
				@param tableClosingHTML HTML de fermeture du tableau (RIEN = Pas de fermeture du tableau)
				@param pixelWidth Largeur de la case en pixels
				@param pixelHeight Hauteur de la case en pixels
				@param commercialLine Commercial JourneyPattern to display
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


			/** Display of line of schedule sheet.
				@param stream Stream to write on
				@param page page to use for display
				@param request Request
				@param commercialLine Commercial JourneyPattern to display
			*/
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				const pt::CommercialLine& commercialLine,
				util::ParametersMap pm = util::ParametersMap(),
				boost::optional<std::size_t> rank = boost::optional<std::size_t>()
			);
		};
	}
}

#endif // SYNTHESE_LineMarkerInterfacePage_H__
