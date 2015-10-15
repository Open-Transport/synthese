
/** PT Use Ruler class header.
	@file PTRuleUserAdmin.hpp

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

#ifndef SYNTHESE_graph_UseRulerAdmin_hpp__
#define SYNTHESE_graph_UseRulerAdmin_hpp__

#include "PTUseRule.h"
#include "HTMLForm.h"
#include "ResultHTMLTable.h"
#include "Request.h"
#include "AdminActionFunctionRequest.hpp"
#include "RuleUserUpdateAction.hpp"
#include "CommercialLineTableSync.h"
#include "PTModule.h"
#include "GraphConstants.h"

#include <deque>

namespace synthese
{
	namespace pt
	{
		/** UseRulerAdmin class.
			@ingroup m35
		*/
		template<class ObjectType, class AdminPage>
		class PTRuleUserAdmin
		{
		public:
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const ObjectType> object,
				const server::Request& request,
				bool pedestrian = true,
				bool handicapped = true,
				bool bikes = true
			);
		};



		template<class ObjectType, class AdminPage>
		void PTRuleUserAdmin<ObjectType,AdminPage>::Display(
			std::ostream& stream,
			boost::shared_ptr<const ObjectType> object,
			const server::Request& request,
			bool pedestrian,
			bool handicapped,
			bool bikes
		){
			stream << "<h1>Conditions d'utilisation par profil</h1>";

			html::HTMLTable::ColsVector c;
			c.push_back(std::string());
			if(pedestrian)
			{
				c.push_back("Piétons");
			}
			if(handicapped)
			{
				c.push_back("Handicapés");
			}
			if(bikes)
			{
				c.push_back("Vélos");
			}

			admin::AdminActionFunctionRequest<graph::RuleUserUpdateAction, AdminPage> updateRequest(request);
			updateRequest.getAction()->setId(object->getKey());

			html::HTMLForm f(updateRequest.getHTMLForm());

			stream << f.open();
			html::HTMLTable t(c, html::ResultHTMLTable::CSS_CLASS);
			stream << t.open();

			std::deque<const graph::RuleUser*> users;
			for(const graph::RuleUser* user(object->_getParentRuleUser()); user; user = user->_getParentRuleUser())
			{
				users.push_front(user);
			}

			BOOST_FOREACH(const graph::RuleUser* user, users)
			{
				stream << t.row();
				stream << t.col(1, std::string(), true) << user->getRuleUserName();
				if(pedestrian)
				{
					stream << t.col() << (user->getRule(graph::USER_PEDESTRIAN) ? user->getRule(graph::USER_PEDESTRIAN)->getUseRuleName() : "(non défini)");
				}
				if(handicapped)
				{
					stream << t.col() << (user->getRule(graph::USER_HANDICAPPED) ? user->getRule(graph::USER_HANDICAPPED)->getUseRuleName() : "(non défini)");
				}
				if(bikes)
				{
					stream << t.col() << (user->getRule(graph::USER_BIKE) ? user->getRule(graph::USER_BIKE)->getUseRuleName() : "(non défini)");
				}
			}

			stream << t.row();
			stream << t.col(1, std::string(), true) << object->getRuleUserName();
			if(pedestrian)
			{
				stream << t.col() << f.getSelectInput(
					graph::RuleUserUpdateAction::PARAMETER_VALUE_PREFIX + PedestrianComplianceId::FIELD.name,
					PTModule::GetPTUseRuleLabels(),
					boost::optional<util::RegistryKeyType>(dynamic_cast<const PTUseRule*>(object->getRule(graph::USER_PEDESTRIAN)) ? dynamic_cast<const PTUseRule*>(object->getRule(graph::USER_PEDESTRIAN))->getKey() : 0)
				);
			}
			if(handicapped)
			{
				stream << t.col() << f.getSelectInput(
					graph::RuleUserUpdateAction::PARAMETER_VALUE_PREFIX + HandicappedComplianceId::FIELD.name,
					PTModule::GetPTUseRuleLabels(),
					boost::optional<util::RegistryKeyType>(dynamic_cast<const PTUseRule*>(object->getRule(graph::USER_HANDICAPPED)) ? dynamic_cast<const PTUseRule*>(object->getRule(graph::USER_HANDICAPPED))->getKey() : 0)
				);
			}
			if(bikes)
			{
				stream << t.col() << f.getSelectInput(
					graph::RuleUserUpdateAction::PARAMETER_VALUE_PREFIX + BikeComplianceId::FIELD.name,
					PTModule::GetPTUseRuleLabels(),
					boost::optional<util::RegistryKeyType>(dynamic_cast<const PTUseRule*>(object->getRule(graph::USER_BIKE)) ? dynamic_cast<const PTUseRule*>(object->getRule(graph::USER_BIKE))->getKey() : 0)
				);
			}

			stream << t.row();
			stream << t.col(1, std::string(), true) << "Résultat";
			if(pedestrian)
			{
				stream << t.col() << (object->getUseRule(graph::USER_PEDESTRIAN - graph::USER_CLASS_CODE_OFFSET).getUseRuleName());
			}
			if(handicapped)
			{
				stream << t.col() << (object->getUseRule(graph::USER_HANDICAPPED - graph::USER_CLASS_CODE_OFFSET).getUseRuleName());
			}
			if(bikes)
			{
				stream << t.col() << (object->getUseRule(graph::USER_BIKE - graph::USER_CLASS_CODE_OFFSET).getUseRuleName());
			}

			stream << t.row();
			stream << t.col(4, std::string(), true) << f.getSubmitButton("Enregistrer les modifications");

			stream << t.close();
			stream << f.close();
		}
	}
}

#endif // SYNTHESE_graph_UseRulerAdmin_hpp__
