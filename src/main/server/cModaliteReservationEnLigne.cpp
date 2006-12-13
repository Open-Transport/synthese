/** Reservation rule with online booking class implementation.
@file cModaliteReservationEnLigne.cpp
*/


#include "cModaliteReservationEnLigne.h"


/** Constructeur.
 @param id Num�ro de la modalit� de r�servation dans l'environnement
 \author Hugues Romain
 \date 2001-2005
*/
cModaliteReservationEnLigne::cModaliteReservationEnLigne( const size_t& id ) : cModaliteReservation( id )
{
    vRELAdresse = Faux;
    vRELNumeroAbonne = Faux;
    vRELPrenom = Faux;
    vRELTelephone = Faux;
    vRELEMail = Faux;
    vMax = 0;
}


/** Destructor.
*/
cModaliteReservationEnLigne::~cModaliteReservationEnLigne()
{}

/*! \brief Ajoute un palier de nombre de r�servations de d�clenchement d'alerte
 \param valeur Nombre de r�servations � atteindre pour d�clencher l'envoi de l'alerte
 \param NombreSeuils Nombre total de paliers pour l'allocation (utilis� lors du premier appel � la m�thode uniquement)
 \return true si le seuil est bien enregistr�
 \warning La m�thode ne contr�le pas que le nombre total de seuils allou�s soit suffisant
 \warning Les seuils doivent �tre rentr�s dans l'ordre. Le contr�le de ce point n'est pas effectu�
 \author Hugues Romain
 \date 2005
*/
bool cModaliteReservationEnLigne::addSeuil( const OccupationLevel valeur, const size_t NombreSeuils )
{
    // Si allocation non faite et si nombre total fourni
    if ( !vSeuils && NombreSeuils )
        // Allocation du tableau de variables
        vSeuils = ( OccupationLevel* ) calloc( NombreSeuils + 1, sizeof( OccupationLevel ) );
    // Si allocation effectu�e et si le seuil est fourni et est un nombre positif
    if ( vSeuils && valeur >= 0 )
    {
        // Num�ro du seuil libre
        size_t i;

        // Recherche du prochain seuil libre
        for ( i = 0; vSeuils[ i ]; i++ )
        { }

        // Ecriture du seuil
        vSeuils[ i ] = valeur;

        // Op�ration effectu�e
        return ( true );
    }
    else
        // L'op�ration n'a pas pu �tre effectu�e
        return ( false );
}


/*! \brief Modificateur du nombre maximal de places r�servables
*/
bool cModaliteReservationEnLigne::setMax( const int valeur )
{
    if ( valeur >= 0 )
    {
        vMax = valeur;
        return ( true );
    }
    else
        return ( false );
}


bool cModaliteReservationEnLigne::SetMail( const std::string &newVal )
{
    vEMail = newVal;
    return ( true );
}

bool cModaliteReservationEnLigne::SetMailCopie( const std::string &newVal )
{
    vEMailCopie = newVal;
    return ( true );
}

bool cModaliteReservationEnLigne::SetRELTel( const tBool3 newVal )
{
    vRELTelephone = newVal;
    return ( true );
}

bool cModaliteReservationEnLigne::SetRELEMail( const tBool3 newVal )
{
    vRELEMail = newVal;
    return ( true );
}

bool cModaliteReservationEnLigne::SetRELAdresse( const tBool3 newVal )
{
    vRELAdresse = newVal;
    return ( true );
}

bool cModaliteReservationEnLigne::SetRELNumeroAbo( const tBool3 newVal )
{
    vRELNumeroAbonne = newVal;
    return ( true );
}

bool cModaliteReservationEnLigne::SetRELPrenom( const tBool3 newVal )
{
    vRELPrenom = newVal;
    return ( true );
}

tBool3 cModaliteReservationEnLigne::GetRELTel() const
{
    return ( vRELTelephone );
}

tBool3 cModaliteReservationEnLigne::GetRELEMail() const
{
    return ( vRELEMail );
}

tBool3 cModaliteReservationEnLigne::GetRELNumeroAbo() const
{
    return ( vRELNumeroAbonne );
}

tBool3 cModaliteReservationEnLigne::GetRELPrenom() const
{
    return ( vRELPrenom );
}

tBool3 cModaliteReservationEnLigne::GetRELAdresse() const
{
    return ( vRELAdresse );
}

const std::string& cModaliteReservationEnLigne::GetMail() const
{
    return ( vEMail );
}

const std::string& cModaliteReservationEnLigne::GetMailCopie() const
{
    return ( vEMailCopie );
}



