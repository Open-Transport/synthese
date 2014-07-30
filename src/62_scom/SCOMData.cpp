/** SCOMData class header.
	@file SCOMData.h

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

#include "SCOMData.h"

#include "Log.h"

namespace synthese
{
	namespace scom
	{
		using namespace synthese::util;

		//------------------------------------- PUBLIC ------------------------------

		SCOMData::SCOMData ()
		{

		}

		void SCOMData::AddXML(const std::string &data)
		{
			Log::GetInstance().debug("SCOM Data : Add message : " + data);
		}

		int SCOMData::GetWaitingTime(const std::string &borne, const std::string &line, const std::string &destination, int originalWaitingTime)
		{
			return 0;
		}

		// Simple setter
		void SCOMData::SetMaxAge(int maxAge)
		{
			_maxAge = maxAge;
		}

		//------------------------------------- PRIVATE ------------------------------

		void SCOMData::_cleanup()
		{

		}

		void SCOMData::_append(const Data &data)
		{

		}
	}
}
