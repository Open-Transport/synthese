
#ifndef SYNTHESE_CALERTE_H
#define SYNTHESE_CALERTE_H

class cAlerte;

#include "cTexte.h"
#include "Temps.h"

// Niveaux d'alerte
#define ALERTE_ATTENTION	99
#define ALERTE_INFO			50

/*! \brief Messages d'alerte
	@ingroup m05
*/
class cAlerte
{
	cTexte	_Message;		//!< Message � afficher lors de l'applicabilit� de l'alerte
	cMoment	_Debut;			//!< D�but de la p�riode d'applicabilit�
	cMoment	_Fin;			//!< Fin de la p�riode d'applicabilit�
	int		_Niveau;		//!< Niveau de l'alerte

public:
	//! \name constructeur
	//@{
	cAlerte();
	//@}
	
	//! \name Accesseurs
	//@{
	const cTexte&	getMessage()	const;
	int				Niveau()		const;
	//@}
	
	//! \name Modificateurs
	//@{
	void	setMessage(const cTexte&);
	void	setMomentDebut(const cMoment&);
	void	setMomentFin(const cMoment&);
	void	setNiveau(int);
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

inline const cTexte& cAlerte::getMessage() const
{
	return (_Message);
}

inline void cAlerte::setMessage(const cTexte& __Message)
{
	_Message = __Message;
}

inline void cAlerte::setMomentDebut(const cMoment& __Debut)
{
	_Debut = __Debut;
}

inline void cAlerte::setMomentFin(const cMoment& __Fin)
{
	_Fin = __Fin;
}



/*!	\brief Accesseur Niveau de l'alerte
	\return Le niveau de l'alerte
*/
inline int cAlerte::Niveau() const
{
	return _Niveau;
}

inline void cAlerte::setNiveau(int __Niveau)
{
	_Niveau = __Niveau;
}
#endif
