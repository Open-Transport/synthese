
//////////////////////////////////////////////////////////////////////////
/// ResaStatisticsAdmin class implementation.
///	@file ResaStatisticsAdmin.cpp
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

#include "ResaStatisticsAdmin.h"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "User.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "ResaStatisticsTableSync.h"
#include "CSVResaStatisticsFunction.h"
#include "RequestException.h"
#include "ResultHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "SearchFormHTMLTable.h"
#include "Profile.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace resa;
	using namespace pt;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ResaStatisticsAdmin>::FACTORY_KEY("ResaStatisticsAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ResaStatisticsAdmin>::ICON("chart_pie.png");
		template<> const string AdminInterfaceElementTemplate<ResaStatisticsAdmin>::DEFAULT_TITLE("Statistiques réservations");
	}

	namespace resa
	{
		const std::string ResaStatisticsAdmin::PARAM_LINE_ID("li");
		const std::string ResaStatisticsAdmin::PARAM_SEARCH_START_DATE("sd");
		const std::string ResaStatisticsAdmin::PARAM_SEARCH_END_DATE("ed");
		const std::string ResaStatisticsAdmin::PARAM_ROW_STEP("rs");
		const std::string ResaStatisticsAdmin::PARAM_COL_STEP("cs");



		ResaStatisticsAdmin::ResaStatisticsAdmin():
			AdminInterfaceElementTemplate<ResaStatisticsAdmin>(),
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
		{ }



		void ResaStatisticsAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			RegistryKeyType id(map.get<RegistryKeyType>(PARAM_LINE_ID));
			try
			{
				_line = CommercialLineTableSync::Get(id, _getEnv());
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



		ParametersMap ResaStatisticsAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAM_SEARCH_START_DATE, gregorian::to_iso_extended_string(_searchPeriod.begin()));
			m.insert(PARAM_SEARCH_END_DATE, gregorian::to_iso_extended_string(_searchPeriod.last()));
			m.insert(PARAM_ROW_STEP, static_cast<int>(_searchRowStep));
			m.insert(PARAM_COL_STEP, static_cast<int>(_searchColStep));
			if(_line.get()) m.insert(PARAM_LINE_ID, _line->getKey());
			return m;
		}



		bool ResaStatisticsAdmin::isAuthorized(
			const security::User& user
		) const	{
			if (!_line.get())
				return false;

			return user.getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_line->getKey()));
		}



		void ResaStatisticsAdmin::display(
			ostream& stream,
			const Request& request
		) const	{

			stream << "<h1>Requête</h1>";

			AdminFunctionRequest<ResaStatisticsAdmin> searchRequest(request, *this);

			SearchFormHTMLTable rt(searchRequest.getHTMLForm());
			stream << rt.open();
			stream << rt.cell("Date début", rt.getForm().getCalendarInput(PARAM_SEARCH_START_DATE, _searchPeriod.begin()));
			stream << rt.cell("Date fin", rt.getForm().getCalendarInput(PARAM_SEARCH_END_DATE, _searchPeriod.last()));
			stream << rt.cell(
				"Lignes",
				rt.getForm().getSelectInput(PARAM_ROW_STEP, _GetStepsVector(), optional<ResaStatisticsTableSync::Step>(_searchRowStep))
			);
			stream << rt.cell(
				"Colonnes",
				rt.getForm().getSelectInput(PARAM_COL_STEP, _GetStepsVector(), optional<ResaStatisticsTableSync::Step>(_searchColStep))
			);
			stream << rt.close();

			stream << "<h1>Résultats</h1>";

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

			HTMLTable::ColsVector v;
			if(hasRowStep)
			{
				v.push_back(GetColumnName(_searchRowStep));
			}
			if(hasColStep)
			{
				BOOST_FOREACH(const _Cols::value_type& col, cols)
				{
					v.push_back(col.first);
				}
			}
			v.push_back("Total");

			HTMLTable t(v, ResultHTMLTable::CSS_CLASS);
			stream << t.open();

			BOOST_FOREACH(const ResaStatisticsTableSync::ResaCountSearchResult::value_type& row, r)
			{
				size_t rowSum(0);
				stream << t.row();
				if(hasRowStep)
				{
					stream << t.col(1, string(), true) << row.first;
				}

				if(hasColStep)
				{
					BOOST_FOREACH(_Cols::value_type& col, cols)
					{
						stream << t.col();

						ResaStatisticsTableSync::ResaCountSearchResult::mapped_type::const_iterator it(row.second.find(col.first));
						if(it == row.second.end())
						{
							stream << 0;
						}
						else
						{
							stream << it->second;
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

				stream << t.col() << rowSum;
			}

			if(r.size() > 1)
			{
				size_t totalSum(0);
				stream << t.row();
				if(hasRowStep)
				{
					stream << t.col(1, string(), true) << "Total";
				}

				if(hasColStep)
				{
					BOOST_FOREACH(const _Cols::value_type col, cols)
					{
						stream << t.col() << col.second;
						totalSum += col.second;
					}
				}
				stream << t.col() << totalSum;
			}

			stream << t.close();

			stream << "<h1>Export CSV</h1>";

			StaticFunctionRequest<CSVResaStatisticsFunction> csvRequest(request, true);
			csvRequest.getFunction()->setLine(_line);
			csvRequest.getFunction()->setPeriod(_searchPeriod);
			csvRequest.getFunction()->setRowStep(_searchRowStep);
			csvRequest.getFunction()->setColStep(_searchColStep);

			stream << "<p>" << HTMLModule::getLinkButton(csvRequest.getURL(), "Export CSV", string(), "/admin/img/page_white_go.png") << "</p>";

		}



		std::string ResaStatisticsAdmin::GetColumnName( ResaStatisticsTableSync::Step step )
		{
			if(step == ResaStatisticsTableSync::SERVICE_STEP) return "Service";
			if(step == ResaStatisticsTableSync::DATE_STEP) return "Date";
			if(step == ResaStatisticsTableSync::HOUR_STEP) return "Heure";
			if(step == ResaStatisticsTableSync::WEEK_DAY_STEP) return "Jour de la semaine";
			if(step == ResaStatisticsTableSync::MONTH_STEP) return "Mois";
			if(step == ResaStatisticsTableSync::YEAR_STEP) return "Année";
			if(step == ResaStatisticsTableSync::DEPARTURE_STOP_STEP) return "Arrêt de départ";
			if(step == ResaStatisticsTableSync::DEPARTURE_CITY_STEP) return "Commune de départ";
			if(step == ResaStatisticsTableSync::ARRIVAL_STOP_STEP) return "Arrêt d'arrivée";
			if(step == ResaStatisticsTableSync::ARRIVAL_CITY_STEP) return "Commune d'arrivée";
			if(	step == ResaStatisticsTableSync::RESERVATION_DELAY_10_MIN_STEP ||
				step == ResaStatisticsTableSync::RESERVATION_DELAY_30_MIN_STEP ||
				step == ResaStatisticsTableSync::RESERVATION_DELAY_60_MIN_STEP
				)	return "Délai de réservation";
			return string();
		}



		std::string ResaStatisticsAdmin::getTitle() const
		{
			if(_line.get())
			{
				stringstream s;
				s <<
					"<span class=\"linesmall " << _line->getStyle() << "\">" <<
					_line->getShortName() <<
					"</span>"
				;
				return s.str();
			}
			return DEFAULT_TITLE;
		}



		void ResaStatisticsAdmin::setCommercialLine( boost::shared_ptr<pt::CommercialLine> value )
		{
			_line = const_pointer_cast<const CommercialLine>(value);
		}



		void ResaStatisticsAdmin::setCommercialLineC( boost::shared_ptr<const pt::CommercialLine> value )
		{
			_line = value;
		}



		boost::shared_ptr<const pt::CommercialLine> ResaStatisticsAdmin::getCommercialLine() const
		{
			return _line;
		}



		bool ResaStatisticsAdmin::_hasSameContent( const AdminInterfaceElement& other ) const
		{
			const ResaStatisticsAdmin& bother(static_cast<const ResaStatisticsAdmin&>(other));
			return
				_line.get() &&
				bother._line.get() &&
				_line->getKey() == bother._line->getKey();
		}



		ResaStatisticsAdmin::_StepsVector ResaStatisticsAdmin::_GetStepsVector()
		{
			_StepsVector v;
			v.push_back(make_pair(ResaStatisticsTableSync::NO_STEP, "(total uniquement)"));
			v.push_back(make_pair(ResaStatisticsTableSync::SERVICE_STEP, GetColumnName(ResaStatisticsTableSync::SERVICE_STEP)));
			v.push_back(make_pair(ResaStatisticsTableSync::DATE_STEP, GetColumnName(ResaStatisticsTableSync::DATE_STEP)));
			v.push_back(make_pair(ResaStatisticsTableSync::HOUR_STEP, GetColumnName(ResaStatisticsTableSync::HOUR_STEP)));
			v.push_back(make_pair(ResaStatisticsTableSync::WEEK_DAY_STEP, GetColumnName(ResaStatisticsTableSync::WEEK_DAY_STEP)));
			v.push_back(make_pair(ResaStatisticsTableSync::MONTH_STEP, GetColumnName(ResaStatisticsTableSync::MONTH_STEP)));
			v.push_back(make_pair(ResaStatisticsTableSync::YEAR_STEP, GetColumnName(ResaStatisticsTableSync::YEAR_STEP)));
			v.push_back(make_pair(ResaStatisticsTableSync::DEPARTURE_STOP_STEP, GetColumnName(ResaStatisticsTableSync::DEPARTURE_STOP_STEP)));
			v.push_back(make_pair(ResaStatisticsTableSync::DEPARTURE_CITY_STEP, GetColumnName(ResaStatisticsTableSync::DEPARTURE_CITY_STEP)));
			v.push_back(make_pair(ResaStatisticsTableSync::ARRIVAL_STOP_STEP, GetColumnName(ResaStatisticsTableSync::ARRIVAL_STOP_STEP)));
			v.push_back(make_pair(ResaStatisticsTableSync::ARRIVAL_CITY_STEP, GetColumnName(ResaStatisticsTableSync::ARRIVAL_CITY_STEP)));
			v.push_back(make_pair(ResaStatisticsTableSync::RESERVATION_DELAY_10_MIN_STEP, "Délai de réservation par 10 minutes"));
			v.push_back(make_pair(ResaStatisticsTableSync::RESERVATION_DELAY_30_MIN_STEP, "Délai de réservation par 30 minutes"));
			v.push_back(make_pair(ResaStatisticsTableSync::RESERVATION_DELAY_60_MIN_STEP, "Délai de réservation par 60 minutes"));
			return v;
		}
	}
}
