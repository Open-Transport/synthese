/***************************************************************************
 * msc.c : The message sequence parser ADT.
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
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "msc.h"

/***************************************************************************
 * Structures
 ***************************************************************************/

struct MscEntityTag
{
    char                *label;
    struct MscAttribTag *attr;
    struct MscEntityTag *next;
};

struct MscEntityListTag
{
    unsigned int         elements;
    struct MscEntityTag *head, *tail;
};

struct MscArcTag
{
    char                *src, *dst;
    MscArcType           type;
    struct MscAttribTag *attr;
    struct MscArcTag    *next;
};

struct MscArcListTag
{
    unsigned int         elements;
    struct MscArcTag    *head, *tail;
};


struct MscAttribTag
{
    MscAttribType       type;
    char                *value;
    struct MscAttribTag *next;
};

struct MscOptTag
{
    MscOptType          type;
    char               *value;
    struct MscOptTag   *next;
};

struct MscTag
{
    struct MscOptTag        *optList;
    struct MscEntityListTag *entityList;
    struct MscArcListTag    *arcList;

    struct MscArcTag        *nextArc;
    struct MscEntityTag     *nextEntity;
};


/***************************************************************************
 * Option Functions
 ***************************************************************************/

/* Allocate some option and set it's value.
 */
struct MscOptTag *MscAllocOpt(MscOptType  type,
                              char       *value)
{
    struct MscOptTag *a = malloc(sizeof(struct MscOptTag));

    a->type  = type;
    a->value = value;
    a->next  = NULL;

    return a;
}

/* Link one or two options together.
 */
struct MscOptTag *MscLinkOpt(struct MscOptTag *head,
                             struct MscOptTag *newHead)
{
    struct MscOptTag *tail = newHead;

    assert(newHead);

    /* Find the end of the list */
    while(tail->next)
    {
        tail = tail->next;
    }

    tail->next = head;
    return newHead;
}

/* MscPrettyOptType
 *  Returns a string that is the human readable form of the option
 *  code passed to the function.
 */
const char *MscPrettyOptType(MscOptType t)
{
    switch(t)
    {
        case MSC_OPT_HSCALE: return "hscale";
        default:
            return "unknown";
    }
}

struct MscOptTag *MscFindOpt(struct MscOptTag *list,
                             MscOptType        type)
{
    struct MscOptTag *elem = list;

    while(elem && elem->type != type)
    {
        printf("%p: %s=%s\n", elem, MscPrettyOptType(elem->type), elem->value);
        elem = elem->next;
    }

    if(elem && elem->type == type)
    {
        return elem;
    }

    return NULL;
}

void MscPrintOptList(struct MscOptTag *list)
{
    struct MscOptTag *elem = list;

    while(elem)
    {
        printf("%p: %s=%s\n", elem, MscPrettyOptType(elem->type), elem->value);
        elem = elem->next;
    }
}

/***************************************************************************
 * Entity Functions
 ***************************************************************************/

/* MscAllocEntity
 *  Allocate some entity and set it's name.
 */
struct MscEntityTag *MscAllocEntity(char *entityName)
{
    struct MscEntityTag *e = malloc(sizeof(struct MscEntityListTag));

    e->label = entityName;
    e->attr  = NULL;
    e->next  = NULL;

    return e;
}


/* MscLinkEntity
 *  Link some entity onto a list, possibly producing a new head element.
 */
struct MscEntityListTag *MscLinkEntity(struct MscEntityListTag *list,
                                       struct MscEntityTag     *elem)
{
    /* Check if the list has been allocated or not */
    if(list == NULL)
    {
        list = calloc(sizeof(struct MscEntityListTag), 1);

    }

    /* Check for an empty list */
    if(list->head == NULL)
    {
        list->head = list->tail = elem;
    }
    else
    {
        /* Add to tail */
        list->tail->next = elem;
        list->tail = elem;
    }

    /* Increment count of elements */
    list->elements++;

    return list;
}


void MscPrintEntityList(struct MscEntityListTag *list)
{
    struct MscEntityTag *elem = list->head;

    while(elem)
    {
        printf("%p: %s\n", elem, elem->label);
        MscPrintAttrib(elem->attr);
        elem = elem->next;
    }
}

