
//////////////////////////////////////////////////////////////////////////
///	DriverActivitiesAdmin class implementation.
///	@file DriverActivitiesAdmin.cpp
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

#include "DriverActivitiesAdmin.hpp"

#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "DriverActivity.hpp"
#include "DriverActivityAdmin.hpp"
#include "GlobalRight.h"
#include "ObjectCreateAction.hpp"
#include "Profile.h"
#include "RemoveObjectAction.hpp"
#include "ResultHTMLTable.h"
#include "User.h"
#include "ParametersMap.h"
#include "PTOperationModule.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace db;
	using namespace html;
	using namespace pt_operation;
	using namespace server;
	using namespace security;
	using namespace util;

	template<> const string FactorableTemplate<AdminInterfaceElement, DriverActivitiesAdmin>::FACTORY_KEY = "DriverActivities";

	namespace admin
	{
		template<>
		const string AdminInterfaceElementTemplate<DriverActivitiesAdmin>::ICON = "building.png";

		template<>
		const string AdminInterfaceElementTemplate<DriverActivitiesAdmin>::DEFAULT_TITLE = "Activités";
	}

	namespace pt_operation
	{

		DriverActivitiesAdmin::DriverActivitiesAdmin()
			: AdminInterfaceElementTemplate<DriverActivitiesAdmin>()
		{ }



		void DriverActivitiesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
		}



		ParametersMap DriverActivitiesAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}



		bool DriverActivitiesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void DriverActivitiesAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			AdminActionFunctionRequest<ObjectCreateAction, DriverActivitiesAdmin> createRequest(request, *this);
			createRequest.getAction()->setTable<DriverActivity>();

			AdminFunctionRequest<DriverActivityAdmin> openRequest(request);

			AdminActionFunctionRequest<RemoveObjectAction, DriverActivitiesAdmin> removeRequest(request, *this);

			HTMLTable::ColsVector h;
			h.push_back(string());
			h.push_back("Nom");
			h.push_back("Code");
			h.push_back(string());

			HTMLForm f(createRequest.getHTMLForm("create"));
			stream << f.open();
			HTMLTable t(
				h,
				ResultHTMLTable::CSS_CLASS
			);
			stream << t.open();

			BOOST_FOREACH(const DriverActivity::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<DriverActivity>())
			{
				// Row
				const DriverActivity& activity(*it.second);
				stream << t.row();

				// Open
				stream << t.col();
				openRequest.getPage()->setActivity(it.second);
				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + DriverActivityAdmin::ICON);

				// Name
				stream << t.col() << activity.get<Name>();

				// Code by sources
				stream << t.col() << activity.getCodeBySources();

				// Remove
				stream << t.col();
				removeRequest.getAction()->setObjectId(activity.get<Key>());
				stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer l'activité "+ activity.get<Name>() + " ?");
			}

			stream << t.row(string());
			stream << t.col();
			stream << t.col() << f.getTextInput(ObjectCreateAction::GetInputName<Name>(), "", "Entrez le nom ici");
			stream << t.col();
			stream << t.col() << f.getSubmitButton("Ajouter");
			stream << t.close();
			stream << f.close();
		}



		AdminInterfaceElement::PageLinks DriverActivitiesAdmin::getSubPagesOfModule(
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



		AdminInterfaceElement::PageLinks DriverActivitiesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			const DriverActivitiesAdmin* ua(
				dynamic_cast<const DriverActivitiesAdmin*>(&currentPage)
			);

			const DriverActivityAdmin* ua2(
				dynamic_cast<const DriverActivityAdmin*>(&currentPage)
			);

			if(ua || ua2)
			{
				BOOST_FOREACH(const DriverActivity::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<DriverActivity>())
				{
					boost::shared_ptr<DriverActivityAdmin> p(getNewPage<DriverActivityAdmin>());
					p->setActivity(const_pointer_cast<const DriverActivity>(it.second));
					links.push_back(p);
				}
			}

			return links;
		}
}	}

