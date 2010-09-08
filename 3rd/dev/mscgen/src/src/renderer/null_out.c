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


#include <stdio.h>
#include <assert.h>

#include "adraw_int.h"

/***************************************************************************
 * API Functions
 ***************************************************************************/

static unsigned int NullTextWidth(struct ADrawTag *ctx,
                                  const char *string)
{
    return 0;
}


static int NullTextHeight(struct ADrawTag *ctx)
{
    return 0;
}


static void NullLine(struct ADrawTag *ctx,
                     unsigned int     x1,
                     unsigned int     y1,
                     unsigned int     x2,
                     unsigned int     y2)
{
}

static void NullDottedLine(struct ADrawTag *ctx,
                           unsigned int     x1,
                           unsigned int     y1,
                           unsigned int     x2,
                           unsigned int     y2)
{
}


static void NullTextR(struct ADrawTag *ctx,
                      unsigned int     x,
                      unsigned int     y,
                      const char      *string)
{
}


static void NullTextL(struct ADrawTag *ctx,
                      unsigned int     x,
                      unsigned int     y,
                      const char      *string)
{
}

static void NullTextC(struct ADrawTag *ctx,
                      unsigned int     x,
                      unsigned int     y,
                      const char      *string)
{
}

static void NullFilledTriangle(struct ADrawTag *ctx,
                               unsigned int x1,
                               unsigned int y1,
                               unsigned int x2,
                               unsigned int y2,
                               unsigned int x3,
                               unsigned int y3)
{
}


static void NullArc(struct ADrawTag *ctx,
                    unsigned int cx,
                    unsigned int cy,
                    unsigned int w,
                    unsigned int h,
                    unsigned int s,
                    unsigned int e)
{
}


static void NullDottedArc(struct ADrawTag *ctx,
                          unsigned int cx,
                          unsigned int cy,
                          unsigned int w,
                          unsigned int h,
                          unsigned int s,
                          unsigned int e)
{
}


static void NullSetPen (struct ADrawTag *ctx,
                        ADrawColour      col)
{
}


static void NullSetFontSize(struct ADrawTag *ctx,
                            ADrawFontSize    size)
{
}


static Boolean NullClose(struct ADrawTag *ctx)
{
    return TRUE;
}



Boolean NullInit(struct ADrawTag *outContext)
{
    /* Fill in the function pointers */
    outContext->line           = NullLine;
    outContext->dottedLine     = NullDottedLine;
    outContext->textL          = NullTextL;
    outContext->textC          = NullTextC;
    outContext->textR          = NullTextR;
    outContext->textWidth      = NullTextWidth;
    outContext->textHeight     = NullTextHeight;
    outContext->filledTriangle = NullFilledTriangle;
    outContext->arc            = NullArc;
    outContext->dottedArc      = NullDottedArc;
    outContext->setPen         = NullSetPen;
    outContext->setFontSize    = NullSetFontSize;
    outContext->close          = NullClose;

    return TRUE;
}

/* END OF FILE */
