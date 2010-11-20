/*

 shapefiles.c -- implements shapefile support [import - export]

 version 2.3, 2008 October 13

 Author: Sandro Furieri a.furieri@lqt.it

 -----------------------------------------------------------------------------
 
 Version: MPL 1.1/GPL 2.0/LGPL 2.1
 
 The contents of this file are subject to the Mozilla Public License Version
 1.1 (the "License"); you may not use this file except in compliance with
 the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/
 
Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the
License.

The Original Code is the SpatiaLite library

The Initial Developer of the Original Code is Alessandro Furieri
 
Portions created by the Initial Developer are Copyright (C) 2008
the Initial Developer. All Rights Reserved.

Contributor(s):

Alternatively, the contents of this file may be used under the terms of
either the GNU General Public License Version 2 or later (the "GPL"), or
the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
in which case the provisions of the GPL or the LGPL are applicable instead
of those above. If you wish to allow use of your version of this file only
under the terms of either the GPL or the LGPL, and not to allow others to
use your version of this file under the terms of the MPL, indicate your
decision by deleting the provisions above and replace them with the notice
and other provisions required by the GPL or the LGPL. If you do not delete
the provisions above, a recipient may use your version of this file under
the terms of any one of the MPL, the GPL or the LGPL.
 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <spatialite/sqlite3.h>
#include <spatialite/gaiaaux.h>
#include <spatialite/gaiageo.h>
#include <spatialite.h>

#if defined(_WIN32) && !defined(__MINGW32__)
#define strcasecmp	_stricmp
#endif

static gaiaDbfFieldPtr
getDbfField (gaiaDbfListPtr list, char *name)
{
/* find a DBF attribute by name */
    gaiaDbfFieldPtr fld = list->First;
    while (fld)
      {
	  if (strcasecmp (fld->Name, name) == 0)
	      return fld;
	  fld = fld->Next;
      }
    return NULL;
}

SPATIALITE_DECLARE int
load_shapefile (sqlite3 * sqlite, char *shp_path, char *table, char *charset,
		int srid, char *column, int verbose, int *rows)
{
    sqlite3_stmt *stmt;
    int ret;
    char *errMsg = NULL;
    char sql[65536];
    char dummyName[4096];
    int already_exists = 0;
    int metadata = 0;
    int sqlError = 0;
    gaiaShapefilePtr shp = NULL;
    gaiaDbfFieldPtr dbf_field;
    int cnt;
    int col_cnt;
    int seed;
    int len;
    int dup;
    int idup;
    int current_row;
    char **col_name = NULL;
    unsigned char *blob;
    int blob_size;
    char *geom_type;
    char *geo_column = column;
    if (!geo_column)
	geo_column = "Geometry";
/* checking if TABLE already exists */
    sprintf (sql,
	     "SELECT name FROM sqlite_master WHERE type = 'table' AND name LIKE '%s'",
	     table);
    ret = sqlite3_prepare_v2 (sqlite, sql, strlen (sql), &stmt, NULL);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "load shapefile error: <%s>\n",
		   sqlite3_errmsg (sqlite));
	  return 0;
      }
    while (1)
      {
	  /* scrolling the result set */
	  ret = sqlite3_step (stmt);
	  if (ret == SQLITE_DONE)
	      break;		/* end of result set */
	  if (ret == SQLITE_ROW)
	      already_exists = 1;
	  else
	    {
		fprintf (stderr, "load shapefile error: <%s>\n",
			 sqlite3_errmsg (sqlite));
		break;
	    }
      }
    sqlite3_finalize (stmt);
    if (already_exists)
      {
	  fprintf (stderr, "load shapefile error: table '%s' already exists\n",
		   table);
	  return 0;
      }
