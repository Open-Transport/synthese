
/** SimpleObjectField_Date class header.
	@file SimpleObjectField_Date.hpp

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

#ifndef SYNTHESE__GeometryField_hpp__
#define SYNTHESE__GeometryField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "CoordinatesSystem.hpp"
#include "FrameworkTypes.hpp"

#include <geos/io/ParseException.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>

namespace geos
{
	namespace geom
	{
		class Point;
		class LineString;
	}
}

namespace synthese
{
	namespace util
	{
		class Env;
	}

	class ObjectBase;

	//////////////////////////////////////////////////////////////////////////
	/// date partial specialization
	template<class C, class T>
	class GeometryField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef boost::shared_ptr<T> Type;

	private:
		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a geometry into a string to be stored (SQL format).
		/// @param g the geometry to convert
		/// @return the converted string
		static std::string _geomToString(const typename Type& g)
		{
			if(!g.get() || g->isEmpty())
			{
				return std::string();
			}

			boost::shared_ptr<geos::geom::Geometry> projected(g);
			if(	CoordinatesSystem::GetStorageCoordinatesSystem().getSRID() !=
				static_cast<CoordinatesSystem::SRID>(g->getSRID())
			){
				projected = CoordinatesSystem::GetStorageCoordinatesSystem().convertGeometry(*g);
			}

			geos::io::WKTWriter writer;
			return writer.write(projected.get());
		}



		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a string into a geometry.
		/// @param d the date to convert
		/// @return the converted string
		static typename Type _stringToGeom(const std::string& s)
		{
			geos::io::WKTReader reader(&CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory());

			try
			{
				return boost::static_pointer_cast<T, geos::geom::Geometry>(
					CoordinatesSystem::GetInstanceCoordinatesSystem().convertGeometry(
						*boost::shared_ptr<geos::geom::Geometry>(reader.read(s))
				)	);
			}
			catch(geos::io::ParseException&)
			{
			}
			return boost::shared_ptr<T>();
		}


	public:
		static void LoadFromRecord(
			typename Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			SimpleObjectFieldDefinition<C>::_LoadFromStringWithDefaultValue<typename Type>(
				fieldObject,
				record,
				_stringToGeom,
				typename Type()
			);
		}



		static void SaveToFilesMap(
			const typename Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				_geomToString
			);
		}



		static void SaveToParametersMap(
			const typename Type& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap<typename Type>(
				fieldObject,
				map,
				prefix,
				withFiles,
				_geomToString
			);
		}




		static void SaveToDBContent(
			const typename Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			boost::optional<std::string> s;
			if(fieldObject.get() && !fieldObject->isEmpty())
			{
				s = _geomToString(fieldObject);
			}
			content.push_back(Cell(s));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){
		}
	};


	#define FIELD_GEOM(N, T) struct N : public GeometryField<N, T> {};

	FIELD_GEOM(PointGeometry, geos::geom::Point)
	FIELD_GEOM(LineStringGeometry, geos::geom::LineString)
}

#endif
