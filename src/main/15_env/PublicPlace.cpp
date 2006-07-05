#include "PublicPlace.h"



namespace synthese
{
namespace env
{


PublicPlace::PublicPlace (const uid& id,
			  const std::string& name,
			  const City* city)

    : synthese::util::Registrable<uid,PublicPlace> (id)
    , Place (name, city)
{

}



PublicPlace::~PublicPlace ()
{


}



}
}
