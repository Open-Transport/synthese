/**	En-tï¿½te et implï¿½mentation inline classes cFichier dï¿½crivant des objets enregistrï¿½s dans les environnements.
	@file cFichierPourEnvironnement.h


Point de tracï¿½
@code
<point x="<X>" y="<Y>" rang="<rang>" datemin="(date_min)" datemax="(date_max") />
@endcode
@param X Longitude du point en projection Lambert II Etendu	
@param Y Latitude du point en projection Lambert II Etendu	
@param date_min	Date de dï¿½but de validitï¿½ du tracï¿½	
@param date_max	Date de fin de validitï¿½ du tracï¿½	
@param rang Rang du point au sein de son objet pï¿½re (ex : rang d'arrï¿½t dans la ligne)

Ligne
@code
<ligne code="<ID>" format="<format>" couleur="<couleur>"
       datemin="<date_min>" datemax="<date_max>">
	<arretphysique /> <point /> <point /> ... <point />
	<arretphysique /> <point /> <point /> ... <point />
	...
	<arretphysique /> <point /> <point /> ... <point />
	<arretphysique />
</ligne>
@endcode
@param ID Code de la ligne (Rï¿½seau/Ligne commerciale/Itinï¿½raire)	
@param couleur	Couleur par dï¿½faut de la ligne (RGB HTML)	
@param datemin	Date de dï¿½but de validitï¿½ du tracï¿½	
@param datemax	Date de fin de validitï¿½ du tracï¿½	
@param nom	Nom de la ligne	
@param <point>, <arretphysique> : Succession de points, d'arrï¿½ts de franchissements de limites communales du tracï¿½, dans l'ordre
@warning Le chagement d'une ligne doit se faire aprï¿½s chargement des arrï¿½ts physiques


Arrï¿½ts physiques
@code
<arretphysique code="(ID)" nom="(nom)">
	<point /> <point /> ... <point />
</arretphysique>
@endcode
@param ID Code de l'arrï¿½t physique	
@param nom	Nom de l'arrï¿½t physique au sein de l'arrï¿½t logique	

A METTRE DANS SYNTHESE :
Durï¿½es de correspondance
<correspondance origine="(ID_origine)" destination="(ID_destination) duree="(duree)" />

Dï¿½signation d'arrï¿½t
@code
<designation commune="(commune)" nom="(nom)" type="(type)" />
@endcode
@param commune ID ou nom de commune
@param nom Nom de l'arrï¿½t dans la commune
@param type Type de dï¿½signation :
	- @c main Arrï¿½t principal de la commune
	- @c normal (dï¿½faut) Arrï¿½t de commune
	- @c site Lieu public
	- @c all Arrï¿½t tous lieux


Fonds de carte
@code
<fond type="<type>" xmin="<xmin>" ymin="<ymin>" xmax="<xmax>" ymax="<ymax>" 
      echelle="<echelle>" src="<chemin>" />
@endcode
@param type	Type du fond (plan, carte, photo)	
@param xmin	Longitude du coin infï¿½rieur gauche de la carte en projection Lambert II Etendu	
@param ymin	Latitude du coin infï¿½rieur gauche de la carte en projection Lambert II Etendu	
@param xmax	Longitude du coin supï¿½rieur droit de la carte en projection Lambert II Etendu	
@param ymax	Latitude du coin supï¿½rieur droit de la carte en projection Lambert II Etendu	
@param echelle	Echelle en pixel par mï¿½tre	
@param chemin Chemin d'accï¿½s au fichier	


*/

#ifndef SYNTHESE_CFICHIERPOURENVIRONNEMENT_H
#define SYNTHESE_CFICHIERPOURENVIRONNEMENT_H

/** @addtogroup m11
	@{ */

//!	@name Balise commune
//@{

/** Dï¿½claration de commune.
	@code
	<commune id="(ID)" nom="(nom)">
		<point /> <point /> ... <point />
	</commune>
	@endcode
	@param ID Code de la commune
	@param nom Nom de la commune
	@param <point> Suite de @ref XML_ENV_POINT "points" formant le contour de la commune
*/
#define XML_ENV_COMMUNE "commune"
#define XML_ENV_COMMUNE_ID "id"
#define XML_ENV_COMMUNE_NOM "nom"
#define XML_ENV_COMMUNE_POINT "point"
//@}


//!	@name Balise arrï¿½t logique
//@{

/** Dï¿½claration d'arrï¿½t logique.
	@code
	<arret_logique id="(ID)" type="(type)>
		<arret_physique /> <arret_physique /> ... <arret_physique />
		<designation /> <designation /> ... <designation />
		<alerte /> <alerte /> ... <alerte />
		<delai /><delai /> ... <delai />
	</arret_logique>
	@endcode
	@param ID Code de l'arrï¿½t logique (voir @ref base)
	@param type Type d'arrï¿½t logique, pouvant avoir les valeurs suivantes :
		- @c passage : arrï¿½t de passage (correspondance interdite)
		- @c correspondance : arrï¿½t de correspondance (correspondance autorisï¿½e)
		- @c echange_courte_distance : arrï¿½t de correspondance recommandï¿½ en courte distance uniquement
		- @c echange_longue_distance : arrï¿½t de correspondance recommante pour toutes distances
	@param <arret_physique> Arrï¿½ts physiques appartenant ï¿½ l'arrï¿½t logique
	@param <designation> @ref XML_ENV_ARRET_DESIGNATION "dï¿½signation" de l'arrï¿½t logique. La premiï¿½re est la dï¿½signation officielle.
	@param <alerte> @ref XML_ENV_ALERTE "alerte" concernant l'arrï¿½t logique.

	Dï¿½tail de la balise <arret_physique> :
	@copydoc XML_ENV_ARRET_PHYSIQUE
*/
#define XML_ENV_ARRET_LOGIQUE "arret_logique"
#define XML_ENV_ARRET_LOGIQUE_ID "id"
#define XML_ENV_ARRET_LOGIQUE_TYPE "type"
//@}


