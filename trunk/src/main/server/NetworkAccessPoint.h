
#ifndef SYNTHESE_NETWORKACCESSPOINT_H
#define SYNTHESE_NETWORKACCESSPOINT_H

#include "Point.h"
#include <string>
#include "temps.h"
#include <vector>
#include <utility>


class LogicalPlace;
class Address;

class NetworkAccessPoint : public cPoint
{
public:

	/** Acc�s � adresse avec dur�e */
	typedef pair<Address*, cDureeEnMinutes> AddressWithAccessDuration;

	/** Liste d'adresses avec dur�e d'acc�s */
	typedef vector<AddressWithAccessDuration> AddressList;

private:
	std::string		_name;	//!< Nom
	LogicalPlace* const	_logicalPlace;	//!< Lieu logique
	const size_t		_rank;			//!< Position dans le lieu logique

public:
	//!	@name Accesseurs
	//@{
		const std::string&	getNom()		const;
		LogicalPlace*		getLogicalPlace() const { return _logicalPlace; }
		size_t				getRankInLogicalPlace() const { return _rank; }
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