
/** InterSYNTHESEPackageContent class header.
	@file InterSYNTHESEPackageContent.hpp

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

#ifndef SYNTHESE_inter_synthese_InterSYNTHESEPackageContent_hpp__
#define SYNTHESE_inter_synthese_InterSYNTHESEPackageContent_hpp__

#include "Env.h"
#include "FrameworkTypes.hpp"
#include "InterSYNTHESEPackage.hpp"

#include <boost/property_tree/ptree.hpp>
#include <map>
#include <set>
#include <utility>

namespace synthese
{
	namespace db
	{
		class DBTransaction;
	}

	namespace inter_synthese
	{
		//////////////////////////////////////////////////////////////////////////
		/// Content of an inter-synthese package.
		/// This class stores all objects contained in a package.
		///	@ingroup m19
		class InterSYNTHESEPackageContent
		{
		private:
			util::Env& _env;
			boost::shared_ptr<InterSYNTHESEPackage> _package;
			boost::property_tree::ptree _objects;
			typedef std::map<std::pair<util::RegistryKeyType, std::string>, std::string> ContentMap;
			std::set<util::RegistryKeyType> _objectsToRemove;
			std::vector<const util::Registrable*> _objectsToSave;

			Objects::Type _loadObjects(
				const boost::property_tree::ptree& node,
				const ContentMap& contentMap,
				boost::optional<const impex::Importer&> importer
			);

			void _parseAndLoad(
				const std::string& s,
				boost::optional<const impex::Importer&> importer
			);

		public:
			InterSYNTHESEPackageContent(
				util::Env& env,
				const std::string& s,
				impex::Import& import,
				boost::optional<const impex::Importer&> importer
			);

			InterSYNTHESEPackageContent(
				util::Env& env,
				const std::string& s,
				const boost::shared_ptr<InterSYNTHESEPackage>& pacakge,
				boost::optional<const impex::Importer&> importer
			);

			void save(
				db::DBTransaction& transaction
			) const;
		};
	}
}

#endif // SYNTHESE_inter_synthese_InterSYNTHESEPackageContent_hpp__
