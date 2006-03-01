
#include "cIndicateurs.h"
#include "cGareLigne.h"
#include "cLigne.h"
#include "cArretPhysique.h"
#include "LogicalPlace.h"
#include "cCommune.h"

#include "04_time/DateTime.h"
#include "04_time/Schedule.h"


/*! \brief Constructeur
 \param NombrePA Nombre de lignes de la colonne
 \author Hugues Romain 
 \date 2001
 @todo VOIR A GERER L'ALLOCATION DU MASQUE QUI A ETE SUPPRIMEE
*/
cColonneIndicateurs::cColonneIndicateurs( size_t newNombreGares, cLigne* newLigne, cJC* newJC )
{
    vNombreGares = newNombreGares;
    vOrigineSpeciale = Texte;
    vDestinationSpeciale = Texte;
    vSuivant = NULL;
    newJC->SetInclusionToMasque( vMasque );
    vColonne = ( const synthese::time::Schedule** ) calloc( vNombreGares, sizeof( synthese::time::Schedule* ) );
    vLigne = newLigne;
    vRenvoi = NULL;
}




cGareIndicateurs::cGareIndicateurs( LogicalPlace* newPA, cGareLigne::tTypeGareLigneDA newTypeDA, tTypeGareIndicateur newObligatoire )
{
    vPA = newPA;
    vTypeDA = newTypeDA;
    vObligatoire = newObligatoire;
    vSuivant = NULL;
}

cIndicateurs::cIndicateurs( const cTexte& newTitre, cEnvironnement* const newEnvironnement )
        : vEnvironnement( newEnvironnement )
        , vTitre( newTitre )
        , vJC( vEnvironnement->PremiereAnnee(), vEnvironnement->DerniereAnnee(), 0, "" )
{
    vPremiereCI = NULL;
    vPremiereGI = NULL;
    vDerniereGI = NULL;
    vNombreGares = 0;
    vCommencePage = false;
}

// NUL Gerer �a avec une petite classe cTypeDA...
bool cGareIndicateurs::EstDepart() const
{
    return ( vTypeDA == cGareLigne::Depart || vTypeDA == cGareLigne::Passage );
}

bool cGareIndicateurs::EstArrivee() const
{
    return ( vTypeDA == cGareLigne::Arrivee || vTypeDA == cGareLigne::Passage );
}


/*! \brief Op�rateur de comparaison <=
 \return true si et seulement si:
   - Au moins une ligne non vide commune entre les deux colonnes
   - L'heure de this est inf�rieure ou �gale � l'heure de l'autre colonne
 \warning Les deux colonnes doivent �tre de la m�me taille
 \todo Voir si les JPlus sont bien g�r�s
*/
int cColonneIndicateurs::operator <= ( const cColonneIndicateurs& AutreColonne ) const
{
    //SET PORTAGE LINUX
    //int i=0;
    //int j=0;
    size_t i;
    size_t j;
    //END PORTAGE

    // Tentative par ligne commune
    for ( i = 0; i < vNombreGares; i++ )
        if ( ( vColonne[ i ] != NULL ) && ( AutreColonne.vColonne[ i ] != NULL ) )
        {
            if ( *vColonne[ i ] < *AutreColonne.vColonne[ i ] )
                return ( true );
            if ( *AutreColonne.vColonne[ i ] < *vColonne[ i ] )
                return ( false );
        }

    // Tentative par succession 1
    for ( i = 0; i < vNombreGares; i++ )
    {
        if ( vColonne[ i ] != NULL )
        {
            for ( j = i + 1; j < vNombreGares; j++ )
                if ( AutreColonne.vColonne[ j ] != NULL )
                {
                    if ( *AutreColonne.vColonne[ j ] < *vColonne[ i ] )
                        return ( false );
                }

            // SET : rajout a cause du unsigned, j et i ne peuvent etre negatif
            if ( i != 0 )
            {
                for ( j = i - 1; j > 0; j-- )
                    if ( AutreColonne.vColonne[ j ] != NULL )
                    {
                        if ( *vColonne[ i ] < *AutreColonne.vColonne[ j ] )
                            return ( true );
                    }
            }
            // end SET
        }
    }
    // Premiere heure
    for ( i = 0; i < vNombreGares; i++ )
        if ( vColonne[ i ] != NULL )
            break;
    for ( j = 0; j < vNombreGares; j++ )
        if ( AutreColonne.vColonne[ j ] != NULL )
            break;
    return ( *vColonne[ i ] < *AutreColonne.vColonne[ j ] );
}
// � Hugues Romain 2001
// ____________________________________________________________________________




