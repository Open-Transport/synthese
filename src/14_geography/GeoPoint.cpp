
/** GeoPoint class implementation.
	@file GeoPoint.cpp

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


#include "GeoPoint.h"

//#include <boost/math/complex/acos.hpp>

//using namespace boost::math;

namespace synthese
{
	namespace geography
	{
		GeoPoint::GeoPoint (double latitude, double longitude, double ellipsoidHeight)
			: _latitude (latitude)
			, _longitude (longitude)
			, _ellipsoidHeight (ellipsoidHeight)
		{
		}



/*		double GeoPoint::operator-( const GeoPoint& other ) const
		{
			return
				6366 *
				acos(
					cos(Deg2rad(getLatitude())) *
					cos(Deg2rad(other.getLatitude())) *
					cos(Deg2rad(other.getLongitude()) - Deg2rad(getLongitude())) +
					sin(Deg2rad(getLatitude())) *
					sin(Deg2rad(other.getLatitude()))
				);
		}
*/

		double GeoPoint::Deg2rad( double deg )
		{
			const float Pi = 3.141592654f;
			return deg / 180 * Pi;
		}

		double GeoPoint::Rad2deg(double rad)
		{
			const float Pi = 3.141592654f;
			return rad / Pi * 180;
		}

		bool
		operator== ( const GeoPoint& op1, const GeoPoint& op2 )
		{
			return 
			(op1.getLatitude () == op2.getLatitude ()) && 
			(op1.getLongitude () == op2.getLongitude ()) && 
			(op1.getEllipsoidHeight () == op2.getEllipsoidHeight ());
		    
		}



		bool
		operator!= ( const GeoPoint& op1, const GeoPoint& op2 )
		{
			return !(op1 == op2);
		}



		std::ostream&
		operator<< ( std::ostream& os, const GeoPoint& op )
		{
			os << "(" << op.getLatitude () << "," << op.getLongitude () << "," << op.getEllipsoidHeight () << ")";
			return os;
		}



	}
}

