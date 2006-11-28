
#ifndef SYNTHESE_AddUserAction_H__
#define SYNTHESE_AddUserAction_H__

#include "30_server/Action.h"

namespace synthese
{
	namespace security
	{
		class Profile;

		class AddUserAction : public server::Action
		{
		public:
			static const std::string PARAMETER_LOGIN;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_PROFILE_ID;

		private:
			std::string _login;
			std::string _name;
			const Profile* _profile;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::Request::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void setFromParametersMap(server::Request::ParametersMap& map);

		public:

			/** Action to run, defined by each subclass.
			*/
			void run();
		};
	}
}

#endif // SYNTHESE_AddUserAction_H__
