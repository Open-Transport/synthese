
/** PublicPlace class header.
	@file PublicPlace.h

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_road_PUBLICPLACE_H
#define SYNTHESE_road_PUBLICPLACE_H

#include "DataSourceLinksField.hpp"
#include "GeometryField.hpp"
#include "NamedPlaceField.hpp"
#include "NamedPlaceTemplate.h"
#include "NumericField.hpp"
#include "StringField.hpp"
#include "Object.hpp"
#include "ImportableTemplate.hpp"
#include "WithGeometry.hpp"

#include "Registry.h"

#include <geos/geom/Point.h>
#include <string>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace road
	{
		class PublicPlaceEntrance;

		FIELD_STRING(Category)
		FIELD_STRING(Details)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(geography::NamedPlaceField),
			FIELD(impex::DataSourceLinks),
			FIELD(Category),
			FIELD(Details),
			FIELD(PointGeometry)
		> PublicPlaceSchema;

		//////////////////////////////////////////////////////////////////////////
		/// Public place.
		///	@ingroup m34
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Public place model :
		/// @image html uml_public_place.png
		///
		class PublicPlace:
			public Object<PublicPlace, PublicPlaceSchema>,
			public geography::NamedPlaceTemplate<PublicPlace>,
			public impex::ImportableTemplate<PublicPlace>
		{
		public:
			static const std::string DATA_ID;
			static const std::string DATA_NAME;
			static const std::string DATA_CATEGORY;
			static const std::string DATA_DETAILS;
			static const std::string DATA_X;
			static const std::string DATA_Y;
			static const std::string DATA_GEOMETRY;

			typedef std::set<PublicPlaceEntrance*> Entrances;

		private:
			/// @name
			//@{
				Entrances _entrances;
			//@}

		public:
			/// Chosen registry class.
			typedef util::Registry<PublicPlace>	Registry;

			PublicPlace (
				util::RegistryKeyType id = 0,
				std::string category = std::string(),
				std::string details = std::string()
			);

			virtual ~PublicPlace ();

			/// @name Getters
			//@{
				const Entrances& getEntrances() const { return _entrances; }
			//@}

			/// @name Modifiers
			//@{
				void addEntrance(PublicPlaceEntrance& entrance);
				void removeEntrance(PublicPlaceEntrance& entrance);
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			virtual std::string getNameForAllPlacesMatcher(
				std::string text = std::string()
			) const;

			virtual std::string getName() const;

			//! @name Virtual queries for geography::Place interface
			//@{
				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result,
					const graph::AccessParameters& accessParameters,
					const geography::Place::GraphTypes& whatToSearch
				) const;

				virtual const boost::shared_ptr<geos::geom::Point>& getPoint() const;
			//@}

			/// @name Services
			//@{
				void toParametersMap(
					util::ParametersMap& pm,
					const CoordinatesSystem* coordinatesSystem,
					const std::string& prefix = std::string()
				) const;

				virtual void toParametersMap(
					util::ParametersMap& map,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif
