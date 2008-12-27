////////////////////////////////////////////////////////////////////////////////
/// DbModuleClass class header.
///	@file DbModuleClass.h
///	@author Marc Jambert
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef DBMODULECLASS_H_
#define DBMODULECLASS_H_

#include "ModuleClass.h"

#include <boost/filesystem/path.hpp>
#include <map>

namespace synthese
{
	namespace db
	{
		////////////////////////////////////////////////////////////////////
		/// Recorder for a module with a db backend.
		/// A special table t_999 config is mutualized through derived class to 
		/// share specific parameters. 
		class DbModuleClass
		:	public util::ModuleClass
		{
		public:
		    typedef std::map<std::string, std::string> Parameters;

		private:

		    typedef void (*PtrCallback) (const std::string& name, const std::string& value);		    

		    static std::map<std::string, PtrCallback> _Callbacks;
		    static Parameters _DefaultParameters;
		    static Parameters _Parameters;
		    static boost::filesystem::path _DatabasePath;
		    

		public:
		    DbModuleClass();

		    static void SetDefaultParameters (const Parameters& parameters);
		    
		    static const boost::filesystem::path& GetDatabasePath ();
		    static void SetDatabasePath (const boost::filesystem::path& databasePath);
		    
		    static bool HasParameter (const std::string& name);
		    static std::string GetParameter (const std::string& name, 
						     const std::string defaultValue = "");
		    static void SetParameter(
				const std::string& name
				, const std::string& value
				, bool runCallback = true
			);
		    
		protected:
		    
		    static void RegisterParameter (const std::string& name,
						   const std::string& defaultValue,
						   PtrCallback cb);

		
		    
		};

	}
}



#endif
