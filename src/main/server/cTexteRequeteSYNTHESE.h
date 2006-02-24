/**	En-t�te classe Texte Requ�te SYNTHESE
	@file cTexteRequeteSYNTHESE.h
*/


#ifndef SYNTHESE_CTEXTEREQUETESYNTHESE_H
#define SYNTHESE_CTEXTEREQUETESYNTHESE_H

#include "cTexte.h"
#include "Temps.h"
#include <map>
#include <string>


//! Taille maximale d'une requ�te SYNTHESE
#define TAILLE_MAX_REQUETE 4096

//! Param�tre Fonction
#define REQUETE_COMMANDE_FONCTION "fonction"
	#define FONCTION_ACCUEIL "ac"
	#define FONCTION_FICHE_HORAIRE "fh"
	#define FONCTION_FICHE_ARRET "fa"
	#define FONCTION_LISTE_COMMUNE "lc"
	#define FONCTION_LISTE_POINT_ARRET "lpa"
	#define FONCTION_VALID_FICHE_HORAIRE "vfh"
	#define FONCTION_TABLEAU_DEPART_GARE "tdg"
	#define FONCTION_TABLEAU_DEPART "td"
	#define FONCTION_MINI_TABLEAU_DEPART "mtd"
	#define FONCTION_FORMULAIRE_RESA "fres"
	#define FONCTION_VALIDATION_RESA "vres"
	#define FONCTION_ANNULATION_RESA "ares"

#define REQUETE_COMMANDE_SITE "site"
#define REQUETE_COMMANDE_RECHERCHE "rec"
#define REQUETE_COMMANDE_COMMUNE "com"
#define REQUETE_COMMANDE_NUMERO_COMMUNE "ncom"
#define REQUETE_COMMANDE_SENS "sens"
#define REQUETE_COMMANDE_DATE "date"
#define REQUETE_COMMANDE_PERIODE "per"
#define REQUETE_COMMANDE_TAXIBUS "tax"
#define REQUETE_COMMANDE_VELO "vel"
#define REQUETE_COMMANDE_HANDICAPE "han"
#define REQUETE_COMMANDE_TARIF "tar"
#define REQUETE_COMMANDE_COMMUNE_DEPART "comd"
#define REQUETE_COMMANDE_COMMUNE_ARRIVEE "coma"
#define REQUETE_COMMANDE_POINT_ARRET_DEPART "ad"
#define REQUETE_COMMANDE_POINT_ARRET_ARRIVEE "aa"	
#define REQUETE_COMMANDE_NUMERO_COMMUNE_DEPART "ncomd"
#define REQUETE_COMMANDE_NUMERO_COMMUNE_ARRIVEE "ncoma"	
#define REQUETE_COMMANDE_NUMERO_POINT_ARRET "npa"
#define REQUETE_COMMANDE_NUMERO_POINT_ARRET_DEPART "npad"
#define REQUETE_COMMANDE_NUMERO_POINT_ARRET_ARRIVEE "npaa"	
#define REQUETE_COMMANDE_NUMERO_DESIGNATION_DEPART "ndd"
#define REQUETE_COMMANDE_NUMERO_DESIGNATION_ARRIVEE "nda"	
#define REQUETE_COMMANDE_NOMBRE_PROPOSITIONS "np"
#define REQUETE_COMMANDE_CODE_LIGNE "lig"
#define REQUETE_COMMANDE_NUMERO_SERVICE 	"serv"
#define REQUETE_COMMANDE_CODE_RESA "res"
#define REQUETE_COMMANDE_CLIENT_NOM "Nom"
#define REQUETE_COMMANDE_CLIENT_PRENOM "Prenom"
#define REQUETE_COMMANDE_CLIENT_ADRESSE					"Adresse"
#define REQUETE_COMMANDE_CLIENT_EMAIL					"Email"
#define REQUETE_COMMANDE_CLIENT_TELEPHONE				"Telephone"
#define REQUETE_COMMANDE_CLIENT_NUMERO_ABONNE			"NumAbonne" 
#define REQUETE_COMMANDE_ADRESSE_DEPART					"AdresseDepart" 
#define REQUETE_COMMANDE_ADRESSE_ARRIVEE				"AdresseArrivee" 
#define REQUETE_COMMANDE_NOMBRE_PLACES					"NbPlaces" 
#define REQUETE_COMMANDE_CODE_TABLEAUDEPART				"tb"