// cIndicateurs 1.0 - Add
// ____________________________________________________________________________
//
// Ajoute la liste de colonnes newCI � la liste du tableau
// Les colonnes sont class�es par heure.
// En cas d'heures similaires, il n'y a pas cr�ation de colonne mais
// seulement r�sum� d'informations (JC, destinations...)
// Renvoie true si une colonne a r�ellement �t� cr��e
// ERR: newCI doit �tre class� et different de NULL.
// ____________________________________________________________________________
bool cIndicateurs::Add( cColonneIndicateurs* newCI, cJC* JC )
{
    if ( vPremiereCI == NULL )
        vPremiereCI = newCI;
    else
    {
        cColonneIndicateurs* curCIInd = vPremiereCI;
        cColonneIndicateurs* precCIInd = NULL;

        while ( true )
        {

            if ( curCIInd == NULL )
            {
                precCIInd->setSuivant( newCI );
                break;
            }
            if ( newCI->getPostScript().Compare( curCIInd->getPostScript() ) )
            {
                if ( curCIInd->getLigne() ->getLineStops().front() ->ArretPhysique() ->getLogicalPlace() != newCI->getLigne() ->getLineStops().front() ->ArretPhysique() ->getLogicalPlace() )
                    curCIInd->setOrigineSpeciale( Indetermine );
                if ( curCIInd->getLigne() ->getLineStops().back() ->ArretPhysique() ->getLogicalPlace() != newCI->getLigne() ->getLineStops().back() ->ArretPhysique() ->getLogicalPlace() )
                    curCIInd->setDestinationSpeciale( Indetermine );
                JC->SetInclusionToMasque( curCIInd->getMasque() );
                delete newCI;
                return ( false );
            }
            else
                if ( *newCI <= *curCIInd )
                {
                    if ( precCIInd == NULL )
                    {
                        vPremiereCI = newCI;
                        newCI = newCI->getSuivant();
                        vPremiereCI->setSuivant( curCIInd );
                        curCIInd = vPremiereCI;
                    }
                    else
                    {
                        precCIInd->setSuivant( newCI );
                        newCI = newCI->getSuivant();
                        precCIInd->getSuivant() ->setSuivant( curCIInd );
                        curCIInd = precCIInd->getSuivant();
                    }
                }
                else
                {
                    precCIInd = curCIInd;
                    curCIInd = curCIInd->getSuivant();
                }

            if ( newCI == NULL )
                break;
        }
    }
    vNombreColonnes++;
    return ( true );
}
// � Hugues Romain 2001
// ____________________________________________________________________________




// cIndicateurs 1.0 - Renvoi
// ____________________________________________________________________________
//
// Retrouve un renvoi dans les colonnes
// ____________________________________________________________________________
cRenvoiIndicateurs* cIndicateurs::Renvoi( size_t Numero ) const
{
    for ( cColonneIndicateurs * curCI = vPremiereCI; curCI != NULL; curCI = curCI->getSuivant() )
    {
        if ( curCI->getRenvoi() != NULL && curCI->getRenvoi() ->Numero() == Numero )
            return ( curCI->getRenvoi() );
    }
    return ( NULL );
}
// � Hugues Romain 2002
// ____________________________________________________________________________




