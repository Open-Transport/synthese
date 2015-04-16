/***************************************************************************
 * parsertest.c : Test function for the msc parser.
 *
 * This file is part of msclib.
 *
 * Msc is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * Msclib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with msclib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 ***************************************************************************/


#include <stdio.h>
#include "msc.h"

int main()
{
    Msc m = MscParse(stdin);

    if(m)
    {
        MscPrint(m);
        return 0;
    }
    else
    {
        fprintf(stderr, "Parse failed!\n");
        return 1;
    }

}

/* END OF FILE */
