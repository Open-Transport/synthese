#include "DbModuleClass.h"

#include "01_util/Log.h"


using synthese::util::Log;


namespace synthese
{
	namespace db
	{
	    boost::filesystem::path DbModuleClass::_DatabasePath;

	    std::map<std::string, typename DbModuleClass::PtrCallback> DbModuleClass::_Callbacks;
	    std::map<std::string, std::string> DbModuleClass::_Parameters;
	    std::map<std::string, std::string> DbModuleClass::_DefaultParameters;

	    
	    DbModuleClass::DbModuleClass()
	    {
		
	    }

	    

	    void 
	    DbModuleClass::SetDefaultParameters (const Parameters& parameters)
	    {
		_DefaultParameters = parameters;
	    }


	    const boost::filesystem::path& 
	    DbModuleClass::GetDatabasePath ()
	    {
		return _DatabasePath;
	    }

	    
	    void 
	    DbModuleClass::SetDatabasePath (const boost::filesystem::path& databasePath)
	    {
		_DatabasePath = databasePath;
	    }



	    
	    bool 
	    DbModuleClass::HasParameter (const std::string& name)
	    {
		Parameters::const_iterator it = 
		    _Parameters.find (name);
		return (it != _Parameters.end ());
	    }



	    std::string 
	    DbModuleClass::GetParameter (const std::string& name)
	    {
		Parameters::const_iterator it = 
		    _Parameters.find (name);
		if (it == _Parameters.end ()) return "";
		return it->second;
	    }




	    void
	    DbModuleClass::SetParameter (const std::string& name, 
					 const std::string& value)
	    {
		_Parameters.insert (std::make_pair (name, value));
		
		Log::GetInstance ().info ("Parameter " + name + " set to : " + value);
		
		// Launches corresponding callback (only one allowed right now)
		std::map<std::string, PtrCallback>::const_iterator it = 
		    _Callbacks.find (name);
		if (it == _Callbacks.end ()) return;
		PtrCallback cb = it->second;
		(*cb) (name, value);
	    }


	    
	    void 
	    DbModuleClass::RegisterParameter (const std::string& name,
					      const std::string& defaultValue,
					      PtrCallback cb)
	    {
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


	}
}
