/*! \file Point.h
\brief En-tête classe cPoint
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


/*!  \brief Point géographique
 \author Hugues Romain
 \date 2000-2003
 
 Les coordonnées géographiques Lambert II sont stockées sous forme de deux entiers : l'un pour la partie "kilométrique" de la coordonnées, l'autre pour la partie "métrique" ( \f$ x=x_{km}*1000+x_m \f$ et \f$ y=y_{km}*1000+y_m \f$)
 
 Le but de cette séparation est de permettre des calculs plus rapides lorsque la précision métrique n'est pas nécessaire, en se cantonnant à des calculs sur la partie kilométrique.
*/
class cPoint
{
        friend class cDistanceCarree;

        //! \name Coordonnées Lambert II
        //@{
        CoordonneeKM _XKM; //!< Longitude en kilomètres, arrondie au kilomètre inférieur
        CoordonneeKM _YKM; //!< Latitude en kilomètres, arrondie au kilomètre inférieur
        CoordonneeM _XM;  //!< Reste de l'arrondi au kilomètre de la longitude, en mètres
        CoordonneeM _YM;  //!< Reste de l'arrondi au kilomètre de la latitude, en mètres
        //@}

    public:

        //!\name Opérateurs
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

