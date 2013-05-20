
/** ModuleClass class implementation.
	@file ModuleClass.cpp
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

#include "ModuleClass.h"

#include "AdminInterfaceElement.h"
#include "BaseAdminFunctionRequest.hpp"
#include "HTMLList.h"
#include "HTMLModule.h"
#include "Log.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace html;
	using namespace util;

	namespace server
	{
	    std::map<std::string, ModuleClass::PtrCallback> ModuleClass::_Callbacks;
	    std::map<std::string, std::string> ModuleClass::_Parameters;
	    std::map<std::string, std::string> ModuleClass::_DefaultParameters;


		void ModuleClass::SetDefaultParameters(
			const Parameters& parameters
		){
			_DefaultParameters = parameters;
	    }



		bool ModuleClass::HasParameter(
			const std::string& name
		){
			Parameters::const_iterator it = _Parameters.find (name);
			return (it != _Parameters.end ());
		}



		std::string ModuleClass::GetParameter(
			const std::string& name,
			const std::string defaultValue
		){
			Parameters::const_iterator it =
				_Parameters.find (name);
			if (it == _Parameters.end ()) return defaultValue;
			return it->second;
	    }

		const ModuleClass::Parameters &ModuleClass::GetParameters()
		{
			return _Parameters;
		}


	    void ModuleClass::SetParameter(
			const std::string& name,
			const std::string& value,
			bool runCallback
		){
			_Parameters[name] = value;

			Log::GetInstance ().info ("Parameter " + name + " set to : " + value);

			if (runCallback)
			{	// Launches corresponding callback (only one allowed right now)
				std::map<std::string, PtrCallback>::const_iterator it =
					_Callbacks.find (name);
				if (it == _Callbacks.end ()) return;
				PtrCallback cb = it->second;
				(*cb) (name, value);
			}
		}



	    void ModuleClass::RegisterParameter(
	    	const std::string& name,
			const std::string& defaultValue,
			PtrCallback cb
		){
			_Callbacks.insert (std::make_pair (name, cb));
			if (HasParameter (name) == false)
			{
				std::string defv (defaultValue);
				Parameters::const_iterator it =
				_DefaultParameters.find (name);

				if (it != _DefaultParameters.end ()) defv = it->second;

				SetParameter (name, defv);
			}
	    }



		void ModuleClass::UnregisterParameter(
			const std::string& name
		){
			// FIXME: investigate why this sometimes fails.
			//assert(HasParameter(name));
			_Callbacks.erase(name);
			_Parameters.erase(name);
			_DefaultParameters.erase(name);
		}



		void ModuleClass::addAdminPageParameters(
			ParametersMap& map,
			const Request& request
		) const	{
		}



		void ModuleClass::setAdminFromParametersMap( const util::ParametersMap& map )
		{
		}



		util::ParametersMap ModuleClass::getAdminParametersMap() const
		{
			return ParametersMap();
		}



		void ModuleClass::displayAdmin(
			ostream& stream,
			const server::Request& request,
			const AdminInterfaceElement& currentPage
		) const	{
			stream << "<h1>Informations sur le module</h1>";

			{
				HTMLList l;
				stream << l.open();
				stream << l.element() << "Code : " << getFactoryKey();
				stream << l.element() << "Nom : " << getName();
				stream << l.close();
			}

			stream << "<h1>Pages d'administration</h1>";

			stream << "<p>Les liens suivants donnent accès aux pages d'administration du module " << getName() << ".</p>";

			HTMLList l;
			stream << l.open();

			AdminInterfaceElement::PageLinks links(
				currentPage.getSubPages(currentPage, request)
			);
			BaseAdminFunctionRequest r(request);
			BOOST_FOREACH(const boost::shared_ptr<const AdminInterfaceElement>& page, links)
			{
				r.setPage(const_pointer_cast<AdminInterfaceElement>(page));
				stream << l.element() << HTMLModule::getHTMLImage("/admin/img/" + page->getIcon(), page->getTitle());
				stream << HTMLModule::getHTMLLink(r.getURL(), page->getTitle());
			}
			stream << l.close();
		}

}	}