/* checking if MetaData GEOMETRY_COLUMNS exists */
    strcpy (sql,
	    "SELECT name FROM sqlite_master WHERE type = 'table' AND name = 'geometry_columns'");
    ret = sqlite3_prepare_v2 (sqlite, sql, strlen (sql), &stmt, NULL);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "load shapefile error: <%s>\n",
		   sqlite3_errmsg (sqlite));
	  return 0;
      }
    while (1)
      {
	  /* scrolling the result set */
	  ret = sqlite3_step (stmt);
	  if (ret == SQLITE_DONE)
	      break;		/* end of result set */
	  if (ret == SQLITE_ROW)
	      metadata = 1;
	  else
	    {
		fprintf (stderr, "load shapefile error: <%s>\n",
			 sqlite3_errmsg (sqlite));
		break;
	    }
      }
    sqlite3_finalize (stmt);
    shp = gaiaAllocShapefile ();
    gaiaOpenShpRead (shp, shp_path, charset, "UTF-8");
    if (!(shp->Valid))
      {
	  fprintf (stderr, "load shapefile error: cannot open shapefile '%s'\n",
		   shp_path);
	  if (shp->LastError)
	      fprintf (stderr, "\tcause: %s\n", shp->LastError);
	  gaiaFreeShapefile (shp);
	  return 0;
      }
/* checking for duplicate / illegal column names and antialising them */
    col_cnt = 0;
    dbf_field = shp->Dbf->First;
    while (dbf_field)
      {
	  /* counting DBF fields */
	  col_cnt++;
	  dbf_field = dbf_field->Next;
      }
    col_name = malloc (sizeof (char *) * col_cnt);
    cnt = 0;
    seed = 0;
    dbf_field = shp->Dbf->First;
    while (dbf_field)
      {
	  /* preparing column names */
	  strcpy (dummyName, dbf_field->Name);
	  dup = 0;
	  for (idup = 0; idup < cnt; idup++)
	    {
		if (strcasecmp (dummyName, *(col_name + idup)) == 0)
		    dup = 1;
	    }
	  if (strcasecmp (dummyName, "PK_UID") == 0)
	      dup = 1;
	  if (strcasecmp (dummyName, geo_column) == 0)
	      dup = 1;
	  if (dup)
	      sprintf (dummyName, "COL_%d", seed++);
	  len = strlen (dummyName);
	  *(col_name + cnt) = malloc (len + 1);
	  strcpy (*(col_name + cnt), dummyName);
	  cnt++;
	  dbf_field = dbf_field->Next;
      }
    if (verbose)
	fprintf (stderr,
		 "========\nLoading shapefile at '%s' into SQLite table '%s'\n",
		 shp_path, table);
/* starting a transaction */
    if (verbose)
	fprintf (stderr, "\nBEGIN;\n");
    ret = sqlite3_exec (sqlite, "BEGIN", NULL, 0, &errMsg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "load shapefile error: <%s>\n", errMsg);
	  sqlite3_free (errMsg);
	  sqlError = 1;
	  goto clean_up;
      }
