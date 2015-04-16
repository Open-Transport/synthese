#include "EnvironmentSyncException.h"


namespace synthese
{
namespace envlssql
{


EnvironmentSyncException::EnvironmentSyncException ( const std::string& message)
    : synthese::Exception (message)
{
}



EnvironmentSyncException::~EnvironmentSyncException () throw ()
{

}





}
}



