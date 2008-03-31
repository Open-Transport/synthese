
/** TridentExportAction class implementation.
@file TridentExportAction.cpp

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
#include "TridentExportAction.h"

#include "01_util/Conversion.h"
#include "01_util/Log.h"
#include "01_util/iostreams/Archive.h"


#include "11_interfaces/Interface.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ServerModule.h"
#include "30_server/ParametersMap.h"

#include "15_env/CommercialLine.h"
#include "15_env/TransportNetwork.h"
#include "TridentExport.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>

#include <fstream>


using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace interfaces;
	using namespace db;
	using namespace env;

	template<> const string util::FactorableTemplate<Action, impex::TridentExportAction>::FACTORY_KEY("tridentexport");

	namespace impex
	{
		const string TridentExportAction::PARAMETER_COMMERCIAL_LINE_REGEX = Action_PARAMETER_PREFIX + "cl_regex";
		const string TridentExportAction::PARAMETER_NETWORK_REGEX = Action_PARAMETER_PREFIX + "nw_regex";
		const string TridentExportAction::PARAMETER_ARCHIVE_BASENAME = Action_PARAMETER_PREFIX + "cl_archbn";


		ParametersMap TridentExportAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_COMMERCIAL_LINE_REGEX, _commercialLineRegex);
			map.insert(PARAMETER_NETWORK_REGEX, _commercialLineRegex);
			map.insert(PARAMETER_ARCHIVE_BASENAME, _archiveBasename);
			return map;
		}

		void TridentExportAction::_setFromParametersMap(const ParametersMap& map)
		{
			_commercialLineRegex = map.getString(PARAMETER_COMMERCIAL_LINE_REGEX, false, FACTORY_KEY);
			if (_commercialLineRegex.empty()) 
			    _commercialLineRegex = ".*";
			
			_networkRegex = map.getString(PARAMETER_NETWORK_REGEX, false, FACTORY_KEY);
			if (_networkRegex.empty()) 
			    _networkRegex = ".*";
			
			_archiveBasename = map.getString(PARAMETER_ARCHIVE_BASENAME, false, FACTORY_KEY);
			if (_archiveBasename.empty())
			    _archiveBasename = "trident_export";
		}

		void TridentExportAction::run()
		{
		    // Create the regex 
		    boost::regex clRegex (_commercialLineRegex);
		    boost::regex nwRegex (_networkRegex);
		    
		    // Create archive directory in global temp directory
		    const boost::filesystem::path& tempDir = ServerModule::GetParameter (ServerModule::MODULE_PARAM_TMP_DIR);
		    const boost::filesystem::path archiveDir (tempDir / _archiveBasename);
		    
		    // Create tar ostream
		    const boost::filesystem::path archfp (tempDir / (_archiveBasename + ".tar"));
		    std::ofstream archos (archfp.string ().c_str ());

		    boost::filesystem::remove (archiveDir);
		    boost::filesystem::create_directory(archiveDir);
		    Archive::Tar (tempDir, boost::filesystem::path(archiveDir.leaf ()), archos);
		    
		    for (CommercialLine::ConstIterator it = CommercialLine::Begin();
			 it != CommercialLine::End (); ++it)
		    {
			boost::shared_ptr<CommercialLine> cl = it->second;
			std::string name = cl->getName ();
			std::string networkName = cl->getNetwork ()->getName ();

			if (boost::regex_match (name, clRegex) == false) continue;
			if (boost::regex_match (networkName, nwRegex) == false) continue;
			
			std::string filename ("trident_line_" + cl->getShortName () + "_" + Conversion::ToString (cl->getKey ()) + ".xml");
			
			boost::filesystem::path xmlfp (archiveDir.string () + "/" + filename);
			std::ofstream out (xmlfp.string ().c_str (), std::ios_base::binary);
			TridentExport::Export (out, cl->getKey ());
			out.close ();

			// Add the file to the archive
			Archive::Tar (tempDir, boost::filesystem::path(archiveDir.leaf () + "/" + filename), archos);
			boost::filesystem::remove (xmlfp);

			Log::GetInstance ().debug ("Commercial line " + cl->getName () + " exported to " + filename);
		    }
		    
		    boost::filesystem::remove (archiveDir);
		}
	}
}
