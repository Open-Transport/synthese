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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "gd.h"
#include "gdfontt.h"  /* Tiny font */
#include "gdfonts.h"  /* Small font */
#include "adraw_int.h"

/***************************************************************************
 * Local types
 ***************************************************************************/

typedef struct GdoContextTag
{
    gdImagePtr  img;
    gdFontPtr   font;

    int         colour[NUM_ADRAW_COL];
    int         pen;

    FILE       *outFile;
}
GdoContext;

/***************************************************************************
 * Helper functions
 ***************************************************************************/


/** Get the context pointer from an ADraw structure.
 */
static GdoContext *getGdoCtx(struct ADrawTag *ctx)
{
    return (GdoContext *)ctx->internal;
}


/** Get the GD image pointer from an ADraw structure.
 */
static gdImagePtr getGdoImg(struct ADrawTag *ctx)
{
    return getGdoCtx(ctx)->img;
}

/** Get the current GD pen index from an ADraw structure.
 */
static int getGdoPen(struct ADrawTag *ctx)
{
    return getGdoCtx(ctx)->pen;
}

/** Set the dashed style.
 */
static void setStyle(struct ADrawTag *ctx)
{
    const GdoContext *context = getGdoCtx(ctx);
    int               style[4];

    /* Create dash pattern */
    style[0] = style[1] = context->pen;
    style[2] = style[3] = context->colour[ADRAW_COL_WHITE];

    gdImageSetStyle(context->img, style, 4);
}

/***************************************************************************
 * API Functions
 ***************************************************************************/

unsigned int gdoTextWidth(struct ADrawTag *ctx,
                          const char *string)
{
    /* Remove 1 pixel since there is usually an uneven gap at
     *  the right of the last character for the fixed width
     *  font.
     */
    return (getGdoCtx(ctx)->font->w * strlen(string)) - 1;
}


int gdoTextHeight(struct ADrawTag *ctx)
{
    return getGdoCtx(ctx)->font->h;
}


void gdoLine(struct ADrawTag *ctx,
             unsigned int     x1,
             unsigned int     y1,
             unsigned int     x2,
             unsigned int     y2)
{
    gdImageLine(getGdoImg(ctx),
                x1, y1, x2, y2, getGdoPen(ctx));
}

void gdoDottedLine(struct ADrawTag *ctx,
                   unsigned int     x1,
                   unsigned int     y1,
                   unsigned int     x2,
                   unsigned int     y2)
{
    setStyle(ctx);
    gdImageLine(getGdoImg(ctx), x1, y1, x2, y2, gdStyled);
}


void gdoTextR(struct ADrawTag *ctx,
              unsigned int     x,
              unsigned int     y,
              const char      *string)
{
    GdoContext *context = getGdoCtx(ctx);

    gdImageFilledRectangle(getGdoImg(ctx),
                           x,
                           y - (gdoTextHeight(ctx) - 1),
                           x + gdoTextWidth(ctx, string),
                           y - 3,
                           context->colour[ADRAW_COL_WHITE]);

    gdImageString(getGdoImg(ctx),
                  getGdoCtx(ctx)->font,
                  x,
                  y - gdoTextHeight(ctx),
                  (char *)string,
                  getGdoPen(ctx));
}


void gdoTextL(struct ADrawTag *ctx,
              unsigned int     x,
              unsigned int     y,
              const char      *string)
{
    gdoTextR(ctx, x - gdoTextWidth(ctx, string), y, string);
}

void gdoTextC(struct ADrawTag *ctx,
              unsigned int     x,
              unsigned int     y,
              const char      *string)
{
    gdoTextR(ctx, x - (gdoTextWidth(ctx, string) / 2), y, string);
}

