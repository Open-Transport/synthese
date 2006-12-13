/*! \file Point.h
\brief En-t�te classe cPoint
\author Hugues Romain
\date 2000-2003
*/

#ifndef SYNTHESE_POINT_H
#define SYNTHESE_POINT_H

#include "Parametres.h"

typedef short int CoordonneeKM;
typedef short int CoordonneeM;
typedef long int DistanceCarreeKM;
typedef long int CoordonneeKMM;


/*!  \brief Point g�ographique
 \author Hugues Romain
 \date 2000-2003
 
 Les coordonn�es g�ographiques Lambert II sont stock�es sous forme de deux entiers : l'un pour la partie "kilom�trique" de la coordonn�es, l'autre pour la partie "m�trique" ( \f$ x=x_{km}*1000+x_m \f$ et \f$ y=y_{km}*1000+y_m \f$)
 
 Le but de cette s�paration est de permettre des calculs plus rapides lorsque la pr�cision m�trique n'est pas n�cessaire, en se cantonnant � des calculs sur la partie kilom�trique.
*/
class cPoint
{
        friend class cDistanceCarree;

        //! \name Coordonn�es Lambert II
        //@{
        CoordonneeKM _XKM; //!< Longitude en kilom�tres, arrondie au kilom�tre inf�rieur
        CoordonneeKM _YKM; //!< Latitude en kilom�tres, arrondie au kilom�tre inf�rieur
        CoordonneeM _XM;  //!< Reste de l'arrondi au kilom�tre de la longitude, en m�tres
        CoordonneeM _YM;  //!< Reste de l'arrondi au kilom�tre de la latitude, en m�tres
        //@}

    public:

        //!\name Op�rateurs
        //@{
        int operator== ( const cPoint& ) const;
        //@}

        //! \name Accesseurs
        //@{
        bool unknownLocation() const;
        CoordonneeKMM XKMM() const;
        CoordonneeKMM YKMM() const;
        //@}

        //! \name Constructeur
        //@{
        cPoint();
        virtual ~cPoint();
        //@}

        //! \name Modificateurs
        //@{
        void setX( const CoordonneeKMM );
        void setY( const CoordonneeKMM );
        //@}
};



#endif

