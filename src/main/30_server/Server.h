#ifndef SYNTHESE_SERVER_SERVER_H
#define SYNTHESE_SERVER_SERVER_H

#include "module.h"


#include <iostream>
#include <string>
#include <map>

#include "30_server/ServerConfig.h"
#include "30_server/ServerModuleException.h"

#include <boost/filesystem/path.hpp>


namespace synthese
{
	namespace util
	{
		class ModuleClass;
	}

	namespace server
	{
		class Session;

		/** Main server class.
			@ingroup m30

			This class holds :
				- the TCP server 
				- request queue (thread-safe access)
				- the threading mechanism 
				- the request dispatcher instance

		*/
		class Server
		{
		public:
			typedef std::map<std::string, Session*> SessionMap;
			typedef std::map<std::string, util::ModuleClass*> ModuleMap;

		private:

			static Server* _instance;

			boost::filesystem::path _dbFile;
			ServerConfig _config;

			ModuleMap _modules;
			SessionMap _sessionMap;

		public:

			Server (const boost::filesystem::path& dbFile);
			~Server ();


			//! @name Getters/Setters
			//@{

		    	const ServerConfig& getConfig () const;
				SessionMap& getSessions();

		    
			//@}


			//! @name Query methods
			//@{
				static Server* Server::GetInstance ();
				static void Server::SetInstance (Server* instance);

				template <class T>
					static util::ModuleClass* GetModule()
				{
					std::string key = util::Factory<util::ModuleClass>::getKey<T>();
					ModuleMap::iterator it = GetInstance()->_modules.find(key);
					if (it != GetInstance()->_modules.end())
						throw ServerModuleException("Module not found");
					return it->second;
				}


			//@}


			//! @name Update methods
			//@{
			void initialize ();

			void run ();


			//@}

		};
	}
}

#endif
