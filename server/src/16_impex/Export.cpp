/** Export class implementation.
	@file Export.cpp

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

#include "Export.hpp"

#include "Profile.h"
#include "Session.h"
#include "User.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	CLASS_DEFINITION(impex::Export, "t115_exports", 115)
	FIELD_DEFINITION_OF_OBJECT(impex::Export, "export_id", "export_ids")



	namespace impex
	{
		Export::Export(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<Export, ExportRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(FileFormatKey),
					FIELD_DEFAULT_CONSTRUCTOR(Parameters),
					FIELD_VALUE_CONSTRUCTOR(Active, true),
					FIELD_DEFAULT_CONSTRUCTOR(Documentation)
			)	)
		{}



		void Export::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(isPermanentThread())
			{
				if(get<Active>() && (&env == &Env::GetOfficialEnv()))
				{
					_getAutoExporter()->runPermanentThread();
				}
			}
		}



		void Export::unlink()
		{
			// Kill the thread if exists
			if(_autoExporter)
			{
				if(isPermanentThread())
				{
					_autoExporter->killPermanentThread();
				}

				// Delete the auto importer cache in case of parameter update
				_autoExporter.reset();
			}
		}



		boost::shared_ptr<Exporter> Export::getExporter() const
		{
			boost::shared_ptr<FileFormat> fileFormat(Factory<FileFormat>::create(get<FileFormatKey>()));
			return fileFormat->getExporter(*this);
		}



		void Export::addAdditionalParameters( util::ParametersMap& map, std::string prefix ) const
		{
			map.merge(get<Parameters>());
		}



		boost::shared_ptr<Exporter> impex::Export::_getAutoExporter() const
		{
			if(!_autoExporter.get())
			{
				boost::shared_ptr<FileFormat> fileFormat(Factory<FileFormat>::create(get<FileFormatKey>()));
				_autoExporter = fileFormat->getExporter(*this);
				_autoExporter->setFromParametersMap(get<Parameters>());
			}
			return _autoExporter;
		}



		bool impex::Export::canExport() const
		{
			if(!Factory<FileFormat>::contains(get<FileFormatKey>()))
			{
				return false;
			}

			boost::shared_ptr<FileFormat> fileFormat(Factory<FileFormat>::create(get<FileFormatKey>()));
			return fileFormat->canExport();
		}



		bool impex::Export::isPermanentThread() const
		{
			if(!Factory<FileFormat>::contains(get<FileFormatKey>()))
			{
				return false;
			}

			boost::shared_ptr<FileFormat> fileFormat(Factory<FileFormat>::create(get<FileFormatKey>()));
			return fileFormat->isExportPermanentThread();
		}


		bool impex::Export::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool impex::Export::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool impex::Export::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
