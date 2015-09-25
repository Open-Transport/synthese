
/** CoordinatesSystem class header.
	@file CoordinatesSystem.hpp

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

#ifndef SYNTHESE_CoordinatesSystem_hpp__
#define SYNTHESE_CoordinatesSystem_hpp__

#include "Exception.h"
#include "FrameworkTypes.hpp"
#include "SimpleObjectFieldDefinition.hpp"

#include <string>
#include <map>
#include <proj_api.h>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#define HAVE_INT64_T_64 // Let geos not to typedef int64 (conflicts with MySQL)
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/CoordinateFilter.h>
#include <boost/thread/mutex.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
	}

	class ObjectBase;

	/** CoordinatesSystem class.
		@ingroup m00
		@author Hugues Romain
		@since 3.2.0
		@date 2010
	*/
	class CoordinatesSystem:
		public SimpleObjectFieldDefinition<CoordinatesSystem>
	{
	public:
		typedef const CoordinatesSystem* Type;

		typedef unsigned int SRID;


		//////////////////////////////////////////////////////////////////////////
		/// CoordinateFilter which converts coordinates from a spatial reference
		/// system into another.
		//////////////////////////////////////////////////////////////////////////
		/// @see http://geos.refractions.net/ro/doxygen_docs/html/classgeos_1_1geom_1_1CoordinateFilter.html
		/// @ingroup m00
		/// @author Hugues Romain
		/// @since 3.2.0
		/// @date 2010
		class ConversionFilter:
			public geos::geom::CoordinateFilter
		{
			const CoordinatesSystem& _source;
			const CoordinatesSystem& _dest;

		public:
			ConversionFilter(
				const CoordinatesSystem& source,
				const CoordinatesSystem& dest
			):	_source(source),
				_dest(dest)
			{}

			virtual void filter_rw(geos::geom::Coordinate* c) const;
		};

		friend class ConversionFilter;

	private:
		SRID _srid;
		std::string _name;
		std::string _projSequence;
		projPJ _projObject;
		geos::geom::GeometryFactory _geometryFactory;
		bool _degrees;
		mutable boost::mutex _proj4Mutex;

		CoordinatesSystem(const CoordinatesSystem&);
		void operator=(const CoordinatesSystem&);

	public:

		CoordinatesSystem(
			SRID srid,
			const std::string& name,
			const std::string& projSequence
		);

		~CoordinatesSystem();

		//! @name Getters
		//@{
			const projPJ& getProjObject() const { return _projObject; }
			const geos::geom::GeometryFactory& getGeometryFactory() const { return _geometryFactory; }
			const std::string& getName() const { return _name; }
			SRID getSRID() const { return _srid; }
		//@}

		//! @name Services
		//@{
			//////////////////////////////////////////////////////////////////////////
			/// Conversion of the coordinate system of a point.
			//////////////////////////////////////////////////////////////////////////
			/// @param source point to convert
			/// @return result of the conversion
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.2.0
			boost::shared_ptr<geos::geom::Point> convertPoint(const geos::geom::Point& source) const;



			boost::shared_ptr<geos::geom::Geometry> convertGeometry(const geos::geom::Geometry& source) const;


			//////////////////////////////////////////////////////////////////////////
			/// Point creation using the registered geos factory.
			//////////////////////////////////////////////////////////////////////////
			/// @param x x coordinate
			/// @param y y coordinate
			/// @return created point
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.2.0
			boost::shared_ptr<geos::geom::Point> createPoint(
				double x,
				double y
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds a CoordinateFilter which converts coordinate from a spatial
			///	reference system to another one.
			/// @param sourceSRID SRID of the object to convert
			/// @return a CoordinateFilter which converts from the source to the current system
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.2.0
			ConversionFilter getConversionFilter(SRID sourceSRID) const;
		//@}

		//! @name Static services
		//@{
			static const std::string _INSTANCE_COORDINATES_SYSTEM;
			static const CoordinatesSystem* _instanceCoordinatesSystem;
			static const CoordinatesSystem* _storageCoordinatesSystem;


			typedef std::map<SRID, boost::shared_ptr<CoordinatesSystem> > CoordinatesSystemsMap;

			////////////////////////////////////////////////////////////////////
			/// All coordinates systems.
			static CoordinatesSystemsMap _CoordinatesSystems;

			class CoordinatesSystemNotFoundException:
				public synthese::Exception
			{
			public:
				CoordinatesSystemNotFoundException(SRID srid):
				  Exception("Coordinates system "+ boost::lexical_cast<std::string>(srid) +" not found")
				  {}
			};



			static void ChangeInstanceCoordinatesSystem(
				const std::string&,
				const std::string& value
			);



			static const CoordinatesSystem& GetStorageCoordinatesSystem() { return *_storageCoordinatesSystem; }
			static void SetStorageCoordinatesSystem(const CoordinatesSystem& value) { _storageCoordinatesSystem = &value; }



			static void AddCoordinatesSystem(
				SRID srid,
				const std::string& name,
				const std::string& projSequence
			);



			//////////////////////////////////////////////////////////////////////////
			/// Remove all the coordinates systems registered through AddCoordinatesSystem.
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			static void ClearCoordinatesSystems();



			//////////////////////////////////////////////////////////////////////////
			/// @throws NotFoundException if the system was not found
			static const CoordinatesSystem& GetCoordinatesSystem(SRID srid);

			static void SetDefaultCoordinatesSystems(SRID instanceSRID);
			static const CoordinatesSystem& GetInstanceCoordinatesSystem() { return *_instanceCoordinatesSystem; }

			static const geos::geom::GeometryFactory& GetDefaultGeometryFactory() { return GetInstanceCoordinatesSystem().getGeometryFactory(); }

			typedef std::map<boost::optional<SRID>, std::string> CoordinatesSystemsTextMap;
			static CoordinatesSystemsTextMap GetCoordinatesSystemsTextMap();
		//@}

		/// @name Field methods
		//@{
			static CoordinatesSystem::Type _stringToPointer(const std::string& text);
			static std::string _pointerToString(const CoordinatesSystem::Type& value);



			static bool LoadFromRecord(
				CoordinatesSystem::Type& fieldObject,
				ObjectBase& object,
				const Record& record,
				const util::Env& env
			);




			static void SaveToFilesMap(
				const CoordinatesSystem::Type& fieldObject,
				const ObjectBase& object,
				FilesMap& map
			);



			static void SaveToParametersMap(
				const CoordinatesSystem::Type& fieldObject,
				util::ParametersMap& map,
				const std::string& prefix,
				boost::logic::tribool withFiles
			);



			static void SaveToParametersMap(
				const CoordinatesSystem::Type& fieldObject,
				const ObjectBase& object,
				util::ParametersMap& map,
				const std::string& prefix,
				boost::logic::tribool withFiles
			);



			static void SaveToDBContent(
				const Type& fieldObject,
				const ObjectBase& object,
				DBContent& content
			);



			static void SaveToDBContent(
				const Type& fieldObject,
				DBContent& content
			);



			static void GetLinkedObjectsIds(
				LinkedObjectsIds& list, 
				const Record& record
			);
		//@}
	};
}

#endif // SYNTHESE_CoordinatesSystem_hpp__
