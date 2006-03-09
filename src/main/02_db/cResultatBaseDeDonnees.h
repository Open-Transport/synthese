
#ifndef SYNTHESE_CRESULTATBASEDEDONNEES_H
#define SYNTHESE_CRESULTATBASEDEDONNEES_H

#include "cResultatBaseDeDonnees_Cellule.h"
#include "Parametres.h"
#include <map>
#include "cTableauDynamique.h"


typedef cTableauDynamiquePointeurs<cResultatBaseDeDonnees_Cellule*> tResultatBaseDeDonnees_Enregistrement;
typedef std::map<char*, int> tPositionChamps;

/** @ingroup m02 */
class cResultatBaseDeDonnees
{
    protected:
        tPositionChamps _PositionChamps; //!< Tableau de positionnement des champs dans les enregistrements
        cTableauDynamiqueObjets<tResultatBaseDeDonnees_Enregistrement> _Donnees;    //!< Donn�es r�sultat
        int _EnregistrementCourant; //!< Enregistrement � lire par d�faut

    public:
        //! \name Acc�s aux donn�es
        //@{
        int getNombre ( const std::string& __Champ, int __Index = INCONNU );
        std::string getTexte ( const std::string& __Champ, int __Index = INCONNU );
        synthese::time::DateTime getMoment ( const std::string& __Champ, int __Index = INCONNU );
        tBool3 getBool ( const std::string& __Champ, int __Index = INCONNU );
        bool GoPremier();
        bool GoSuivant();
        //@}

        //! \name Autres accesseurs
        //@{
        int Taille() const;
        const std::map<char*, int>& getPositionsChamps() const;
        //@}

        //! \name Modificateurs
        //@{
        void Vide();
        bool SetPositionChamp( char* __NomChamp, int __Position );
        bool AddEnregistrement();
        bool SetValeur( int __Position, cResultatBaseDeDonnees_Cellule* __Cellule );
        //@}

        cResultatBaseDeDonnees() { Vide(); }
};



#endif
