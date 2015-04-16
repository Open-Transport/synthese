
/** DeparturesTableBenchmarkAdmin class header.
	@file DeparturesTableBenchmarkAdmin.h

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

#ifndef SYNTHESE_DeparturesTableBenchmarkADMIN_H
#define SYNTHESE_DeparturesTableBenchmarkADMIN_H

#include "AdminInterfaceElementTemplate.h"
#include "ResultHTMLTable.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace departure_boards
	{
		class DisplayScreen;
		class DisplayScreenCPU;

		/** Departures table benchmark admin page class.
			@ingroup m54Admin refAdmin

			@image html cap_admin_display_types.png
			@image latex cap_admin_display_types.png "Ecran d'édition des types d'affichage" width=14cm

			<i>Titre de la fenêtre</i> : SYNTHESE Admin - Benchmark

			<i>Barre de navigation</i> :

			<i>Zone de contenus</i> :

			<i>Sécurité</i> :
				- Une habilitation publique ArrivalDepartureTableRight de niveau DELETE sur le périmètre "tout" est nécessaire pour accéder à la page.

			<i>Journal</i> :
		*/
		class DeparturesTableBenchmarkAdmin :
			public admin::AdminInterfaceElementTemplate<DeparturesTableBenchmarkAdmin>
		{
			struct TestCase
			{
				boost::shared_ptr<const DisplayScreen> screen;
				boost::shared_ptr<const DisplayScreenCPU> cpu;
				std::string	method;
				boost::posix_time::time_duration duration;
				size_t size;
			};
			typedef std::vector<TestCase> TestCases;

			bool		_doIt;

		public:
			static const std::string PARAMETER_DOIT;

			DeparturesTableBenchmarkAdmin();

			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			virtual void setFromParametersMap(
				const util::ParametersMap& map
			);

			/** Parameters map generator, used when building an url to the admin page.
				@return util::ParametersMap The generated parameters map
				@author Hugues Romain
				@date 2007
			*/
			virtual util::ParametersMap getParametersMap() const;


			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;



			bool isAuthorized(
				const security::User& user
			) const;

			/** Gets sub page of the designed parent page, which are from the current class.
				@param factoryKey Key of the parent class
				@return PageLinks A link to the page if the parent is DisplaySearch
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;


			/** Gets the opening position of the node in the tree view.
				@return Always visible
				@author Hugues Romain
				@date 2008
			*/
			virtual bool isPageVisibleInTree(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

		};
	}
}

#endif
