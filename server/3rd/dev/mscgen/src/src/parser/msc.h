/***************************************************************************
 * msc.h : The message sequence parser API.
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

#if !defined(MSC_H)
#define MSC_H

#include "bool.h"

/***************************************************************************
 * Types
 ***************************************************************************/

/** Msc Options.
 */
typedef enum MscOptTypeTag
{
    MSC_OPT_HSCALE
}
MscOptType;

/** Arc attributes.
 * An arc may have one or more attributes listed in square brackets after
 * the declaration.  This gives an enumerated type for each permissible
 * attribute.
 */
typedef enum MscAttribTypeTag
{
    MSC_ATTR_LABEL,
    MSC_ATTR_ID,
    MSC_ATTR_URL,
    MSC_ATTR_IDURL
}
MscAttribType;


typedef enum
{
    MSC_ARC_METHOD,
    MSC_ARC_RETVAL,
    MSC_ARC_SIGNAL,
    MSC_ARC_CALLBACK,
    MSC_ARC_DISCO,      /* ... Discontinuity in time line */
    MSC_ARC_DIVIDER,    /* --- Divider */

    MSC_INVALID_ARC_TYPE
}
MscArcType;


/***************************************************************************
 * Abstract types
 ***************************************************************************/

typedef struct MscTag           *Msc;

typedef struct MscOptTag        *MscOpt;

typedef struct MscEntityTag     *MscEntity;

typedef struct MscEntityListTag *MscEntityList;

typedef struct MscArcTag        *MscArc;

typedef struct MscArcListTag    *MscArcList;

typedef struct MscAttribTag     *MscAttrib;


/***************************************************************************
 * MSC Building Functions
 ***************************************************************************/

/** Parse some input to build a message sequence chart.
 * This will parse characters from \a in and build a message sequence chart
 * ADT.
 * \retval Msc  The message sequence chart, which may equal \a NULL is a
 *               parse error occurred.
 */
Msc           MscParse(FILE *in);

MscEntity     MscAllocEntity(char *entityName);

MscEntityList MscLinkEntity(MscEntityList list, MscEntity elem);

void          MscPrintEntityList(MscEntityList list);

MscOpt        MscAllocOpt(MscOptType  type,
                          char       *value);

MscOpt        MscLinkOpt(MscOpt head,
                         MscOpt newHead);

MscArc        MscAllocArc(char      *srcEntity,
                          char      *dstEntity,
                          MscArcType type);

MscArcList    MscLinkArc (MscArcList list,
                          MscArc     elem);

void          MscPrintArcList(struct MscArcListTag *list);

MscAttrib     MscAllocAttrib(MscAttribType  type,
                             char          *value);

MscAttrib     MscLinkAttrib(MscAttrib head,
                            MscAttrib newHead);

void          MscArcLinkAttrib(MscArc    arc,
                               MscAttrib att);

void          MscEntityLinkAttrib(MscEntity ent,
                                  MscAttrib att);

void          MscPrintAttrib(const struct MscAttribTag *att);

const char   *MscPrettyAttribType(MscAttribType t);

Msc           MscAlloc(MscOpt        optList,
                       MscEntityList entityList,
                       MscArcList    arcList);

/** Print the passed msc in textual form to stdout.
 * This prints a human readable format of the parsed msc to stdout.  This
 * is primarily of use in debugging the parser.
 */
void          MscPrint(Msc m);

unsigned int  MscGetNumEntities(Msc m);

unsigned int  MscGetNumArcs(Msc m);

unsigned int  MscGetNumOpts(Msc m);

/** Get an MSC option, returning the value as a float.
 *
 * \param[in]     m      The MSC to analyse.
 * \param[in]     type   The option type to retrieve.
 * \param[in,out] f      Pointer to be filled with parsed value.
 * \retval TRUE  If the option was found and parsed successfully.
 */
Boolean       MscGetOptAsFloat(struct MscTag *m, MscOptType type, float *f);

/** Get the index of some entity.
 * This returns the column index for the entity identified by the passed
 * label.
 *
 * \param  m      The MSC to analyse.
 * \param  label  The label to find.
 * \retval -1     If the label was not found, otherwise the columnn index.
 */
int           MscGetEntityIndex(struct MscTag *m, const char *label);

/***************************************************************************
 * Entity processing functions
 ***************************************************************************/

/** \defgroup EntityFuncs  Entity handling functions
 * @{
 */

/** Reset the entity interator.
 * This moves the pointer to the current entity to the head of the list.
 */
void          MscResetEntityIterator(Msc m);

/** Move to the next entity in the MSC.
 * \retval TRUE if there is another entity, otherwise FALSE if the end of the
 *          list has been reached.
 */
Boolean       MscNextEntity(struct MscTag *m);

/** Get the value of some attribute for the current entity.
 * \retval The attribute string, or NULL if unset.
 */
const char   *MscGetCurrentEntAttrib(Msc m, MscAttribType a);

/** @} */

/***************************************************************************
 * Arc processing functions
 ***************************************************************************/

/** \defgroup ArcFuncs  Arc handling functions
 * @{
 */

/** Reset the arc interator.
 * This moves the pointer to the current arc to the head of the list.
 */
void          MscResetArcIterator   (Msc m);

/** Move to the next arc in the MSC.
 * \retval TRUE if there is another arc, otherwise FALSE if the end of the
 *          list has been reached.
 */
Boolean       MscNextArc(struct MscTag *m);


/** Get the name of the entity from which the current arc originates.
 * \retvat The label for the entity from which the current arc starts.
 *          The returned string must not be modified.
 */
const char   *MscGetCurrentArcSource(Msc m);


/** Get the name of the entity at which the current arc terminates.
 * \retval The label for the entity at which the current arc stops.
 *          The returned string must not be modified.
 */
const char   *MscGetCurrentArcDest(Msc m);

/** Get the type for some arc.
 *
 */
MscArcType    MscGetCurrentArcType(struct MscTag *m);

/** Get the value of some attribute for the current arc.
 * \retval The attribute string, or NULL if unset.
 */
const char   *MscGetCurrentArcAttrib(Msc m, MscAttribType a);

/** @} */

#endif

/* END OF FILE */
