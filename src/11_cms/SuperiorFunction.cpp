
/** SuperiorFunction class implementation.
	@file SuperiorFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "SuperiorFunction.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::SuperiorFunction>::FACTORY_KEY(">");

	namespace cms
	{
		const string SuperiorFunction::PARAMETER_L("l");
		const string SuperiorFunction::PARAMETER_R("r");


		ParametersMap SuperiorFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_L, _left);
			map.insert(PARAMETER_R, _right);
			return map;
		}



		void SuperiorFunction::_setFromParametersMap(const ParametersMap& map)
		{

            std::string leftValue(ParametersMap::Trim(map.getValue(PARAMETER_L)));
            try
            {
                _left = boost::lexical_cast<double>(leftValue);
            }
            catch(boost::bad_lexical_cast&)
            {
                _leftString = leftValue;
            }

            std::string rightValue(ParametersMap::Trim(map.getValue(PARAMETER_R)));
            try
            {
                _right = boost::lexical_cast<double>(rightValue);
            }
            catch(boost::bad_lexical_cast&)
            {
                _rightString = rightValue;
            }
		}



		util::ParametersMap SuperiorFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
            if (_leftString.empty() && _rightString.empty())
            {
                stream << (_left > _right);
            }
            else
            {
                stream << (_leftString > _rightString);
            }
                
			return util::ParametersMap();
		}



		bool SuperiorFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string SuperiorFunction::getOutputMimeType() const
		{
			return "text/plain";
		}



		SuperiorFunction::SuperiorFunction():
            _left(0), _right(0), _leftString(""), _rightString("")
		{}
}	}