/*! \brief D�tection du franchissement d'un palier de nombre de r�servations pour envoi d'alerte
 \param ancienneValeur Nombre de r�servations sur le service avant la modification
 \param nouvelleValeur Nombre de r�servations sur le service apr�s la modification
 \return Le palier franchi : le plus grand si le nombre de r�servations est croissant, le plus petit sinon
 \todo R�int�grer la notion de seuils si besoin
*/ 
/*tSeuilResa cModaliteReservationEnLigne::getSeuil(const int ancienneValeur, const int nouvelleValeur) const
{
 //Aucun seuil d�fini
 if (!vSeuils)
  return 0;
 
 tSeuilResa valeurPalier=0;
 // Nombre de r�servations croissant (nouvelle r�servation)
 if (nouvelleValeur >= ancienneValeur)
 {
  // D�tection du dernier palier d�pass� strictement
  for (size_t i=0; vSeuils[i]; i++)
  {
   if (ancienneValeur<= vSeuils[i] && vSeuils[i]< nouvelleValeur)
    valeurPalier = vSeuils[i];
  }
 }
 else // Nombre de r�servations d�croissant (annulation de r�servation)
 {
  // D�tection du premier palier d�pass� strictement
  for (size_t i=0; vSeuils[i]; i++)
  {
   if (ancienneValeur>= vSeuils[i] && vSeuils[i]> nouvelleValeur)
   {
    valeurPalier = vSeuils[i];
    break;
   }
  }
 }
 
 return valeurPalier;
}*/



/*! \brief Extraction du nombre de places disponibles dans un service
 \author Hugues Romain
 \param tCirculation Service sur lequel on doit r�server
 \param tDate Moment de d�part
 \param tBaseManager Gestionnaire de base de donn�es
 \return Le nombre de places disponibles sur le service. En cas d'�chec de la requ�te, renvoie 0.
 \date 2005
 \warning Cette m�thode ne fonctionne que si la circulation dure moins de 24 heures
 \warning Pour assurer la validit� des r�sultats s'ils doivent �tre utilis�s par la suite, il appartient � la fonctoin appelante de bloquer la base de donn�es � l'aide de cDatabaseManager::AttendEtBloque().
 
Cette m�thode raisonne sur le principe d'un nombre maximal de places r�serv�es par service et non simultan�ment par tron�on
*/ 
/*size_t cModaliteReservationEnLigne::Disponibilite(const cTrain* tCirculation, const synthese::time::DateTime& tDate, cDatabaseManager* tBaseManager) const
{
 // Calcul de la plage de dates sur laquelle seront consid�r�es les r�servations d�j� prises comme places occup�es
 synthese::time::DateTime tMomentDebutCirculation;
 synthese::time::DateTime tMomentFinMaxCirculation;
 tMomentDebutCirculation = tDate;
 
 // Si l'heure de d�part souhait�e est inf�rieure � l'heure � l'origine, alors la circulation est partie le jour pr�c�dent
 if (*tCirculation->getHoraireDepartPremier() > tDate.getHeure())
  tMomentDebutCirculation--;
 
 // L'heure de d�but de la plage est l'heure de d�part de l'origine
 tMomentDebutCirculation = *tCirculation->getHoraireDepartPremier(); 
 
 // L'heure de fin de la plage est l'heure de d�part de l'origine plus 24 heures
 tMomentFinMaxCirculation = tMomentDebutCirculation;
 tMomentFinMaxCirculation++;
 
 //preparation de la requete
 std::stringstream sout;
 synthese::util::PlainCharFilter filter;
 boost::iostreams::filtering_ostream requete;
 requete.push (filter);
 requete.push (sout);
    
 
 requete  << "SELECT IFNULL(SUM("<< TABLE_RESERVATION_NOMBRE_PLACES <<"),0)"
 << " FROM "<< TABLE_RESERVATION 
 << " WHERE " << TABLE_RESERVATION_CODE_LIGNE <<"='"<< tCirculation->getLigne()->Axe()->getCode()
 <<"' AND " << TABLE_RESERVATION_NUM_SERVICE <<"='"<< tCirculation->getNumero() << "'";
     
 requete   << " AND " << TABLE_RESERVATION_DATE_DEPART << " >= '" << tMomentDebutCirculation << "'";
     
 requete   << " AND " << TABLE_RESERVATION_DATE_DEPART << " < '" << tMomentFinMaxCirculation << "'"
 << " AND " << TABLE_RESERVATION_ETAT << "='R'"
 << ";";
 
 bool status = tBaseManager->execute(sout.str ());
 try
 {
  if (status)
  {
   Row row;
   const Result * res = tBaseManager->getResultats();
     Result::iterator i = res->begin();
     row = *i; 
    
     //nombre de places deja reserv�es
     int tPlacesDisponibles = vMax - (int) row[0];
   
   if (tPlacesDisponibles > 0)
    return tPlacesDisponibles;
  }
  else
  {
   // log(vBaseManager->getMessage());
  }
 }
 catch(BadConversion er)
 {
//  std::string temp;
//  temp << "Probl�me de conversion des donn�es:" << std::string(er.data) << " en " << std::string(er.type_name);
//  log(temp);
//  Tampon << MESSAGE_ERREUR_GENERIQUE;
 }
 catch(BadNullConversion er)
 {
//  log(std::string("Probl�me de conversion d'une donn�e nulle"));
//  Tampon << MESSAGE_ERREUR_GENERIQUE;
 }
   
 return 0;
}*/



