
#include "VinciPrintedContractInterfacePage.h"

namespace synthese
{
	using namespace interfaces;

	namespace vinci
	{

		void VinciPrintedContractInterfacePage::display(std::ostream& stream
			, const VinciContract* contract
			, const std::string& address
			, const std::string& birthdate
			, const std::string& phone
			, const std::string& passport
			, const std::string& drivingLicence
			, const std::string& bikeNumber
			, const std::string& antivolNumber
			, const std::string& startDate
			, const std::string& endDate
			, const std::string& fare
			, const std::string& cautionType


			, const server::Request* request /*= NULL*/) const
		{
			ParametersVector pv;
			pv.push_back(Conversion::ToString(contract->getKey()));
			pv.push_back(Conversion::ToString(contract->getUser()->getName()));
			pv.push_back(Conversion::ToString(contract->getUser()->getSurname()));
			pv.push_back(name);
			pv.push_back(surname);
			pv.push_back(address);
			pv.push_back(birthdate);
			pv.push_back(phone);
			pv.push_back(passport);
			pv.push_back(drivingLicence);
			pv.push_back(bikeNumber);
			pv.push_back(antivolNumber);
			pv.push_back(startDate);
			pv.push_back(fare);
			pv.push_back(cautionType);

			InterfacePage::display(stream, pv, NULL, request);
		}
	}
}
 