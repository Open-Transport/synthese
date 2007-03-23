
/** LogoutHTMLLinkInterfaceElement class header.
	@file LogoutHTMLLinkInterfaceElement.h

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

#ifndef SYNTHESE_LogoutHTMLLinkInterfaceElement_H__
#define SYNTHESE_LogoutHTMLLinkInterfaceElement_H__

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace server
	{
		class LogoutHTMLLinkInterfaceElement : public interfaces::ValueInterfaceElement
		{
			interfaces::ValueInterfaceElement* _redirectionURL;
			interfaces::ValueInterfaceElement* _page_key;
			interfaces::ValueInterfaceElement* _content;
			interfaces::ValueInterfaceElement* _icon;

		public:
			/** Parameters parser.
				Parameters order :
					-# Redirection URL after logout (empty if the following parameter is filled)
					-# Page key to redirect after logout (empty if URL is filled)
					-# Text to put in the link
			*/
			void storeParameters(interfaces::ValueElementList& vel);
			std::string getValue(const interfaces::ParametersVector& parameters, interfaces::VariablesMap& variables, const void* rootObject = NULL, const server::Request* request = NULL) const;
			LogoutHTMLLinkInterfaceElement();
			~LogoutHTMLLinkInterfaceElement();
		};
	}
}

#endif // SYNTHESE_LogoutHTMLLinkInterfaceElement_H__

