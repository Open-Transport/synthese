
#ifndef SYNTHESE_UpdateCustomerAction_H__
#define SYNTHESE_UpdateCustomerAction_H__


#include "30_server/Action.h"

namespace synthese
{
	namespace vinci
	{
		class VinciUpdateCustomerAction : public server::Action
		{
		public:
			static const std::string PARAMETER_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_SURNAME;
			static const std::string PARAMETER_ADDRESS;
			static const std::string PARAMETER_POST_CODE;
			static const std::string PARAMETER_CITY;
			static const std::string PARAMETER_COUNTRY;
			static const std::string PARAMETER_EMAIL;
			static const std::string PARAMETER_PHONE;

		private:
			uid			_id;
			std::string _name;
			std::string _surname;
			std::string _address;
			std::string _postCode;
			std::string _city;
			std::string _country;
			std::string _email;
			std::string _phone;

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

#endif // SYNTHESE_UpdateCustomerAction_H__
