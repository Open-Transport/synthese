/****************************************************************************/
/*********    CDBFile, a dBase III / .DBF file handling object    ***********/
/****************************************************************************/
/*
  
    Copyright (C) 1997 Hervé GOURMELON, ENSSAT
    gourmelon@enssat.fr
    http://www.enssat.fr

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library (COPYING2.TXT); if not, write to the
Free Software Foundation, Inc., 675 Mass Ave, Cambridge,
MA 02139, USA.

*/

// If you are building a Windoze MFC app under Visual C++, remove the comment 
// slashes in front of the following line:
//#include "stdafx.h"
// and turn the following lines into comments:

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cdbfile.h" 

namespace synthese
{
	namespace util
	{

/* Implementation of the member functions for CField		*/

// Default constructor
CField::CField()
{	Name[0]='\0';
	Type='C';
	Length=0;
	DecCount=0;
	FieldNumber=0;
	Offset=0;
	Next=this;
}


// Constructor with an initialization of most data members
CField::CField(char* NName,char NType,unsigned char NLength,
				unsigned char NDecCount,unsigned char FieldNum)
{	
	strcpy(Name, NName);
	Type=NType;
	Length=NLength;
	DecCount=NDecCount;
	FieldNumber=FieldNum;
}


// Destructor
CField::~CField()
{}


unsigned short CField::GetNumberOfFields(CField* Start, unsigned short i)
{
	if (Start==NULL) 
		{	Start=this;		// NULL is passed for the initial call of
			i=1;
			return Next->GetNumberOfFields(Start, i);
		}		// this  recursive function in the program 

	else 
		if (Start==this) return i;	// We've checked all the elements of	
		else						//	the dynamic ring: return i */
		{	i+=1;
			return Next->GetNumberOfFields(Start, i); 
			/* Proceed to next element */
		}
}

	
	
/* Implementation of the member functions of CDBFile		*/

/* public : */
// Default constructor
CDBFile::CDBFile()	// Classical init procedure
{
	PathName[0]=0;
	FileHandle=NULL;
	HeaderSize=0;
	FieldCount=0;
	RecordCount=0;
	ModifiedFlag=false;
	FullFileInMemory=false;
	RecordLength=0;

	RecordList=NULL;
	FirstField=NULL;
}

CDBFile::CDBFile(char* Path)  // Creating the object with a path as argument
// Automatically opens the file
{
if (OpenFile(Path)==false) Path[0]=0;
}


bool CDBFile::Clean() 	
// Resets the CDBFile object, regardless of any modifications to be written
{
CField *Delendum;
Record *DelRec;
	
	if (FileHandle!=NULL) 
			fclose(FileHandle); // Closes the file by default
	// Resets all the member data
	PathName[0]=0;
	FileHandle=NULL;
	HeaderSize=0;
	FieldCount=0;
	RecordCount=0;
	ModifiedFlag=false;
	FullFileInMemory=false;
	RecordLength=0;

	if (FirstField!=NULL)	// Delete the dynamic list of fields
	{	
		while (FirstField->GetNext()!=FirstField)
		{	Delendum=FirstField->GetNext();
			FirstField->SetNext(Delendum->GetNext());
			delete Delendum;
		}
		delete FirstField;
		FirstField=NULL;
	}

	if (RecordList!=NULL)	// Delete the dynamic list of records
	{	
		while (RecordList->Next!=NULL)
		{	DelRec=RecordList->Next;
			RecordList->Next=DelRec->Next;
			delete[] DelRec->Contents;
			delete DelRec;
		}
		delete[] RecordList->Contents; 
		delete RecordList;
		RecordList=NULL;
	}
	return true;

}

CDBFile::~CDBFile()	//	Default destructor
{
	
	Clean();
}


void CDBFile::ClearAllRecords()
// Cleaning the contents before rewriting the file (without changing the
// data structure)
{
	Record* DelRec;

	if (FileHandle==NULL) return;
	RecordCount=0;
	if (RecordList!=NULL)	// Delete the dynamic list of records
	{	
		while (RecordList->Next!=NULL)
		{	DelRec=RecordList->Next;
			RecordList->Next=DelRec->Next;
			delete[] DelRec->Contents;
			delete DelRec;
		}
		delete[] RecordList->Contents; 
		delete RecordList;
		RecordList=NULL;
	}
}	

bool CDBFile::OpenFile(char* Path)
{

unsigned char dBaseVersion,NLength,NDecCount;
char NType, NName[11];
unsigned short Offst;
CField *Tail, *Current;

	// Please refer to the dBase III file structure to debug
	// this procedure

	Clean();   // Closes the previous active file and resets the object

	if (strlen(Path)>256)
	{	fprintf (stderr, "File path too long : %s \n", Path);
		return false;
	}
	strcpy (PathName, Path);
	FileHandle = fopen (PathName, "r+");
	if (FileHandle==NULL) return false;	// error opening the file
	fseek (FileHandle, 0L, SEEK_SET);
	fread (&dBaseVersion, 1, sizeof (char), FileHandle);
	
	if (dBaseVersion != 3)	// typical of the dBase III files; ignores the 
							// case of a DBF file with an associated DBT file
	{
		fprintf (stderr, "%s is not a dBASE III file!\n", Path);
		return false;
	}
	
	// Reading the header:

	// I ignored the date of last udate. If anybody finds it necessary, just
	// add it to the member data and read it here...
	fseek (FileHandle, 4L, SEEK_SET);
	fread (&RecordCount, sizeof (RecordCount), 1, FileHandle);

	fseek (FileHandle, 8L, SEEK_SET);
	fread (&HeaderSize, sizeof (HeaderSize), 1, FileHandle);
	FieldCount = HeaderSize / 32 - 1;
	fread (&RecordLength, sizeof (RecordLength), 1, FileHandle);

	Offst=1;	// Data records are preceded by one byte that is 0x20 (space)
	
	// Reading the field descriptor arrays:

	for (unsigned char i=1; i<=FieldCount; i++) 
	{
		fseek (FileHandle, (long) 32 * i, SEEK_SET);
		fread (NName, 11, sizeof (char), FileHandle);
		fread (&NType, 1, sizeof (char), FileHandle);
		fseek (FileHandle, (long)(32*i+16), SEEK_SET);
		fread (&NLength, 1, sizeof (unsigned char), FileHandle);
		fread (&NDecCount, 1, sizeof (unsigned char), FileHandle);
		Current=new CField(NName,NType,NLength,NDecCount,i);
		if (FirstField==NULL)	// Creating the list of CFields
		{
			FirstField=Current;
			Current->SetNext(FirstField);
			Tail=FirstField;
		}
		else
		{
			Tail->SetNext(Current);
			Tail=Current;
			Current->SetNext(FirstField);
		}// end if
		Current->SetOffset(Offst);
		Offst+=NLength;
	}// end for

return true;
}


bool CDBFile::CloseFile()	// If anybody finds any use to it...  
{							
	return Clean();
}		  

		
unsigned long CDBFile::LoadFileToMemory()
// Loads all the records to memory. Necessary when you have to sort them or to
// delete some of them from the file, or to save it as another file.
{
	unsigned long i;
	Record *Current;

	// If the file is closed or the records are already loaded, return.
	if ((FileHandle==NULL)||(FullFileInMemory==true)) return false;
	else
	{
		for (i=1;i<=RecordCount;i++)
		{
			Current=ReadRecord(i);
			Append(Current);	// This will overwrite the records with the
			// same numbers, unless their ModifFlag is set to true
			// (see the Append function)
			if (Current==NULL) return i-1;
		}
 		FullFileInMemory=true;
		return i;
	}
}		 	


unsigned long CDBFile::WriteAllToFile(char* Path)
// Saves the file as "Path". If "Path"==NULL, the current file is overwritten.
// Returns the number of records actually written.
{
	unsigned long i;
	Record *CurRec;

	// If the file is closed or not all the records are loaded, return.
	if ((FileHandle==NULL)||(FullFileInMemory==false)) return 0L;
	else
	{
		if (WriteHeader(Path)==true)	// Writes the header of the file.
		{
			CurRec = RecordList;
			for (i=1;i<=RecordCount;i++)
			{
				if (WriteRecord(CurRec, i))
				{
					if (CurRec->Next==NULL) return i-1;
					else CurRec=CurRec->Next;
				}
				else return i-1;
			}// end for
			return i;
		}//end if WriteHeader
		else return 0L;
	}//end if
}//end


unsigned long CDBFile::WriteModified()
{
	unsigned long i=0;
	Record* CurRec;
	if ((FileHandle==NULL)||(FullFileInMemory==true)) return false;
 	else
	{	CurRec=RecordList;
		do
		{	if(CurRec->ModifFlag==true)
			{
				if (WriteRecord(CurRec, CurRec->RecordNumber)) i++;
				else return i;
			}
			CurRec=CurRec->Next;
		}while(CurRec!=NULL);
		return i;
	}
}
			

void CDBFile::SortAllRecords(Record *Head, Record *Tail, 
									CField* Criter1 /*, CField* Criter2*/)
// Warning : "Tail" must not be the last record in the list. There must be
// an empty cell following Tail in the list, which should be non-NULL. 
// The function below is an implementation of the quick sort algorithm.
{

Record  *i, *j, *s;
void *v1, *vj /*, *v2*/;

if ((Head->RecordNumber)<(Tail->RecordNumber))
{
	v1=GetFieldValue(Head, Criter1);
	i=Head;
	j=Head->Next;
	
	s=Tail->Next;

	while ((j->RecordNumber)<(s->RecordNumber))
	{
		// Sorting on primary criterium
		vj = GetFieldValue(j,Criter1);
		if (IsSmaller(vj, v1, Criter1))
		{
			Swap (i,j);
			i=i->Next;
			j=j->Next;
		}
		else if (IsBigger(vj, v1, Criter1))
			 {
			 	Swap(j, s->Previous);
			 	s=s->Previous;
			 }
	/* attempt to sort on a secundary criterium. Unfortunately, this method failed.
	Any idea ?
		 
		 	else if (Criter2!=NULL) 
			   {
			   		v2=GetFieldValue(Head, Criter2);	 
					if (IsSmaller(GetFieldValue(j,Criter2), v2, Criter2))
					{
						Swap (i,j);
						i=i->Next;
						j=j->Next;
					}
					else 
					if (IsBigger(GetFieldValue(j,Criter2), v2, Criter2))
			 		{
					 	Swap(j, s->Previous);
					 	s=s->Previous;
					}
				    else j=j->Next;	 	
			   } 			*/
			 else j=j->Next;
	DeleteVoidPointer(vj, Criter1);
	DeleteVoidPointer(v1, Criter1);

	}
	SortAllRecords(Head, i->Previous, Criter1/*, Criter2*/);
	SortAllRecords(s, Tail, Criter1/*, Criter2*/); 
}//end if
}

void CDBFile::SortOn(unsigned short Criterium1/* , unsigned short Criterium2*/)
// Calls SortAllRecords(); the criterium is specified by the field number 
{
	Record *Head, *Tail, *CurRec;
	CField *Crit1 /*, *Crit2*/;	/* Crit2 : optional secundary field */

	if (FullFileInMemory!=true) return;
	CurRec=RecordList;
	while (CurRec->Next!=NULL) CurRec=CurRec->Next;

	// Two additional records have to be appended temporarily to the list,
	// in order to execute the quick sort correctly.

	Tail=new Record;			Head=new Record;
	Head->Next=RecordList;		Tail->Previous=CurRec;
	RecordList->Previous=Head;	CurRec->Next=Tail;
	Head->Previous=NULL;		Tail->Next=NULL;
	Head->Contents=NULL;		Tail->Contents=NULL;
	Head->RecordNumber=0L;		Tail->RecordNumber=RecordCount+1;

	Crit1=FirstField->GetField(Criterium1);
	/*Crit2=FirstField->GetField(Criterium2);*/
	SortAllRecords(RecordList, CurRec, Crit1/*, Crit2*/);

	RecordList->Previous=NULL; 	CurRec->Next=NULL;
	delete Head; 				delete Tail;
}
	

void* CDBFile::GetFieldValue(Record* Rec, CField* Field)	
// Returns a pointer to the value of the field *Field in the record *Rec.
// Since we do not know the type of the value, we have to return a "void*"
// pointer, which has to be deleted by the recipient of the pointer.
// See also : SetFieldValue(), DeleteVoidPointer().
{
	char* Data;
	char* RecContents;

	RecContents = &(Rec->Contents[Field->GetOffset()]);
	// RecContents points at the beginning of the field within the record.
	Data=new char[Field->GetLength()+1];  // Data is allocated dynamically
	strncpy(Data, RecContents, Field->GetLength());
	// Data now contains the string that has to be converted.

	switch(Field->GetType())
	{
		case 'N':  // The field is of numeric type, either :
				if (Field->GetDecCount()==0)	// an integer (no decimals)
				{
					long *Result1;
					Result1=new long;
					*Result1=atol(Data);
					delete []Data;
					return (void*)Result1;
				}
				else							// a double
				{
					double *Result1;	
					Result1=new double;
					*Result1=atof(Data);
					delete []Data;
					return (void*)Result1;
				}
		case 'D': // I won't do any specific conversion for dates
				  // Please feel free to do it if you feel like.
		case 'C': // Here, we add a Clipper / FoxPro notion: the field decimal
				  // count is considered as an additional field length number.
				char* Result2;
				Result2=strncpy(Data, RecContents,
							 Field->GetLength()+256*Field->GetDecCount()+1);
				Result2[Field->GetLength()+256*Field->GetDecCount()]=0;
			
				return (void*)Result2;
		case 'L': // Here I decided that Logical values should be converted to
			      // booleans. I could have kept it as a single character.
				char c;
				bool* Result3;
				Result3=new bool;
				sscanf(Data, "%c", c);
				*Result3= (c=='Y')||(c=='y')||(c=='T')||(c=='t');
				delete []Data;
				return (void*)Result3;
		case 'M':
		default: 
				return NULL;
	}// end switch;
	
}	


// Overloaded, public versions of GetFieldValue. :
void* CDBFile::GetFieldValue(char* Field)
{return GetFieldValue(CurrentRec,FirstField->GetField(Field));}

void* CDBFile::GetFieldValue(unsigned short FieldNum)
{return GetFieldValue(CurrentRec,FirstField->GetField(FieldNum));}


void CDBFile::DeleteVoidPointer(void* Pointer, CField* Field)
// This function should be used to delete the void pointers allocated and
// returned by GetFieldValue(). It detects the type of the pointers.
{
	switch (Field->GetType())
	{
	case 'N' :
	case 'L' :
			delete Pointer;
			break;
	case 'D' :
	case 'C' :
	default	 :
			delete []Pointer;
	}
}


// Public overloaded versions of DeleteVoidPointer()
void CDBFile::DeleteVoidPointer(void* Pointer, unsigned short Field)
{DeleteVoidPointer(Pointer,FirstField->GetField(Field));} 

void CDBFile::DeleteVoidPointer(void* Pointer, char* Field)
{DeleteVoidPointer(Pointer,FirstField->GetField(Field));}


Record* CDBFile::GetRecord(unsigned long Number)
// This function returns a record identified by its number. This is for 
// records that are already loaded in memory.
{
	Record* Current;

	Current=RecordList;
	if (RecordList==NULL) return NULL;
	else
	{
		if (Current->RecordNumber==Number) return Current;
		else
		{
			while (Current->Next!=NULL)
			{
			Current=Current->Next;
			if (Current->RecordNumber==Number) return Current;
			}
			// The matching record was not found in memory : it
			// must be on the disk... Return the first record.
			return RecordList;		
		}
	}//end "if (RecordList==NULL)"
}



Record* CDBFile::CreateNewRecord()
// When an additional record is created, the number of records in the files is 
// incremented, the file is modified.
{
	Record* Current;

	Current=new Record;
	Current->Next=NULL;
	Current->Previous=NULL;
	Current->Contents=new char[RecordLength];
	for (int i=0; i<RecordLength; i++)
		Current->Contents[i]=0x20;
	Current->ModifFlag=false;
	Current->RecordNumber=RecordCount+1;
	RecordCount++;
	ModifiedFlag=true;
	return Current;	
}


void CDBFile::Append(Record* Rec, Record* Tail)
// Inserts Rec after Tail in the list if Tail!=NULL; inserts it in order
// of RecordNumber if Tail==NULL (which is the default) and if a record with 
// the same number is not already present in the list. In that case, the 
// contents of the former record is replaced by Rec->Contents, unless the 
// former record has been modified and is not saved yet.
{
	Record* Current;

	if (Tail==NULL)		// by default...
	{
		Current=RecordList;
		if (RecordList==NULL)  // The list is empty
		{
			RecordList=Rec;	   // Create a new list
			return;
		}
		while(Current!=NULL) 
		{
			if (Current->RecordNumber>Rec->RecordNumber)  
			// This is the right place to insert Rec
			{
				Rec->Next=Current;
				if (Current->Previous!=NULL) (Current->Previous)->Next=Rec;
				else RecordList=Rec;
				Rec->Previous=Current->Previous;
				Current->Previous=Rec;
				ModifiedFlag=true;
				return;
			}
			else if (Current->RecordNumber==Rec->RecordNumber)	
			{
				if(Current->ModifFlag==true)
				// Current has been modified, but not saved yet
				{
					delete Rec->Contents;
					delete Rec;
					return;
				}
				else
				// Current can be replaced by Rec without losing modifications
				{
				 	delete Current->Contents;
					Current->Contents=Rec->Contents;
					Current->ModifFlag=Rec->ModifFlag;
					delete Rec;
				}
				// The file has been modified (by adding this record:)
				ModifiedFlag=true;
				return;
			}
			else if (Current->Next==NULL)
			// This is the right place to append Rec
			{
				Rec->Previous=Current;
				Current->Next=Rec;
				Rec->Next=NULL;
				return;
			}//end if
			Current=Current->Next;
		}// end while	
	}// end if
	else 
	{	// Rec is to be inserted after Tail
		Current=Tail;
		Rec->Next=Current->Next;
		Rec->Previous=Current;
		if (Current->Next!=NULL) (Current->Next)->Previous=Rec;
		Current->Next=Rec;
	}
}//end Append

	
void CDBFile::DeleteRecord(Record* Rec)
{	// This function really deletes a record if all the records are loaded 
	// in memory and  if the whole data is saved to a file (current file 
	// or a new one). If only a few records are loaded, it will delete the
	// pointed record from the memory, discarding any changes.
	if (Rec==NULL) return;
	if (Rec==RecordList)
	{	RecordList=RecordList->Next;
		if (RecordList) RecordList->Previous=NULL;
		delete(Rec);
		if (FullFileInMemory==true) RecordCount--;
		ModifiedFlag=true;
		return;
	}
	else
	{
		(Rec->Previous)->Next=Rec->Next;
		if (Rec->Next) (Rec->Next)->Previous=Rec->Previous;
		delete(Rec);
		ModifiedFlag=true;
		if (FullFileInMemory==true) RecordCount--;
	}
}

//void CDBFile::AddField(CField* NewField);	// Not implemented in this version

void CDBFile::SetFieldValue(Record* Rec, CField* Field, void* Value)
// Here we have the same problems as in GetFieldValue(), see above. 
{
	char* Data;
	char* RecContents;
	unsigned short FLength;
	unsigned short ResLength;

	if (Field->GetType()=='C') 
			FLength=Field->GetLength()+256*Field->GetDecCount();
	else
		 	FLength=Field->GetLength();

	RecContents = &(Rec->Contents[Field->GetOffset()]);
	// RecContents points at the beginning of the field within the record.
	Data=new char[FLength+1]; // Data is allocated dynamically
	for (int i=0; i<FLength; i++) Data[i]=' ';	// Data is blanked
	Data[FLength]=0;  // Last character is set to null, '\0' (end of string)

	switch(Field->GetType())
	{
		case 'N':	// Numeric
				if (Field->GetDecCount()==0)	// Integer (long)
				{
					char *Num=new char[FLength+1];
					long *Result1;
					Result1=(long *)Value;
					sprintf(Num, "%d", *Result1);
					ResLength=strlen(Num);
					// Text alignment for numbers is flush-right :
					strncpy(&Data[FLength-ResLength], Num, ResLength);
					strncpy(RecContents, Data, FLength);
					delete []Num;
					break;
				}
				else 							// Float (double)
				{
					char *Num=new char[FLength+1];
					double *Result1;	
					Result1=(double *)Value;
					gcvt(*Result1, FLength,Num);
					ResLength=strlen(Num);
					// Text alignment for numbers is flush-right :
					strncpy(&Data[FLength-ResLength], Num, ResLength);
					strncpy(RecContents, Data, FLength);
					delete []Num;
					break;
				}
		case 'D':	// Date : no specific conversion (for portability)
		case 'C':	// Character :
				ResLength=strlen((char *)Value);
				strncpy(Data, (char *)Value, ResLength);
				strncpy(RecContents, Data, FLength);
				break;
		case 'L': 	// Logical : conversion from 'bool' type
				bool* Result3;
				Result3=(bool *)Value;
				if (*Result3==true) Data[0]='Y';
				else Data[0]='N';
				strncpy(RecContents, Data, FLength);
				break;
		case 'M':
		default: 
				break;
	}// end switch;
	delete []Data;
	Rec->ModifFlag=true;
}	


// Overloaded, public versions of SetFieldValue. :
void CDBFile::SetFieldValue(char* Field, void* Value)
{ SetFieldValue(CurrentRec,FirstField->GetField(Field), Value);}

void CDBFile::SetFieldValue(unsigned short FieldNum, void* Value)
{ SetFieldValue(CurrentRec,FirstField->GetField(FieldNum), Value);}


void CDBFile::DumpCurrentContents(int i, char* String)
// Dumps the contents of the current record (raw ascii, no formatting)
{
	if (CurrentRec!=NULL)
	{
	strncpy(String, CurrentRec->Contents, i);
	String[i]=0;
	}
	else String[0]=0;
}



Record* CDBFile::ReadRecord(unsigned long RecNum)
// Reads a record from the file, returns a newly allocated *Record pointer
// The file offset is determined by RecNum and RecordLength.
{
	unsigned char Res;
	char* Buffer;
	Record *NewRec;

	if ((FileHandle==NULL)||(RecNum>RecordCount)||(RecNum<1)) return NULL;
	else
	{
		Buffer=new char[RecordLength];
		fseek (FileHandle, (long)(HeaderSize + (RecNum-1)*RecordLength), SEEK_SET);
		Res=fread (Buffer, RecordLength, 1, FileHandle);
		if (Res==0) return NULL;
		NewRec=new Record;
		NewRec->Contents=Buffer;
		NewRec->ModifFlag=false;
		NewRec->RecordNumber=RecNum;
		NewRec->Next=NULL;
		NewRec->Previous=NULL;
		return NewRec;
	}		
}

bool CDBFile::WriteHeader(char* Path)
// Use that function to rewrite or to save the current file under another 
// name. It writes the header of the current CDBFile. In order to update the
// "date of last update" field, I introduced two structures : time_t Date, and
// tm *Date_s, which will have to be changed for a port under UNIX. For more
// details, have a look at OpenFile() above and	the dBase III file structure.
{
unsigned char NLength,NDecCount;
char NType;
char Written;
CField *Current;
char dBaseVersion=0x03;
char FieldTerm=0x0D;
time_t Date;
struct tm *Date_s;
char Zero[256];
char Day, Year, Month;

	// Initialize a zero buffer;
	for (unsigned short j=0; j<256; j++) Zero[j]=0x00;

	// Validity of the path name
	if (Path!=NULL)
	{
		if (strlen(Path)>256)
		{
			fprintf (stderr, "File path too long : %s \n", Path);
			return false;
		}
		strcpy (PathName, Path);
	}

	// Close the currently open file
	if (FileHandle!=NULL) fclose(FileHandle);
	FileHandle = fopen (PathName, "w");
	if (FileHandle==NULL) return false;
	fseek (FileHandle, 0L, SEEK_SET);
	Written=fwrite (&dBaseVersion, 1, sizeof (char), FileHandle);


	time(&Date);
	Date_s = gmtime(&Date);
	Year=(char)(Date_s->tm_year);
	Month=(char)(Date_s->tm_mon);
	Day=(char)(Date_s->tm_mday);
	fseek (FileHandle, 1L, SEEK_SET);
	Written=fwrite (&Year, sizeof (char), 1, FileHandle);
	Written=fwrite (&Month, sizeof (char), 1, FileHandle);
	Written=fwrite (&Day, sizeof (char), 1, FileHandle);
	fseek (FileHandle, 4L, SEEK_SET);
	Written=fwrite (&RecordCount, sizeof (RecordCount), 1, FileHandle);
	fseek (FileHandle, 8L, SEEK_SET);
 	Written=fwrite (&HeaderSize, sizeof (HeaderSize), 1, FileHandle);
	Written=fwrite (&RecordLength, sizeof (RecordLength), 1, FileHandle);
	Written=fwrite (&Zero, sizeof(char), 20, FileHandle);

	for (unsigned short i=1; i<=FieldCount; i++) 
	{
		
		Current=FirstField->GetField(i);
		NLength=Current->GetLength();
		NType=Current->GetType();
		NDecCount=Current->GetDecCount();
		fwrite (Current->GetName(), 11, sizeof (char), FileHandle);
		fwrite (&NType, sizeof (char), 1, FileHandle);
		fwrite (&Zero, sizeof(char), 4, FileHandle);
		fwrite (&NLength, sizeof (unsigned char), 1, FileHandle);
		fwrite (&NDecCount, sizeof (unsigned char), 1, FileHandle);
		fwrite (&Zero, sizeof(char), 14, FileHandle);
	}// end for
	fwrite (&FieldTerm, sizeof (char), 1, FileHandle);

return true;

}

bool CDBFile::WriteRecord(Record* Current, unsigned long RecNum)
// Writes a record at the specified place,using RecNum to calculate the offset
{	
	size_t Res;

	if ((FileHandle==NULL)||(RecNum>RecordCount)||(RecNum<1)) return false;
	else
	{
		fseek(FileHandle,(long)(HeaderSize+(RecNum-1)*RecordLength),SEEK_SET);
		Res=fwrite (Current->Contents, RecordLength, 1, FileHandle);
		if (Res==1) 
		{
			Current->ModifFlag=false;
			// now that it has been written to file, it's no longer modified.
			return true;
		}
		else return false;
	}		
}	

bool CDBFile::Swap(Record* Rec1, Record* Rec2)
// swaps the contents of Rec1 and Rec2 (used in the SortAll function)
{
	char* BufCont;
	bool BufFlag;


	BufCont=Rec1->Contents;
	BufFlag=Rec1->ModifFlag;
	Rec1->Contents=Rec2->Contents;
	Rec1->ModifFlag=Rec2->ModifFlag;
	Rec2->Contents=BufCont;
	Rec2->ModifFlag=BufFlag;
	return true;
	
}

bool CDBFile::IsBigger(void *v1, void *v2, CField* Criterium)
// Compares v1 and v2 using Criterium to determine their type.
{
	switch(Criterium->GetType())
	{
	case 'N':	  // Numeric
		double *val1, *val2;
		val1=(double*)v1; val2=(double*)v2;
		return *val1>*val2;
	case 'C':
	default :	  // Caracter
		char *str1, *str2;
		str1=(char*)v1; str2=(char*)v2;
		return strcmp(str1, str2)>0;
	}
}


bool CDBFile::IsSmaller(void *v1, void *v2, CField* Criterium)
// Compares v1 and v2 using Criterium to determine their type.
{
	switch(Criterium->GetType())
	{
	case 'N':		// Numeric
		double *val1, *val2;
		val1=(double*)v1; val2=(double*)v2;
		return *val1<*val2;
	case 'C':
	default :		// Caracter
		char *str1, *str2;
		str1=(char*)v1; str2=(char*)v2;
		return strcmp(str2, str1)>0;
	}
}
}
}