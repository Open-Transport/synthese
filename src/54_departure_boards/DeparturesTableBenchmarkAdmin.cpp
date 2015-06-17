
/** DeparturesTableBenchmarkAdmin class implementation.
	@file DeparturesTableBenchmarkAdmin.cpp

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

#include "HTMLForm.h"
#include "Interface.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "AdminInterfaceElement.h"
#include "ModuleAdmin.h"
#include "DeparturesTableBenchmarkAdmin.h"
#include "DeparturesTableModule.h"
#include "DisplayScreen.h"
#include "DisplayScreenCPU.h"
#include "DisplayAdmin.h"
#include "DisplayScreenContentFunction.h"
#include "DisplayScreenTableSync.h"
#include "DisplayScreenCPUTableSync.h"
#include "ArrivalDepartureTableRight.h"
#include "ActionResultHTMLTable.h"
#include "InterfaceTableSync.h"
#include "UpdateDisplayPreselectionParametersAction.h"
#include "CPUGetWiredScreensFunction.h"
#include "Profile.h"
#include "User.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace departure_boards;
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

	namespace departure_boards
	{
		const string DeparturesTableBenchmarkAdmin::PARAMETER_DOIT("di");

		void DeparturesTableBenchmarkAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_doIt = map.getDefault<bool>(PARAMETER_DOIT, false);
		}



		ParametersMap DeparturesTableBenchmarkAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAMETER_DOIT, _doIt);
			return m;
		}



		void DeparturesTableBenchmarkAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			if(_doIt)
			{
				TestCases	_testCases;

				DisplayScreenTableSync::SearchResult screens(
					DisplayScreenTableSync::Search(Env::GetOfficialEnv())
				);
				DisplayScreenCPUTableSync::SearchResult cpus(
					DisplayScreenCPUTableSync::Search(Env::GetOfficialEnv())
				);

				StaticFunctionRequest<DisplayScreenContentFunction> r(_request, true);
				StaticFunctionRequest<CPUGetWiredScreensFunction> r2(_request, true);
				ptime t0(microsec_clock::local_time());
				time_duration duration;
				BOOST_FOREACH(const boost::shared_ptr<const DisplayScreen>& screen, screens)
				{
					// Avoid subscreens (their content will be certainly generated by their parent screen)
					if(screen->getParent(true))
					{
						continue;
					}

					stringstream s;
					r.getFunction()->setScreen(screen);
					r.run(s);
					ptime t2(microsec_clock::local_time());
					TestCase t;
					t.method = DisplayScreenContentFunction::FACTORY_KEY;
					t.screen = screen;
					t.duration = t2 - (t0 + duration);
					t.size = s.str().size();
					_testCases.push_back(t);
					duration = t2 - t0;
				}

				BOOST_FOREACH(const boost::shared_ptr<const DisplayScreenCPU>& cpu, cpus)
				{
					stringstream s;
					if(!cpu->getMacAddress().empty())
					{
						r2.getFunction()->setCPU(cpu->getMacAddress());
					}
					else
					{
						r2.getFunction()->setCPU(cpu->getKey());
					}
					r2.run(s);
					ptime t2(microsec_clock::local_time());
					TestCase t;
					t.method = CPUGetWiredScreensFunction::FACTORY_KEY;
					t.cpu = cpu;
					t.duration = t2 - (t0 + duration);
					t.size = s.str().size();
					_testCases.push_back(t);
					duration = t2 - t0;
				}

				AdminFunctionRequest<DeparturesTableBenchmarkAdmin> reloadRequest(_request, *this);
				reloadRequest.getPage()->_doIt = true;

				stream << "<h1>Résultats</h1>";

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
					if(testCase.method == DisplayScreenContentFunction::FACTORY_KEY)
					{
						stream << "Tableau de départs " << UpdateDisplayPreselectionParametersAction::GetFunctionList()[UpdateDisplayPreselectionParametersAction::GetFunction(*testCase.screen)];
						stream << t.col();
						stream << testCase.screen->getFullName();
					}
					if(testCase.method == CPUGetWiredScreensFunction::FACTORY_KEY)
					{
						stream << "Unité centrale ";
						stream << t.col();
						stream << testCase.cpu->getFullName();
					}
					stream << t.col();
					stream << testCase.duration.total_microseconds() << " &micro;s";
					stream << t.col();
					stream << setprecision(2) << fixed << (static_cast<float>(testCase.size) / 1024) << " ko";
					total_duration += testCase.duration;
					total_size += testCase.size;
				}

				stream << t.row();
				stream << t.col(3) << "TOTAL";
				stream << t.col() << total_duration.total_milliseconds() << " ms";
				stream << t.col() << setprecision(2) << fixed << (static_cast<float>(total_size) / 1024) << " ko";

				if(rank > 1)
				{
					stream << t.row();
					stream << t.col(3) << "MOYENNE PAR EQUIPEMENT";
					stream << t.col() << total_duration.total_milliseconds() / (rank - 1) << " ms";
					stream << t.col() << setprecision(2) << fixed << (static_cast<float>(total_size) / (1024 * (rank - 1))) << " ko";;

					stream << t.row();
					stream << t.col(3) << "Taux d'utilisation du serveur si une requête par minute";
					stream << t.col(2) << setprecision(2) << fixed << (static_cast<float>(total_duration.total_milliseconds()) / 600) << " %";

					if(total_duration.total_milliseconds() > 0)
					{
						stream << t.row();
						stream << t.col(3) << "Capacité du serveur si une requête par minute";
						stream << t.col(2) << "Env. " << setprecision(0) << fixed << ((60000 * (rank - 1)) / total_duration.total_milliseconds()) << " équipements";
					}
				}

				stream << t.close();

				stream << "<p>" << reloadRequest.getHTMLForm().getLinkButton("Relancer le benchmark", string(), ICON) << "</p>";

				stream << "<h1>Informations</h1>";
				stream << "<p class=\"info\">Les temps mesurés s'entendent hors requêtes de supervision.</p>";
				stream << "<p class=\"info\">Les tailles mesurées s'entendent hors fichiers joints éventuels (images, etc.)</p>";
			}
			else
			{
				AdminFunctionRequest<DeparturesTableBenchmarkAdmin> doRequest(_request, *this);

				stream << "<p class=\"info\">Le lancement du benchmark peut affecter les performances du système durant le test. Etes-vous sûr de vouloir lancer le benchmark ?</p>";
				HTMLForm f(doRequest.getHTMLForm());
				stream << f.open();
				stream << "<p>" << f.getOuiNonRadioInput(PARAMETER_DOIT, false) << "</p>";
				stream << f.getSubmitButton("Lancer le benchmark");
				stream << f.close();
			}

		}

		bool DeparturesTableBenchmarkAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ArrivalDepartureTableRight>(
				DELETE_RIGHT,
				UNKNOWN_RIGHT_LEVEL,
				GLOBAL_PERIMETER
			);
		}

		DeparturesTableBenchmarkAdmin::DeparturesTableBenchmarkAdmin()
			: AdminInterfaceElementTemplate<DeparturesTableBenchmarkAdmin>(),
			_doIt(false)
		{

		}

		AdminInterfaceElement::PageLinks DeparturesTableBenchmarkAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(	dynamic_cast<const DeparturesTableModule*>(&module) && request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				links.push_back(getNewCopiedPage());
			}
			return links;
		}



		bool DeparturesTableBenchmarkAdmin::isPageVisibleInTree(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const {
			return true;
		}
	}
}