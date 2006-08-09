#include "PublicPlace.h"



namespace synthese
{
namespace env
{


PublicPlace::PublicPlace (const uid& id,
			  const std::string& name,
			  const City* city)

    : synthese::util::Registrable<uid,PublicPlace> (id)
    , AddressablePlace (name, city)
{

}



PublicPlace::~PublicPlace ()
{


}



}
}
