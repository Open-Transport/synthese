
#ifndef SYNTHESE_NETWORKACCESSPOINT_H
#define SYNTHESE_NETWORKACCESSPOINT_H

#include "Point.h"
#include <string>
#include "temps.h"
#include <set>
#include <utility>

class Address;
class LogicalPlace;

class NetworkAccessPoint : public cPoint
{
public:

	/** Liste d'adresses avec durée d'accès */
	typedef vector<pair<Address*, cDureeEnMinutes> > AddressList;

private:
	std::string		_name;	//!< Nom
	LogicalPlace* const	_logicalPlace;	//!< Lieu logique
	const size_t		_rank;			//!< Position dans le lieu logique

public:
	//!	@name Accesseurs
	//@{
		const std::string&	getNom()		const;
		LogicalPlace*		getLogicalPlace() const { return _logicalPlace; }
	//@}

	//!	@name Calculateurs
	//@{
		virtual AddressList getAddresses(bool forDeparture) const = 0;
	//@}
	
	//!	@name Modificateurs
	//@{
		void setNom(const std::string&);
	//@}

	//!	@name Constructeur et destructeur
	//@{
		NetworkAccessPoint(LogicalPlace*, size_t);
		~NetworkAccessPoint();
	//@}

};

#endif