
#ifndef SYNTHESE_CTABLEAUAFFICHAGE_H
#define SYNTHESE_CTABLEAUAFFICHAGE_H



#include "ForcedDestinationsArrivalDepartureTableGenerator.h"
#include "StandardArrivalDepartureTableGenerator.h"
#include <set>



namespace synthese
{
namespace departurestable
{

/** Terminal d'affichage.
	@ingroup m34

	Un terminal d'affichage est un dispositif mat�riel d�di� � l'affichage non interactif d'informations provenant de SYNTHESE (exemple: tableau de d�parts).
	Un terminal d'affichage est reli� � un emplacement (exemple : arr�t logique).
	On consid�re par hypoth�se que l'arr�t logique li� correspond � la fois � l'emplacement du panneau et � la d�finition premi�re de ce qui y est affich�.
	L'emplacement est pr�cis� par un champ texte ("tableau de d�part situ� au-dessus de l'escalier nord") ainsi que des coordonn�es g�ographiques permettant de le situer sur une carte (ces donn�es sont facultatives et ne servent qu'� d�signer le terminal dans le cadre d'un composant d'administration par exemple).
	A un terminal d'affichage sont reli�s des clauses visant � d�finir le contenu qui y est diffus� ainsi que le format du contenu (protocole, charte graphique, etc...) :
		- Nature de l'affichage (fonction SYNTHESE) :
				- Tableau de d�part chronologique
				- Tableau de d�part � destination privil�gi�e
				- Tableau d'arriv�e chronologique
				- Tableau d'arriv�e � provenance privil�gi�e
		- Format de sortie (d�fini par l'interface utilis�e)
		- Param�trage du contenu g�n�r� par la fonction : 
				- Titre
				- Nombre de d�parts ou d'arriv�es
				- Filtre de terminus (sert � n'afficher que les lignes ayant pour origine le lieu affich�)
				- Filtre d'arr�t(s) physique(s)
				- Liste des points de passage � afficher
				- Autres param�tres (liste compl�te : voir IHM)

*/
class DisplayScreen
{
	/* THIS CODE COMES FROM SYNTHESE 2. IT MUST BE REFRESHED.

class cTableauAffichage : public cSite
	public:
	typedef enum { STANDARD_METHOD, WITH_FORCED_DESTINATIONS_METHOD } GenerationMethod;

protected:
	//!	\name Param�tres du tableau
	//@{
	cTexte								_Titre;				//!< Titre pour affichage
	tIndex								_NombreDeparts;		//!< Nombre de d�parts affich�s
	tIndex								_PremierDepart;		//!< Premier d�part affich�
	bool								_OriginesSeulement;	//!< Origines seulement ?
	tIndex								_NumeroPanneau;		//!< Num�ro du panneau dans la gare pour afficheur
	int									_maxDelay;			//!< Max time length for the table

	//@}

	//!	\name Donn�es
	//@{
	const GenerationMethod				_generationMethod;
	const cGare*						_PointArret;		//!< Point d'arr�t affich�
	set<const cQuai*>					_Quais;				//!< Quai(s) affich�s
	set<const cLigne*>					_LignesInterdites;	//!< Lignes ne devant pas �tre affich�es
	set<const cGare*>					_displayedPlaces;
	set<const cGare*>					_forcedDestinations;	//!< Destinations � afficher absolument
	set<const cGare*>					_forbiddenArrivalPlaces;	//!< Places not to serve. If so, then the line is not selected
	int									_destinationForceDelay;	//!< Dur�e pendant laquelle une destination est forc�e

	//@}

	//!	\name M�thodes prot�g�es
	//@{
	cMoment	_MomentFin(const cMoment& __MomentDebut)			const;
	//@}

public:
	//!	\name Constructeur et destructeur
	//@{
		cTableauAffichage(const cTexte& __Code, GenerationMethod);
		~cTableauAffichage(void);
	//@}

	//!	\name Modificateurs
	//@{
		void			SetPointArret(const cGare* __PointArret);
		void			SetNombreDeparts(tIndex __NombreDeparts);
		void			AddLigneInterdte(const cLigne* __Ligne);
		void			AddQuaiAutorise(tIndex __NumeroQuai);
		void			SetTitre(const cTexte&);
		void			SetOriginesSeulement(bool __Valeur);
		void			AddDestinationAffichee(const cGare* __PointArret);
		void			SetNumeroPanneau(tIndex);
		void			addForcedDestination(const cGare*);
		void			setDestinationForceDelay(int);
		void			setMaxDelay(int);
		void			addForbiddenPlace(const cGare*);
	//@}

	//!	\name Accesseurs and computers
	//@{
		const cTexte&					getTitre()			const;
		tIndex							getNumeroPanneau()	const;
		ArrivalDepartureTableGenerator*	getGenerator(const cMoment& startTime) const;
	//@}
*/

};

}
}

#endif
