
/** DeparturesTableBenchmarkAdmin class implementation.
	@file DeparturesTableBenchmarkAdmin.cpp

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

#include "HTMLForm.h"
#include "Interface.h"
#include "ActionFunctionRequest.h"
#include "AdminRequest.h"
#include "ModuleAdmin.h"
#include "DeparturesTableBenchmarkAdmin.h"
#include "DeparturesTableModule.h"
#include "DisplayScreen.h"
#include "DisplayScreenCPU.h"
#include "DisplayAdmin.h"
#include "DisplayScreenContentRequest.h"
#include "DisplayScreenTableSync.h"
#include "DisplayScreenCPUTableSync.h"
#include "ArrivalDepartureTableRight.h"
#include "Request.h"
#include "ActionResultHTMLTable.h"
#include "InterfaceTableSync.h"
#include "UpdateDisplayPreselectionParametersAction.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace departurestable;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,DeparturesTableBenchmarkAdmin>::FACTORY_KEY("3benchmark");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DeparturesTableBenchmarkAdmin>::ICON("lightning.png");
		template<> const string AdminInterfaceElementTemplate<DeparturesTableBenchmarkAdmin>::DEFAULT_TITLE("Benchmark");
	}

	namespace departurestable
	{
		const string DeparturesTableBenchmarkAdmin::PARAMETER_DOIT("di");

		void DeparturesTableBenchmarkAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			_doIt = map.getBool(PARAMETER_DOIT, false, false, FACTORY_KEY);

			if(!doDisplayPreparationActions || !_doIt) return;

			DisplayScreenTableSync::Search(_env);
			DisplayScreenCPUTableSync::Search(_env);
			
			FunctionRequest<DisplayScreenContentRequest> r(_request);
			BOOST_FOREACH(shared_ptr<const DisplayScreen> screen, _env.getRegistry<DisplayScreen>())
			{
				stringstream s;
				ptime t1(microsec_clock::local_time());
				r.getFunction()->setDisplay(screen->getKey());
				r.run(s);
				ptime t2(microsec_clock::local_time());
				TestCase t;
				t.method = DisplayScreenContentRequest::FACTORY_KEY;
				t.screen = screen;
				t.duration = t2 - t1;
				t.size = s.str().size();
				_testCases.push_back(t);
			}

			BOOST_FOREACH(shared_ptr<const DisplayScreenCPU> cpu, _env.getRegistry<DisplayScreenCPU>())
			{
			}
		}



		ParametersMap DeparturesTableBenchmarkAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAMETER_DOIT, _doIt);
			return m;
		}
		
			
			
		void DeparturesTableBenchmarkAdmin::display(
			ostream& stream,
			interfaces::VariablesMap& variables
		) const	{

			if(_doIt)
			{
				HTMLTable::ColsVector h;
				h.push_back("N°");
				h.push_back("Nature");
				h.push_back("Objet");
				h.push_back("Temps calcul");
				h.push_back("Taille générée");
				HTMLTable t(h, ResultHTMLTable::CSS_CLASS);
				
				int rank(1);
				time_duration total_duration;
				size_t total_size(0);
				stream << t.open();
				BOOST_FOREACH(const TestCase& testCase, _testCases)
				{
					stream << t.row();
					stream << t.col();
					stream << rank++;
					stream << t.col();
					if(testCase.method == DisplayScreenContentRequest::FACTORY_KEY)
					{
						stream << "Tableau de départs " << UpdateDisplayPreselectionParametersAction::GetFunctionList()[UpdateDisplayPreselectionParametersAction::GetFunction(*testCase.screen)];
					}
					stream << t.col();
					stream << testCase.screen->getFullName();
					stream << t.col();
					stream << testCase.duration.total_microseconds() << " &micro;s";
					stream << t.col();
					stream << (static_cast<float>(testCase.size) / 1024) << " ko";
					total_duration += testCase.duration;
					total_size += testCase.size;
				}

				stream << t.row();
				stream << t.col(3) << "TOTAL";
				stream << t.col() << total_duration.total_milliseconds() << " ms";
				stream << t.col() << (static_cast<float>(total_size) / 1024) << " ko";
				stream << t.close();

				stream << "<p class=\"info\">Les temps mesurés s'entendent hors requêtes de supervision.</p>";
				stream << "<p class=\"info\">Les tailles mesurées s'entendent hors fichiers joints éventuels (images, etc.)</p>";
			}
			else
			{
				FunctionRequest<AdminRequest> doRequest(_request);
				doRequest.getFunction()->setSamePage(this);

				stream << "<p class=\"info\">Le lancement du benchmark peut affecter les performances du système durant le test. Etes-vous sûr de vouloir lancer le benchmark ?</p>";
				HTMLForm f(doRequest.getHTMLForm());
				stream << f.open();
				stream << "<p>" << f.getOuiNonRadioInput(PARAMETER_DOIT, false) << "</p>";
				stream << f.getSubmitButton("Lancer le benchmark");
				stream << f.close();
			}
			
		}

		bool DeparturesTableBenchmarkAdmin::isAuthorized() const
		{
			return _request->isAuthorized<ArrivalDepartureTableRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, GLOBAL_PERIMETER);
		}

		DeparturesTableBenchmarkAdmin::DeparturesTableBenchmarkAdmin()
			: AdminInterfaceElementTemplate<DeparturesTableBenchmarkAdmin>(),
			_doIt(false)
		{

		}

		AdminInterfaceElement::PageLinks DeparturesTableBenchmarkAdmin::getSubPagesOfParent(
			const PageLink& parentLink,
			const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == DeparturesTableModule::FACTORY_KEY)
			{
				links.push_back(getPageLink());
			}
			return links;
		}



		bool DeparturesTableBenchmarkAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			return true;
		}



		AdminInterfaceElement::PageLinks DeparturesTableBenchmarkAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
		) const {
			AdminInterfaceElement::PageLinks links;
			return links;
		}
	}
}
