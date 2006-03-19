#include "Path.h"

#include "Service.h"


#include <assert.h>


namespace synthese
{
namespace env
{



Path::Path ()
    : Regulated (0) // No parent regulation right now
{
    
}
    


Path::~Path ()
{
}




const std::vector<Service*>& 
Path::getServices () const
{
    return _services;
}




const Service* 
Path::getService (int serviceNumber) const
{
    return _services.at (serviceNumber);
}




void 
Path::addService (Service* service)
{
    _services.push_back (service);
}




}
}
