#ifndef SYNTHESE_COMPLIANCE_H
#define SYNTHESE_COMPLIANCE_H


#include "01_util/Registrable.h"

#include <boost/logic/tribool.hpp>
#include <string>

namespace synthese
{
namespace env
{




/** Compliance class.
 @ingroup m15
 */
class Compliance : public synthese::util::Registrable<int,Compliance>
{
private:
        boost::logic::tribool _compliant;
	int _capacity;

protected:

        Compliance (const int& id, 
		    const boost::logic::tribool& compliant,
		    const int& capacity);
	
public:

        ~Compliance();

	//! @name Getters/Setters
	//@{
	int getCapacity () const;
        boost::logic::tribool isCompliant () const;
	//@}

};


}
}


#endif
