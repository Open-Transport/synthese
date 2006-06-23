#include "PublicPlace.h"



namespace synthese
{
namespace env
{


PublicPlace::PublicPlace (const int& id,
			  const std::string& name,
			  const City* city)

    : synthese::util::Registrable<int,PublicPlace> (id)
    , Place (name, city)
{

}



PublicPlace::~PublicPlace ()
{


}



}
}