/***************************************************************************
 * Arc Functions
 ***************************************************************************/

/* MscAllocArc
 *  Allocate an arc, filling in the src and dst entities.
 */
struct MscArcTag *MscAllocArc(char       *srcEntity,
                              char       *dstEntity,
                              MscArcType  type)
{
    struct MscArcTag *a = malloc(sizeof(struct MscArcTag));

    /* A discontinuity arcs are not between entities */
    if(type == MSC_ARC_DISCO)
    {
        assert(srcEntity == NULL && dstEntity == NULL);
    }

    a->src  = srcEntity;
    a->dst  = dstEntity;
    a->type = type;
    a->next = NULL;
    a->attr = NULL;

    return a;
}


/* MscLinkArc
 *  Link some entity onto a list, possibly producing a new head element.
 */
struct MscArcListTag *MscLinkArc(struct MscArcListTag *list,
                                 struct MscArcTag     *elem)
{
    /* Check if the list has been allocated or not */
    if(list == NULL)
    {
        list = calloc(sizeof(struct MscArcListTag), 1);

    }

    /* Check for an empty list */
    if(list->head == NULL)
    {
        list->head = list->tail = elem;
    }
    else
    {
        /* Add to tail */
        list->tail->next = elem;
        list->tail = elem;
    }

    /* Increment count of elements */
    list->elements++;

    return list;
}


/* MscPrintArcList
 *  Dump and arc list.
 */
void MscPrintArcList(struct MscArcListTag *list)
{
    struct MscArcTag *elem = list->head;

    while(elem)
    {
        printf("%p: '%s' -> '%s'\n", elem, elem->src, elem->dst);
        MscPrintAttrib(elem->attr);

        elem = elem->next;
    }
}


/***************************************************************************
 * Attribute functions
 ***************************************************************************/

/* MscAllocAttrib
 *  Allocate some attribute.
 */
struct MscAttribTag *MscAllocAttrib(MscAttribType  type,
                                    char          *value)
{
    struct MscAttribTag *a = malloc(sizeof(struct MscAttribTag));

    a->type  = type;
    a->value = value;
    a->next  = NULL;

    return a;
}


/* MscLinkAttrib
 *  Link some attributes.  The ordering of attributes is semi-important
 *  so the list is grown from the tail.
 */
struct MscAttribTag *MscLinkAttrib(struct MscAttribTag *head,
                                   struct MscAttribTag *newHead)
{
    struct MscAttribTag *tail = newHead;

    assert(newHead);

    /* Find the end of the list */
    while(tail->next)
    {
        tail = tail->next;
    }

    tail->next = head;
    return newHead;
}


/* MscArcLinkAttrib
 *  Attach some attributes to some arc.
 */
void MscArcLinkAttrib(struct MscArcTag    *arc,
                      struct MscAttribTag *att)
{
    if(arc->attr)
    {
        arc->attr = MscLinkAttrib(arc->attr, att);
    }
    else
    {
        arc->attr = att;
    }
}


/* MscEntityLinkAttrib
 *  Attach some attributes to some entity.
 */
void MscEntityLinkAttrib(struct MscEntityTag *ent,
                         struct MscAttribTag *att)
{
    if(ent->attr)
    {
        ent->attr = MscLinkAttrib(ent->attr, att);
    }
    else
    {
        ent->attr = att;
    }
}


/* MscPrintAttrib
 *  String a human readable version of the passed attribute list to stdout.
 */
void MscPrintAttrib(const struct MscAttribTag *att)
{
    while(att)
    {
        printf("  %s = %s\n", MscPrettyAttribType(att->type), att->value);
        att = att->next;
    }

}


/* MscPrettyAttribType
 *  Returns a string that is the human readable form of the attribute
 *  code passed to the function.
 */
const char *MscPrettyAttribType(MscAttribType t)
{
    switch(t)
    {
        case MSC_ATTR_LABEL: return "label";
        case MSC_ATTR_URL:   return "url";
        case MSC_ATTR_ID:    return "id";
        case MSC_ATTR_IDURL: return "idurl";
        default:
            return "unknown";
    }
}


