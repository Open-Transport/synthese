
#ifndef SYNTHESE_AddCustomerAction_H__
#define SYNTHESE_AddCustomerAction_H__


#include "30_server/Action.h"

namespace synthese
{
	namespace vinci
	{
		class AddCustomerAction : public server::Action
		{
		public:
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_SURNAME;

		private:
			std::string _name;
			std::string _surname;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:

			/** Action to run, defined by each subclass.
			*/
			void run();
		};
	}
}

#endif // SYNTHESE_AddCustomerAction_H__

