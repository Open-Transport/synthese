/***************************************************************************
 * This file is part of mscgen, a message sequence chart renderer.
 * Copyright (C) 2005 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 **************************************************************************/

#if !defined(GD_OUT_H)
#define      GD_OUT_H

#include "adraw.h"

Boolean NullInit(struct ADrawTag *outContext);

Boolean GdoInit(unsigned int     w,
                unsigned int     h,
                const char      *file,
                struct ADrawTag *outContext);

Boolean PsInit(unsigned int     w,
               unsigned int     h,
               const char      *file,
               struct ADrawTag *outContext);

Boolean SvgInit(unsigned int     w,
                unsigned int     h,
                const char      *file,
                struct ADrawTag *outContext);

#endif

/* END OF FILE */
