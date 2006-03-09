/*! \file cInterface.h
\brief En-t�te classes d'interface
*/

#ifndef SYNTHESE_CINTERFACE_H
#define SYNTHESE_CINTERFACE_H

#include "cInterface_Objet_AEvaluer_PageEcran.h"
#include "cTableauDynamique.h"

#include "04_time/HourPeriod.h"
#include "04_time/Date.h"


// Inclusions
#include <string>
#include <vector>

#include "04_time/module.h"


#include <string>
#include <vector>
#include <iostream>

class cInterface;





/** Impl�mentation de la notion d'Interface
 \author Hugues Romain
 \date 2001-2005
 \todo Lancer une d�marche globale de d�finition du format de sortie en collaboration avec std::string et ses d�riv�s
 @ingroup m11
 
Cette classe d�finit les diff�rentes interfaces pour l'affichage de r�sultats issus de calculs SYNTHESE. Une interface est constitu�e de mod�les d'affichage, permettant la d�finition du graphisme en sortie, fournis � SYNTHESE en tant que donn�es. Des mod�les de fichiers de sortie doivent �tre fournis au chargement d'apr�s un format de codage d�fini pour l'occasion.</p>
 
La liste des \ref InterfaceObjetsStandard d�finit les objets qui peuvent �tre d�crits par un mod�le. La fourniture d'un mod�le pour chacun de ces objets est facultative. En cas de demande d'affichage d'un objet pour lequel aucun mod�le n'a �t� fourni, une page vide sera retourn�e.
 
Les mod�les sont d�finis par la classe cElementInterface et ses d�riv�s, et sont ind�x�s dans le tableau de pointeurs vElement, selon une indexation par num�ro d'objet standard, selon la nomenclature des \ref InterfaceObjetsStandard .
 
Des constantes permettent de d�signer les index du tableau vElement et sont d�crits dans le fichier Parametres.h.
 
Les p�riodes de la journ�e correspondent � des masques permettant de n'effectuer des calculs qu'entre deux heures de la journ�e (ex: fiche horaire de 10:00 � 12:00)
 
@attention Par convention, la p�riode portant l'index 0 d�crit la journ�e enti�re. Elle doit donc �tre d�crite obligatoirement par les donn�es pour que l'interface soit valide.
*/
class cInterface
{
    public:
        /** Période journée */
        static const size_t ALL_DAY_PERIOD;

    private:
        const size_t _id;         //!< Index de l'interface dans la base de donn�es SYNTHESE
        cInterface_Objet_AEvaluer_PageEcran* _Element;        //!< Tableau des �l�ments standard d�finis
        std::vector<synthese::time::HourPeriod*> _Periode;        //!< Tableau des p�riodes de la journ�e
        std::string _LibelleJourSemaine[ synthese::time::DAYS_PER_WEEK ]; //!< Tableau des libell�s des jours de semaine
        std::string _LibelleMois[ synthese::time::MONTHS_PER_YEAR + 1 ];   //!< Tableau des libell�s des mois
        cTableauDynamique<std::string> _PrefixesAlerte;      //!< Pr�fixes de messages d'alerte


    public:
        //! \name M�thode d'enregistrement
        //@{
        // bool     Enregistre(cObjetInterfaceStandard&, int);
        //@}

        //! \name Accesseurs
        //@{
        cInterface_Objet_AEvaluer_PageEcran& Element( int );
        const cInterface_Objet_AEvaluer_PageEcran& operator[] ( int ) const;
        const size_t& Index() const;
        const synthese::time::HourPeriod* GetPeriode( size_t __Index = ALL_DAY_PERIOD ) const;
        const std::string& getPrefixeAlerte( int __NiveauAlerte ) const;
        //  const std::string&        LibelleJourSemaine(int)       const;
        //@}

        //! \name Calculateurs
        //@{
        bool OK() const;

        /** Affichage d'une date selon les param�tres de l'interface
         @param __Date Date � afficher
         @param __Textuel true pour obtenir une date en texte, false pour l'obtenir au format num�rique (d�faut = true)
         @param __JourDeSemaine true pour afficher le jour de la semaine (d�faut = true)
         @param __Annee true pour afficher l'ann�e (d�faut = true)
        */
        void AfficheDate( std::ostream& __Flux, 
			  const synthese::time::Date& __Date, 
			  bool __Textuel = true, 
			  bool __JourDeSemaine = true, 
			  bool __Annee = true ) const
        {
            if ( __JourDeSemaine && __Textuel )
                __Flux << _LibelleJourSemaine[ __Date.getWeekDay() ] << " ";
            __Flux << __Date.getDay();
            if ( __Textuel )
                __Flux << " " << _LibelleMois[ __Date.getMonth () ] << " ";
            else
                __Flux << "/" << __Date.getMonth ();
            if ( __Annee )
            {
                if ( __Textuel )
                    __Flux << " ";
                else
                    __Flux << "/";
                __Flux << __Date.getYear ();
            }
        }
        //@}

        //! \name Modificateurs
        //@{
        void AddPeriode( synthese::time::HourPeriod* );
        bool SetLibelleJour( int, const std::string& );
        bool SetLibelleMois( int, const std::string& );
        bool SetPrefixeAlerte( int, const std::string& );
        //@}

        cInterface( const size_t& );
        ~cInterface();
};


/** @} */


#endif