//!	@name Balise arrï¿½t physique
//@{

/** Dï¿½claration d'un arrï¿½t physique.
	@code
	<arret_physique rang="(rang)" x="(x)" y="(y)"/>
	@endcode
*/
#define XML_ENV_ARRET_PHYSIQUE "arret_physique"
#define XML_ENV_ARRET_PHYSIQUE_RANG "rang"
#define XML_ENV_ARRET_PHYSIQUE_X "x"
#define XML_ENV_ARRET_PHYSIQUE_Y "y"
//@}





//! @name Fichiers d'horaires
//@{
#define HORAIRESFORMAT "horaires"
#define HORAIRESEXTENSION "horaires.per"

/** Appel de fichier inclus
	@code > nom_fichier @endcode
	@param nom_fichier chemin du fichier inclus, relativement au rï¿½pertoire du fichier courant, ne portant pas lï¿½extension .horaires.per (Ex&nbsp;: &gt; MP/index envoie vers <i>&lt;rï¿½pertoire courant&gt;</i>/MP/index.horaires.per)
*/
#define HORAIRESFORMATLIGNELien						TYPELien


/** Dï¿½claration d'axe
	@code #type code @endcode
	@param type Type d'\ref defAxe :
		- # : \ref defAxeLibre
		- ï¿½ : \ref defAxeInterdit
		- (rien) : Axe classique
	@param code Code unique d'axe (@ref baseHoraires)
*/
#define HORAIRESFORMATLIGNEAxe						TYPESection


/** Dï¿½claration de ligne
	@code [code_ligne @endcode
	@param code_ligne Code unique de la @ref defLigne dï¿½clarï¿½e (voir @ref baseHoraires )
	@warning Toute ligne doit ï¿½tre contenue dans un axe dï¿½clarï¿½ auparavant.
*/
#define HORAIRESFORMATLIGNELigne					TYPESousSection

#define HORAIRESFORMATLIGNEDocumentation			0
#define HORAIRESFORMATLIGNEJoursCirculationLigne		1
#define HORAIRESFORMATLIGNEReseau					2


/** Libellï¿½ simple de ligne
	@code CAR libellï¿½ @endcode
	@param libellï¿½ Libellï¿½ simple de la ligne
*/
#define HORAIRESFORMATLIGNECartouche				3


/** Style de la ligne
	@code STY code_style @endcode
	@param code_style nom de style CSS rï¿½fï¿½rence aux dï¿½finitions fournies dans le fichier <b>couleurs.css</b>.
*/
#define HORAIRESFORMATLIGNEStyle					4
#define HORAIRESFORMATLIGNEMateriel				5
#define HORAIRESFORMATLIGNEResa					6
#define HORAIRESFORMATLIGNEVelo					7
#define HORAIRESFORMATLIGNETarification				8
#define HORAIRESFORMATLIGNEJoursCirculationServices	9

/** Dï¿½claration de @ref defServiceContinu : attente aux arrï¿½ts
	@code ATT attente1 @endcode
	@param attente Attente ï¿½ l'arrï¿½t en minutes.
	@warning Un service continu doit ï¿½tre le seul service de la @ref defLigne.
	Le fait de dï¿½clarer une attente ï¿½ un service le proclame au rang de service continu.
*/
#define HORAIRESFORMATLIGNEAttente					10
#define HORAIRESFORMATLIGNEFin					11


/** Codes des services
	@code NUM num1 num2 ... @endcode
	@param num1 Code du premier service
	@param num2 Code du second service
	@param ... etc
	@warning La numï¿½rotation des services est facultative, sauf dans le cas de lignes offrant la rï¿½servation en ligne.
*/
#define HORAIRESFORMATLIGNENumero					12

#define HORAIRESFORMATLIGNEHoraires				13
#define HORAIRESFORMATLIGNEAAfficher				14
#define HORAIRESFORMATLIGNECodeIndicateur			15
#define HORAIRESFORMATLIGNECodeImportationSource		16
#define HORAIRESFORMATLIGNECodeBaseTrains			17


/** (facultatif) Libellï¿½ complet de ligne
	@code DES libellï¿½ @endcode
	@param libellï¿½ Libellï¿½ complet de la ligne
	Si ce paramï¿½tre n'est pas fourni, alors le libellï¿½ complet gï¿½nï¿½rï¿½ par l'interface sera appliquï¿½.
*/
#define HORAIRESFORMATLIGNELibelleComplet			18


/** Chemin de lï¿½image reprï¿½sentant la ligne 
	@code IMG nom_fichier @endcode
	@param nom_fichier nom du fichier image sans extension et sans rï¿½pertoire (ces informations sont produites par l'interface)
	@warning Au moins un des deux libellï¿½s @ref HORAIRESFORMATLIGNECartouche ou @ref HORAIRESFORMATLIGNEImage doit ï¿½tre fourni.
*/
#define HORAIRESFORMATLIGNEImage					19
#define HORAIRESFORMATLIGNEAffIndicateurs			20
#define HORAIRESFORMATLIGNEDepot					21
#define HORAIRESFORMATLIGNEHandicape				22
#define HORAIRESFORMATLIGNEAlerte					23
#define HORAIRESFORMATLIGNEAlerteDebut				24
#define HORAIRESFORMATLIGNEAlerteFin				25
#define HORAIRESFORMATLIGNEGirouette				26
#define HORAIRESFORMATLIGNENombreFormats			27

