
#ifndef SYNTHESE_RentABikeAction_H__
#define SYNTHESE_RentABikeAction_H__


#include "30_server/Action.h"

namespace synthese
{
	namespace vinci
	{
		class VinciContract;

		class RentABikeAction : public server::Action
		{
		public:
			static const std::string PARAMETER_RATE_ID;
			static const std::string PARAMETER_BIKE_ID;
			static const std::string PARAMETER_CONTRACT_ID;

		private:
			uid				_rateId;
			uid				_bikeId;
			VinciContract*	_contract;

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

#endif 

