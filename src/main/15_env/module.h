/** 15_env module wide definitions
	@file module.h
*/

#ifndef SYNTHESE_ENV_MODULE_H
#define SYNTHESE_ENV_MODULE_H


namespace synthese
{

	/** @defgroup m15 15 Environment data model
		
		@{
	*/

	/** 15_env namespace */
	namespace env
	{
	    static const int UNKNOWN_VALUE = -1;

	    typedef enum { FROM_ORIGIN, TO_DESTINATION } AccessDirection ;
	    
	    typedef struct {
		double maxApproachDistance;
		double maxApproachTime;
		double approachSpeed;
		
		bool bikeCompliance;
		bool handicappedCompliance;
		bool pedestrianCompliance;
		bool withReservation;
		
	    } AccessParameters;
	    
	}

	/** @} */

}

#endif
