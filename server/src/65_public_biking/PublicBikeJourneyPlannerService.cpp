
/** PublicBikeJourneyPlannerService class implementation.
	@file PublicBikeJourneyPlannerService.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
*/

#include "PublicBikeJourneyPlannerService.hpp"

using namespace std;
using namespace geos::geom;
using namespace geos::io;

namespace synthese
{
	template<> const string util::FactorableTemplate<server::Function, public_biking::PublicBikeJourneyPlannerService>::FACTORY_KEY("PublicBikeJourneyPlannerService");

	namespace public_biking
	{
		PublicBikeJourneyPlannerService::PublicBikeJourneyPlannerService()
		{
		}



		util::ParametersMap PublicBikeJourneyPlannerService::_getParametersMap() const
		{
			util::ParametersMap map;

			return map;
		}



		void PublicBikeJourneyPlannerService::_setFromParametersMap(const util::ParametersMap& map)
		{
		}



		util::ParametersMap PublicBikeJourneyPlannerService::run(
			ostream& stream,
			const server::Request& request
		) const {
			util::ParametersMap result;



			return result;
		}



		bool PublicBikeJourneyPlannerService::isAuthorized(
			const server::Session* session
		) const {
			return true;
		}

		std::string PublicBikeJourneyPlannerService::getOutputMimeType() const
		{
			return _outputFormat;
		}
	}
}
