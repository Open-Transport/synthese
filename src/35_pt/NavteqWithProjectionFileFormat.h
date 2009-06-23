
/** NavteqWithProjectionFileFormat class header.
	@file RoadTableSync.h

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

#ifndef SYNTHESE_NavteqWithProjectionFileFormat_H__
#define SYNTHESE_NavteqWithProjectionFileFormat_H__

#include "FileFormatTemplate.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}
	namespace road
	{
		class Address;
	}
	namespace geography
	{
		class City;
	}
	
	namespace pt
	{

		//////////////////////////////////////////////////////////////////////////
		/// Navteq with projection file format.
		/// @ingroup m35
		//////////////////////////////////////////////////////////////////////////
		class NavteqWithProjectionFileFormat
		:	public impex::FileFormatTemplate<NavteqWithProjectionFileFormat>
		{
		public:
			static const std::string FILE_NODES;
			static const std::string FILE_STREETS;
			static const std::string FILE_MTDAREA;
		    
		private:
			static const std::string _FIELD_ID;
			static const std::string _FIELD_X;
			static const std::string _FIELD_Y;
			static const std::string _FIELD_LINK_ID;
			static const std::string _FIELD_ST_NAME;
			static const std::string _FIELD_L_AREA_ID;
			static const std::string _FIELD_R_AREA_ID;
			static const std::string _FIELD_REF_IN_ID;
			static const std::string _FIELD_NREF_IN_ID;
			static const std::string _FIELD_AREA_ID;
			static const std::string _FIELD_AREACODE_3;
			static const std::string _FIELD_GOVT_CODE;
			static const std::string _FIELD_ADMIN_LVL;

			typedef std::map<std::string, road::Address*> _AddressesMap;
			typedef std::map<std::string, geography::City*> _CitiesMap;

			_AddressesMap _navteqAddressses;
			_CitiesMap _citiesMap;

		protected:

			virtual bool _controlPathsMap(
				const FilePathsMap& paths
			);
			
			virtual void _parse(
				const std::string& path,
				std::ostream& os,
				std::string key
			);

	
		
		public:

			NavteqWithProjectionFileFormat(
				util::Env* env = NULL
			);
			~NavteqWithProjectionFileFormat();

			/** -> 
			 */
			virtual void build(std::ostream& os);
			
			virtual void save(std::ostream& os) const;

		private:
		    
		};
	}
}

#endif
