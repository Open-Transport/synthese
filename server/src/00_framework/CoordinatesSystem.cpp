
/** CoordinatesSystem class implementation.
	@file CoordinatesSystem.cpp

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

#include "CoordinatesSystem.hpp"

#include "SchemaMacros.hpp"

#include <geos/geom/Point.h>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <geos/algorithm/Angle.h>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace boost::algorithm;
using namespace geos::algorithm;

namespace synthese
{
	FIELD_DEFINITION_OF_TYPE(CoordinatesSystem, "srid", SQL_INTEGER)

	const string CoordinatesSystem::_INSTANCE_COORDINATES_SYSTEM("instance_coordinates_system");
	const CoordinatesSystem* CoordinatesSystem::_instanceCoordinatesSystem(NULL);
	const CoordinatesSystem* CoordinatesSystem::_storageCoordinatesSystem(NULL);
	CoordinatesSystem::CoordinatesSystemsMap CoordinatesSystem::_CoordinatesSystems;



	boost::shared_ptr<geos::geom::Point> CoordinatesSystem::convertPoint(
		const geos::geom::Point& source
	) const	{
		return dynamic_pointer_cast<Point>(convertGeometry(source));
	}



	boost::shared_ptr<geos::geom::Geometry> CoordinatesSystem::convertGeometry( const geos::geom::Geometry& source ) const
	{
		mutex::scoped_lock lock (_proj4Mutex);

		boost::shared_ptr<geos::geom::Geometry> result(_geometryFactory.createGeometry(&source));

		CoordinatesSystem::ConversionFilter filter(
			GetCoordinatesSystem(source.getSRID()),
			*this
		);

		result->apply_rw(&filter);
		result->setSRID(_srid);

		return result;
	}



	void CoordinatesSystem::ChangeInstanceCoordinatesSystem( const std::string&, const std::string& value )
	{
		SetDefaultCoordinatesSystems(lexical_cast<SRID>(value));
	}



	const CoordinatesSystem& CoordinatesSystem::GetCoordinatesSystem( SRID srid )
	{
		CoordinatesSystemsMap::const_iterator it(_CoordinatesSystems.find(srid));
		if(it == _CoordinatesSystems.end())
		{
			throw CoordinatesSystemNotFoundException(srid);
		}
		return *(it->second);
	}



	void CoordinatesSystem::AddCoordinatesSystem( SRID srid, const std::string& name, const std::string& projSequence )
	{
		_CoordinatesSystems.insert(make_pair(srid, boost::shared_ptr<CoordinatesSystem>(new CoordinatesSystem(srid, name, projSequence))));
	}



	void CoordinatesSystem::ClearCoordinatesSystems()
	{
		_instanceCoordinatesSystem = NULL;
		_storageCoordinatesSystem = NULL;
		if (_CoordinatesSystems.size() != 0)
			_CoordinatesSystems.clear();
	}



	void CoordinatesSystem::SetDefaultCoordinatesSystems( SRID instanceSRID )
	{
		_instanceCoordinatesSystem = &GetCoordinatesSystem(instanceSRID);
	}



	boost::shared_ptr<geos::geom::Point> CoordinatesSystem::createPoint( double x, double y ) const
	{
		Coordinate c(x,y);
		return boost::shared_ptr<Point>(_geometryFactory.createPoint(c));
	}



	CoordinatesSystem::CoordinatesSystem(
		SRID srid,
		const std::string& name,
		const std::string& projSequence
	):	_srid(srid),
		_name(name),
		_projSequence(projSequence),
		_projObject(pj_init_plus(projSequence.c_str())),
		_geometryFactory(new geos::geom::PrecisionModel(), srid),
		_degrees(contains(projSequence,"+proj=longlat"))
	{}



	CoordinatesSystem::~CoordinatesSystem() {
		pj_free(_projObject);
	}



	CoordinatesSystem::CoordinatesSystemsTextMap CoordinatesSystem::GetCoordinatesSystemsTextMap()
	{
		CoordinatesSystemsTextMap result;
		BOOST_FOREACH(const CoordinatesSystemsMap::value_type& it, _CoordinatesSystems)
		{
			result.insert(make_pair(it.first, lexical_cast<string>(it.first) +" "+ it.second->getName()));
		}
		return result;
	}



	//////////////////////////////////////////////////////////////////////////
	/// Conversion of a string into a coordinate system pointer.
	/// @param text the text to convert
	/// @return the pointer
	CoordinatesSystem::Type CoordinatesSystem::_stringToPointer( const std::string& text )
	{
		return text.empty() ?
			NULL :
			&GetCoordinatesSystem(lexical_cast<SRID>(text))
		;
	}



	bool CoordinatesSystem::LoadFromRecord(
		CoordinatesSystem::Type& fieldObject,
		ObjectBase& object,
		const Record& record,
		const util::Env& env
	){
		return SimpleObjectFieldDefinition<CoordinatesSystem>::_LoadFromStringWithDefaultValue(
			fieldObject,
			record,
			_stringToPointer,
			Type(NULL)
		);
	}



	void CoordinatesSystem::SaveToFilesMap(
		const Type& fieldObject,
		const ObjectBase& object,
		FilesMap& map
	){
		SimpleObjectFieldDefinition<CoordinatesSystem>::_SaveToFilesMap(
			fieldObject,
			map,
			_pointerToString
		);
	}



	std::string CoordinatesSystem::_pointerToString( const CoordinatesSystem::Type& value )
	{
		if(value == NULL)
		{
			return string();
		}
		return lexical_cast<string>(value->getSRID());
	}



	void CoordinatesSystem::SaveToParametersMap(
		const Type& fieldObject, util::ParametersMap& map, const std::string& prefix, boost::logic::tribool withFiles )
	{
		SimpleObjectFieldDefinition<CoordinatesSystem>::_SaveToParametersMap(
			fieldObject,
			map,
			prefix,
			withFiles,
			_pointerToString
		);
	}



	void CoordinatesSystem::SaveToParametersMap(
		const Type& fieldObject, const ObjectBase& object, util::ParametersMap& map, const std::string& prefix, boost::logic::tribool withFiles )
	{
		SimpleObjectFieldDefinition<CoordinatesSystem>::_SaveToParametersMap(
			fieldObject,
			map,
			prefix,
			withFiles,
			_pointerToString
		);
	}



	void CoordinatesSystem::SaveToDBContent( const Type& fieldObject, const ObjectBase& object, DBContent& content )
	{
		SaveToDBContent(fieldObject, content);
	}



	void CoordinatesSystem::SaveToDBContent( const Type& fieldObject, DBContent& content )
	{
		if(fieldObject == NULL)
		{
			boost::optional<std::string> emptyString;
			content.push_back(Cell(emptyString));
		}
		else
		{
			int i(static_cast<int>(fieldObject->getSRID()));
			content.push_back(Cell(i));
		}
	}



	void CoordinatesSystem::GetLinkedObjectsIds( LinkedObjectsIds& list, const Record& record )
	{

	}



	void CoordinatesSystem::ConversionFilter::filter_rw( geos::geom::Coordinate* c ) const
	{
		if(_source._degrees)
		{
			c->x = Angle::toRadians(c->x);
			c->y = Angle::toRadians(c->y);
		}

		pj_transform(
			_source.getProjObject(),
			_dest.getProjObject(),
			1, 1,
			&c->x, &c->y, NULL
		);

		if(_dest._degrees)
		{
			c->x = Angle::toDegrees(c->x);
			c->y = Angle::toDegrees(c->y);
		}
}	}