#define HORAIRESFORMATCOLONNEPointHectometrique		0
#define HORAIRESFORMATCOLONNEArretLogique				1
#define HORAIRESFORMATCOLONNETypeArretLogique			2
#define HORAIRESFORMATCOLONNEArretPhysique					3
#define HORAIRESFORMATCOLONNEHoraire				4
#define HORAIRESFORMATCOLONNEDateAlerte				5
#define HORAIRESFORMATCOLONNEAlerte					6
#define HORAIRESFORMATCOLONNENombreFormats			7
//@}

//! @name Fichiers de points d'arrï¿½t
//@{
#define PAFORMAT "pointsarret"
#define PAEXTENSION "gares.per"

/** Dï¿½claration de point d'arrï¿½t
	@code [TypeCode @endcode
	@param Type Type du point d'arrï¿½t, pouvant avoir les valeurs suivantes :
		- @c S : arrï¿½t de passage (correspondance interdite)
		- @c C : arrï¿½t de correspondance (correspondance autorisï¿½e)
		- @c Q : arrï¿½t de correspondance recommandï¿½ en courte distance uniquement
		- @c R : arrï¿½t de correspondance recommante pour toutes distances
	@param Code Identifiant numï¿½rique du point dï¿½arrï¿½t dans la base (voir @ref base)
	@warning Le code du point dï¿½arrï¿½t doit ï¿½tre strictement compris entre 0 et le numï¿½ro max de point dï¿½arrï¿½t prï¿½cï¿½demment dï¿½fini.
*/
#define PAFORMATLIGNEArretLogique						TYPESousSection


/** Allocation des quais
	@code NQ nombre @endcode
	@param nombre Nombre de quais dï¿½clarï¿½s par la suite.
	@warning Le nombre de quais doit ï¿½tre strictement ï¿½gal au nombre de quais dï¿½clarï¿½.
	@todo Supprimer la nï¿½cessitï¿½ d'allouer les quais ï¿½ l'avance
*/
#define PAFORMATLIGNENombreArretPhysiques					0
#define PAFORMATLIGNENombreDescriptions				1

/** Longitude moyenne du point d'arrï¿½t
	@code GX longitude @endcode
	@param longitude Longitude exprimï¿½e en mï¿½tres, selon la projection <b>Lambert Zone II Etendu</b>. Ordres de grandeur :
		- Bayonne : 292075
		- Toulouse : 525861
		- Nice : 995737
*/
#define PAFORMATLIGNEGpsx						2


/** Latitude moyenne du point d'arrï¿½t
	@code GY latitude @endcode
	@param latitude Latitude exprimï¿½e en mï¿½tres, selon la projection <b>Lambert Zone II Etendu</b>. Ordres de grandeur :
		- Bayonne : 1839352
		- Toulouse : 1845706
		- Lille : 2626673
*/
#define PAFORMATLIGNEGpsy						3


/** Dï¿½signation de point d'arrï¿½t
	@code DE TypeCommune(Nom @endcode
	@param Type Type de la dï¿½signation par rapport ï¿½ la commune, l'une des valeurs suivantes :
		- @c 0 : Dï¿½signation classique
		- @c 1 : Arrï¿½t principal de commune
		- @c 2 : Arrï¿½t tout lieu
	@param Commune : Nom complet de la commune (faisant office dï¿½identificateur)
	@param Nom : libellï¿½ du point dï¿½arrï¿½t au sein de la commune
*/
#define PAFORMATLIGNEDescription					4
#define PAFORMATLIGNEHtml						5


/** Dï¿½claration de quai
	Arrï¿½ts de correspondance :
	@code numï¿½ro_quai:durï¿½e_correspondance1 durï¿½e_correspondance2 ... description @endcode
	Arrï¿½ts de passage :
	@code numï¿½ro_quai:description @endcode
	@param numï¿½ro_quai Numï¿½ro de quai au sein du point d'arrï¿½t (deux chiffres)
	@param durï¿½e_correspondance1 Durï¿½e en minutes vers le quai 1 (deux chiffres)
	@param durï¿½e_correspondance2 Durï¿½e en minutes vers le quai 2 (deux chiffres)
	@param ... etc (autant que de quais dans le point d'arrï¿½t)
	@param description Dï¿½signation du quai et commandes de description. Les commandes sont prï¿½cï¿½dï¿½es du signe @c # et requiï¿½rent un paramï¿½tre qui suit la commande sans espace :
		- @c GX @ref defGeolocalisation "Longitude" du quai en Lambert II Etendu (8 chiffres)
		- @c GY @ref defGeolocalisation "Latitude" du quai en Lambert II Etendu (8 chiffres)
		- @c PH Photo reliï¿½e au quai (code 8 chiffres relatif au fichier des photos)
*/
#define PAFORMATLIGNEArretPhysique						6
#define PAFORMATLIGNEPostScriptOD					7


/** Photo de point d'arrï¿½t
	@code PH code @endcode
	@param code Code de photo rï¿½fï¿½rence aux fichier des photos
*/
#define PAFORMATLIGNEPhoto						8


/** Allocation des photos de point d'arrï¿½t
	@code NP nombre @endcode
	@param nombre Nombre de photos dans le point d'arrï¿½t
	@todo Supprimer l'allocation des photos (tableau dynamique)
*/
#define PAFORMATLIGNENombrePhotos					9


