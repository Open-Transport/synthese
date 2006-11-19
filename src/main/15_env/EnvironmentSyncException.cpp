#include "EnvironmentSyncException.h"


namespace synthese
{
namespace envlssql
{


EnvironmentSyncException::EnvironmentSyncException ( const std::string& message)
    : synthese::util::Exception (message)
{
}



EnvironmentSyncException::~EnvironmentSyncException () throw ()
{

}





}
}


