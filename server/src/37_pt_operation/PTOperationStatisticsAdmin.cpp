
//////////////////////////////////////////////////////////////////////////
/// PTOperationStatisticsAdmin class implementation.
///	@file PTOperationStatisticsAdmin.cpp
///	@author Hugues Romain
///	@date 2011
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

#include "PTOperationStatisticsAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTOperationModule.hpp"
#include "User.h"
#include "GlobalRight.h"
#include "AdminFunctionRequest.hpp"
#include "SearchFormHTMLTable.h"
#include "RequestException.h"

using namespace std;
using namespace boost;


namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt_operation;
	using namespace html;


	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTOperationStatisticsAdmin>::FACTORY_KEY("PTOperationStatisticsAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTOperationStatisticsAdmin>::ICON("chart_pie.png");
		template<> const string AdminInterfaceElementTemplate<PTOperationStatisticsAdmin>::DEFAULT_TITLE("Statistiques");
	}

	namespace pt_operation
	{
		const std::string PTOperationStatisticsAdmin::PARAM_SEARCH_START_DATE("sd");
		const std::string PTOperationStatisticsAdmin::PARAM_SEARCH_END_DATE("ed");
		const std::string PTOperationStatisticsAdmin::PARAM_ROW_STEP("rs");
		const std::string PTOperationStatisticsAdmin::PARAM_COL_STEP("cs");
		const std::string PTOperationStatisticsAdmin::PARAM_WHAT("wh");



		PTOperationStatisticsAdmin::PTOperationStatisticsAdmin()
		:	AdminInterfaceElementTemplate<PTOperationStatisticsAdmin>(),
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
			_searchRowStep(PTOperationStatisticsTableSync::SERVICE_STEP),
			_searchColStep(PTOperationStatisticsTableSync::DATE_STEP),
			_searchWhat(PTOperationStatisticsTableSync::KM)
		{ }



		void PTOperationStatisticsAdmin::setFromParametersMap(
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

			if(map.getOptional<string>(PARAM_ROW_STEP))
				_searchRowStep = static_cast<PTOperationStatisticsTableSync::Step>(map.get<int>(PARAM_ROW_STEP));
			if(map.getOptional<string>(PARAM_COL_STEP))
				_searchColStep = static_cast<PTOperationStatisticsTableSync::Step>(map.get<int>(PARAM_COL_STEP));
			if(map.getOptional<string>(PARAM_WHAT))
				_searchWhat = static_cast<PTOperationStatisticsTableSync::What>(map.get<int>(PARAM_WHAT));
		}



		ParametersMap PTOperationStatisticsAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAM_SEARCH_START_DATE, gregorian::to_iso_extended_string(_searchPeriod.begin()));
			m.insert(PARAM_SEARCH_END_DATE, gregorian::to_iso_extended_string(_searchPeriod.last()));
			m.insert(PARAM_ROW_STEP, static_cast<int>(_searchRowStep));
			m.insert(PARAM_COL_STEP, static_cast<int>(_searchColStep));
			m.insert(PARAM_WHAT, static_cast<int>(_searchWhat));
			return m;
		}



		bool PTOperationStatisticsAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void PTOperationStatisticsAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			stream << "<h1>Requête</h1>";

			AdminFunctionRequest<PTOperationStatisticsAdmin> searchRequest(request, *this);

			SearchFormHTMLTable rt(searchRequest.getHTMLForm());
			stream << rt.open();
			stream << rt.cell("Date début", rt.getForm().getCalendarInput(PARAM_SEARCH_START_DATE, _searchPeriod.begin()));
			stream << rt.cell("Date fin", rt.getForm().getCalendarInput(PARAM_SEARCH_END_DATE, _searchPeriod.last()));
			stream << rt.cell(
				"Lignes",
				rt.getForm().getSelectInput(PARAM_ROW_STEP, _GetStepsVector(), optional<PTOperationStatisticsTableSync::Step>(_searchRowStep))
			);
			stream << rt.cell(
				"Colonnes",
				rt.getForm().getSelectInput(PARAM_COL_STEP, _GetStepsVector(), optional<PTOperationStatisticsTableSync::Step>(_searchColStep))
			);
			stream << rt.cell(
				"Résultat",
				rt.getForm().getSelectInput(PARAM_WHAT, _GetWhatVector(), optional<PTOperationStatisticsTableSync::What>(_searchWhat))
			);
			stream << rt.close();

			stream << "<h1>Résultats</h1>";

			PTOperationStatisticsTableSync::CountSearchResult r(
				PTOperationStatisticsTableSync::Count(
					_searchPeriod,
					_searchRowStep,
					_searchColStep,
					_searchWhat
			)	);

			typedef map<PTOperationStatisticsTableSync::CountSearchResult::mapped_type::key_type, size_t> _Cols;
			_Cols cols;
			BOOST_FOREACH(const PTOperationStatisticsTableSync::CountSearchResult::value_type& row, r)
			{
				BOOST_FOREACH(const PTOperationStatisticsTableSync::CountSearchResult::mapped_type::value_type& cell, row.second)
				{
					if(cols.find(cell.first) == cols.end())
					{
						cols.insert(make_pair(cell.first, 0));
					}
				}
			}

			bool hasRowStep(_searchRowStep != PTOperationStatisticsTableSync::NO_STEP);
			bool hasColStep(_searchColStep != PTOperationStatisticsTableSync::NO_STEP);

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

			BOOST_FOREACH(const PTOperationStatisticsTableSync::CountSearchResult::value_type& row, r)
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

						PTOperationStatisticsTableSync::CountSearchResult::mapped_type::const_iterator it(row.second.find(col.first));
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