/** Vitesses maximales pour optimisation de calculs
	@code VM v5v10v15v20v25v30v50v100v150v200vmax @endcode
	@param v5 Vitesse maximale vers tout point situï¿½ ï¿½ 5 km au plus (en km/h sur 3 chiffres)
	@param v10 Vitesse maximale vers tout point situï¿½ entre 5 km et 10 km (en km/h sur 3 chiffres)
	@param v15 Vitesse maximale vers tout point situï¿½ entre 10 km et 15 km (en km/h sur 3 chiffres)
	@param v20 Vitesse maximale vers tout point situï¿½ entre 15 km et 20 km (en km/h sur 3 chiffres)
	@param v25 Vitesse maximale vers tout point situï¿½ entre 20 km et 25 km (en km/h sur 3 chiffres)
	@param v30 Vitesse maximale vers tout point situï¿½ entre 25 km et 30 km (en km/h sur 3 chiffres)
	@param v50 Vitesse maximale vers tout point situï¿½ entre 30 km et 50 km (en km/h sur 3 chiffres)
	@param v100 Vitesse maximale vers tout point situï¿½ entre 50 km et 100 km (en km/h sur 3 chiffres)
	@param v150 Vitesse maximale vers tout point situï¿½ entre 100 km et 150 km (en km/h sur 3 chiffres)
	@param v200 Vitesse maximale vers tout point situï¿½ entre 150 km et 200 km (en km/h sur 3 chiffres)
	@param vmax Vitesse maximale vers tout point situï¿½ ï¿½ plus de 200 km (en km/h sur 3 chiffres)
	@warning des vitesses maximales fausses peuvent entraï¿½ner des rï¿½sultats faux oï¿½ mï¿½me l'arrï¿½t intempestif des calculs, bien mettre ï¿½ jour les Vmax aprï¿½s chaque modification de la base dï¿½horaires.
	Les seuils de distance sont dï¿½finis en tant que constantes et correspondent ï¿½ des segments de marchï¿½ relevant de moyens de transport diffï¿½rents (urbain, rï¿½gional routier, rï¿½gional ferroviaire, national)
	
	Ces donnï¿½es ne sont pas utilisï¿½es ï¿½ l'heure actuelle
*/
#define PAFORMATLIGNEVMax						10


/** Allocation des services en gare
	@code NS nombre @endcode
	@param nombre Nombre de services
	@todo Supprimer l'allocation des services (tableau dynamique)
*/
#define PAFORMATLIGNENombreServices				15


/** Dï¿½claration de service en gare
	@code SE type photo dï¿½signation @endcode
	@param type Type de service, valeurs suivantes :
		- @c I : Guichet d'information
		- @c V : Guichet de vente
		- @c A : Automate de vente
		- @c W : Guichet d'information vente
		- @c B : Bar
		- @c R : Restaurant
		- @c T : Tabac/Presse
		- @c P : Boulangerie
		- @c D : Distributeur de billets de banque
	@param photo Code de photo du service, rï¿½fï¿½rence au fichier des photos (6 chiffres, 000000 = pas de photo)
	@param dï¿½signation Dï¿½signation du service
*/
#define PAFORMATLIGNEService						16
#define PAFORMATLIGNEAlerte						17
#define PAFORMATLIGNEAlerteDebut					18
#define PAFORMATLIGNEAlerteFin					19


/** Désignation du point d'arrêt sur 13 caractères.
	@code D13 designation @endcode
	@param designation Valeur
*/
#define PAFORMATLIGNEDesignation13					20


/** Désignation du point d'arrêt sur 26 caractères.
	@code D26 designation @endcode
	@param designation Valeur
*/
#define PAFORMATLIGNEDesignation26					21

#define PAFORMATLIGNENombreFormats					22

#define PAFORMATCOLONNEStandard					0
#define PAFORMATCOLONNEVMax						1
#define PAFORMATCOLONNEServiceType					2
#define PAFORMATCOLONNEServicePhoto				3
#define PAFORMATCOLONNEServiceDesignation			4
#define PAFORMATCOLONNEDesignationLibelle			5
#define PAFORMATCOLONNEDesignationType				6
#define PAFORMATCOLONNEDateAlerte					7
#define PAFORMATCOLONNEAlerte						8
#define PAFORMATCOLONNEIndexArretPhysique					9
#define PAFORMATCOLONNEDelaiCorrespondance			10
#define PAFORMATCOLONNENombreFormats				11
/** @} */

//! \name Format Photos
//@{
#define PHFORMAT "photos"
#define PHEXTENSION "photos.per"

#define PHFORMATLIGNEDesignationGenerale			0
#define PHFORMATLIGNEDesignationLocale				1
#define PHFORMATLIGNEZoneCliquableURL				2
#define PHFORMATLIGNEZoneCliquablePhoto				3
#define PHFORMATLIGNENombreMaps					4
#define PHFORMATLIGNENomFichier					5
#define PHFORMATLIGNENombreFormats					6

#define PHFORMATCOLONNECoordonneesMap				0
#define PHFORMATCOLONNENumeroPhotoLiee				1
#define PHFORMATCOLONNEAltMapPhoto					2
#define PHFORMATCOLONNEAltUrlMapUrl				3
#define PHFORMATCOLONNEStandard					4
#define PHFORMATCOLONNENombreFormats				5
//@}

//! @name Format de fichier des calendriers
//@{
#define JCFORMAT "jours"
#define JCFILEEXTENSION "jours.per"
#define JCEXTENSION ".jours.per"


