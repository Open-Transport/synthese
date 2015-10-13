
/** Function class implementation.
	@file Function.cpp

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

#include "Function.h"

#include "Request.h"
#include "MimeTypes.hpp"
#include "SecurityPolicy.h"

#include <sstream>

using namespace std;

namespace synthese
{
	using namespace util;

	namespace util
	{
		class ParametersMap;
	}



	namespace server
	{
		const string Function::PARAMETER_OUTPUT_FORMAT_COMPAT = "of";
		const string Function::PARAMETER_OUTPUT_FORMAT = "output_format";

		util::ParametersMap Function::runWithoutOutput() const
		{
			stringstream fakeStream;
			Request fakeRequest;
			return run(fakeStream, fakeRequest);
		}



		void Function::setOutputFormatFromMap(const ParametersMap& pm, const string& defaultFormat)
		{
			if(pm.isDefined(PARAMETER_OUTPUT_FORMAT))
			{
				_outputFormat = pm.get<string>(PARAMETER_OUTPUT_FORMAT);
			}
			else if(pm.isDefined(PARAMETER_OUTPUT_FORMAT_COMPAT))
			{
				_outputFormat = pm.get<string>(PARAMETER_OUTPUT_FORMAT_COMPAT);
			}
			else
			{
				_outputFormat = defaultFormat;
			}

			// Shortcuts
			if(_outputFormat == "json")
			{
				_outputFormat = MimeTypes::JSON;
			}
			if(_outputFormat == "xml")
			{
				_outputFormat = MimeTypes::XML;
			}
			if(_outputFormat == "csv")
			{
				_outputFormat = MimeTypes::CSV;
			}
		}



		bool Function::outputParametersMap(
			const ParametersMap& pm,
			ostream& stream,
			const string& tag,
			const string& xmlSchemaLocation,
			bool sorted,/* = true */
			const string& xmlUnsortedSchemaLocation /* = "" */
		) const {

			if(_outputFormat == MimeTypes::JSON)
			{
				if(sorted)
				{
					pm.outputJSON(
						stream,
						tag
					);
				}
				else
				{
					(*pm.getSubMaps(tag).begin())->outputJSON(
						stream,
						tag
					);
				}
			}
			else if(_outputFormat == MimeTypes::XML)
			{
				if(sorted)
				{
					pm.outputXML(
						stream,
						tag,
						true,
						xmlSchemaLocation
					);
				}
				else
				{
					(*pm.getSubMaps(tag).begin())->outputXML(
						stream,
						tag,
						true,
						xmlUnsortedSchemaLocation
					);
				}
			}
			else if(_outputFormat == MimeTypes::CSV)
			{
				pm.outputCSV(
					stream,
					tag
				);
			}
			else
			{
				return false;
			}

			return true;
		}



		const string Function::getOutputMimeTypeFromOutputFormat(const string& defaultMime /* = "" */) const
		{
			if(_outputFormat.empty())
			{
				return defaultMime;
			}
			return _outputFormat;
		}

		FunctionAPI Function::getAPI() const
		{
			return FunctionAPI();
		}


		bool
		Function::isAuthorized(const Session* session) const
		{
			return SecurityPolicy::GetInstance().isAuthorized(getFactoryKey(), session);
		}




}	}