/* creating the Table */
    sprintf (sql, "CREATE TABLE %s", table);
    strcat (sql, " (\nPK_UID INTEGER PRIMARY KEY AUTOINCREMENT");
    cnt = 0;
    dbf_field = shp->Dbf->First;
    while (dbf_field)
      {
	  strcat (sql, ",\n");
	  strcat (sql, *(col_name + cnt));
	  cnt++;
	  switch (dbf_field->Type)
	    {
	    case 'C':
		strcat (sql, " TEXT");
		break;
	    case 'N':
		fflush (stderr);
		if (dbf_field->Decimals)
		    strcat (sql, " DOUBLE");
		else
		  {
		      if (dbf_field->Length <= 18)
			  strcat (sql, " INTEGER");
		      else
			  strcat (sql, " DOUBLE");
		  }
		break;
	    case 'D':
		strcat (sql, " DOUBLE");
		break;
	    case 'F':
		strcat (sql, " DOUBLE");
		break;
	    case 'L':
		strcat (sql, " INTEGER");
		break;
	    };
	  dbf_field = dbf_field->Next;
      }
    if (metadata)
	strcat (sql, ")");
    else
      {
	  strcat (sql, ",\n");
	  strcat (sql, geo_column);
	  strcat (sql, " BLOB)");
      }
    if (verbose)
	fprintf (stderr, "%s;\n", sql);
    ret = sqlite3_exec (sqlite, sql, NULL, 0, &errMsg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "load shapefile error: <%s>\n", errMsg);
	  sqlite3_free (errMsg);
	  sqlError = 1;
	  goto clean_up;
      }
    if (metadata)
      {
	  /* creating Geometry column */
	  switch (shp->Shape)
	    {
	    case 1:
	    case 11:
	    case 21:
		geom_type = "POINT";
		break;
	    case 8:
		geom_type = "MULTIPOINT";
		break;
	    case 3:
	    case 13:
	    case 23:
		gaiaShpAnalyze (shp);
		if (shp->EffectiveType == GAIA_LINESTRING)
		    geom_type = "LINESTRING";
		else
		    geom_type = "MULTILINESTRING";
		break;
	    case 5:
	    case 15:
	    case 25:
		gaiaShpAnalyze (shp);
		if (shp->EffectiveType == GAIA_POLYGON)
		    geom_type = "POLYGON";
		else
		    geom_type = "MULTIPOLYGON";
		break;
	    };
	  sprintf (sql, "SELECT AddGeometryColumn('%s', '%s', %d, '%s', 2)",
		   table, geo_column, srid, geom_type);
	  if (verbose)
	      fprintf (stderr, "%s;\n", sql);
	  ret = sqlite3_exec (sqlite, sql, NULL, 0, &errMsg);
	  if (ret != SQLITE_OK)
	    {
		fprintf (stderr, "load shapefile error: <%s>\n", errMsg);
		sqlite3_free (errMsg);
		sqlError = 1;
		goto clean_up;
	    }
      }
    else
      {
	  /* no Metadata */
	  if (shp->Shape == 3 || shp->Shape == 13 || shp->Shape == 23 ||
	      shp->Shape == 5 || shp->Shape == 15 || shp->Shape == 25)
	    {
		/* fixing anyway the Geometry type for LINESTRING/MULTILINESTRING or POLYGON/MULTIPOLYGON */
		gaiaShpAnalyze (shp);
	    }
      }
    /* preparing the INSERT INTO parametrerized statement */
    sprintf (sql, "INSERT INTO %s (PK_UID,", table);
    cnt = 0;
    dbf_field = shp->Dbf->First;
    while (dbf_field)
      {
	  /* columns corresponding to some DBF attribute */
	  strcat (sql, *(col_name + cnt++));
	  strcat (sql, ",");
	  dbf_field = dbf_field->Next;
      }
    strcat (sql, geo_column);	/* the GEOMETRY column */
    strcat (sql, ")\nVALUES (?");
    dbf_field = shp->Dbf->First;
    while (dbf_field)
      {
	  /* column values */
	  strcat (sql, ", ?");
	  dbf_field = dbf_field->Next;
      }
    strcat (sql, ", ?)");	/* the GEOMETRY column */
    ret = sqlite3_prepare_v2 (sqlite, sql, strlen (sql), &stmt, NULL);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "load shapefile error: <%s>\n",
		   sqlite3_errmsg (sqlite));
	  sqlError = 1;
	  goto clean_up;
      }
    current_row = 0;
    while (1)
      {
	  /* inserting rows from shapefile */
	  ret = gaiaReadShpEntity (shp, current_row, srid);
	  if (!ret)
	    {
		if (!(shp->LastError))	/* normal SHP EOF */
		    break;
		fprintf (stderr, "%s\n", shp->LastError);
		sqlError = 1;
		goto clean_up;
	    }
	  current_row++;
	  /* binding query params */
	  sqlite3_reset (stmt);
	  sqlite3_clear_bindings (stmt);
	  sqlite3_bind_int (stmt, 1, current_row);
	  cnt = 0;
	  dbf_field = shp->Dbf->First;
	  while (dbf_field)
	    {
		/* column values */
		if (!(dbf_field->Value))
		    sqlite3_bind_null (stmt, cnt + 2);
		else
		  {
		      switch (dbf_field->Value->Type)
			{
			case GAIA_INT_VALUE:
			    sqlite3_bind_int64 (stmt, cnt + 2,
						dbf_field->Value->IntValue);
			    break;
			case GAIA_DOUBLE_VALUE:
			    sqlite3_bind_double (stmt, cnt + 2,
						 dbf_field->Value->DblValue);
			    break;
			case GAIA_TEXT_VALUE:
			    sqlite3_bind_text (stmt, cnt + 2,
					       dbf_field->Value->TxtValue,
					       strlen (dbf_field->Value->
						       TxtValue),
					       SQLITE_STATIC);
			    break;
			default:
			    sqlite3_bind_null (stmt, cnt + 2);
			    break;
			}
		  }
		cnt++;
		dbf_field = dbf_field->Next;
	    }
	  if (shp->Dbf->Geometry)
	    {
		gaiaToSpatiaLiteBlobWkb (shp->Dbf->Geometry, &blob, &blob_size);
		sqlite3_bind_blob (stmt, cnt + 2, blob, blob_size, free);
	    }
	  else
	    {
		/* handling a NULL-Geometry */
		sqlite3_bind_null (stmt, cnt + 2);
	    }
	  ret = sqlite3_step (stmt);
	  if (ret == SQLITE_DONE || ret == SQLITE_ROW)
	      ;
	  else
	    {
		fprintf (stderr, "load shapefile error: <%s>\n",
			 sqlite3_errmsg (sqlite));
		sqlite3_finalize (stmt);
		sqlError = 1;
		goto clean_up;
	    }
      }
    sqlite3_finalize (stmt);
  clean_up:
    gaiaFreeShapefile (shp);
    if (col_name)
      {
	  /* releasing memory allocation for column names */
	  for (cnt = 0; cnt < col_cnt; cnt++)
	      free (*(col_name + cnt));
	  free (col_name);
      }
    if (sqlError)
      {
	  /* some error occurred - ROLLBACK */
	  if (verbose)
	      fprintf (stderr, "ROLLBACK;\n");
	  ret = sqlite3_exec (sqlite, "ROLLBACK", NULL, 0, &errMsg);
	  if (ret != SQLITE_OK)
	    {
		fprintf (stderr, "load shapefile error: <%s>\n", errMsg);
		sqlite3_free (errMsg);
	    }
	  if (rows)
	      *rows = current_row;
	  return 0;
      }
    else
      {
	  /* ok - confirming pending transaction - COMMIT */
	  if (verbose)
	      fprintf (stderr, "COMMIT;\n");
	  ret = sqlite3_exec (sqlite, "COMMIT", NULL, 0, &errMsg);
	  if (ret != SQLITE_OK)
	    {
		fprintf (stderr, "load shapefile error: <%s>\n", errMsg);
		sqlite3_free (errMsg);
		return 0;
	    }
	  if (rows)
	      *rows = current_row;
	  if (verbose)
	      fprintf (stderr,
		       "\nInserted %d rows into '%s' from SHAPEFILE\n========\n",
		       current_row, table);
	  return 1;
      }
}