// cIndicateurs 1.0 - ConstruitRenvois
// ____________________________________________________________________________
//
// Construit et attache les renvois aux colonnes calcul�es
// ____________________________________________________________________________
void cIndicateurs::ConstruitRenvois()
{
    cColonneIndicateurs * curCI;
    cColonneIndicateurs* curCI2;
    cJC* curJC;
    vNombreRenvois = 0;

    for ( curCI = vPremiereCI; curCI != NULL; curCI = curCI->getSuivant() )
        if ( !vJC.TousPointsCommuns( vJC, curCI->getMasque() ) )
        {
            // Identification du JC
            curJC = vEnvironnement->GetJC( curCI->getMasque(), vJC );

            // Tentative de renvoi existant
            for ( curCI2 = vPremiereCI; curCI2 != curCI; curCI2 = curCI2->getSuivant() )
            {
                if ( curCI2->getRenvoi() != NULL && curCI2->getRenvoi() ->getJC() == curJC )
                {
                    curCI->setRenvoi( curCI2->getRenvoi() );
                    break;
                }
            }
            // Non trouv�
            if ( curCI2 == curCI )
            {
                vNombreRenvois++;
                curCI->setRenvoi( new cRenvoiIndicateurs( curJC, vNombreRenvois ) );
            }
        }
}
// � Hugues Romain 2002
// ____________________________________________________________________________




// cRenvoiIndicateurs 1.0 - Constructeur
// ____________________________________________________________________________
//
// ____________________________________________________________________________
cRenvoiIndicateurs::cRenvoiIndicateurs( cJC*newJC, size_t newNumero )
{
    vJC = newJC;
    vNumero = newNumero;
}
// � Hugues Romain 2002
// ____________________________________________________________________________




cColonneIndicateurs* cIndicateurs::Colonne( size_t Numero ) const
{
    cColonneIndicateurs * curCI = vPremiereCI;
    for ( ; Numero != 0; Numero-- )
        curCI = curCI->getSuivant();
    return ( curCI );
}




