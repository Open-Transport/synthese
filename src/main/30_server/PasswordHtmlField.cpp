
#include <sstream>

#include "30_server/LoginAction.h"
#include "30_server/PasswordHtmlField.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;

	namespace server
	{
		void PasswordHtmlField::storeParameters(ValueElementList& vel)
		{
		}

		string PasswordHtmlField::getValue(const interfaces::ParametersVector& parameters, const void* rootObject /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			stringstream stream;
			stream << "<input type=\"password\" name=\"" << LoginAction::PARAMETER_PASSWORD << "\" />";
			return stream.str();
		}

		PasswordHtmlField::~PasswordHtmlField()
		{
		}
	}
}