SPATIALITE_DECLARE int
dump_shapefile (sqlite3 * sqlite, char *table, char *column, char *shp_path,
		char *charset, char *geom_type, int verbose, int *xrows)
{
/* SHAPEFILE dump */
    char sql[1024];
    char dummy[1024];
    int shape = -1;
    int len;
    int ret;
    char *errMsg = NULL;
    sqlite3_stmt *stmt;
    int row1 = 0;
    int n_cols = 0;
    int offset = 0;
    int i;
    int rows = 0;
    int type;
    const unsigned char *char_value;
    const void *blob_value;
    gaiaShapefilePtr shp = NULL;
    gaiaDbfListPtr dbf_export_list = NULL;
    gaiaDbfListPtr dbf_list = NULL;
    gaiaDbfListPtr dbf_write;
    gaiaDbfFieldPtr dbf_field;
    gaiaGeomCollPtr geom;
    int *max_length = NULL;
    int *sql_type = NULL;
    if (geom_type)
      {
	  /* normalizing required geometry type */
	  if (strcasecmp ((char *) geom_type, "POINT") == 0)
	      shape = GAIA_POINT;
	  if (strcasecmp ((char *) geom_type, "LINESTRING") == 0)
	      shape = GAIA_LINESTRING;
	  if (strcasecmp ((char *) geom_type, "POLYGON") == 0)
	      shape = GAIA_POLYGON;
	  if (strcasecmp ((char *) geom_type, "MULTIPOINT") == 0)
	      shape = GAIA_POINT;
      }
    if (shape < 0)
      {
	  /* preparing SQL statement [no type was explicitly required, so we'll read GEOMETRY_COLUMNS */
	  char **results;
	  int rows;
	  int columns;
	  int i;
	  char metatype[256];
	  sprintf (sql,
		   "SELECT type FROM geometry_columns WHERE f_table_name = '%s' AND f_geometry_column = '%s'",
		   table, column);
	  ret =
	      sqlite3_get_table (sqlite, sql, &results, &rows, &columns,
				 &errMsg);
	  if (ret != SQLITE_OK)
	    {
		fprintf (stderr, "dump shapefile MetaData error: <%s>\n",
			 errMsg);
		sqlite3_free (errMsg);
		return 0;
	    }
	  for (i = 1; i <= rows; i++)
	      strcpy (metatype, results[(i * columns)]);
	  sqlite3_free_table (results);
	  if (strcasecmp ((char *) metatype, "POINT") == 0)
	      shape = GAIA_POINT;
	  if (strcasecmp ((char *) metatype, "MULTIPOINT") == 0)
	      shape = GAIA_MULTIPOINT;
	  if (strcasecmp ((char *) metatype, "LINESTRING") == 0
	      || strcasecmp ((char *) metatype, "MULTILINESTRING") == 0)
	      shape = GAIA_LINESTRING;
	  if (strcasecmp ((char *) metatype, "POLYGON") == 0
	      || strcasecmp ((char *) metatype, "MULTIPOLYGON") == 0)
	      shape = GAIA_POLYGON;
      }
    if (shape < 0)
      {
	  fprintf (stderr,
		   "Unable to detect GeometryType for \"%s\".\"%s\" ... sorry\n",
		   table, column);
	  return 0;
      }
    if (verbose)
	fprintf (stderr,
		 "========\nDumping SQLite table '%s' into shapefile at '%s'\n",
		 table, shp_path);
    /* preparing SQL statement */
    sprintf (sql, "SELECT * FROM \"%s\" WHERE GeometryType(\"%s\") = ", table,
	     column);
    if (shape == GAIA_LINESTRING)
      {
	  strcat (sql, "'LINESTRING' OR GeometryType(\"");
	  strcat (sql, (char *) column);
	  strcat (sql, "\") = 'MULTILINESTRING'");
      }
    else if (shape == GAIA_POLYGON)
      {
	  strcat (sql, "'POLYGON' OR GeometryType(\"");
	  strcat (sql, (char *) column);
	  strcat (sql, "\") = 'MULTIPOLYGON'");
      }
    else if (shape == GAIA_MULTIPOINT)
      {
	  strcat (sql, "'POINT' OR GeometryType(\"");
	  strcat (sql, (char *) column);
	  strcat (sql, "\") = 'MULTIPOINT'");
      }
    else
	strcat (sql, "'POINT'");
/* fetching anyway NULL Geometries */
    strcat (sql, " OR \"");
    strcat (sql, (char *) column);
    strcat (sql, "\" IS NULL");
/* compiling SQL prepared statement */
    ret = sqlite3_prepare_v2 (sqlite, sql, strlen (sql), &stmt, NULL);
    if (ret != SQLITE_OK)
	goto sql_error;
    while (1)
      {
	  /* Pass I - scrolling the result set to compute real DBF attributes' sizes and types */
	  ret = sqlite3_step (stmt);
	  if (ret == SQLITE_DONE)
	      break;		/* end of result set */
	  if (ret == SQLITE_ROW)
	    {
		/* processing a result set row */
		row1++;
		if (n_cols == 0)
		  {
		      /* this one is the first row, so we are going to prepare the DBF Fields list */
		      n_cols = sqlite3_column_count (stmt);
		      dbf_export_list = gaiaAllocDbfList ();
		      max_length = malloc (sizeof (int) * n_cols);
		      sql_type = malloc (sizeof (int) * n_cols);
		      for (i = 0; i < n_cols; i++)
			{
			    /* initializes the DBF export fields */
			    strcpy (dummy, sqlite3_column_name (stmt, i));
			    gaiaAddDbfField (dbf_export_list, dummy, '\0', 0, 0,
					     0);
			    max_length[i] = 0;
			    sql_type[i] = SQLITE_NULL;
			}
		  }
		for (i = 0; i < n_cols; i++)
		  {
		      /* update the DBF export fields analyzing fetched data */
		      type = sqlite3_column_type (stmt, i);
		      if (type == SQLITE_NULL || type == SQLITE_BLOB)
			  continue;
		      if (type == SQLITE_TEXT)
			{
			    char_value = sqlite3_column_text (stmt, i);
			    len = sqlite3_column_bytes (stmt, i);
			    sql_type[i] = SQLITE_TEXT;
			    if (len > max_length[i])
				max_length[i] = len;
			}
		      else if (type == SQLITE_FLOAT
			       && sql_type[i] != SQLITE_TEXT)
			  sql_type[i] = SQLITE_FLOAT;	/* promoting a numeric column to be DOUBLE */
		      else if (type == SQLITE_INTEGER &&
			       (sql_type[i] == SQLITE_NULL
				|| sql_type[i] == SQLITE_INTEGER))
			  sql_type[i] = SQLITE_INTEGER;	/* promoting a null column to be INTEGER */
		  }
	    }
	  else
	      goto sql_error;
      }
    if (!row1)
	goto empty_result_set;
    i = 0;
    offset = 0;
    dbf_list = gaiaAllocDbfList ();
    dbf_field = dbf_export_list->First;
    while (dbf_field)
      {
	  /* preparing the final DBF attribute list */
	  if (sql_type[i] == SQLITE_NULL)
	    {
		i++;
		dbf_field = dbf_field->Next;
		continue;
	    }
	  if (sql_type[i] == SQLITE_TEXT)
	    {
		gaiaAddDbfField (dbf_list, dbf_field->Name, 'C', offset,
				 max_length[i], 0);
		offset += max_length[i];
	    }
	  if (sql_type[i] == SQLITE_FLOAT)
	    {
		gaiaAddDbfField (dbf_list, dbf_field->Name, 'N', offset, 24, 6);
		offset += 24;
	    }
	  if (sql_type[i] == SQLITE_INTEGER)
	    {
		gaiaAddDbfField (dbf_list, dbf_field->Name, 'N', offset, 18, 0);
		offset += 18;
	    }
	  i++;
	  dbf_field = dbf_field->Next;
      }
    free (max_length);
    free (sql_type);
    gaiaFreeDbfList (dbf_export_list);
/* resetting SQLite query */
    if (verbose)
	fprintf (stderr, "\n%s;\n", sql);
    ret = sqlite3_reset (stmt);
    if (ret != SQLITE_OK)
	goto sql_error;
/* trying to open shapefile files */
    shp = gaiaAllocShapefile ();
    gaiaOpenShpWrite (shp, shp_path, shape, dbf_list, "UTF-8", charset);
    if (!(shp->Valid))
	goto no_file;
    while (1)
      {
	  /* Pass II - scrolling the result set to dump data into shapefile */
	  ret = sqlite3_step (stmt);
	  if (ret == SQLITE_DONE)
	      break;		/* end of result set */
	  if (ret == SQLITE_ROW)
	    {
		rows++;
		geom = NULL;
		dbf_write = gaiaCloneDbfEntity (dbf_list);
		for (i = 0; i < n_cols; i++)
		  {
		      if (strcasecmp
			  ((char *) column,
			   (char *) sqlite3_column_name (stmt, i)) == 0)
			{
			    /* this one is the internal BLOB encoded GEOMETRY to be exported */
			    if (sqlite3_column_type (stmt, i) != SQLITE_BLOB)
			      {
				  /* this one is a NULL Geometry */
				  dbf_write->Geometry = NULL;
			      }
			    else
			      {
				  blob_value = sqlite3_column_blob (stmt, i);
				  len = sqlite3_column_bytes (stmt, i);
				  dbf_write->Geometry =
				      gaiaFromSpatiaLiteBlobWkb (blob_value,
								 len);
			      }
			}
		      strcpy (dummy, sqlite3_column_name (stmt, i));
		      dbf_field = getDbfField (dbf_write, dummy);
		      if (!dbf_field)
			  continue;
		      if (sqlite3_column_type (stmt, i) == SQLITE_NULL)
			{
			    /* handling NULL values */
			    gaiaSetNullValue (dbf_field);
			}
		      else
			{
			    switch (dbf_field->Type)
			      {
			      case 'N':
				  if (sqlite3_column_type (stmt, i) ==
				      SQLITE_INTEGER)
				      gaiaSetIntValue (dbf_field,
						       sqlite3_column_int64
						       (stmt, i));
				  else if (sqlite3_column_type (stmt, i) ==
					   SQLITE_FLOAT)
				      gaiaSetDoubleValue (dbf_field,
							  sqlite3_column_double
							  (stmt, i));
				  else
				      gaiaSetNullValue (dbf_field);
				  break;
			      case 'C':
				  if (sqlite3_column_type (stmt, i) ==
				      SQLITE_TEXT)
				    {
					strcpy (dummy,
						(char *)
						sqlite3_column_text (stmt, i));
					gaiaSetStrValue (dbf_field, dummy);
				    }
				  else
				      gaiaSetNullValue (dbf_field);
				  break;
			      };
			}
		  }
		if (!gaiaWriteShpEntity (shp, dbf_write))
		    fprintf (stderr, "shapefile write error\n");
		gaiaFreeDbfList (dbf_write);
	    }
	  else
	      goto sql_error;
      }
    sqlite3_finalize (stmt);
    gaiaFlushShpHeaders (shp);
    gaiaFreeShapefile (shp);
    if (verbose)
	fprintf (stderr, "\nExported %d rows into SHAPEFILE\n========\n", rows);
    if (xrows)
	*xrows = rows;
    return 1;
  sql_error:
/* some SQL error occurred */
    sqlite3_finalize (stmt);
    if (dbf_export_list)
	gaiaFreeDbfList (dbf_export_list);
    if (dbf_list)
	gaiaFreeDbfList (dbf_list);
    if (shp)
	gaiaFreeShapefile (shp);
    fprintf (stderr, "SELECT failed: %s", sqlite3_errmsg (sqlite));
    return 0;
  no_file:
/* shapefile can't be created/opened */
    if (dbf_export_list)
	gaiaFreeDbfList (dbf_export_list);
    if (dbf_list)
	gaiaFreeDbfList (dbf_list);
    if (shp)
	gaiaFreeShapefile (shp);
    fprintf (stderr, "ERROR: unable to open '%s' for writing", shp_path);
    return 0;
  empty_result_set:
/* the result set is empty - nothing to do */
    sqlite3_finalize (stmt);
    if (dbf_export_list)
	gaiaFreeDbfList (dbf_export_list);
    if (dbf_list)
	gaiaFreeDbfList (dbf_list);
    if (shp)
	gaiaFreeShapefile (shp);
    fprintf (stderr,
	     "The SQL SELECT returned an empty result set ... there is nothing to export ...");
    return 0;
}
