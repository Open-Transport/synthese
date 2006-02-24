/** Alert message class header.
	@file cAlerte.h
*/

#ifndef SYNTHESE_CALERTE_H
#define SYNTHESE_CALERTE_H

#include <string>
#include "cMoment.h"


/** Alert message.
	@ingroup m05
*/
class cAlerte
{
public:
	typedef int Level;	//!< Level of alert

	static const Level LEVEL_WARNING;	//!< Warning alert : the solution could not be available, or requires action from the passenger (reservation...)
	static const Level LEVEL_INFO;		//!< Information alert : the solution is available, but some information is provided for better comfort

private:
	std::string	_Message;		//!< Message à afficher lors de l'applicabilité de l'alerte
	cMoment	_Debut;			//!< Début de la période d'applicabilité
	cMoment	_Fin;			//!< Fin de la période d'applicabilité
	Level		_Niveau;		//!< Niveau de l'alerte

public:
	//! \name constructeur
	//@{
		cAlerte();
	//@}
	
	//! \name Accesseurs
	//@{
		const std::string&	getMessage()	const;
		const Level&			Niveau()		const;
	//@}
	
	//! \name Modificateurs
	//@{
		void	setMessage(const std::string&);
	void	setMomentDebut(const cMoment&);
	void	setMomentFin(const cMoment&);
	void	setNiveau(const Level&);
	//@}
			
	//! \name Calculateurs
	//@{
	bool showMessage(const cMoment& __Debut, const cMoment& __Fin)	const;
	//@}
	
		// xml export methode
		/*template <class T> T& toXML(T& Tampon) const
		{
			Tampon << "<alerte><message>" << vMessage << "</message>";
			vMomentDebut.toXML(Tampon,"datedebut");
			vMomentFin.toXML(Tampon,"datefin");
			Tampon << "</alerte>";
			return (Tampon);
		}*/
};

#endif
