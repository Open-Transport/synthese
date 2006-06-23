#include "Document.h"



namespace synthese
{
namespace env
{



Document::Document( const int& id,
		    const std::string& url,
		    const std::string& globalDescription,
		    const std::string& localDescription)
    : synthese::util::Registrable<int,Document> (id)
    , _url (url)
    , _globalDescription (globalDescription)
    , _localDescription (localDescription)
{

}




Document::~Document()
{
}





const std::string& 
Document::getUrl () const
{
    return _url;
}
        


Document::DocumentType 
Document::getType () const
{
    return _type;
}




const std::string& 
Document::getLocalDescription() const
{
    return _localDescription;
}



const std::string& 
Document::getGlobalDescription () const
{
    return _globalDescription;
}



 








}
}

