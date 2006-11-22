
#include <sstream>

#include "30_server/LoginAction.h"
#include "30_server/LoginHtmlField.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;

	namespace server
	{
		void LoginHtmlField::storeParameters(ValueElementList& vel)
		{
		}

		string LoginHtmlField::getValue(const interfaces::ParametersVector& parameters, const void* rootObject /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			stringstream stream;
			stream << "<input type=\"text\" name=\"" << Action::PARAMETER_PREFIX << LoginAction::PARAMETER_LOGIN << "\" />";
			return stream.str();
		}

		LoginHtmlField::~LoginHtmlField()
		{
		}
	}
}