/*			stream << "<h1>Export CSV</h1>";

			StaticFunctionRequest<CSVResaStatisticsFunction> csvRequest(request, true);
			csvRequest.getFunction()->setLine(_line);
			csvRequest.getFunction()->setPeriod(_searchPeriod);
			csvRequest.getFunction()->setRowStep(_searchRowStep);
			csvRequest.getFunction()->setColStep(_searchColStep);

			stream << "<p>" << HTMLModule::getLinkButton(csvRequest.getURL(), "Export CSV", string(), "/admin/img/page_white_go.png") << "</p>";
*/
		}



		AdminInterfaceElement::PageLinks PTOperationStatisticsAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const PTOperationModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		PTOperationStatisticsAdmin::_StepsVector PTOperationStatisticsAdmin::_GetStepsVector()
		{
			_StepsVector v;
			v.push_back(make_pair(PTOperationStatisticsTableSync::NO_STEP, "(total uniquement)"));
			v.push_back(make_pair(PTOperationStatisticsTableSync::DATE_STEP, GetColumnName(PTOperationStatisticsTableSync::DATE_STEP)));
			v.push_back(make_pair(PTOperationStatisticsTableSync::HOUR_STEP, GetColumnName(PTOperationStatisticsTableSync::HOUR_STEP)));
			v.push_back(make_pair(PTOperationStatisticsTableSync::WEEK_DAY_STEP, GetColumnName(PTOperationStatisticsTableSync::WEEK_DAY_STEP)));
			v.push_back(make_pair(PTOperationStatisticsTableSync::MONTH_STEP, GetColumnName(PTOperationStatisticsTableSync::MONTH_STEP)));
			v.push_back(make_pair(PTOperationStatisticsTableSync::YEAR_STEP, GetColumnName(PTOperationStatisticsTableSync::YEAR_STEP)));
			v.push_back(make_pair(PTOperationStatisticsTableSync::LINE_STEP, GetColumnName(PTOperationStatisticsTableSync::LINE_STEP)));
			v.push_back(make_pair(PTOperationStatisticsTableSync::SERVICE_STEP, GetColumnName(PTOperationStatisticsTableSync::SERVICE_STEP)));
			v.push_back(make_pair(PTOperationStatisticsTableSync::VEHICLE_STEP, GetColumnName(PTOperationStatisticsTableSync::VEHICLE_STEP)));
			return v;
		}



		std::string PTOperationStatisticsAdmin::GetColumnName( PTOperationStatisticsTableSync::Step step )
		{
			if(step == PTOperationStatisticsTableSync::DATE_STEP) return "Date";
			if(step == PTOperationStatisticsTableSync::HOUR_STEP) return "Heure";
			if(step == PTOperationStatisticsTableSync::WEEK_DAY_STEP) return "Jour de la semaine";
			if(step == PTOperationStatisticsTableSync::MONTH_STEP) return "Mois";
			if(step == PTOperationStatisticsTableSync::YEAR_STEP) return "Année";
			if(step == PTOperationStatisticsTableSync::LINE_STEP) return "Ligne";
			if(step == PTOperationStatisticsTableSync::SERVICE_STEP) return "Service";
			if(step == PTOperationStatisticsTableSync::VEHICLE_STEP) return "Véhicule";
			return string();
		}



		std::string PTOperationStatisticsAdmin::GetWhatName( PTOperationStatisticsTableSync::What what )
		{
			if(what == PTOperationStatisticsTableSync::KM) return "Distance";
			return string();
		}



		PTOperationStatisticsAdmin::_WhatVector PTOperationStatisticsAdmin::_GetWhatVector()
		{
			_WhatVector v;
			v.push_back(make_pair(PTOperationStatisticsTableSync::KM, GetWhatName(PTOperationStatisticsTableSync::KM)));
			return v;
		}
	}
}
