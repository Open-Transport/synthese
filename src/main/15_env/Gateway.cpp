#include "Gateway.h"



namespace synthese
{
namespace env
{


Gateway::Gateway (int rank, const std::string& name, const LogicalPlace* logicalPlace)
    : _rank (rank)
    , _name (name)
    , _logicalPlace (logicalPlace)
{
}


Gateway::~Gateway()
{
}



int
Gateway::getRank () const 
{ 
    return _rank;
}

const std::string& 
Gateway::getName () const
{
    return _name;
}


const LogicalPlace* 
Gateway::getLogicalPlace() const
{
    return _logicalPlace;
}








}
}
