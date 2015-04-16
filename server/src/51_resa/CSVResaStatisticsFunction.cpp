
/** CSVResaStatisticsFunction class implementation.
	@file CSVResaStatisticsFunction.cpp
	@author Hugues
	@date 2009

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

#include "CSVResaStatisticsFunction.h"

#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "Profile.h"
#include "RequestException.h"
#include "Request.h"
#include "ResaRight.h"
#include "ResaStatisticsTableSync.h"
#include "ResaStatisticsAdmin.h"
#include "Session.h"
#include "User.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace pt;

	template<> const string util::FactorableTemplate<Function,resa::CSVResaStatisticsFunction>::FACTORY_KEY("CSVResaStatisticsFunction");

	namespace resa
	{
		const std::string CSVResaStatisticsFunction::PARAM_LINE_ID("li");
		const std::string CSVResaStatisticsFunction::PARAM_SEARCH_START_DATE("sd");
		const std::string CSVResaStatisticsFunction::PARAM_SEARCH_END_DATE("ed");
		const std::string CSVResaStatisticsFunction::PARAM_ROW_STEP("rs");
		const std::string CSVResaStatisticsFunction::PARAM_COL_STEP("cs");

		ParametersMap CSVResaStatisticsFunction::_getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAM_SEARCH_START_DATE, gregorian::to_iso_extended_string(_searchPeriod.begin()));
			m.insert(PARAM_SEARCH_END_DATE, gregorian::to_iso_extended_string(_searchPeriod.last()));
			m.insert(PARAM_ROW_STEP, static_cast<int>(_searchRowStep));
			m.insert(PARAM_COL_STEP, static_cast<int>(_searchColStep));
			if(_line.get()) m.insert(PARAM_LINE_ID, _line->getKey());
			return m;
		}

		void CSVResaStatisticsFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType id(map.get<RegistryKeyType>(PARAM_LINE_ID));
			try
			{
				_line = CommercialLineTableSync::Get(id, *getEnv());
			}
			catch (...)
			{
				throw RequestException("Bad value for line ID");
			}

			if(	!map.getDefault<string>(PARAM_SEARCH_START_DATE).empty() &&
				!map.getDefault<string>(PARAM_SEARCH_END_DATE).empty()
			){
				_searchPeriod = gregorian::date_period(
					gregorian::from_string(map.get<string>(PARAM_SEARCH_START_DATE)),
					gregorian::from_string(map.get<string>(PARAM_SEARCH_END_DATE)) + gregorian::days(1)
				);
			}

			if(_searchPeriod.is_null())
				throw RequestException("Dates invalides");

			if(map.getOptional<string>(PARAM_ROW_STEP))
				_searchRowStep = static_cast<ResaStatisticsTableSync::Step>(map.get<int>(PARAM_ROW_STEP));
			if(map.getOptional<string>(PARAM_COL_STEP))
				_searchColStep = static_cast<ResaStatisticsTableSync::Step>(map.get<int>(PARAM_COL_STEP));
		}



		util::ParametersMap CSVResaStatisticsFunction::run( std::ostream& stream, const Request& request ) const
		{
			ResaStatisticsTableSync::ResaCountSearchResult r(
				ResaStatisticsTableSync::CountCalls(
				_searchPeriod,
				_searchRowStep,
				_searchColStep,
				_line->getKey()
				)	);

			typedef map<ResaStatisticsTableSync::ResaCountSearchResult::mapped_type::key_type, size_t> _Cols;
			_Cols cols;
			BOOST_FOREACH(const ResaStatisticsTableSync::ResaCountSearchResult::value_type& row, r)
			{
				BOOST_FOREACH(const ResaStatisticsTableSync::ResaCountSearchResult::mapped_type::value_type& cell, row.second)
				{
					if(cols.find(cell.first) == cols.end())
					{
						cols.insert(make_pair(cell.first, 0));
					}
				}
			}

			bool hasRowStep(_searchRowStep != ResaStatisticsTableSync::NO_STEP);
			bool hasColStep(_searchColStep != ResaStatisticsTableSync::NO_STEP);

			if(hasRowStep)
			{
				stream << ResaStatisticsAdmin::GetColumnName(_searchRowStep) << ";";
			}
			if(hasColStep)
			{
				BOOST_FOREACH(const _Cols::value_type& col, cols)
				{
					stream << col.first << ";";
				}
			}
			stream << "Total" << "\r\n";

			BOOST_FOREACH(const ResaStatisticsTableSync::ResaCountSearchResult::value_type& row, r)
			{
				size_t rowSum(0);
				if(hasRowStep)
				{
					stream << row.first << ";";
				}

				if(hasColStep)
				{
					BOOST_FOREACH(_Cols::value_type& col, cols)
					{
						ResaStatisticsTableSync::ResaCountSearchResult::mapped_type::const_iterator it(row.second.find(col.first));
						if(it == row.second.end())
						{
							stream << 0 << ";";
						}
						else
						{
							stream << it->second << ";";
							rowSum += it->second;
							col.second += it->second;
						}
					}
				}
				else
				{
					rowSum = row.second.begin()->second;
					cols.begin()->second += rowSum;
				}

				stream << rowSum << "\r\n";
			}

			if(r.size() > 1)
			{
				size_t totalSum(0);
				if(hasRowStep)
				{
					stream << "Total;";
				}

				if(hasColStep)
				{
					BOOST_FOREACH(const _Cols::value_type col, cols)
					{
						stream << col.second << ";";
						totalSum += col.second;
					}
				}

				stream << totalSum << "\r\n";
			}

			return util::ParametersMap();
		}



		bool CSVResaStatisticsFunction::isAuthorized(const server::Session* session) const
		{
			if (!_line.get())
				return false;

			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_line->getKey()));
		}



		std::string CSVResaStatisticsFunction::getOutputMimeType() const
		{
			return "text/csv";
		}



		CSVResaStatisticsFunction::CSVResaStatisticsFunction():
			_searchPeriod(
				gregorian::date(
					gregorian::day_clock::local_day().month() == 1 ? gregorian::day_clock::local_day().year() - 1 : static_cast<size_t>(gregorian::day_clock::local_day().year()),
					gregorian::day_clock::local_day().month() == 1 ? 12 : gregorian::day_clock::local_day().month() - 1,
					1
				),
				gregorian::date(
					gregorian::day_clock::local_day().year(),
					gregorian::day_clock::local_day().month(),
					1
				)
			),
			_searchRowStep(ResaStatisticsTableSync::SERVICE_STEP),
			_searchColStep(ResaStatisticsTableSync::DATE_STEP)
		{

		}



		void CSVResaStatisticsFunction::setLine( boost::shared_ptr<const pt::CommercialLine> value )
		{
			_line = value;
		}



		void CSVResaStatisticsFunction::setPeriod( boost::gregorian::date_period value )
		{
			_searchPeriod = value;
		}



		void CSVResaStatisticsFunction::setRowStep( ResaStatisticsTableSync::Step value )
		{
			_searchRowStep = value;
		}



		void CSVResaStatisticsFunction::setColStep( ResaStatisticsTableSync::Step value )
		{
			_searchColStep = value;
		}



		std::string CSVResaStatisticsFunction::getFileName() const
		{
			return _line.get() ? ("stats-resa-"+ _line->getShortName() +".csv") : "stats-resa.csv";
		}
	}
}
