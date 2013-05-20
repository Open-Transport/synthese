
//////////////////////////////////////////////////////////////////////////////////////////
///	FileFormatsService class implementation.
///	@file FileFormatsService.cpp
///	@author hromain
///	@date 2013
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "FileFormatsService.hpp"

#include "FileFormat.h"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,impex::FileFormatsService>::FACTORY_KEY = "file_formats";
	
	namespace impex
	{
		const string FileFormatsService::TAG_FORMAT = "format";
		


		ParametersMap FileFormatsService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void FileFormatsService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap FileFormatsService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			
			BOOST_FOREACH(boost::shared_ptr<FileFormat> format, Factory<FileFormat>::GetNewCollection())
			{
				boost::shared_ptr<ParametersMap> formatPM(new ParametersMap);
				format->toParametersMap(*formatPM);
				map.insert(TAG_FORMAT, formatPM);
			}

			return map;
		}
		
		
		
		bool FileFormatsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string FileFormatsService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
