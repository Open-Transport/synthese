
#include "cTexteSQL.h"

/*! \brief Conversion d'un caractère standard en caractère compatible SQL (simulation si pas de chaîne fournie)
 \param c Caractère à convertir
 \retval Tampon char* où écrire le résultat (NULL = pas d'écriture)
 \return Longueur de la chaîne convertie
 \author Hugues Romain
 \date 2005
 \todo Terminer l'implémentation des cas les plus courants
*/
int cTexteSQL::ConvertChar( const char c, char* Tampon ) const
{
    switch ( c )
    {
        case '\0':
            if ( Tampon )
                strncpy( Tampon, "\\0", 2 );
            return ( 2 );
        case '\n':
            if ( Tampon )
                strncpy( Tampon, "\\n", 2 );
            return ( 2 );
        case '\r':
            if ( Tampon )
                strncpy( Tampon, "\\r", 2 );
            return ( 2 );
        case '\\':
            if ( Tampon )
                strncpy( Tampon, "\\\\", 2 );
            return ( 2 );
        case '\"':
            if ( Tampon )
                strncpy( Tampon, "'", 1 );
            return ( 1 );
        case '\032':
            if ( Tampon )
                strncpy( Tampon, "\\Z;", 2 );
            return ( 2 );
        case ';':
            if ( Tampon )
                * Tampon = ',';
            return ( 1 );
        default:
            if ( Tampon )
                * Tampon = c;
            return ( 1 );
    }
    return 1;
}
