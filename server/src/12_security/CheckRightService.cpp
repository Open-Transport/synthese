
//////////////////////////////////////////////////////////////////////////////////////////
///	CheckRightService class implementation.
///	@file CheckRightService.cpp
///	@author Gael Sauvanet
///	@date 2012
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

#include "CheckRightService.hpp"

#include "Profile.h"
#include "SecurityRight.h"
#include "RequestException.h"
#include "Request.h"
#include "UserTableSync.h"
#include "Webpage.h"
#include "MimeTypes.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace cms;
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,security::CheckRightService>::FACTORY_KEY = "CheckRightService";

	namespace security
	{
		const string CheckRightService::PARAMETER_PAGE_ID = "page_id";
		const string CheckRightService::PARAMETER_USER_ID = "user_id";
		const string CheckRightService::PARAMETER_RIGHT_NAME = "name";
		const string CheckRightService::PARAMETER_RIGHT_LEVEL = "level";
		const string CheckRightService::PARAMETER_RIGHT_TYPE = "type";
		const string CheckRightService::PARAMETER_PERIMETER = "perimeter";

		const string CheckRightService::ATTR_VALID = "valid";
		const string CheckRightService::TAG_RESULT = "result";



		ParametersMap CheckRightService::_getParametersMap() const
		{
			ParametersMap map;

			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			if(_user.get())
			{
				map.insert(PARAMETER_USER_ID, _user->getKey());
			}
			if(!_outputFormat.empty())
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			map.insert(PARAMETER_RIGHT_NAME, _right);
			map.insert(PARAMETER_RIGHT_LEVEL, Right::getLevelLabel(_level));
			if(_private)
			{
				map.insert(PARAMETER_RIGHT_TYPE, string("private"));
			}
			else
			{
				map.insert(PARAMETER_RIGHT_TYPE, string("public"));
			}
			map.insert(PARAMETER_PERIMETER, _perimeter);

			return map;
		}



		void CheckRightService::_setFromParametersMap(const ParametersMap& map)
		{
			// User id
			RegistryKeyType userId(
				map.getDefault<RegistryKeyType>(PARAMETER_USER_ID, 0)
			);
			if(userId)
			{
				try
				{
					_user = UserTableSync::GetEditable(userId, *_env);
				}
				catch(ObjectNotFoundException<User>&)
				{
					throw RequestException("No such user");
				}
			}

			optional<RegistryKeyType> pid(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID));
			if(pid) try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(*pid);
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such main page");
			}
			if(!_page.get())
			{
				setOutputFormatFromMap(map, MimeTypes::XML);
			}

			_right = map.getDefault<string>(PARAMETER_RIGHT_NAME);
			_level = Right::getRightLevel(map.getDefault<string>(PARAMETER_RIGHT_LEVEL));
			if(map.getDefault<string>(PARAMETER_RIGHT_TYPE) == "private")
			{
				_private = true;
			}
			else
			{
				_private = false;
			}
			_perimeter = map.getDefault<string>(PARAMETER_PERIMETER, "*");
		}



		ParametersMap CheckRightService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Declarations
			ParametersMap result;
			bool valid = false;

			if(_user.get() && _user.get()->getProfile())
			{
				// 1 Reading of the global right
				boost::shared_ptr<const Right> right =  _user.get()->getProfile()->getRight();
				if (right != NULL)
				{
					if( (_private && (right->getPrivateRightLevel() >= _level)) ||
						(!_private && (right->getPublicRightLevel() >= _level)))
					{
						valid = true;
					}
				}

				// 2 Attempting to find same right
				right = _user.get()->getProfile()->getRight(_right, _perimeter);
				if(right != NULL)
				{
					if( (_private && (right->getPrivateRightLevel() >= _level)) ||
						(!_private && (right->getPublicRightLevel() >= _level)))
					{
						valid = true;
					}
				}
			}

			result.insert(ATTR_VALID, valid);

			// CMS Display
			if(_page.get())
			{
				_page->display(stream, request, result);
			}
			else
			{
				outputParametersMap(
					result,
					stream,
					TAG_RESULT,
					""
				);
			}

			return result;
		}



		bool CheckRightService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string CheckRightService::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat();
		}
}	}
