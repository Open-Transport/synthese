
/** VinciReportsAdminInterfaceElement class implementation.
	@file VinciReportsAdminInterfaceElement.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#include "71_vinci_bike_rental/VinciReportsAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/VinciRate.h"
#include "71_vinci_bike_rental/VinciRateTableSync.h"

#include "57_accounting/TransactionPartTableSync.h"
#include "05_html/SearchFormHTMLTable.h"
#include "05_html/ResultHTMLTable.h"

#include "04_time/Date.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/HomeAdmin.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace time;
	using namespace accounts;
	using namespace html;
	using namespace vinci;

	namespace util
	{
	    template<> const string FactorableTemplate<AdminInterfaceElement,VinciReportsAdminInterfaceElement>::FACTORY_KEY("vincireports");
	}

	namespace admin
	{
		template <> const string AdminInterfaceElementTemplate<VinciReportsAdminInterfaceElement>::ICON("chart_curve.png");
		template <> const string AdminInterfaceElementTemplate<VinciReportsAdminInterfaceElement>::DEFAULT_TITLE("Etats statistiques");
	}

	namespace vinci
	{
		VinciReportsAdminInterfaceElement::VinciReportsAdminInterfaceElement()
			: AdminInterfaceElementTemplate<VinciReportsAdminInterfaceElement>() 
			, _startDate(TIME_CURRENT)
			, _endDate(TIME_CURRENT)
		{}

		const std::string VinciReportsAdminInterfaceElement::PARAM_START_DATE = "vraiepsd";
		const std::string VinciReportsAdminInterfaceElement::PARAM_END_DATE = "vraieped";


		void VinciReportsAdminInterfaceElement::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			// Report Launch request
			FunctionRequest<AdminRequest> reportRequest(request);
			reportRequest.getFunction()->setPage<VinciReportsAdminInterfaceElement>();
		
			// Search form
			stream << "<h1>Dates de l'état journalier</h1>";
			SearchFormHTMLTable t(reportRequest.getHTMLForm("search"));
			stream << t.open();
			stream << t.cell("Date début", t.getForm().getCalendarInput(PARAM_START_DATE, _startDate));
			stream << t.cell("Date fin", t.getForm().getCalendarInput(PARAM_END_DATE, _endDate));
			stream << t.close();

			// Rents per day
			if (!_resultsPerDay.empty())
			{
				stream << "<h1>Trafic journalier</h1>";
				ResultHTMLTable::HeaderVector h1;
				h1.push_back(make_pair(string(), "Date"));
				h1.push_back(make_pair(string(), "Départs"));
				h1.push_back(make_pair(string(), "Retours"));
				ResultHTMLTable v1(h1, t.getForm(), ResultHTMLTable::RequestParameters(), ResultHTMLTable::ResultParameters());
				stream << v1.open();

				for(std::map<time::Date, RentReportResult>::const_iterator it = _resultsPerDay.begin();
					it != _resultsPerDay.end(); ++it)
				{
					stream << v1.row();
					stream << v1.col() << it->first.toString();
					stream << v1.col() << it->second.starts;
					stream << v1.col() << it->second.ends;
				}
				stream << v1.close();
			}

			// Rents par rate
			if (!_resultsPerRate.empty())
			{
				stream << "<h1>Trafic par tarif</h1>";
				ResultHTMLTable::HeaderVector h2;
				h2.push_back(make_pair(string(), "Tarif"));
				h2.push_back(make_pair(string(), "Départs"));
				h2.push_back(make_pair(string(), "Retours"));
				ResultHTMLTable v2(h2, t.getForm(), ResultHTMLTable::RequestParameters(), ResultHTMLTable::ResultParameters());
				stream << v2.open();

				for(std::map<uid, RentReportResult>::const_iterator it2 = _resultsPerRate.begin();
					it2 != _resultsPerRate.end(); ++it2)
				{
					shared_ptr<const VinciRate> rate;
					try
					{
						rate = VinciRateTableSync::Get(it2->first);
					}
					catch (...)
					{
					}

					stream << v2.row();
					stream << v2.col() << (rate.get() ? rate->getName() : "inconnu");
					stream << v2.col() << it2->second.starts;
					stream << v2.col() << it2->second.ends;
				}
				stream << v2.close();
			}
		}

		void VinciReportsAdminInterfaceElement::setFromParametersMap(const ParametersMap& map)
		{
			_startDate = map.getDate(PARAM_START_DATE, false, FACTORY_KEY);
			if (_startDate.isUnknown())
				_startDate = Date(TIME_CURRENT);
			_endDate = map.getDate(PARAM_END_DATE, false, FACTORY_KEY);
			if (_endDate.isUnknown())
				_endDate = Date(TIME_CURRENT);
            _resultsPerDay = getRentsPerDay(_startDate, _endDate);
			_resultsPerRate = getRentsPerRate(_startDate, _endDate);
		}

		bool VinciReportsAdminInterfaceElement::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return true;
		}

		AdminInterfaceElement::PageLinks VinciReportsAdminInterfaceElement::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage		, const server::FunctionRequest<admin::AdminRequest>* request
			) const
		{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == HomeAdmin::FACTORY_KEY)
			{
				links.push_back(_pageLink);
			}
			return links;
		}
	}
}