/** Bornes des calendriers de circulations (obligatoire en premier)
	@code Service=annee_debut/annee_fin @endcode
	@param annee_debut Premiï¿½re annï¿½e des calendriers sur 4 chiffres obligatoirement
	@param annee_fin Derniï¿½re annï¿½e des calendriers sur 4 chiffres obligatoirement
	@warning La derniï¿½re annï¿½e doit ï¿½tre supï¿½rieure ou ï¿½gale ï¿½ la premiï¿½re
	Cette information obligatoire permet lï¿½allocation mï¿½moire&nbsp; des tableaux de binaires contenant les calendriers.
*/
#define JCFORMATLIGNEService	"Service="


/** Dï¿½claration de calendrier
	@code [codeLibellï¿½ @endcode
	@param code Code du calendrier sur 4 chiffres (voir @ref baseHoraires)
	@param Libellï¿½ descriptif du calendrier de longueur quelconque (facultatif)
	@todo Porter le nombre de calendriers possibles ï¿½ \f$ 10^7 \f$.
*/
#define JCFORMATLIGNECalendrier	TYPESousSection


/** @ref defCategorieCalendrier
	@code =valeur @endcode
	@param valeur @ref defCategorieCalendrier

	Lï¿½appel ï¿½ cette commande est facultatif, la catï¿½gorie 255 est choisie par dï¿½faut.

*/
#define JCFORMATLIGNECategorie '='


/** Ajout de jour(s) circulï¿½(s) ï¿½ un calendrier
	@code +date_debut/date_fin/periode @endcode
	@param date_debut Premiï¿½re date ajoutï¿½e
	@param date_fin Derniï¿½re date ajoutï¿½e (facultatif : dï¿½faut = premiï¿½re date)
	@param periode Nombre de jours entre chaque date ajoutï¿½e (facultatif : dï¿½faut = 0)

	Le format d'ï¿½criture des dates est en 8 caractï¿½res obligatoirement :
		- soit une date explicite : format AAAAMMJJ
		- soit Date min pour le premier jour possible
		- soit Date max pour le dernier jour possible
		- soit une date gï¿½nï¿½rique : format ****MMJJ

	Dans le cadre dï¿½une commande de type plage de dates gï¿½nï¿½riques, si la date supï¿½rieure est infï¿½rieure ï¿½ la date infï¿½rieure, alors la date supï¿½rieure est considï¿½rï¿½e comme ï¿½tant de lï¿½annï¿½e suivante.

	Exemples pour un environnement 2001/2003 :
		- @c +20020105 donne l'ajout du 5/1/2002
		- @c +20031215/20040501 donne l'ajout du 15/12/2003 au 31/12/2003
		- @c +20031017/20031028/05 donne l'ajout de 17/10/2003, 22/10/2003, 27/10/2003
		- @c +****0105 donne l'ajout de 5/1/2001, 5/1/2002, 5/1/2003
		- @c +****1230/****0101 donne l'ajout de 30/12/2001, 31/12/2001, 1/1/2002, 30/12/2002, 31/12/2002, 1/1/2003, 30/12/2003, 31/12/2003.
		- @c +****1225/****0108/07 donne l'ajout de 25/12/2001, 1/1/2002, 8/1/2002, 25/12/2002, 1/1/2003, 8/1/2003, 25/12/2003

	@warning Il est interdit de coupler date gï¿½nï¿½rique et date absolue dans une mï¿½me commande. (Erreur 06007)
*/
#define JCFORMATLIGNEAjoutDate '+'


/** Suppression de jour(s) circulï¿½(s) ï¿½ un calendrier
	@code -date_debut/date_fin/periode @endcode
	@param date_debut Premiï¿½re date ajoutï¿½e
	@param date_fin Derniï¿½re date ajoutï¿½e (facultatif : dï¿½faut = premiï¿½re date)
	@param periode Nombre de jours entre chaque date ajoutï¿½e (facultatif : dï¿½faut = 0)

	Le format d'ï¿½criture des dates est en 8 caractï¿½res obligatoirement :
		- soit une date explicite : format AAAAMMJJ
		- soit Date min pour le premier jour possible
		- soit Date max pour le dernier jour possible
		- soit une date gï¿½nï¿½rique : format ****MMJJ

	Dans le cadre dï¿½une commande de type plage de dates gï¿½nï¿½riques, si la date supï¿½rieure est infï¿½rieure ï¿½ la date infï¿½rieure, alors la date supï¿½rieure est considï¿½rï¿½e comme ï¿½tant de lï¿½annï¿½e suivante.

	Exemples pour un environnement 2001/2003 :
		- @c -20020105 donne la suppression du 5/1/2002
		- @c -20031215/20040501 donne la suppression du 15/12/2003 au 31/12/2003
		- @c -20031017/20031028/05 donne la suppression de 17/10/2003, 22/10/2003, 27/10/2003
		- @c -****0105 donne la suppression de 5/1/2001, 5/1/2002, 5/1/2003
		- @c -****1230/****0101 donne la suppression de 30/12/2001, 31/12/2001, 1/1/2002, 30/12/2002, 31/12/2002, 1/1/2003, 30/12/2003, 31/12/2003.
		- @c -****1225/****0108/07 donne la suppression de 25/12/2001, 1/1/2002, 8/1/2002, 25/12/2002, 1/1/2003, 8/1/2003, 25/12/2003

	@warning Il est interdit de coupler date gï¿½nï¿½rique et date absolue dans une mï¿½me commande. (Erreur 06007)
*/
#define JCFORMATLIGNESuppressionDate '-'


