
#ifndef SYNTHESE_CityListRequest_H__
#define SYNTHESE_CityListRequest_H__

#include "30_server/Request.h"

namespace synthese
{
	namespace placeslist
	{

		/** City list request.
			@ingroup m36

		*/
		class CityListRequest : public server::Request
		{
		private:
			std::string _input;
			size_t _n;

		protected:
			ParametersMap getParametersMap() const;
			void setFromParametersMap(const ParametersMap& map);

		public:
			void run(std::ostream& stream) const;

		};
	}
}

#endif // SYNTHESE_CityListRequest_H__