/** 

Requ�tes

S�lection de lignes
@code
<lignes code="<ID>" date="<date>" couleur="<couleur>"
        rangmin="<rang_min>" rangmax="<rang_max>" />
@endcode
@param ID	S�lection sur le code	
@param date	S�lection sur la date	
@param couleur Couleur d'affichage des lignes s�lectionn�es	
@param rang_min Rang minimum d�terminant le sergment � afficher
@param rang_max Rang maximum d�terminant le segment � afficher

S�lection d'arr�ts physiques
@code
<arretsphysiques code="<ID>" date="<date>" couleur="<couleur>" />
@endcode
@param ID	S�lection sur le code	
@param date	S�lection sur la date	
@param couleur	Couleur d'affichage des arr�ts s�lectionn�es	
		
S�lection d'arr�ts logiques
@code
<arretslogiques	code="<ID>" date="<date>" couleur="<couleur>" arretsphysiques="<arrets_physiques>" />
@endcode
@param ID	S�lection sur le code	
@param date	S�lection sur la date	
@param couleur	Couleur d'affichage des arr�ts s�lectionn�es	
@param arretsphysiques 1=Arrets physiques affich�s
	
Requ�te
@code
<requete format="<format>" typefond="<typefond>"
         xmin="<xmin>" ymin="<ymin>" xmax="<xmax>" ymax="<ymax>"
         largeur="<largeur>" hauteur="<hauteur>">
	<!-- Partie donn�es de trac� -->
	<ligne /> <ligne /> ... <ligne />
	<arretlogique /> <arretlogique /> ... <arretlogique />
	
	<!-- S�lection des objets � tracer -->
	<lignes /> <lignes /> ... <lignes />
	<arretslogiques /> <arretslogiques /> ... <arretslogiques />
	<arretsphysiques /> <arretsphysiques /> ... <arretsphysiques />
</requete>
@endcode
@param format	Format de la sortie (PS, PDF, JPEG, HTML, Mapinfo)	
@param typefond	Type de fond de carte � s�lectionner (plan, carte, photo)	
@param xmin	Longitude du coin inf�rieur gauche de la carte en projection Lambert II Etendu	
@param ymin	Latitude du coin inf�rieur gauche de la carte en projection Lambert II Etendu	
@param xmax	Longitude du coin sup�rieur droit de la carte en projection Lambert II Etendu	
@param ymax	Latitude du coin sup�rieur droit de la carte en projection Lambert II Etendu	
@param largeur	Largeur du fichier image en pixels	
@param hauteur	Hauteur du fichier image en pixels	
@param <ligne> D�claration de ligne(s) @ref xx
@param <arretlogique> D�claration d'arr�t(s) logique(s) @ref xx
@param <lignes> S�lection de lignes @ref xx
@param <arretslogiques> S�lection d'arr�ts logiques @ref xx
@param <arretsphysiques> S�lection d'arr�ts physiques @ref xx

*/


/*!	\brief Classe de texte contenant une requ�te SYNTHESE
	\author Christophe Romain
	\author Hugues Romain
	\date 2005
	@ingroup m70

Cette classe permet de stocker une requ�te SYNTHESE, et fournit des m�thodes d'acc�s rapide aux diverses informations
contenues dans la requ�te.

Le format d'une requ�te SYNTHESE est similaire aux r�gles de construction d'une URL :
 - les param�tres sont d�crits par un couple <i>nom_parametre</i> = <i>valeur</i>
 - les param�tres sont s�par�s par le caract�re &.
 
Pour �tre valable, une requ�te SYNTHESE doit en g�n�ral au moins contenir les param�tres suivants :
 - <i>fct</i> : Nom de la fonction (voir SYNTHESE::RunRequete())
 - <i>site</i> : Site d'acc�s (voir cSite)
*/
class cTexteRequeteSYNTHESE : public cTexte
{
	map<string,string>	params;	//!< Map contenant les param�tres pars�s
	
	void Finalise();

public:
	//!	\name Modificateurs
	//@{
	template <class T>
	void	AddParam(const cTexte& __Param, const T& __Valeur);
	tIndex	ConvertChar(const char c, char* Tampon=NULL) const;
	//@}
	
	//! \name Calculateurs
	//@{
	bool	checkParam(const char* nomParam)	const;
	tBool3	getBool3(const char* nomParam);
	cTexte	getTexte(const char* nomParam);
	int		getInt(const char* nomParam);
	cDate	getDate(const char* __NomParam);
	cMoment	getMoment(const char* __NomParam);
	template <class C>
	void	AfficheFormulaireHTML(C& __Flux);
	//@}
	
	//!	\name Constructeur et destructeur
	//@{
	cTexteRequeteSYNTHESE() : cTexte() { }
	~cTexteRequeteSYNTHESE() { }
	//@}
	
};


/*!	\brief Initialisation d'une requ�te
	\param __Debut Debut du texte de la requ�te
	\warning Cette m�thode ne contr�le pas l'unicit� du param�tre entr�
	\author Hugues Romain
	\date 2005
*/
template <class T>
void cTexteRequeteSYNTHESE::AddParam(const cTexte& __Parametre, const T& __Valeur)
{
	cTexteCodageInterne __Texte;
	if (Taille())
		__Texte << "&";
	__Texte << __Parametre << "=" << __Valeur;
	Copie(__Texte);
}

/*!	\brief Affichage des param�tres de la requ�te sous forme de champs cach�s HTML
	\param __Flux Flux sur lequel afficher
	\todo Mettre cette fonction en const (probleme avec le map)
	\author Hugues Romain
	\date 2005
*/
template <class C>
void cTexteRequeteSYNTHESE::AfficheFormulaireHTML(C& __Flux)
{
	for(map<string,string>::iterator __Iter = params.begin(); __Iter != params.end(); __Iter++)
		__Flux << "<input type=\"hidden\" name=\"" << __Iter->first << "\" value=\"" << __Iter->second << "\" />";
}

#endif