/** Ajout ï¿½ un calendrier des dates de circulation d'un autre calendrier dï¿½jï¿½ dï¿½clarï¿½
	@code +#code @endcode
	@param code Code du calendrier ï¿½ lire
    
	@warning Lï¿½ajout ne met pas ï¿½ NON les bits 0 du calendrier inclus. Il ne fait que mettre ï¿½ OUI les bits 1 du calendrier inclus. 
	@warning Le code de calendrier inclus doit correspondre ï¿½ un calendrier dï¿½jï¿½ dï¿½clarï¿½.

	Table de vï¿½ritï¿½ de la commande :
	<table class="tableau">
	<tr><td></td><td colspan="2">Calendrier inclus</td></tr>
	<tr><td>Avant</td><td>0</td><td>1</td></td>
	<tr><td>0</td><td>0</td><td>1</td></tr>
	<tr><td>1</td><td>1</td><td>1</td></tr>
	</table>
*/
#define JCFORMATLIGNEAjoutCalendrier '#'


/** Suppression d'un calendrier des dates de circulation d'un autre calendrier dï¿½jï¿½ dï¿½clarï¿½
	@code -#code @endcode
	@param code Code du calendrier ï¿½ lire
    
	@warning La suppression ne met pas ï¿½ OUI les bits 0 du calendrier inclus. Il ne fait que mettre ï¿½ NON les bits 1 du calendrier inclus. 
	@warning Le code de calendrier inclus doit correspondre ï¿½ un calendrier dï¿½jï¿½ dï¿½clarï¿½.

	Table de vï¿½ritï¿½ de la commande :
	<table class="tableau">
	<tr><td></td><td colspan="2">Calendrier inclus</td></tr>
	<tr><td>Avant</td><td>0</td><td>1</td></td>
	<tr><td>0</td><td>0</td><td>0</td></tr>
	<tr><td>1</td><td>1</td><td>0</td></tr>
	</table>
*/
#define JCFORMATLIGNESuppressionCalendrier '#'


/** Exï¿½cution de commandes d'ajout/suppression de dates dï¿½crites dans un fichier
	@code +>fichier @endcode
	@param fichier Chemin du fichier ï¿½ lire relativement ï¿½ l'emplacement du fichier courant, sans l'extension .jours.per
    
	@warning Le fichier lu ne doit contenir que des commandes d'ajout/suppression de dates (@ref JCFORMATLIGNEAjoutDate et @ref JCFORMATLIGNESuppressionDate).
*/
#define JCFORMATLIGNEAjoutFichier '>'


/** Exï¿½cution inversï¿½e de commandes d'ajout/suppression de dates dï¿½crites dans un fichier
	@code ->fichier @endcode
	@param fichier Chemin du fichier ï¿½ lire relativement ï¿½ l'emplacement du fichier courant, sans l'extension .jours.per
    @warning Le fichier lu ne doit contenir que des commandes d'ajout/suppression de dates (@ref JCFORMATLIGNEAjoutDate et @ref JCFORMATLIGNESuppressionDate).
	Cette commande exï¿½cute les commandes inverses de celles contenues dans le fichier (+ devient - et inversement)
*/
#define JCFORMATLIGNESuppressionFichier '>'
//@}


//! @name Format du fichier des rï¿½sas
//@{
#define RESASFORMAT		"resa"
#define RESASEXTENSION	"resa.per"


/** (obligatoire) Nombre de modalitï¿½s de rï¿½servation
	@code Nombre=numï¿½ro_max_code_resa @endcode
	@param numï¿½ro_max_code_resa Plus grand code de rï¿½sa. 
		NB : ce numï¿½ro peut ï¿½tre supï¿½rieur au nombre rï¿½el de rï¿½sas dï¿½clarï¿½es, il nï¿½est en effet pas obligatoire de remplir tous les codes de rï¿½sas.
	@warning Attention ï¿½ bien modifier le numï¿½ro max de modalitï¿½s de rï¿½servation lors dï¿½un ajout.
	@todo Supprimer le nombre de modalitï¿½s de rï¿½servation (tableau dynamique)

	Ce nombre est nï¿½cessaire pour permettre lï¿½allocation mï¿½moire lors du chargement.
*/
#define RESASFORMATLIGNENombre						"Nombre="


/** Dï¿½claration de modalitï¿½ de rï¿½servation
	@code [code @endcode
	@param code Code de modalitï¿½ de rï¿½servation (voir @ref baseHoraires)
	@warning Le code de la modalitï¿½ de rï¿½servation doit ï¿½tre strictement compris entre 0 et le numï¿½ro max de modalitï¿½ de rï¿½servation dï¿½fini par @ref RESASFORMATLIGNENombre.
*/
#define RESASFORMATLIGNEReservation					TYPESousSection



/** Caractï¿½re obligatoire de la rï¿½servation
	@code TYP valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- N : Impossible (le service n'est pas ouvert ï¿½ la rï¿½servation)
		- F : Facultative (le service peut ï¿½tre utilisï¿½ sans rï¿½servation, mais la rï¿½servation est ouverte pour garantir une place)
		- C : Obligatoire collectivement (au moins un client doit avoir rï¿½servï¿½ pour que le service soit utilisable)
		- O : Obligatoire (chaque client doit avoir rï¿½servï¿½ pour utiliser le service)
*/
#define RESASFORMATLIGNEType						0


/** Dï¿½lai minimal en minutes entre la rï¿½servation et le dï¿½part
	@code MIM valeur @endcode
	@param valeur Dï¿½lai minimal en minutes entre la rï¿½servation et le dï¿½part
	@see cModaliteReservation::momentLimiteReservation
*/
#define RESASFORMATLIGNEDelaiMinMinutes				1


/** Heure maximale de rï¿½servation le dernier jour de la pï¿½riode de validitï¿½
	@code MIH valeur @endcode
	@param valeur Heure maximale de rï¿½servation le dernier jour de la pï¿½riode de validitï¿½
	@see cModaliteReservation::momentLimiteReservation
*/
#define RESASFORMATLIGNEDelaiMinHeureMax			2


