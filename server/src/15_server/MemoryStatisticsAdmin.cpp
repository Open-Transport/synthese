
//////////////////////////////////////////////////////////////////////////
///	MemoryStatisticsAdmin class implementation.
///	@file MemoryStatisticsAdmin.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "MemoryStatisticsAdmin.hpp"

#include "AdminParametersException.h"
#include "Profile.h"
#include "ResultHTMLTable.h"
#include "User.h"
#include "ParametersMap.h"
#include "ServerModule.h"
#include "ServerAdminRight.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace html;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace server;

	template<> const string FactorableTemplate<AdminInterfaceElement, MemoryStatisticsAdmin>::FACTORY_KEY = "memory_statistics";

	namespace admin
	{
		template<>
		const string AdminInterfaceElementTemplate<MemoryStatisticsAdmin>::ICON = "chart_pie.png";

		template<>
		const string AdminInterfaceElementTemplate<MemoryStatisticsAdmin>::DEFAULT_TITLE = "Statistiques mémoire";
	}

	namespace server
	{
		const string MemoryStatisticsAdmin::TAB_REGISTRY = "tab_registry";



		MemoryStatisticsAdmin::MemoryStatisticsAdmin()
			: AdminInterfaceElementTemplate<MemoryStatisticsAdmin>()
		{ }


		
		void MemoryStatisticsAdmin::setFromParametersMap(
			const ParametersMap& map
		){
		}



		ParametersMap MemoryStatisticsAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}


		
		bool MemoryStatisticsAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ServerAdminRight>(READ);
		}



		void MemoryStatisticsAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// REGISTRY TAB
			if (openTabContent(stream, TAB_REGISTRY))
			{
				// Title
				stream << "<h1>Classes</h1>";

				stream << "<p class=\"info\">ATTENTION : La taille de certaines classes est sous-estimées en raison de la présence de données auto-générées non reliée aux registres.</p>";

				// Result storage
				size_t sizeSum(0);
				size_t numberSum(0);
				typedef map<string, pair<size_t, size_t> > ResultMap;
				ResultMap result;
				BOOST_FOREACH(const Env::RegistryMap::value_type& registry, Env::GetOfficialEnv().getMap())
				{
					// Local variables
					size_t objectSize(Env::GetObjectSize(registry.first));
					size_t registrySize(registry.second->size());

					// The item
					result.insert(
						make_pair(
							registry.first,
							make_pair(
								objectSize,
								registrySize
					)	)	);

					// The sums
					sizeSum += objectSize * registrySize;
					numberSum += registrySize;
				}

				// Table header
				HTMLTable::ColsVector c;
				c.push_back("Classe");
				c.push_back("Taille");
				c.push_back("Nombre");
				c.push_back("%");
				c.push_back("Mémoire");
				c.push_back("%");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();

				// Table Rows
				BOOST_FOREACH(const ResultMap::value_type& item, result)
				{
					// Row declaration
					stream << t.row();

					// Class
					stream << t.col();
					stream << item.first;

					// Size
					stream << t.col();
					stream << item.second.first;

					// Number
					stream << t.col();
					stream << item.second.second;

					// Ratio
					stream << t.col();
					stream << fixed << setprecision(2) << (double(100 * item.second.second) / double(numberSum)) << "%";

					// Memory
					stream << t.col();
					stream << (item.second.first * item.second.second);

					// Ratio
					stream << t.col();
					stream << fixed << setprecision(2) << (double(100 * item.second.first * item.second.second) / double(sizeSum)) << "%";
				}

				// Sum
				stream << t.row();
				stream << t.col(1, string(), true) << "TOTAL";
				stream << t.col(1, string(), true) << fixed << setprecision(2) << (double(sizeSum) / double(numberSum));
				stream << t.col(1, string(), true) << numberSum;
				stream << t.col(1, string(), true) << "100%";
				stream << t.col(1, string(), true) << sizeSum;
				stream << t.col(1, string(), true) << "100%";

				// Table closing
				stream << t.close();
			}
			
			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		AdminInterfaceElement::PageLinks MemoryStatisticsAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			if(	dynamic_cast<const ServerModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}
			
			return links;
		}



		void MemoryStatisticsAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Registres", TAB_REGISTRY, profile.isAuthorized<ServerAdminRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));

			_tabBuilded = true;
		}
}	}

