
/** Right class implementation.
	@file Right.cpp

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

#include "12_security/Right.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace security
	{
		Right::Right()
		    : FactoryBase<Right>()
			, _privateRightLevel(FORBIDDEN)
			, _publicRightLevel(FORBIDDEN)
		{	}



		Right::~Right()
		{	}



		RightLevel Right::getPrivateRightLevel() const
		{
			return _privateRightLevel;
		}



		RightLevel Right::getPublicRightLevel() const
		{
			return _publicRightLevel;
		}


		std::string Right::getLevelLabel(RightLevel level)
		{
			switch (level)
			{
				case FORBIDDEN: return "Interdit";
				case USE: return "Utilisation";
				case READ: return "Lecture";
				case CANCEL: return "Annulation";
				case WRITE: return "Lecture/Ecriture";
				case DELETE_RIGHT: return "Contrôle total";
				case UNKNOWN_RIGHT_LEVEL: return "Inconnu";
			}
			return "";
		}



		RightLevel Right::getRightLevel(const std::string level)
		{
			if(level == "FORBIDDEN")
				return FORBIDDEN;
			else if(level == "USE")
				return USE;
			else if(level == "READ")
				return READ;
			else if(level == "WRITE")
				return WRITE;
			else if(level == "DELETE_RIGHT")
				return DELETE_RIGHT;
			else 
				return UNKNOWN_RIGHT_LEVEL;
		}



		void Right::setPrivateLevel( const RightLevel& level )
		{
			_privateRightLevel = level;
		}



		void Right::setPublicLevel( const RightLevel& level )
		{
			_publicRightLevel = level;
		}



		void Right::setParameter( const std::string& parameter )
		{
			_parameter = parameter;
		}



		const std::string& Right::getParameter() const
		{
			return _parameter;
		}
	}
}

