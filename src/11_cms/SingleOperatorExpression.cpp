
/** SingleOperatorExpression class implementation.
	@file SingleOperatorExpression.cpp

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

#include "SingleOperatorExpression.hpp"

#include "ModuleClass.h"
#include "WebsiteConfig.hpp"

#include <cmath>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace server;
	using namespace util;
	
	namespace cms
	{
		SingleOperatorExpression::SingleOperatorExpression(
			boost::shared_ptr<Expression> operand,
			const Operator& op
		):	_operand(operand),
			_operator(op)
		{}



		std::string SingleOperatorExpression::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			std::string value(_operand->eval(request, additionalParametersMap, page, variables));
			trim(value);

			switch(_operator)
			{
			// Double operators
			case SQRT:
			case NEG:
				try
				{
					double valueDbl(lexical_cast<double>(value));

					switch(_operator)
					{
					case SQRT:
						return lexical_cast<string>(sqrt(valueDbl));

					case NEG:
						return lexical_cast<string>(-valueDbl);
					default:
						break;
					}
				}
				catch(bad_lexical_cast&)
				{
					return string();
				}

			// ID operators
			case DECODE_TABLE:
				try
				{
					RegistryKeyType id(lexical_cast<RegistryKeyType>(value));

					switch(_operator)
					{
					case DECODE_TABLE:
						return lexical_cast<string>(decodeTableId(id));

					default:
						break;
					}
				}
				catch(bad_lexical_cast&)
				{
					return string();
				}

			// Type independent operators
			case NOT:
				if(value.empty() || value == "0")
				{
					return "1";
				}
				else
				{
					return "0";
				}

			case GLOBAL:
				return ModuleClass::GetParameter(value);

			case READ_CONFIG:
				WebsiteConfig* config(
					page.getRoot()->getConfig()
				);
				if(config)
				{
					stringstream str;
					config->get<ConfigScript>().display(
						str,
						request,
						additionalParametersMap,
						page,
						variables
					);
				}
				return string();

			}

			return string();
		}



		boost::optional<SingleOperatorExpression::Operator> SingleOperatorExpression::ParseOperator(
			std::string::const_iterator& it,
			const std::string::const_iterator end
		){
			if(	CompareText(it, end, "!") ||
				CompareText(it, end, "~not")
			){
				return NOT;
			}
			if(	CompareText(it, end, "-"))
			{
				return NEG;
			}
			if(	CompareText(it, end, "~sqrt"))
			{
				return SQRT;
			}
			if(	CompareText(it, end, "~global"))
			{
				return GLOBAL;
			}
			if(	CompareText(it, end, "~read_config"))
			{
				return READ_CONFIG;
			}
			if(	CompareText(it, end, "~decode_table"))
			{
				return DECODE_TABLE;
			}
			return optional<Operator>();
		}
}	}

