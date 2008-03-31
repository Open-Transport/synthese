#ifndef DBMODULECLASS_H_
#define DBMODULECLASS_H_


#include <boost/filesystem/path.hpp>

#include "01_util/ModuleClass.h"
#include <map>

namespace synthese
{
	namespace db
	{

		/** Recorder for a module with a db backend.
		    A special table t_999 config is mutualized through derived class to 
		    share specific parameters. 
		*/
	        class DbModuleClass : public util::ModuleClass
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
		    static void SetParameter (const std::string& name, 
					      const std::string& value);
		    
		protected:
		    
		    static void RegisterParameter (const std::string& name,
						   const std::string& defaultValue,
						   PtrCallback cb);

		
		    
		};

	}
}



#endif
