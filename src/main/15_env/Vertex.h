#ifndef SYNTHESE_ENV_VERTEX_H
#define SYNTHESE_ENV_VERTEX_H



namespace synthese
{
namespace env
{



/** Vertex base class.

A vertex is the abstract graph node used for route planning.

 @ingroup m15
*/
class Vertex
{

private:


protected:

    Vertex ();

public:

    virtual ~Vertex ();


    //! @name Getters/Setters
    //@{
//    virtual const AddressablePlace* getAddressablePlace () const = 0;  // Plutot connection place ??
    //@}

    
};


}
}

#endif 	    
