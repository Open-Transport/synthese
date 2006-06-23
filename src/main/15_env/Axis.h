#ifndef SYNTHESE_ENV_AXIS_H
#define SYNTHESE_ENV_AXIS_H


#include "01_util/Registrable.h"

#include <string>


namespace synthese
{
namespace env
{


/** Axis handling class.
 @ingroup m15
*/
class Axis : public synthese::util::Registrable<std::string,Axis>
{

    bool _free;   //!< Whether or not this axis is a free axis.
    bool _authorized;  //!< Whether or not this axis is an authorized axis.

    public:

    Axis (const std::string& id,
	  bool free = true, bool authorized = false);
    ~Axis ();

    //! @name Getters/Setters
    //@{
    bool isFree () const;
    bool isAuthorized () const;
    //@}


};

}
}


#endif
