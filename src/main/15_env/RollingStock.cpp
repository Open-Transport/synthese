#include "RollingStock.h"


namespace synthese
{
namespace env
{


RollingStock::RollingStock (const int& id,
			    const std::string& name,
			    const std::string& article,
			    const std::string& indicator)
    : Registrable<int,RollingStock> (id)
    , _name (name)
    , _article (article)
    , _indicator (indicator)
{
}



RollingStock::~RollingStock()
{

}


const std::string& 
RollingStock::getName() const
{
    return _name;
}


const std::string& 
RollingStock::getArticle() const
{
    return _article;
}



const std::string& 
RollingStock::getIndicator() const
{
    return _indicator;
}





}
}

