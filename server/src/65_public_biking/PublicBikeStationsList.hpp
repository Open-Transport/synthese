
//////////////////////////////////////////////////////////////////////////////////////////
/// PublicBikeStationsListFunction class header.
/// @file PublicBikeStationsListFunction.hpp
/// @author Camille Hue
/// @date 2015
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

#ifndef SYNTHESE_public_biking_PublicBikeStationsListFunction_H__
#define SYNTHESE_public_biking_PublicBikeStationsListFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include <geos/geom/Envelope.h>
#include <map>

namespace synthese
{
	class CoordinatesSystem;

	namespace impex
	{
		class DataSource;
	}

	namespace public_biking
	{
		class PublicBikeStation;

		class PublicBikeStationsListFunction:
			public util::FactorableTemplate<server::Function,PublicBikeStationsListFunction>
		{
		public:
			static const std::string PARAMETER_BBOX;
			static const std::string PARAMETER_SRID;
			static const std::string PARAMETER_SORT_BY_DISTANCE_TO_BBOX_CENTER;
			static const std::string PARAMETER_MAX_SOLUTIONS_NUMBER; 
			static const std::string PARAMETER_DATA_SOURCE_FILTER;

		protected:
			static const std::string TAG_PUBLIC_BIKE_STATION;
			static const std::string TAG_PUBLIC_BIKE_STATIONS;

			static const std::string DATA_DISTANCE_TO_BBOX_CENTER;

			util::ParametersMap _getParametersMap() const;

			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);



		public:
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;
			virtual bool isAuthorized(const server::Session* session) const;
			virtual std::string getOutputMimeType() const;

			virtual server::FunctionAPI getAPI() const;

		private:
			//! \name Page parameters
			//@{
				boost::optional<geos::geom::Envelope> _bbox;
				const CoordinatesSystem* _coordinatesSystem;
				boost::optional<std::size_t> _maxSolutionsNumber;
				boost::shared_ptr<const impex::DataSource> _dataSourceFilter;
				bool _isSortByDistanceToBboxCenter;
			//@}

			class SortablePublicBikeStation
			{
			private:
				const PublicBikeStation * _pbs;
				int _distanceToBboxCenter;
				bool _isSortByDistanceToBboxCenter;
			public:
				SortablePublicBikeStation(const PublicBikeStation * pbs, int distanceToBboxCenter, bool isSortByDistanceToBboxCenter);
				bool operator<(SortablePublicBikeStation const &otherPublicBikeStation) const;
				const PublicBikeStation * getPublicBikeStation() const;
				int getDistanceToBboxCenter() const;
			};
			typedef std::set<SortablePublicBikeStation> PublicBikeStationSetType;

			int CalcDistanceToBboxCenter (const PublicBikeStation & publicBikeStation) const;
		};
	}
}

#endif // SYNTHESE_PTPhysicalStopsListFunction_H__
