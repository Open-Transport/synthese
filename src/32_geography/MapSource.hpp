
/** MapSource class header.
	@file MapSource.hpp

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

#ifndef SYNTHESE_geography_MapSource_hpp__
#define SYNTHESE_geography_MapSource_hpp__

#include "Registrable.h"
#include "Exception.h"
#include "Registry.h"

#include <boost/optional.hpp>

namespace synthese
{
	class CoordinatesSystem;

	namespace server
	{
		class Session;
	}


	namespace geography
	{
		//////////////////////////////////////////////////////////////////////////
		/// Data sources to put as base layers of a map.
		/// @ingroup m32
		/// @author Hugues Romain
		/// @date 2011
		/// @since 3.3.0
		//////////////////////////////////////////////////////////////////////////
		/// Implementation notes :
		/// The CoordinatesSystem is stored as a pointer to allow default constructor
		/// required by the framework, but should ever be a non NULL value after a load.
		/// To ensure that, no getter nor setter are defined for this attribute and
		/// are replaced by a query and a modifier based on references instead of
		/// pointers.
		///
		/// Registrable class :
		/// Table Id is 75.
		/// Object Id must be 0 or belong to [21110623253299200,21392098230009855]
		class MapSource:
			public util::Registrable
		{
		public:
			static const std::string SESSION_VARIABLE_CURRENT_MAPSOURCE;

			typedef util::Registry<MapSource> Registry;



			//////////////////////////////////////////////////////////////////////////
			/// Exception to throw when the object is accessed with no defined CoordinatesSystem
			class NoCoordinatesSystemException:
				public synthese::Exception
			{
			public:
				//////////////////////////////////////////////////////////////////////////
				/// Constructor.
				/// @param name the name of the MapSource object with undefined CoordinatesSystem
				NoCoordinatesSystemException(const std::string& name);
			};



			//////////////////////////////////////////////////////////////////////////
			/// Each type corresponds to an OpenLayers Layer subclass.
			typedef enum {
				WMS = 0,
				OSM = 1,
				IGN = 2
			} Type;



			//////////////////////////////////////////////////////////////////////////
			/// Max int value of type (used to check values at string parsing).
			/// Update this const if Type is extended
			static const int MAX_TYPE_INT;



			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// Sets the type to WMS and SRID to NULL
			//////////////////////////////////////////////////////////////////////////
			/// @param id id of the object
			MapSource(
				util::RegistryKeyType id=0
			);

		private:
			std::string _url;
			const synthese::CoordinatesSystem* _coordinatesSystem;
			Type _type;
			std::string _name;

		public:
			//! @name Getters
			//@{
				const std::string& getURL() const { return _url; }
				Type getType() const { return _type; }
				virtual std::string getName() const { return _name; }
			//@}

			//! @name Queries
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Gets a reference to the CoordinatesSystem of the object.
				/// @throws NoSRIDException if the SRID of the object is not defined
				/// @return reference to the SRID of the object
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				const synthese::CoordinatesSystem& getCoordinatesSystem() const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the Javascript code to initialize an OpenLayers layer of the map
				/// source.
				/// @return the javascript code
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				std::string getOpenLayersConstructor() const;



				bool hasCoordinatesSystem() const;



				//////////////////////////////////////////////////////////////////////////
				/// Reads the current map source from the session.
				/// If there is no current map source, the first map source of the registry
				/// is chosen (typically OSM as it was added at the module first run).
				/// @param session the current session
				/// @return the current map source (NULL if no map source exists in the database)
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				static MapSource* GetSessionMapSource(
					const server::Session& session
				);



				//////////////////////////////////////////////////////////////////////////
				/// Transforms a type as a string.
				/// @param type the type to transform
				/// @return the type as string (same as the enum code)
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				static std::string GetTypeString(Type type);



				typedef std::map<boost::optional<Type>, std::string> TypesMap;


				//////////////////////////////////////////////////////////////////////////
				/// Builds a map containing each type.
				/// @return the generated map
				static TypesMap GetTypesMap();
			//@}

			//! @name Setters
			//@{
				void setURL(const std::string& value){ _url = value; }
				void setType(Type value){ _type = value; }
				void setName(const std::string& value){ _name = value; }
			//@}

			//! @name Modifiers
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// CoordinatesSystem modifier.
				/// It is recommended to use shared instances of SRID provided by the
				/// CoordinatesSystem::GetCoordinatesSystem() static method.
				/// @param value the value of the SRID : is a reference and not a pointer.
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				void setCoordinatesSystem(const synthese::CoordinatesSystem& value);
			//@}
		};
	}
}

#endif // SYNTHESE_geography_MapSource_hpp__