/*! \brief Tente d'effectuer une r�servation
 \param tService Service sur lequel r�server
 \param tPADepart Point d'arr�t de d�part
 \param tPAArrivee Point d'arr�t d'arriv�e
 \param tDateDepart Moment du d�part souhait�
 \param tNom Nom du client
 \param tNomBrut Nom du client tel que demand�
 \param tPrenom Pr�nom du client
 \param tAdresse Adresse du client
 \param tEmail Email du client
 \param tTelephone Num�ro de t�l�phone du client
 \param tNumAbonne Num�ro d'abonnement du client
 \param tAdressePAArrivee Compl�ment d'adresse du lieu d'arriv�e (si arr�t d'arriv�e tout lieu)
 \param tAdressePADepart Compl�ment d'adresse du lieu de d�part (si arr�t de d�part tout lieu)
 \param tNombrePlaces Nombre de places demand�es
 \param tBaseManager Objet d'acc�s � la base de donn�es des r�servations
 \return true si la r�servation a �t� prise
 \author Hugues Romain
 \date 2005
 \todo G�rer les paliers
 \todo Ne plus associer la r�servation proprement dite � la notion d'�ch�ance de r�sa qui doivent faire l'objet d'une table s�par�e pour pouvoir envoyer un mail meme s'il n'y a personne. Cette table doit �tre remplie au chargement de SYNTHESE. Voir refonte compl�te du module de r�servation
 \todo Faire une classe client contenant toutes ses coordonn�es et pouvant �tre retrouv�e dans un fichier ou dans la base
*/ 
/*bool cModaliteReservationEnLigne::Reserver(const cTrain* tService
     , const LogicalPlace* tPADepart, const LogicalPlace* tPAArrivee
     , const synthese::time::DateTime& tDateDepart
     , const std::stringSQL& tNom, const std::stringSQL& tNomBrut, const std::stringSQL& tPrenom
     , const std::stringSQL& tAdresse, const std::stringSQL& tEmail, const std::stringSQL& tTelephone, const std::stringSQL& tNumAbonne
     , const std::stringSQL& tAdressePAArrivee, const std::stringSQL& tAdressePADepart
     , const size_t tNombrePlaces
     , cDatabaseManager* tBaseManager
     ) const
{
 // Statut de la demande
 bool vRetour = false;
 
 // Acc�s exclusif � la base
 if (tBaseManager->AttendEtBloque("synthese_resa"))
 {
  // Date de la demande de r�servation
  synthese::time::DateTime tMaintenant;
  tMaintenant.setMoment();
  
  // V�rification des possibilit�s de r�servation
  bool delaisOK = reservationPossible(tService, tMaintenant, tDateDepart);
  bool maxOK = (Disponibilite(tService, tDateDepart, tBaseManager) >= tNombrePlaces);
  
  // Collecte des donn�es
  synthese::time::DateTime tMomentLimite = momentLimiteReservation(tService, tDateDepart);
  int tNumeroResa = tBaseManager->GetNumeroReservation(tMaintenant.getDate());
  
  // on construit le numero de reservation qui doit etre unique et augment� de 1 a partir de maintenant
  if (tNumeroResa)
  {
   //construction de l'id
   std::string tCodeResa;
   tCodeResa << tMaintenant.getDate().toInternalString () << TXT2(tNumeroResa,6);
   
   //preparation de la requete d'insertion des donn�es
   cRequeteSQLInsert requete(TABLE_RESERVATION);
   requete.AddChamp(TABLE_RESERVATION_NUMERO, tCodeResa);
   requete.AddChamp(TABLE_RESERVATION_NOM, tNom);
   requete.AddChamp(TABLE_RESERVATION_PRENOM, tPrenom);
   requete.AddChamp(TABLE_RESERVATION_ADRESSE, tAdresse);
   requete.AddChamp(TABLE_RESERVATION_TELEPHONE, tTelephone);
   requete.AddChamp(TABLE_RESERVATION_EMAIL, tEmail);
   requete.AddChamp(TABLE_RESERVATION_NUM_ABONNE, tNumAbonne);
   requete.AddChamp(TABLE_RESERVATION_NOMBRE_PLACES, TXT(tNombrePlaces));
   requete.AddChamp(TABLE_RESERVATION_CODE_LIGNE, tService->getLigne()->Axe()->getCode());
   requete.AddChamp(TABLE_RESERVATION_LIBELLE_LIGNE, tService->getLigne()->getLibelleComplet());
   requete.AddChamp(TABLE_RESERVATION_MOD_RESA, vIndex);
   requete.AddChamp(TABLE_RESERVATION_NUM_SERVICE, tService->getNumero());
   requete.AddChamp(TABLE_RESERVATION_CODE_PA_DEPART, TXT(tPADepart->Index()));
   requete.AddChamp(TABLE_RESERVATION_DES_PA_DEPART, tAdressePADepart);
   requete.AddChamp(TABLE_RESERVATION_CODE_PA_ARRIVEE , TXT(tPAArrivee->Index()));
   requete.AddChamp(TABLE_RESERVATION_DES_PA_ARRIVEE , tAdressePAArrivee);
   requete.AddChamp(TABLE_RESERVATION_DATE_DEPART, tDateDepart);
   requete.AddChamp(TABLE_RESERVATION_EMAIL_TRANSPORTEUR, vEMail);
   requete.AddChamp(TABLE_RESERVATION_EMAIL_COPIE, vEMailCopie);
   requete.AddChamp(TABLE_RESERVATION_DATE_ENVOI, tMomentLimite);
   requete.AddChamp(TABLE_RESERVATION_DATE_RESA, "NOW()", true); 
   
   if (!delaisOK) //prob de delai de reservation
   {
    requete.AddChamp(TABLE_RESERVATION_ETAT, "N");
    requete.AddChamp(TABLE_RESERVATION_RAISON_ECHEC, "D�lai d�pass�");
   }
   else if (!maxOK) // prob de nombre max de reservation
   {
    requete.AddChamp(TABLE_RESERVATION_ETAT, "N");
    requete.AddChamp(TABLE_RESERVATION_RAISON_ECHEC, "Nombre de places disponibles insuffisant");
   }
   else // OK
    requete.AddChamp(TABLE_RESERVATION_ETAT, "R");
   
   // on execute la requete d'insertion
   vRetour = tBaseManager->execute(requete) && delaisOK && maxOK;
  }
  
  // Arr�t de l'acc�s exclusif � la base de donn�es  
  tBaseManager->Debloque("synthese_resa");
 }
 
 return vRetour;
}*/



