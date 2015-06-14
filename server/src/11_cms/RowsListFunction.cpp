
/** RowsListFunction class implementation.
	@file RowsListFunction.cpp
	@author Gaël Sauvanet
	@date 2011

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

#include "RowsListFunction.hpp"
#include "HTMLModule.h"
#include "RequestException.h"
#include "MimeTypes.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace cms;
	using namespace db;

	template<> const string util::FactorableTemplate<Function,RowsListFunction>::FACTORY_KEY("lr");

	namespace cms
	{
		const string RowsListFunction::PARAMETER_INPUT("t");
		const string RowsListFunction::PARAMETER_TABLE("table");
		const string RowsListFunction::PARAMETER_OPTIONAL("o");
		const string RowsListFunction::PARAMETER_NUMBER("n");

		const std::string RowsListFunction::DATA_RESULTS_SIZE("size");
		const std::string RowsListFunction::DATA_CONTENT("content");

		const string RowsListFunction::DATA_ROW("row");
		const string RowsListFunction::DATA_ROWS("rows");
		const string RowsListFunction::DATA_RANK("rank");



		ParametersMap RowsListFunction::_getParametersMap() const
		{
			ParametersMap pm;
			pm.insert(PARAMETER_INPUT, _input);
			pm.insert(PARAMETER_NUMBER, _n.value_or(0));
			pm.insert(PARAMETER_TABLE, static_cast<int>(_table));
			if(!_outputFormat.empty())
			{
				pm.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			return pm;
		}



		void RowsListFunction::_setFromParametersMap( const util::ParametersMap& map )
		{
			_input = map.getDefault<string>(PARAMETER_INPUT);
			_table = map.getDefault<RegistryTableType>(PARAMETER_TABLE);
			_optional = map.getOptional<string>(PARAMETER_OPTIONAL);
			_n = map.getOptional<size_t>(PARAMETER_NUMBER);
			if (!_input.empty() && !_n)
			{
				throw RequestException("Number of result must be limited");
			}
			setOutputFormatFromMap(map, "");
		}



		util::ParametersMap RowsListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			boost::shared_ptr<DBTableSync> tableSync = DBModule::GetTableSync(_table);

			RowsList result = tableSync->SearchForAutoComplete(_input, _n, _optional);

			ParametersMap pm;

			size_t i(0);
			for (RowsList::const_iterator it = result.begin(); it != result.end(); ++it)
			{
				boost::shared_ptr<ParametersMap> rowPm(new ParametersMap());

				rowPm->insert(DATA_RANK, i++);
				rowPm->insert("name", it->second);
				rowPm->insert("roid", it->first);

				pm.insert(DATA_ROW, rowPm);
			}

			outputParametersMap(
				pm,
				stream,
				DATA_ROWS,
				"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/11_cms/RowsListFunction.xsd"
			);

			return pm;
		}



		bool RowsListFunction::isAuthorized(const Session* session
		) const {
			return true;
		}



		std::string RowsListFunction::getOutputMimeType() const
		{
			return getOutputMimeTypeFromOutputFormat(MimeTypes::XML);
		}
}	}
