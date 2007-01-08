
#ifndef SYNTHESE_ReturnGuaranteeAction_H__
#define SYNTHESE_ReturnGuaranteeAction_H__


#include "30_server/Action.h"

namespace synthese
{
	namespace accounts
	{
		class Transaction;
	}

	namespace vinci
	{
		class VinciReturnGuaranteeAction : public server::Action
		{
		public:
			static const std::string PARAMETER_GUARANTEE_ID;

		private:
			accounts::Transaction* _guarantee;
			
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

			VinciReturnGuaranteeAction();
			~VinciReturnGuaranteeAction();
		};
	}
}

#endif // SYNTHESE_AddGuaranteeAction_H__