void cIndicateurs::EcritTableaux( size_t HDispo, size_t NumeroColonne, size_t NombreTableaux, bool RenvoisAEcrire, size_t NumeroPageRelatif, ofstream& FichierSortie ) const
{
    cColonneIndicateurs * curCI;
    cCommune* curCommune;
    cGareIndicateurs* curGI;
    size_t y;
    size_t NumeroGare;
    size_t iRenvoi;

    // C Ecriture du fichier
    // 0: Param�tres g�n�raux
    FichierSortie << "/haut " << HDispo << " def\n";
    FichierSortie << "/nombretableauxvertical " << NombreTableaux << " def\n";
    FichierSortie << "/nombrelignesarray [" << vNombreGares << "] def\n";
    FichierSortie << "/nombretableauxhorizontal 1 def\n";

    // 1: Liste des gares
    FichierSortie << "/garesarray [ [";
    for ( curGI = vPremiereGI; curGI != NULL; curGI = curGI->getSuivant() )
    {
        FichierSortie << "(" << curGI->getArretLogique() ->getName() << ") ";
    }
    FichierSortie << "] ] def\n";

    // 2: Liste des communes
    FichierSortie << "/communesarray [ [";
    curCommune = NULL;
    for ( curGI = vPremiereGI; curGI != NULL; curGI = curGI->getSuivant() )
    {
        FichierSortie << "(";
        if ( curGI->getArretLogique() ->getTown() != curCommune )
        {
            curCommune = curGI->getArretLogique() ->getTown();
            FichierSortie << curCommune->getName();
        }
        FichierSortie << ") ";
    }
    FichierSortie << "] ] def\n";

    // 3: Codes de gare
    FichierSortie << "/codesgaresarray [ [";
    for ( curGI = vPremiereGI; curGI != NULL; curGI = curGI->getSuivant() )
    {
        FichierSortie << "(" << curGI->getArretLogique() ->Index() << ") ";
    }
    FichierSortie << "] ] def\n";

    // 4: D/A
    FichierSortie << "/DAarray [ [";
    for ( curGI = vPremiereGI; curGI != NULL; curGI = curGI->getSuivant() )
    {
        FichierSortie << "(";
        if ( curGI->TypeDA() == cGareLigne::Depart )
            FichierSortie << "D";
        else if ( curGI->TypeDA() == cGareLigne::Arrivee )
            FichierSortie << "A";

        FichierSortie << ") ";
    }
    FichierSortie << "] ] def\n";

    // Ecriture du titre
    FichierSortie << "(" << vTitre;
    if ( NumeroPageRelatif != 0 )
        FichierSortie << " - " << NumeroPageRelatif;
    FichierSortie << ") titre\n";

    // Ecriture de l'entete
    if ( NombreTableaux != 0 )
    {
        FichierSortie << "dessineentetes\n";

        // Ecriture des colonnes
        curCI = Colonne( NumeroColonne );

        FichierSortie << "/y 0 def\n";
    }
    else
    {
        curCI = NULL;

        FichierSortie << "/y -1 def\n";
    }

    y = 0;
    for ( NumeroColonne = 0; ( NombreTableaux != 0 && curCI != NULL ); curCI = curCI->getSuivant() )
    {
        FichierSortie << "/c " << NumeroColonne << " def\n";
        FichierSortie << "[[(";
        if ( curCI->getRenvoi() != NULL )
            FichierSortie << "    " << ( short int ) curCI->getRenvoi() ->Numero();
        FichierSortie << ") (" << curCI->getLigne() ->getNomPourIndicateur() << ") (" << curCI->getLigne() ->Materiel() ->getIndicateur() << ") () () ()] ";
        switch ( curCI->OrigineSpeciale() )
        {
            case Texte:
                FichierSortie << "0 (" << curCI->getLigne() ->getLineStops().front() ->ArretPhysique() ->getLogicalPlace() ->getDesignationOD() << ") ";
                break;

            case Terminus:
                FichierSortie << "1 () ";
                break;

            case Indetermine:
                FichierSortie << "2 () ";
        }

        switch ( curCI->DestinationSpeciale() )
        {
            case Texte:
                FichierSortie << "0 (" << curCI->getLigne() ->getLineStops().back() ->ArretPhysique() ->getLogicalPlace() ->getDesignationOD() << ") ";
                break;

            case Terminus:
                FichierSortie << "1 () ";
                break;

            case Indetermine:
                FichierSortie << "2 () ";
        }

        FichierSortie << curCI->getPostScript() << " [()] [";
        for ( NumeroGare = 0; NumeroGare < vNombreGares; NumeroGare++ )
            FichierSortie << "0 ";
        FichierSortie << "]]\ndessinecolonne\n";
        NumeroColonne++;
        if ( NumeroColonne == IndicateursNombreColonnesMax && curCI->getSuivant() != NULL )
        {
            y++;
            FichierSortie << "/y " << y << " def\n";
            NumeroColonne -= IndicateursNombreColonnesMax;
            NombreTableaux--;
        }
    }

    // Ecriture des renvois

    if ( vNombreRenvois != 0 && RenvoisAEcrire )
    {
        FichierSortie << "/renvoisarray [(RENVOIS:) ";
        for ( iRenvoi = 1; iRenvoi <= vNombreRenvois; iRenvoi++ )
            if ( Renvoi( iRenvoi ) ->getJC() != NULL )
                FichierSortie << "([" << ( short int ) iRenvoi << "] - " << Renvoi( iRenvoi ) ->getJC() << ") ";
            else
                FichierSortie << "([" << ( short int ) iRenvoi << "] - INDETERMINE) ";
        FichierSortie << "] def\ndessinerenvois\n";
    }
}

void cColonneIndicateurs::setSuivant( cColonneIndicateurs *newVal )
{
    vSuivant = newVal;
}

const cTexte& cColonneIndicateurs::getPostScript() const
{
    return ( vPostScript );
}