/***************************************************************************
 * MSC Functions
 ***************************************************************************/

struct MscTag *MscAlloc(struct MscOptTag        *optList,
                        struct MscEntityListTag *entityList,
                        struct MscArcListTag    *arcList)
{
    struct MscTag *m = malloc(sizeof(struct MscTag));

    /* Copy the lists */
    m->optList    = optList;
    m->entityList = entityList;
    m->arcList    = arcList;

    /* Reset the iterators */
    MscResetEntityIterator(m);
    MscResetArcIterator(m);

    return m;
}

void MscPrint(struct MscTag *m)
{
    printf("Option list (%d options)\n", MscGetNumOpts(m));
    MscPrintOptList(m->optList);

    printf("Entity list (%d entities)\n", MscGetNumEntities(m));
    MscPrintEntityList(m->entityList);

    printf("\nArc list (%d arcs)\n", MscGetNumArcs(m));
    MscPrintArcList(m->arcList);
}

unsigned int MscGetNumEntities(struct MscTag *m)
{
    return m->entityList->elements;
}

unsigned int MscGetNumArcs(Msc m)
{
    return m->arcList->elements;
}

unsigned int MscGetNumOpts(Msc m)
{
    struct MscOptTag *elem  = m->optList;
    unsigned int      count = 0;

    while(elem)
    {
        count++;
        elem = elem->next;
    }

    return count;
}

int MscGetEntityIndex(struct MscTag *m, const char *label)
{
    struct MscEntityTag *entity = m->entityList->head;
    int                  c      = 0;

    assert(label);

    while(entity != NULL && strcmp(entity->label, label) != 0)
    {
        entity = entity->next;
        c++;
    }

    if(entity == NULL)
    {
        return -1;
    }
    else
    {
        return c;
    }
}

void MscResetEntityIterator(struct MscTag *m)
{
    m->nextEntity = m->entityList->head;
}

Boolean MscNextEntity(struct MscTag *m)
{
    if(m->nextEntity->next != NULL)
    {
        m->nextEntity = m->nextEntity->next;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

const char *MscGetCurrentEntAttrib(struct MscTag *m, MscAttribType a)
{
    struct MscAttribTag *attr;

    if(!m->nextEntity)
    {
       return NULL;
    }

    attr = m->nextEntity->attr;

    while(attr != NULL && attr->type != a)
    {
        attr = attr->next;
    }

    if(attr != NULL)
    {
        return attr->value;
    }
    else if(a == MSC_ATTR_LABEL)
    {
        return m->nextEntity->label;
    }
    else
    {
        return NULL;
    }

}

void MscResetArcIterator(struct MscTag *m)
{
    m->nextArc    = m->arcList->head;
}

Boolean MscNextArc(struct MscTag *m)
{
    if(m->nextArc->next != NULL)
    {
        m->nextArc = m->nextArc->next;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

const char *MscGetCurrentArcSource(struct MscTag *m)
{
    return m->nextArc ? m->nextArc->src : NULL;
}

const char *MscGetCurrentArcDest(struct MscTag *m)
{
    return m->nextArc ? m->nextArc->dst : NULL;
}

MscArcType MscGetCurrentArcType(struct MscTag *m)
{
    return m->nextArc ? m->nextArc->type : MSC_INVALID_ARC_TYPE;
}

const char *MscGetCurrentArcAttrib(struct MscTag *m, MscAttribType a)
{
    struct MscAttribTag *attr;

    if(!m->nextArc)
    {
       return NULL;
    }

    attr = m->nextArc->attr;

    while(attr != NULL && attr->type != a)
    {
        attr = attr->next;
    }

    if(attr != NULL)
    {
        return attr->value;
    }
    else
    {
        return NULL;
    }

}

Boolean MscGetOptAsFloat(struct MscTag *m, MscOptType type, float *f)
{
    struct MscOptTag *opt = MscFindOpt(m->optList, type);

    if(opt != NULL)
    {
        *f = (float)atof(opt->value);
        return *f != 0.0f;
    }

    return FALSE;
}

/* END OF FILE */
