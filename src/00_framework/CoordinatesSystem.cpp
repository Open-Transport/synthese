
/** CoordinatesSystem class implementation.
	@file CoordinatesSystem.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include <geos/geom/Point.h>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <geos/algorithm/Angle.h>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace boost::algorithm;
using namespace geos::algorithm;

namespace synthese
{
	const string CoordinatesSystem::_INSTANCE_COORDINATES_SYSTEM("instance_coordinates_system");
	const CoordinatesSystem* CoordinatesSystem::_instanceCoordinatesSystem(NULL);
	CoordinatesSystem::CoordinatesSystemsMap CoordinatesSystem::_coordinates_systems;



	boost::shared_ptr<geos::geom::Point> CoordinatesSystem::convertPoint(
		const geos::geom::Point& source
	) const	{
		return static_pointer_cast<Point,Geometry>(convertGeometry(static_cast<const Geometry&>(source)));
	}



	boost::shared_ptr<geos::geom::Geometry> CoordinatesSystem::convertGeometry( const geos::geom::Geometry& source ) const
	{
		shared_ptr<geos::geom::Geometry> result(_geometryFactory.createGeometry(&source));

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
		CoordinatesSystemsMap::const_iterator it(_coordinates_systems.find(srid));
		if(it == _coordinates_systems.end())
		{
			throw CoordinatesSystemNotFoundException(srid);
		}
		return it->second;
	}



	void CoordinatesSystem::AddCoordinatesSystem( SRID srid, const std::string& name, const std::string& projSequence )
	{
		_coordinates_systems[srid] = CoordinatesSystem(srid,name, projSequence);
	}



	void CoordinatesSystem::SetDefaultCoordinatesSystems( SRID instanceSRID )
	{
		_instanceCoordinatesSystem = &GetCoordinatesSystem(instanceSRID);
	}



	boost::shared_ptr<geos::geom::Point> CoordinatesSystem::createPoint( double x, double y ) const
	{
		Coordinate c(x,y);
		return shared_ptr<Point>(_geometryFactory.createPoint(c));
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
