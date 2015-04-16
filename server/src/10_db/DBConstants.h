////////////////////////////////////////////////////////////////////////////////
/// DB Constants header.
///	@file DBConstants.h
///	@author Marc Jambert
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_db_DBConstants_h__
#define SYNTHESE_db_DBConstants_h__

#include <string>


namespace synthese
{
	namespace db
	{
		/** @addtogroup m10
			@{
		*/

		static const std::string TABLE_COL_ID("id");
		static const std::string TABLE_COL_GEOMETRY("geometry");

		static const std::string CONFIG_TABLE_NAME ("t999_config");
		static const std::string CONFIG_TABLE_COL_PARAMNAME ("param_name");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE ("param_value");

		/** @} */
	}
}

#endif // SYNTHESE_db_DBConstants_h__
