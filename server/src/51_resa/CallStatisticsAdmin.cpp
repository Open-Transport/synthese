//////////////////////////////////////////////////////////////////////////
/// CallStatisticsAdmin class implementation.
///	@file CallStatisticsAdmin.cpp
///	@author Hugues
///	@date 2009
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

#include "CallStatisticsAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "AdminFunctionRequest.hpp"
#include "ResultHTMLTable.h"
#include "RequestException.h"
#include "SearchFormHTMLTable.h"
#include "Profile.h"
#include "User.h"

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace resa;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, CallStatisticsAdmin>::FACTORY_KEY("CallStatisticsAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<CallStatisticsAdmin>::ICON("chart_pie.png");
		template<> const string AdminInterfaceElementTemplate<CallStatisticsAdmin>::DEFAULT_TITLE("Statistiques appels");
	}

	namespace resa
	{
		const string CallStatisticsAdmin::PARAM_SEARCH_START_DATE("sd");
		const string CallStatisticsAdmin::PARAM_SEARCH_END_DATE("ed");
		const string CallStatisticsAdmin::PARAM_SEARCH_STEP("st");



		CallStatisticsAdmin::CallStatisticsAdmin():
			AdminInterfaceElementTemplate<CallStatisticsAdmin>(),
			_searchPeriod(
				gregorian::date(
					gregorian::day_clock::local_day().month() == 1 ?
						gregorian::day_clock::local_day().year() - 1 :
						static_cast<size_t>(gregorian::day_clock::local_day().year()),
					gregorian::day_clock::local_day().month() == 1 ?
						12 :
						gregorian::day_clock::local_day().month() - 1,
					1
				),
				gregorian::date(
					gregorian::day_clock::local_day().year(),
					gregorian::day_clock::local_day().month(),
					1
				)
			),
			_searchStep(CallStatisticsTableSync::MONTH_STEP)
		{
		}



		void CallStatisticsAdmin::setFromParametersMap(
			const ParametersMap& map
		){
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

			if(map.getOptional<string>(PARAM_SEARCH_STEP))
				_searchStep = static_cast<CallStatisticsTableSync::Step>(map.get<int>(PARAM_SEARCH_STEP));
		}



		ParametersMap CallStatisticsAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAM_SEARCH_START_DATE, gregorian::to_iso_extended_string(_searchPeriod.begin()));
			m.insert(PARAM_SEARCH_END_DATE, gregorian::to_iso_extended_string(_searchPeriod.last()));
			m.insert(PARAM_SEARCH_STEP, static_cast<int>(_searchStep));
			return m;
		}



		bool CallStatisticsAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ResaRight>(READ);
		}



		void CallStatisticsAdmin::display(
			ostream& stream,
			const Request& request
		) const	{

			AdminFunctionRequest<CallStatisticsAdmin> searchRequest(request, *this);

			{
				SearchFormHTMLTable t(searchRequest.getHTMLForm());
				stream << "<h1>Recherche</h1>";
				stream << t.open();
				stream << t.cell("Date début", t.getForm().getCalendarInput(PARAM_SEARCH_START_DATE, _searchPeriod.begin()));
				stream << t.cell("Date fin", t.getForm().getCalendarInput(PARAM_SEARCH_END_DATE, _searchPeriod.last()));
				stream << t.cell(
					"Pas",
					t.getForm().getSelectInput(
						PARAM_SEARCH_STEP,
						_GetStepsVector(),
						optional<CallStatisticsTableSync::Step>(_searchStep)
				)	);
				stream << t.close();
			}

			{
				CallStatisticsTableSync::CallCountSearchResult r(
					CallStatisticsTableSync::CountCalls(
					_searchPeriod,
					_searchStep,
					false
					)	);

				typedef set<CallStatisticsTableSync::CallCountSearchResult::mapped_type::key_type> _Cols;
				_Cols cols;
				BOOST_FOREACH(const CallStatisticsTableSync::CallCountSearchResult::value_type& type, r)
				{
					BOOST_FOREACH(const CallStatisticsTableSync::CallCountSearchResult::mapped_type::value_type step, type.second)
					{
						if(cols.find(step.first) == cols.end())
							cols.insert(step.first);
					}
				}

				HTMLTable::ColsVector v;
				v.push_back("Type");
				v.push_back("Type");
				BOOST_FOREACH(const _Cols::value_type& col, cols)
				{
					v.push_back(col);
				}
				v.push_back("Total");
				HTMLTable t(v, ResultHTMLTable::CSS_CLASS);
				stream << "<h1>Statistiques centre d'appels</h1>";
				stream << t.open();

				BOOST_FOREACH(const CallStatisticsTableSync::CallCountSearchResult::value_type& type, r)
				{
					size_t typeSum(0);

					stream << t.row();
					stream << t.col() << ResaDBLog::GetIcon(type.first);
					stream << t.col() << ResaDBLog::GetText(type.first);

					BOOST_FOREACH(const _Cols::value_type col, cols)
					{
						stream << t.col();

						CallStatisticsTableSync::CallCountSearchResult::mapped_type::const_iterator it(type.second.find(col));
						if(it == type.second.end())
						{
							stream << 0;
						}
						else
						{
							stream << it->second;
							typeSum += it->second;
						}
					}

					stream << t.col() << typeSum;
				}

				stream << t.close();
			}

			{
				CallStatisticsTableSync::CallCountSearchResult r(
					CallStatisticsTableSync::CountCalls(
					_searchPeriod,
					_searchStep,
					true
				)	);

				typedef set<CallStatisticsTableSync::CallCountSearchResult::mapped_type::key_type> _Cols;
				_Cols cols;
				BOOST_FOREACH(const CallStatisticsTableSync::CallCountSearchResult::value_type& type, r)
				{
					BOOST_FOREACH(const CallStatisticsTableSync::CallCountSearchResult::mapped_type::value_type step, type.second)
					{
						if(cols.find(step.first) == cols.end())
							cols.insert(step.first);
					}
				}

				HTMLTable::ColsVector v;
				v.push_back("Type");
				v.push_back("Type");
				BOOST_FOREACH(const _Cols::value_type& col, cols)
				{
					v.push_back(col);
				}
				v.push_back("Total");
				HTMLTable t(v, ResultHTMLTable::CSS_CLASS);
				stream << "<h1>Statistiques auto-réservation</h1>";
				stream << t.open();

				BOOST_FOREACH(const CallStatisticsTableSync::CallCountSearchResult::value_type& type, r)
				{
					size_t typeSum(0);

					stream << t.row();
					stream << t.col() << ResaDBLog::GetIcon(type.first);
					stream << t.col() << ResaDBLog::GetText(type.first);

					BOOST_FOREACH(const _Cols::value_type col, cols)
					{
						stream << t.col();

						CallStatisticsTableSync::CallCountSearchResult::mapped_type::const_iterator it(type.second.find(col));
						if(it == type.second.end())
						{
							stream << 0;
						}
						else
						{
							stream << it->second;
							typeSum += it->second;
						}
					}

					stream << t.col() << typeSum;
				}

				stream << t.close();
			}

		}



		CallStatisticsAdmin::_StepsVector CallStatisticsAdmin::_GetStepsVector()
		{
			_StepsVector result;
			result.push_back(make_pair(CallStatisticsTableSync::HOUR_STEP, "Tranche horaire"));
			result.push_back(make_pair(CallStatisticsTableSync::DAY_STEP, "Jour"));
			result.push_back(make_pair(CallStatisticsTableSync::WEEK_DAY_STEP, "Jour de semaine"));
			result.push_back(make_pair(CallStatisticsTableSync::WEEK_STEP, "Semaine"));
			result.push_back(make_pair(CallStatisticsTableSync::MONTH_STEP, "Mois"));
			result.push_back(make_pair(CallStatisticsTableSync::YEAR_STEP, "Année"));
			return result;
		}
	}
}