cLigne* cColonneIndicateurs::getLigne() const
{
    return ( vLigne );
}

void cColonneIndicateurs::setOrigineSpeciale( tTypeOD newVal )
{
    vOrigineSpeciale = newVal;
}

void cColonneIndicateurs::setDestinationSpeciale( tTypeOD newVal )
{
    vDestinationSpeciale = newVal;
}

cJC::Calendar& cColonneIndicateurs::getMasque()
{
    return ( vMasque );
}

cColonneIndicateurs* cColonneIndicateurs::getSuivant() const
{
    return ( vSuivant );
}

cRenvoiIndicateurs* cColonneIndicateurs::getRenvoi() const
{
    return ( vRenvoi );
}

size_t cRenvoiIndicateurs::Numero() const
{
    return ( vNumero );
}

cJC* cRenvoiIndicateurs::getJC() const
{
    return ( vJC );
}

void cColonneIndicateurs::setRenvoi( cRenvoiIndicateurs *newVal )
{
    vRenvoi = newVal;
}

cGareIndicateurs* cGareIndicateurs::getSuivant() const
{
    return ( vSuivant );
}

LogicalPlace* cGareIndicateurs::getArretLogique() const
{
    return ( vPA );
}

cGareLigne::tTypeGareLigneDA cGareIndicateurs::TypeDA() const
{
    return ( vTypeDA );
}

tTypeOD cColonneIndicateurs::OrigineSpeciale() const
{
    return ( vOrigineSpeciale );
}

tTypeOD cColonneIndicateurs::DestinationSpeciale() const
{
    return ( vDestinationSpeciale );
}

void cIndicateurs::setCommencePage( bool newVal )
{
    vCommencePage = newVal;
}

void cIndicateurs::setJC( const cJC& newVal, const cJC& newVal2 )
{
    vJC.setMasque( newVal.Et( newVal2 ) );
}

void cIndicateurs::addArretLogique( LogicalPlace *newArretLogique, cGareLigne::tTypeGareLigneDA newTypeDA, tTypeGareIndicateur newTypeGI )
{
    if ( newArretLogique )
    {
        cGareIndicateurs * curGareIndicateur = new cGareIndicateurs( newArretLogique, newTypeDA, newTypeGI );
        vNombreGares++;
        if ( vDerniereGI )
        {
            vDerniereGI->setSuivant( curGareIndicateur );
            vDerniereGI = curGareIndicateur;
        }
        else
        {
            vDerniereGI = curGareIndicateur;
            vPremiereGI = curGareIndicateur;
        }
    }
}

void cGareIndicateurs::setSuivant( cGareIndicateurs *newVal )
{
    vSuivant = newVal;
}

void cIndicateurs::Reset()
{
    vNombreColonnes = 0;
}

cGareIndicateurs* cIndicateurs::getPremiereGI() const
{
    return ( vPremiereGI );
}

tTypeGareIndicateur cGareIndicateurs::Obligatoire() const
{
    return ( vObligatoire );
}

const cJC& cIndicateurs::getJC() const
{
    return ( vJC );
}

void cColonneIndicateurs::setColonne( size_t n, const synthese::time::Schedule* newVal )
{
    vColonne[ n ] = newVal;
    vPostScript << " ( ";
    if ( newVal == NULL )
        vPostScript << "     ";
    else
      vPostScript << newVal->getHour ().toString();

    vPostScript << " )";
}

size_t cIndicateurs::NombreGares() const
{
    return ( vNombreGares );
}

void cColonneIndicateurs::CopiePostScript( const cTexte &newVal )
{
    vPostScript << newVal;
}

bool cIndicateurs::CommencePage() const
{
    return ( vCommencePage );
}

size_t cIndicateurs::NombreColonnes() const
{
    return ( vNombreColonnes );
}

size_t cIndicateurs::NombreRenvois() const
{
    return ( vNombreRenvois );
}

const cTexte& cIndicateurs::getTitre() const
{
    return ( vTitre );
}
