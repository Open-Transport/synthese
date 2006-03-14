#ifndef SYNTHESE_ENV_DOCUMENT_H
#define SYNTHESE_ENV_DOCUMENT_H


#include "Registrable.h"

#include <string>


namespace synthese
{
namespace env
{




/** @ingroup m15 */
class Document : public Registrable<int,Document>
{
public:
    
    typedef enum
        {
            DOCUMENT_TYPE_PDF,
            DOCUMENT_TYPE_HTML,
            DOCUMENT_TYPE_GIF,
            DOCUMENT_TYPE_JPEG,
            DOCUMENT_TYPE_PNG
	} DocumentType;

    
private:

    std::string _url; //!< Document url for display with web server
    DocumentType _type;    //!< Document type
    std::string _globalDescription; //!< Context independant description
    std::string _localDescription;  //!< Context dependant description (ex : relative to a certain page)
    
 public:

        Document( const int& id,
		  const std::string& url,
		  const std::string& globalDescription,
		  const std::string& localDescription);

        virtual ~Document();


	//! @name Getters/Setters
	//@{
        const std::string& getUrl () const;
        DocumentType getType () const;
        const std::string& getLocalDescription() const;
        const std::string& getGlobalDescription () const;
        //@}
	
};


}
}

#endif