/*! \brief Annulation de r�servation
 \param CodeReservation Code de la r�servation
 \param tBaseManager Gestionnaire de base de donn�es
 \return true si l'annulation a fonctionn�
 \author Hugues Romain
 \date 2005
 
L'annulation est accept�e si :
 - le code de r�servation a pu d�terminer une r�servation existante
 - la date d'�ch�ance de la r�servation n'est pas atteinte
 
 \warning Seul un code de r�servation valide suffit � actionner cette m�thode. Il est donc n�cessaire d'adjoindre un contr�le de s�curit� si ce code peut �tre entr� par des utilisateurs
 
 \todo G�rer les paliers
*/ 
/*bool cModaliteReservationEnLigne::Annuler(const std::string& CodeReservation, cDatabaseManager* tBaseManager) const
{ 
 // Statut de la demande
 bool vRetour = false;
 
 // Acc�s exclusif � la base
 if (tBaseManager->AttendEtBloque("synthese_resa"))
 {
  // Date de la demande de r�servation
  synthese::time::DateTime Maintenant;
  Maintenant.setMoment();
  std::stringSQL sqlMaintenant; 
  sqlMaintenant << Maintenant;
  
  // Requete de modification incluant le test de non d�passement de l'�ch�ance
  std::string Requete;
  Requete << "UPDATE " << TABLE_RESERVATION 
   << " SET " << TABLE_RESERVATION_ETAT << "='A'"
    << ", " << TABLE_RESERVATION_DATE_ANNUL << "='" << sqlMaintenant << "'"
   << " WHERE " << TABLE_RESERVATION_NUMERO << "='" << CodeReservation << "'"
    << " AND " << TABLE_RESERVATION_DATE_ENVOI << ">='" << sqlMaintenant << "'"
   << " LIMIT 1;";
    
  // Ex�cution de la requ�te
  if(tBaseManager->execute(Requete))
   vRetour = true;
   
  // Lib�ration de l'acc�s exclusif
  tBaseManager->Debloque("synthese_resa");
 }
  
 // Sortie
 return vRetour;
}*/

