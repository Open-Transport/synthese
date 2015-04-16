
/** TransportNetworkAdmin class header.
	@file TransportNetworkAdmin.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_TransportNetworkAdmin_H__
#define SYNTHESE_TransportNetworkAdmin_H__

#include "AdminInterfaceElementTemplate.h"
#include "TreeFolderAdmin.hpp"

#include "ResultHTMLTable.h"
#include "StaticActionRequest.h"

namespace synthese
{
	namespace tree
	{
		class TreeFolderUpNode;
	}

	namespace pt
	{
		class TransportNetwork;
		class CommercialLineAddAction;

		//////////////////////////////////////////////////////////////////////////
		/// Transport network administration panel.
		///	@ingroup m35Admin refAdmin
		///	@author Hugues Romain
		///	@date 2008
		class TransportNetworkAdmin:
			public admin::AdminInterfaceElementTemplate<TransportNetworkAdmin>,
			public tree::TreeFolderAdmin<TransportNetwork, TransportNetworkAdmin>
		{
			std::string	_searchName;

			html::ResultHTMLTable::RequestParameters	_requestParameters;

		public:
			static const std::string PARAMETER_SEARCH_NAME;
			static const std::string PARAMETER_SEARCH_NETWORK_ID;


			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
			*/
			void setFromParametersMap(
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
				@author Hugues Romain
				@date 2008
			*/
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;



			/** Authorization control.
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized(
				const security::User& user
			) const;



			/** Sub pages getter.
				@return PageLinks Ordered vector of sub pages links
				@author Hugues Romain
				@date 2008

				The default implementation handles the auto registration of administrative components by getSuperiorVirtual() method.
				This method can be overloaded to create customized sub tree.
			*/
			virtual PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

			virtual std::string getTitle() const;


			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;


			virtual AdminInterfaceElement::PageLinks _getCurrentTreeBranch() const;

			static void getHTMLLineSearchForm(
				std::ostream& stream,
				const html::HTMLForm& form,
				boost::optional<util::RegistryKeyType> networkId,
				boost::optional<const std::string&> lineName
			);

			//! @name Setters
			//@{
				void setNetwork(const TransportNetwork& value){ setRoot(value); }
			//@}
		};
	}
}

#endif // SYNTHESE_TransportNetworkAdmin_H__