/** Dï¿½lai minimal en jours entre la rï¿½servation et le dï¿½part
	@code MIJ valeur @endcode
	@param valeur Dï¿½lai minimal en jours entre la rï¿½servation et le dï¿½part
	@see cModaliteReservation::momentLimiteReservation
*/
#define RESASFORMATLIGNEDelaiMinJours				3


/** Dï¿½lai maximal en jours entre la rï¿½servation et le dï¿½part
	@code MAJ valeur @endcode
	@param valeur Dï¿½lai maximal en jours entre la rï¿½servation et le dï¿½part
	@warning Le dï¿½lai maximal en jours doit ï¿½tre supï¿½rieur au dï¿½lai minimal en jours
	@see cModaliteReservation::momentDebutReservation
*/
#define RESASFORMATLIGNEDelaiMaxJours				4


/** Description complï¿½mentaire de la modalitï¿½ de rï¿½servation
	@code DES valeur @endcode
	@param valeur Description complï¿½mentaire de la modalitï¿½ de rï¿½servation
*/
#define RESASFORMATLIGNEDoc						5


/** Prix de la rï¿½servation (non gï¿½rï¿½)
	@code PRI valeur @endcode
	@param valeur Prix de la rï¿½servation
*/
#define RESASFORMATLIGNEPrix						6


/** Numï¿½ro de tï¿½lï¿½phone de la centrale de rï¿½servation
	@code TEL valeur @endcode
	@param valeur Numï¿½ro de tï¿½lï¿½phone de la centrale de rï¿½servation
*/
#define RESASFORMATLIGNETel						7


/** Horaires d'ouverture de la centrale de rï¿½servation tï¿½lï¿½phonique
	@code HTL valeur @endcode
	@param valeur Horaires d'ouverture de la centrale de rï¿½servation tï¿½lï¿½phonique
*/
#define RESASFORMATLIGNEHorairesTel				8


/** E-mail oï¿½ doivent ï¿½tre envoyï¿½s les listes de rï¿½servations (obligatoire pour rï¿½servation en ligne)
	@code EML valeur @endcode
	@param valeur E-mail oï¿½ doivent ï¿½tre envoyï¿½s les listes de rï¿½servations
*/
#define RESASFORMATLIGNEEMail						9
#define RESASFORMATLIGNEEMailCopie					10

/** Site web de rï¿½servation
	@code URL valeur @endcode
	@param valeur Site web de rï¿½servation
*/
#define RESASFORMATLIGNESiteWeb					11

/** Possibilitï¿½ de rï¿½servation en ligne
	@code REL valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- 0 : Rï¿½servation en ligne impossible
		- 1 : Rï¿½servation en ligne possible
*/
#define RESASFORMATLIGNEReservationEnLigne			12


/** Utilisation du champ prï¿½nom pour la rï¿½servation en ligne
	@code RLP valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- 0 : Champ prï¿½nom absent
		- 1 : Champ prï¿½nom prï¿½sent, remplissage facultatif
		- 2 : Champ prï¿½nom prï¿½sent, remplissage obigatoire
*/
#define RESASFORMATLIGNERELPrenom					13


/** Utilisation du champ adresse pour la rï¿½servation en ligne
	@code RLP valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- 0 : Champ adresse absent
		- 1 : Champ adresse prï¿½sent, remplissage facultatif
		- 2 : Champ adresse prï¿½sent, remplissage obigatoire
*/
#define RESASFORMATLIGNERELAdresse					14


/** Utilisation du champ e-mail pour la rï¿½servation en ligne
	@code RLE valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- 0 : Champ e-mail absent
		- 1 : Champ e-mail prï¿½sent, remplissage facultatif
		- 2 : Champ e-mail prï¿½sent, remplissage obigatoire
*/
#define RESASFORMATLIGNERELEMail					15


/** Utilisation du champ tï¿½lï¿½phone pour la rï¿½servation en ligne
	@code RLT valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- 0 : Champ tï¿½lï¿½phone absent
		- 1 : Champ tï¿½lï¿½phone prï¿½sent, remplissage facultatif
		- 2 : Champ tï¿½lï¿½phone prï¿½sent, remplissage obigatoire
*/
#define RESASFORMATLIGNERELTelephone				16


/** Utilisation du champ numï¿½ro d'abonnï¿½ pour la rï¿½servation en ligne
	@code RLN valeur @endcode
	@param valeur L'une des valeurs suivantes :
		- 0 : Champ numï¿½ro d'abonnï¿½ absent
		- 1 : Champ numï¿½ro d'abonnï¿½ prï¿½sent, remplissage facultatif
		- 2 : Champ numï¿½ro d'abonnï¿½ prï¿½sent, remplissage obigatoire
*/
#define RESASFORMATLIGNERELNumeroAbo				17
#define RESASFORMATLIGNEMax						18
#define RESASFORMATLIGNESeuil						19
#define RESASFORMATLIGNEReferenceALOrigine			20
#define RESASFORMATLIGNENombreFormats				21

#define RESASFORMATCOLONNEStandard					0
#define RESASFORMATCOLONNEHeure					1
#define RESASFORMATCOLONNESNombreFormats			2
//@}

//! @name Format du fichier des vï¿½los
//@{
#define VELOSFORMAT "velo"
#define VELOSEXTENSION "velo.per"
#define VELOSFORMATLIGNEType 						0
#define VELOSFORMATLIGNEContenance 				1
#define VELOSFORMATLIGNEDoc 						2
#define VELOSFORMATLIGNEPrix						3
#define VELOSFORMATLIGNEResa						4
#define VELOSFORMATLIGNENombreFormats				5

