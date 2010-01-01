
/** TransportNetworkAdmin class header.
	@file TransportNetworkAdmin.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_TransportNetworkAdmin_H__
#define SYNTHESE_TransportNetworkAdmin_H__

#include "ResultHTMLTable.h"

#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace pt
	{
		class TransportNetwork;
	
		/** TransportNetworkAdmin Class.
			@ingroup m35Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class TransportNetworkAdmin : public admin::AdminInterfaceElementTemplate<TransportNetworkAdmin>
		{
			boost::shared_ptr<const pt::TransportNetwork>	_network;

			std::string	_searchName;

			html::ResultHTMLTable::RequestParameters	_requestParameters;
			
		public:
			static const std::string PARAMETER_SEARCH_NAME;
			
			
			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
			*/
			void setFromParametersMap(
				const server::ParametersMap& map
			);
			
			
			/** Parameters map generator, used when building an url to the admin page.
					@return server::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007					
				*/
			virtual server::ParametersMap getParametersMap() const;
			
			

			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@author Hugues Romain
				@date 2008
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& variables,
					const admin::AdminRequest& _request) const;
			
			/** Authorization control.
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized(
				const security::Profile& profile
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
				const admin::AdminRequest& request
			) const;

			virtual std::string getTitle() const;
			
			
			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;
			
			//! @name Setters
			//@{
				void setNetwork(boost::shared_ptr<const pt::TransportNetwork> value);
			//@}
		};
	}
}

#endif // SYNTHESE_TransportNetworkAdmin_H__