void gdoFilledTriangle(struct ADrawTag *ctx,
                       unsigned int x1,
                       unsigned int y1,
                       unsigned int x2,
                       unsigned int y2,
                       unsigned int x3,
                       unsigned int y3)
{
    gdPoint p[3];

    p[0].x = x1; p[0].y = y1;
    p[1].x = x2; p[1].y = y2;
    p[2].x = x3; p[2].y = y3;

    gdImageFilledPolygon(getGdoImg(ctx), p, 3, getGdoPen(ctx));
}


void gdoArc(struct ADrawTag *ctx,
            unsigned int cx,
            unsigned int cy,
            unsigned int w,
            unsigned int h,
            unsigned int s,
            unsigned int e)
{
    gdImageArc(getGdoImg(ctx), cx, cy, w, h, s, e, getGdoPen(ctx));
}


void gdoDottedArc(struct ADrawTag *ctx,
                  unsigned int cx,
                  unsigned int cy,
                  unsigned int w,
                  unsigned int h,
                  unsigned int s,
                  unsigned int e)
{
    setStyle(ctx);
    gdImageArc(getGdoImg(ctx), cx, cy, w, h, s, e, gdStyled);
}


void gdoSetPen (struct ADrawTag *ctx,
                ADrawColour      col)
{
    GdoContext *context = getGdoCtx(ctx);

    context->pen = context->colour[col];
}


void gdoSetFontSize(struct ADrawTag *ctx,
                    ADrawFontSize    size)
{
    switch(size)
    {
        case ADRAW_FONT_TINY:
            getGdoCtx(ctx)->font = gdFontGetTiny();
            break;

        case ADRAW_FONT_SMALL:
            getGdoCtx(ctx)->font = gdFontGetSmall();
            break;

        default:
            assert(0);
    }
}


Boolean gdoClose(struct ADrawTag *ctx)
{
    GdoContext *context = getGdoCtx(ctx);

    /* Output the image to the disk file in PNG format. */
    gdImagePng(getGdoImg(ctx), context->outFile);
    fclose(context->outFile);

    /* Destroy the image in memory */
    gdImageDestroy(context->img);

    /* Free and destroy context */
    free(context);
    ctx->internal = NULL;

    return TRUE;
}



Boolean GdoInit(unsigned int     w,
                unsigned int     h,
                const char      *file,
                struct ADrawTag *outContext)
{
    GdoContext *context;

    /* Create context */
    context = outContext->internal = malloc(sizeof(GdoContext));
    if(context == NULL)
    {
        return FALSE;
    }

    /* Open the output file */
    context->outFile = fopen(file, "wb");
    if(!context->outFile)
    {
        fprintf(stderr, "GdoInit: Failed to open output file '%s':\n", file);
        perror(NULL);
        return FALSE;
    }

    /* Allocate the image */
    context->img = gdImageCreate(w, h);

    /* Get the colours */
    context->colour[ADRAW_COL_WHITE] = gdImageColorAllocate(context->img, 255, 255, 255); /* Background */
    context->colour[ADRAW_COL_BLACK] = gdImageColorAllocate(context->img, 0, 0, 0);
    context->colour[ADRAW_COL_BLUE]  = gdImageColorAllocate(context->img, 0, 0, 255);

    /* Set pen colour to black */
    context->pen = context->colour[ADRAW_COL_BLACK];

    /* Get the default font */
    context->font = gdFontGetSmall();

    /* Now fill in the function pointers */
    outContext->line           = gdoLine;
    outContext->dottedLine     = gdoDottedLine;
    outContext->textL          = gdoTextL;
    outContext->textC          = gdoTextC;
    outContext->textR          = gdoTextR;
    outContext->textWidth      = gdoTextWidth;
    outContext->textHeight     = gdoTextHeight;
    outContext->filledTriangle = gdoFilledTriangle;
    outContext->arc            = gdoArc;
    outContext->dottedArc      = gdoDottedArc;
    outContext->setPen         = gdoSetPen;
    outContext->setFontSize    = gdoSetFontSize;
    outContext->close          = gdoClose;

    return TRUE;
}

/* END OF FILE */
