#ifndef SYNTHESE_ENV_GATEWAY_H
#define SYNTHESE_ENV_GATEWAY_H

#include <string>
#include <vector>
#include <utility>


namespace synthese
{
namespace env
{

class LogicalPlace;



/** @ingroup m15 */
class Gateway 
{
private:

    int _rank; //!< Rank in logical place
    std::string _name; //!< Name
    const LogicalPlace* _logicalPlace; //!< Enclosing logical place.

 public:

    Gateway (const int rank,
	     const std::string& name, 
	     const LogicalPlace* logicalPlace);
    virtual ~Gateway();

    //! @name Getters/Setters
    //@{
    int getRank () const;
    const std::string& getName () const;
    const LogicalPlace* getLogicalPlace() const;
    //@}

};


}
}

#endif