#define VELOSFORMATCOLONNEStandard					0
#define VELOSFORMATCOLONNESNombreFormats			1
//@}

//! @name Format du fichier des personnes handicapï¿½es
//@{
#define HANDICAPESFORMAT "handicape"
#define HANDICAPESEXTENSION "handicape.per"

#define HANDICAPESFORMATLIGNEType					0
#define HANDICAPESFORMATLIGNEContenance				1
#define HANDICAPESFORMATLIGNEDoc					2
#define HANDICAPESFORMATLIGNEPrix					3
#define HANDICAPESFORMATLIGNEResa					4
#define HANDICAPESFORMATLIGNENombreFormats			5

#define HANDICAPESFORMATCOLONNEStandard				0
#define HANDICAPESFORMATCOLONNESNombreFormats		1
//@}

//! @name Format du fichier des tarifs
//@{
#define TARIFSFORMAT "tarif"
#define TARIFSEXTENSION "tarif.per"

#define TARIFSFORMATLIGNEType						0
#define TARIFSFORMATLIGNENbCorrespondance			1
#define TARIFSFORMATLIGNEDuree					2
#define TARIFSFORMATLIGNEPkt						3
#define TARIFSFORMATLIGNEPra						4
#define TARIFSFORMATLIGNEPrb						5
#define TARIFSFORMATLIGNELibelle					6
#define TARIFSFORMATLIGNENombreFormats				7

#define TARIFSFORMATCOLONNEStandard				0
#define TARIFSFORMATCOLONNESNombreFormats			1
//@}

//! @name Format du fichier des rï¿½seaux
//@{
#define RESEAUXFORMAT "reseaux"
#define RESEAUXEXTENSION "reseaux.per"

#define RESEAUXFORMATLIGNENom						0
#define RESEAUXFORMATLIGNEUrl						1
#define RESEAUXFORMATLIGNEDoc						2
#define RESEAUXFORMATLIGNECartouche				3
#define RESEAUXFORMATLIGNENombreFormats				4

#define RESEAUXFORMATCOLONNEStandard				0
#define RESEAUXFORMATCOLONNESNombreFormats			1
//@}


//! @name Format du repertoire de donnees carto
//@{
#define CARTOEXTENSION "carto"


//!	@name Format du fichier du matï¿½riel
//@{
#define MATERIELSFORMAT "materiel"
#define MATERIELSEXTENSION "materiel.per"

/** (obligatoire) Libellï¿½ simple
	@code NOM libellï¿½ @endcode
	@param libellï¿½ Libellï¿½ simple
*/
#define MATERIELSFORMATLIGNENom					0
#define MATERIELSFORMATLIGNEDoc					1

/** Vitesse maximale des lignes exploitï¿½es avec le matï¿½riel dï¿½crit
	@code VIT vitesse @endcode
	@param vitesse Vitesse maximale en km/h
*/
#define MATERIELSFORMATLIGNEVitesse				2


/** Libellï¿½ du matï¿½riel ï¿½ utiliser dans les tableaux d'indicateur papier
	@code IND libellï¿½ @endcode
	@param libellï¿½ Libellï¿½ du matï¿½riel ï¿½ utiliser dans les tableaux d'indicateur papier
*/
#define MATERIELSFORMATLIGNEIndicateur				3


/** (obligatoire) Article pour insertion du libellï¿½ dans une phrase
	@code ART article @endcode
	@param article Article pour insertion du libellï¿½ dans une phrase
	@warning L'article fourni doit inclure l'ï¿½ventuel espace situï¿½ aprï¿½s celui-ci.
*/
#define MATERIELFORMATLIGNEArticle					4
#define MATERIELSFORMATLIGNENombreFormats			5

#define MATERIELSFORMATCOLONNEStandard				0
#define MATERIELSFORMATCOLONNESNombreFormats			1
//@}

//! @name Format du fichier des indicateurs
//@{
#define INDICATEURSFORMAT "indicateurs"
#define INDICATEURSEXTENSION "indicateurs.per"

#define INDICATEURSFORMATLIGNEJC					0
#define INDICATEURSFORMATLIGNEGare					1
#define INDICATEURSFORMATLIGNECP					2
#define INDICATEURSFORMATLIGNENombreFormats			3

#define INDICATEURSFORMATCOLONNEStandard			0
#define INDICATEURSFORMATCOLONNEObligatoire			1
#define INDICATEURSFORMATCOLONNEDepartArrivee		2
#define INDICATEURSFORMATCOLONNENombreFormats		3
//@}



#include "cFichier.h"
#include "cJourCirculation.h"

class cEnvironnement;


/** Fichier dï¿½crivant un calendrier de circulation
*/
class cFichierJoursCirculation : public cFichier
{
	bool 		JCExecuterCommande(cJC& JC, cJC::tSens Sens, cTexte& Tampon, cEnvironnement*);
	TypeSection 	RemplirJC(cJC& JC, cJC::tSens Sens, cTexte& Tampon, cEnvironnement*);

public:
	bool Charge(cEnvironnement*);
	cFichierJoursCirculation(const cTexte& NomFichier) : cFichier(NomFichier, NULL) {}
};

/** Fichier dï¿½crivant les points d'arrï¿½t
*/
class cFichierPointsArret : public cFichier
{
protected:
public:
	bool Charge(cEnvironnement*);
	cFichierPointsArret(const cTexte& __NomFichier, const cTexte& NomFichierFormats) : cFichier(__NomFichier
		, new cFormatFichier(NomFichierFormats, PAFORMAT, PAFORMATLIGNENombreFormats, PAFORMATCOLONNENombreFormats)) {}
};

/** @} */

#endif
