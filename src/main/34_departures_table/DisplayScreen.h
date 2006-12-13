
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

	Un terminal d'affichage est un dispositif matériel dédié à l'affichage non interactif d'informations provenant de SYNTHESE (exemple: tableau de départs).
	Un terminal d'affichage est relié à un emplacement (exemple : arrêt logique).
	On considère par hypothèse que l'arrêt logique lié correspond à la fois à l'emplacement du panneau et à la définition première de ce qui y est affiché.
	L'emplacement est précisé par un champ texte ("tableau de départ situé au-dessus de l'escalier nord") ainsi que des coordonnées géographiques permettant de le situer sur une carte (ces données sont facultatives et ne servent qu'à désigner le terminal dans le cadre d'un composant d'administration par exemple).
	A un terminal d'affichage sont reliés des clauses visant à définir le contenu qui y est diffusé ainsi que le format du contenu (protocole, charte graphique, etc...) :
		- Nature de l'affichage (fonction SYNTHESE) :
				- Tableau de départ chronologique
				- Tableau de départ à destination privilégiée
				- Tableau d'arrivée chronologique
				- Tableau d'arrivée à provenance privilégiée
		- Format de sortie (défini par l'interface utilisée)
		- Paramétrage du contenu généré par la fonction : 
				- Titre
				- Nombre de départs ou d'arrivées
				- Filtre de terminus (sert à n'afficher que les lignes ayant pour origine le lieu affiché)
				- Filtre d'arrêt(s) physique(s)
				- Liste des points de passage à afficher
				- Autres paramètres (liste complète : voir IHM)

*/
class DisplayScreen
{
	/* THIS CODE COMES FROM SYNTHESE 2. IT MUST BE REFRESHED.

class cTableauAffichage : public cSite
	public:
	typedef enum { STANDARD_METHOD, WITH_FORCED_DESTINATIONS_METHOD } GenerationMethod;

protected:
	//!	\name Paramètres du tableau
	//@{
	cTexte								_Titre;				//!< Titre pour affichage
	tIndex								_NombreDeparts;		//!< Nombre de départs affichés
	tIndex								_PremierDepart;		//!< Premier départ affiché
	bool								_OriginesSeulement;	//!< Origines seulement ?
	tIndex								_NumeroPanneau;		//!< Numéro du panneau dans la gare pour afficheur
	int									_maxDelay;			//!< Max time length for the table

	//@}

	//!	\name Données
	//@{
	const GenerationMethod				_generationMethod;
	const cGare*						_PointArret;		//!< Point d'arrêt affiché
	set<const cQuai*>					_Quais;				//!< Quai(s) affichés
	set<const cLigne*>					_LignesInterdites;	//!< Lignes ne devant pas être affichées
	set<const cGare*>					_displayedPlaces;
	set<const cGare*>					_forcedDestinations;	//!< Destinations à afficher absolument
	set<const cGare*>					_forbiddenArrivalPlaces;	//!< Places not to serve. If so, then the line is not selected
	int									_destinationForceDelay;	//!< Durée pendant laquelle une destination est forcée

	//@}

	//!	\name Méthodes protégées
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
