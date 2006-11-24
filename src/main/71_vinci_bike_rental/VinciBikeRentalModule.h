
#include "01_util/ModuleClass.h"

namespace synthese
{
	namespace vinci
	{

		/** @defgroup m71 71 Vinci bike rental.
			
			Most of the events are treated as transaction parts :
				- customer giving a guarantee :
					- positive amount on the template guarantee account, tagged with the corresponding user
					- negative amount on the financial account used for payment (warning : the financial accounts for guarantee should be separated for the ones corresponding to the standard payments in several organizations)
				- destroying of a customer guarantee :
					- negative amount on the template guarantee account, tagged with the corresponding user
					- positive amount on the financial account used for the payment
				- starting a bike rental :
					- +1 on the bike stock of the used station, tagged with the number of the corresponding bike in the tradedObjectId field
					- -1 on the template outgoing bikes account, tagged :
						- with the corresponding user
						- with the number of the corresponding bike in the tradedObjectId field 
					- if the fare needs payment :
						- negative starting price on the template customer account, tagged with the corresponding user
						- positive starting price (deducted with the taxes) on the bike rental product account, tagged with the corresponding fare
						- positive starting tax on the tax account
					- if the fare needs ticket uses :
						- -1 on the template customer tickets account
						- +1 on the bike rental product payed with transport tickets account, tagged with the corresponding fare
				- ending a bike rental :
					- -1 on the bike stock of the used station, tagged with the number of the corresponding bike in the tradedObjectId field
					- +1 on the template outgoing bikes account, tagged :
						- with the corresponding user
						- with the number of the corresponding bike in the tradedObjectId field 
					- if the fare needs payment :
						- negative starting price on the template customer account, tagged with the corresponding user
						- positive starting price (deducted with the taxes) on the bike rental product account, tagged with the corresponding fare
						- positive starting tax on the tax account
					- if the fare needs ticket uses :
						- -1 on the template customer tickets account
						- +1 on the bike rental product payed with transport tickets account, tagged with the corresponding fare
				- payment at the starting of the bike rental :
					- positive amount on the template customer account, tagged with the corresponding user
					- negative amount on the account used for the payment
				- payment with transport tickets uses :
					- positive amount on the template customer tickets account, tagged with the corresponding user
					- negative amount on the "ticket uses" account
			
			Remarks : the "ticket uses" and the payment accounts will never go down without 
				
				

		@{
		*/

		class VinciBikeRentalModule : public util::ModuleClass
		{
		public:
			void initialize();
		};
		/** @} */
	}
}

