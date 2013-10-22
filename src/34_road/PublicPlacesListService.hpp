
//////////////////////////////////////////////////////////////////////////////////////////
/// PublicPlacesListService class header.
/// @file PublicPlacesListService.hpp
///
/// This file belongs to the SYNTHESE project (public transportation specialized software)
/// Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
/// This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License
/// as published by the Free Software Foundation; either version 2
/// of the License, or (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program; if not, write to the Free Software
/// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_PublicPlacesListService_H__
#define SYNTHESE_PublicPlacesListService_H__

#include "UtilTypes.h"
#include "FactorableTemplate.h"
#include "Function.h"

#include <geos/geom/Envelope.h>

namespace synthese
{
	class CoordinatesSystem;

	namespace cms
	{
		class Webpage;
	}

	namespace road
	{
		class PublicPlacesListService:
			public util::FactorableTemplate<server::Function, PublicPlacesListService>
		{
		public:
			static const std::string PARAMETER_PAGE_ID;
			static const std::string PARAMETER_BBOX;
			static const std::string PARAMETER_SRID;

		protected:
			static const std::string TAG_PLACES;
			static const std::string TAG_PLACE;

			static const std::string DATA_ID;
			static const std::string DATA_NAME;
			static const std::string DATA_CATEGORY;
			static const std::string DATA_DETAILS;
			static const std::string DATA_X;
			static const std::string DATA_Y;

			static const std::string DATA_DISTANCE_TO_BBOX_CENTER;
	
			boost::shared_ptr<const cms::Webpage> _page;
			boost::optional<geos::geom::Envelope> _bbox;
			const CoordinatesSystem* _coordinatesSystem;



			util::ParametersMap _getParametersMap() const;



			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);



		public:
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			virtual bool isAuthorized(const server::Session* session) const;



			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_PublicPlacesListService_H__
