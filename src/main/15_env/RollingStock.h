#ifndef SYNTHESE_ENV_ROLLING_STOCK_H
#define SYNTHESE_ENV_ROLLING_STOCK_H


#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include <string>

namespace synthese
{
namespace env
{


/** Rolling stock.
 @ingroup m15
*/
class RollingStock : public synthese::util::Registrable<uid,RollingStock>
{

        std::string _name;
        std::string _article;
        std::string _indicator;

    public:

        RollingStock (const uid& id,
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
