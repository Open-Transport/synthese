
/** TridentExportFunction class implementation.
	@file TridentExportFunction.cpp
	@author Hugues Romain
	@date 2007

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

#include "Conversion.h"

#include "RequestException.h"

#include "CommercialLineTableSync.h"

#include "TridentExportFunction.h"
#include "TridentFileFormat.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace env;

	template<> const string util::FactorableTemplate<Function,pt::TridentExportFunction>::FACTORY_KEY(
		"tridentexport"
	);

	namespace pt
	{
		const string TridentExportFunction::PARAMETER_LINE_ID("li");
		const string TridentExportFunction::PARAMETER_WITH_TISSEO_EXTENSION("wt");
		
		ParametersMap TridentExportFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_line.get()) map.insert(PARAMETER_LINE_ID, _line->getKey());
			map.insert(PARAMETER_WITH_TISSEO_EXTENSION, _withTisseoExtension);
			return map;
		}

		void TridentExportFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_LINE_ID));
			if (id == UNKNOWN_VALUE)
				throw RequestException("Line id must be specified");

			try
			{
				_line = CommercialLineTableSync::Get(id, *_env);
			}
			catch (...)
			{
				throw RequestException("No such line");
			}

			_withTisseoExtension = map.getBool(PARAMETER_WITH_TISSEO_EXTENSION, false, false, FACTORY_KEY);
		}

		void TridentExportFunction::_run( std::ostream& stream ) const
		{
			TridentFileFormat t(_env.get(), _line->getKey(), _withTisseoExtension);
			t.build(stream);
		}



		bool TridentExportFunction::_isAuthorized(
		) const {
			return true;
		}

		std::string TridentExportFunction::getOutputMimeType() const
		{
			return "text/xml";
		}

		TridentExportFunction::TridentExportFunction()
		:	FactorableTemplate<Function, TridentExportFunction>(),
			_withTisseoExtension(false)
		{
			setEnv(shared_ptr<Env>(new Env));
		}



		void TridentExportFunction::setCommercialLine( boost::shared_ptr<const env::CommercialLine> line )
		{
			_line = line;
		}



		void TridentExportFunction::setWithTisseoExtension( bool value )
		{
			_withTisseoExtension = value;
		}
	}
}
