
#ifndef SYNTHESE_CTRAJET_H
#define SYNTHESE_CTRAJET_H

class cTrajet;

#include "cElementTrajet.h"
#include "Temps.h"


/** Trajet
	@ingroup m33
	\author Hugues Romain
	\date 2005

	\todo Regarder si CalculeDuree ne peut pas etre int�gr�e � Finalise pour optimiser.
	
	Cette classe se nommait auparavant cDescriptionElementTrajet
*/
class cTrajet
{
private:
	cTexte			_NiveauMaxAlerte;			//!< Niveau maximal de messages d'alerte du trajet, li�s aux arr�ts ou aux lignes
	cElementTrajet*	_PremierET;					//!< Acc�s aux donn�es du trajet
	cElementTrajet*	_DernierET;					//!< Acc�s direct au dernier �l�ment du trajet
	int				_Taille;					//!< Nombre d'�l�ments contenus dans le trajet
	tDureeEnMinutes	_Duree;						//!< Dur�e totale du trajet
	tDureeEnMinutes	_DureeRoulee;				//!< Temps pass� dans les transports
	tDureeEnMinutes _AmplitudeServiceContinu;	//!< Amplitude de desserte si service continu
		
	//!	\name M�thode de finalisation
	//@{
	void	CalculeDuree();
	//@}
	
public:
	//!	\name Accesseurs
	//@{
	const NetworkAccessPoint*		getOrigin()						const;
	const NetworkAccessPoint*		getDestination()				const;
	cElementTrajet*			getDernierElement();
	const tDureeEnMinutes&	getDuree()						const;
	const tDureeEnMinutes&	getDureeRoulee()				const;
	const cMoment&			getMomentArrivee()				const;
	const cMoment&			getMomentDepart()				const;
	const cTexte&			getNiveauMaxAlerte()			const;
	cElementTrajet*			getPremierElement();
	const cElementTrajet*	PremierElement()				const;
	const cElementTrajet*	DernierElement()				const;
	int						Taille()						const;
	tDureeEnMinutes			getAmplitudeServiceContinu()	const;
	//@}

	//!	\name Modificateurs
	//@{
	void					DeliePremier();
	void					DelieDernier();
	void					DelieTous();
	void					LieEnPremier(cElementTrajet*);
	void					LieEnDernier(cElementTrajet*);
	int						GenererNiveauxAlerte();
	void					setAmplitudeServiceContinu(const tDureeEnMinutes&);
	//void					setPremierET(cElementTrajet*);
	void					SupprimeDernier();
	void					SupprimePremier();
	void					Vide();
 	void					operator=(cTrajet&);
	void					Finalise();
	//@}
	
	//!	\name Calculateurs
	//@{
	int						operator>(const cTrajet&)		const;
	//@}
	
	cTrajet();
	~cTrajet();
};



/*!	\brief Suppression du lien vers l'ensemble des �l�ments de trajet sans les supprimer
	\author Hugues Romain
	\date 2005
	\warning Si les �l�ments de trajet ne sont pas point�s par ailleurs, ils seront laiss�s en m�moire sans moyen de les retrouver. Il s'agirait alors d'une fuite m�moire. A utiliser avec pr�caution sur ce point l�.
*/
inline void cTrajet::DelieTous()
{
	_PremierET = NULL;
	Vide();
}



/*!	\brief Accesseur niveau maximal d'alerte du trajet
	\return Le niveau maximal d'alerte sous format r�f�rence texte pour exploitabilit� directe dans le module d'interface en tant que texte dynamique (attribut)
	\author Hugues Romain
	\date 2005
*/
inline const cTexte& cTrajet::getNiveauMaxAlerte() const
{
	return _NiveauMaxAlerte;
}


inline int cTrajet::Taille() const
{
	return _Taille;
}


inline cElementTrajet* cTrajet::getPremierElement()
{
	return _PremierET;
}

inline const cElementTrajet* cTrajet::PremierElement() const
{
	return _PremierET;
}

inline const tDureeEnMinutes& cTrajet::getDuree() const
{
	return _Duree;
}


inline const tDureeEnMinutes& cTrajet::getDureeRoulee() const
{
	return _DureeRoulee;
}



/*!	\brief Transfert de la description d'un trajet vers un autre
	\param __Trajet Trajet � copier
	\author Hugues Romain
	\date 2005
	
Cet op�rateur effectue les op�rations suivantes :
*/
inline void cTrajet::operator =(cTrajet& __Trajet)
{
	/*!
	- Suppression des �l�ments de trajets anciennements pr�sents dans l'objet (s'il y a lieu)	*/
	Vide();
	
	/*!
	- Copie des donn�es du trajet � copier	*/
 	_Taille = __Trajet.Taille();
	_Duree = __Trajet.getDuree();
	_DureeRoulee = __Trajet.getDureeRoulee();
	_PremierET = __Trajet._PremierET;
	_DernierET = __Trajet._DernierET;
	_NiveauMaxAlerte = __Trajet._NiveauMaxAlerte;
	_AmplitudeServiceContinu = __Trajet._AmplitudeServiceContinu;
	
	/*!
	 - Effacement du trajet � copier (le trajet devient vide)	*/
	__Trajet.DelieTous();
}


inline const cElementTrajet* cTrajet::DernierElement() const
{
	return _DernierET;
}


inline cElementTrajet* cTrajet::getDernierElement()
{
	return _DernierET;
}



/*!	\brief Op�rateur de comparaison entre deux trajets du point de vue de l'int�r�t � �tre s�lectionn� dans un calcul d'itin�raires
	\param __Trajet Trajet � comparer � l'objet courant
	\return true si le trajet courant est jug� meilleur que le trajet � comparer
	\author Hugues Romain
	\date 2005
	
La comparaison intervient entre deux trajets cens�s remplir les m�mes fonctionnalit�s :
 - Ils vont tous deux du m�me point vers le m�me autre
 - Ils sont tous deux susceptibles d'�tre s�lectionn�s par rapport � leurs heures de d�part et/ou d'arriv�e
 - Ils sont tous deux utilisables sans condition, vis � vis des r�gles usuelles et des filtres d�finis par l'utilisateur
 
La comparaison est �tablie sur les crit�res suivants, dans l'ordre d�croissant d'importance :
*/
inline int cTrajet::operator >(const cTrajet& __Trajet) const
{
	/*! <li>Un trajet vide ne peut �tre sup�rieur � un autre</li> */
	if (!Taille())
		return false;
		
	/*! <li>Un trajet non vide est sup�rieur � un trajet vide</li> */
	if (!__Trajet.Taille())
		return true;
		
	/*! <li>Un trajet de dur�e strictement plus courte qu'un autre est sup�rieur � celui-ci</li> */
	if (getDuree() != __Trajet.getDuree())
		return getDuree() < __Trajet.getDuree();
	
	/*! <li>Un trajet comportant moins de correspondances qu'un autre est sup�rieur � celui-ci</li> */
	if (Taille() != __Trajet.Taille())
		return Taille() < __Trajet.Taille();
	
	/*! <li>Un trajet o� l'on circule moins longtemps est sup�rieur � celui-ci (plus de marge de fiabilit� pour les correspondaces)</li> */
	if (getDureeRoulee() != __Trajet.getDureeRoulee())
		return getDureeRoulee() < __Trajet.getDureeRoulee();
		
	return false;
}

#define SYNTHESE_CTRAJET

#endif
