
/** Importer class implementation.
	@file Importer.cpp

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

#include "Importer.hpp"

namespace synthese
{
	namespace impex
	{
		Importer::Logger::Logger(
			Level minLevel
		):	_minLevel(minLevel),
			_maxLoggedLevel(ALL)
		{}



		void Importer::Logger::log( Level level, const std::string& content )
		{
			if(level >= _minLevel)
			{
				_entries.push_back(
					Entry(level, content)
				);
				if(level > _maxLoggedLevel)
				{
					_maxLoggedLevel = level;
				}
			}
		}



		void Importer::Logger::output(
			std::ostream& stream
		) const {
			BOOST_FOREACH(const Entry& entry, _entries)
			{
				switch(entry.level)
				{
				case DEBG: stream << "DEBG"; break;
				case LOAD: stream << "LOAD"; break;
				case CREA: stream << "CREA"; break;
				case INFO: stream << "INFO"; break;
				case WARN: stream << "WARN"; break;
				case NOTI: stream << "NOTI"; break;
				case ERROR: stream << "ERR "; break;
				case ALL: stream << "ALL "; break;
				case NOLOG: stream << "NOLOG "; break;
				}
				stream << " " << entry.content << "<br />";
			}
		}
}	}

