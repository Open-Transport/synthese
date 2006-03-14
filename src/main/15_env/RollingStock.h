#ifndef SYNTHESE_ENV_ROLLING_STOCK_H
#define SYNTHESE_ENV_ROLLING_STOCK_H


#include "Registrable.h"
#include <string>

namespace synthese
{
namespace env
{


/** Rolling stock.
 @ingroup m15
*/
class RollingStock : public Registrable<int,RollingStock>
{

        std::string _name;
        std::string _article;
        std::string _indicator;

    public:

        RollingStock (const int& id,
		      const std::string& name,
		      const std::string& article,
		      const std::string& indicator);

        ~RollingStock();


	//! @name Getters/Setters
	//@{
        const std::string& getName() const;
        const std::string& getArticle() const;
        const std::string& getIndicator() const;
        //@}


};


}
}


#endif
