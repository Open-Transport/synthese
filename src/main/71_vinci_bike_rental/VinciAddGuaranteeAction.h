
#ifndef SYNTHESE_AddGuaranteeAction_H__
#define SYNTHESE_AddGuaranteeAction_H__


#include "30_server/Action.h"

namespace synthese
{
	namespace vinci
	{
		class VinciContract;

		class VinciAddGuaranteeAction : public server::Action
		{
		public:
			static const std::string PARAMETER_AMOUNT;
			static const std::string PARAMETER_CONTRACT_ID;

		private:
			double	_amount;
			VinciContract* _contract;

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

			VinciAddGuaranteeAction();
			~VinciAddGuaranteeAction();
		};
	}
}

#endif // SYNTHESE_AddGuaranteeAction_H__
