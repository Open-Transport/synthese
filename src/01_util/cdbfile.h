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

#ifndef SYNTHESE_util_cdbfile_h__
#define SYNTHESE_util_cdbfile_h__

#include <string>

namespace synthese
{
	namespace util
	{
		/* The records will be stored as a dynamic list of raw character strings,
		in the same format as in the file. I could have converted them into fully-
		blown data structures or classes, but genericity implies dynamic structures, 
		which are known to be memory consuming. */

		struct rec 
		{	char* Contents;		//raw character string 
			bool ModifFlag;		// Indicates whether that record has been modified 
			unsigned long RecordNumber;	// Position of the record within the file 
			struct rec *Next;		// Points to the next record in the list 
			struct rec *Previous;	// Points to the previous record in the list 
		};

		typedef struct rec Record;


		/* The type and the number of fields are stored once and for all in a 
		dynamic list, which will be referred to by the functions which will
		decode and read the values stored in the records. This dynamic list
		is in fact a dynamic ring, as we consider that the "Next" field of the
		last element in the list points to the head of the list.*/
		class CField
		{
		private :
			
		char	 		Name[11];		// field name in ASCII zero-filled 
		char	 		Type;			// field type in ASCII 
		unsigned char	Length;			// field length in binary 
		unsigned char 	DecCount;		// field decimal count in binary 
		unsigned char	FieldNumber;	// field number within the record 
		unsigned short	Offset;			// field offset within the character string 
		CField*			Next;			// Next field in the list 


		public :
		CField();
		CField(char* NName,char NType,unsigned char NLength,unsigned char NDecCount,
								unsigned char FieldNum);
		~CField();
		char* GetName() 			{	return Name;	}
		void SetName(char* NewName)	{	strcpy(Name, NewName);	}
		char GetType()				{	return Type;	}
		void SetType(char NewType)	{	Type=NewType;	}
		unsigned char GetLength()	{	return Length;	}
		void SetLength(unsigned char NewLength)	{	Length=NewLength;	}
		unsigned char GetDecCount()				{	return DecCount;	}
		void SetDecCount(unsigned char NewDecCount)	{	DecCount=NewDecCount;	}
		unsigned char GetFieldNumber()				{	return FieldNumber;	}
		void SetFieldNumber(unsigned char NewFieldNumber) 	
												{	FieldNumber=NewFieldNumber;	}
		unsigned short GetOffset()				{	return Offset;	}
		void SetOffset(unsigned short NewOffset)	{	Offset=NewOffset;	}
		CField* GetNext()				{	return Next;	}
		void SetNext(CField* NextField)	{	Next=NextField;	}

		CField* GetField(const std::string& FieldName, CField* Start=NULL)
		{	// Pass the name of the field for argument 
			if (Start==NULL) Start=this;	/* NULL is passed for the initial call of
											this  recursive function in the program */
			else 
				if (Start==this) return NULL;	/* We've checked all the elements of 
											the dynamic ring: none matches FieldName */
			if (strcmp(FieldName.c_str(), Name)!=0) 
				return Next->GetField(FieldName, Start); /* Proceed to next element */
			else
				return this;	/* FieldName matches Name */
		}	
			
									
		CField* GetField(unsigned short Number, CField* Start=NULL)	
		{	 // Pass the number of the field for argument 
			if (Start==NULL) Start=this;	/* NULL is passed for the initial call of
											this  recursive function in the program */
			else 
				if (Start==this) return NULL;	/* We've checked all the elements of 
											the dynamic ring: none matches Number */
			if (Number!=FieldNumber) 
				return Next->GetField(Number, Start); /* Proceed to next element */
			else
				return this;	/* Number matches FieldNumber */
		}	

									
		unsigned short GetNumberOfFields(CField* Start=NULL, unsigned short i=0);


		};



		class CDBFile
		{
		private :

		char PathName[256];			// Path name for the dBase III file 
		FILE *FileHandle;			// Handler for the dBase III file 
		unsigned short HeaderSize;	// Length of header structure 
		unsigned short FieldCount;	// Number of fields in each record 
		unsigned long RecordCount;	// Number of records in the file 
		bool ModifiedFlag;			// Indicates whether the data has been modified 
		bool FullFileInMemory;		// Indicates whether the entire file is loaded 
		unsigned short RecordLength;	// Length of the record strings 

		Record* RecordList;	// Head of the list of records 
		Record*	CurrentRec;	// Current record pointed in the list
		CField* FirstField;	// Head of the list of fields 

		public:

		CDBFile();
		~CDBFile();
		bool IsOpen()	{ return FileHandle!=NULL; }
		bool Clean();
		bool OpenFile(const char* Path);
		bool CloseFile();
		unsigned long LoadFileToMemory();
		unsigned long WriteAllToFile(char *Path=NULL);
		unsigned long WriteModified();
		void SortOn(unsigned short Criterium1/*, unsigned short Criterium2*/);
		void* GetFieldValue(const std::string& Field);
		void* GetFieldValue(unsigned short FieldNum);
		std::string getText(const std::string& field);
		void GetAtRecord(unsigned long RecordNum) { CurrentRec=GetRecord(RecordNum); }
		unsigned long GetRecordNum() {	if (CurrentRec) 
											return CurrentRec->RecordNumber;
										else return 0L;}
		bool GetNextRecord()	  { if (CurrentRec) { CurrentRec=CurrentRec->Next; return true;}
									else return false; }
		bool GetPreviousRecord()  { if (CurrentRec) { CurrentRec=CurrentRec->Previous; return true;}
									else return false; }
		void LoadRecord(unsigned long RecordNum)  { CurrentRec=ReadRecord(RecordNum);
															 Append(CurrentRec); }
		void DeleteCurrentRec()		{ DeleteRecord(CurrentRec);}
		void CreateAndAppend()		{ CurrentRec=CreateNewRecord();
										Append(CurrentRec);}	
		void ClearAllRecords();		
		//void AddField(CField* NewField);  /* NOT IMPLEMENTED YET. Any volunteers ? */
		void SetFieldValue(char* Field, void* Value);
		void SetFieldValue(unsigned short FieldNum, void* Value);
		void DumpCurrentContents(int i, char* String);
		unsigned char GetFieldDecCount(unsigned short NumField)
				{ return (FirstField->GetField(NumField))->GetDecCount(); } 
		char GetFieldType(unsigned short NumField) 
				{ return (FirstField->GetField(NumField))->GetType(); } 
		unsigned short GetFieldCount()	{ return FieldCount; }
		unsigned long GetRecordCount()	{ return RecordCount; }
		void DeleteVoidPointer(void* Pointer, unsigned short Field);
		void DeleteVoidPointer(void* Pointer, char* Field);

		private:
		bool WriteRecord(Record* Current, unsigned long RecNum);
		bool Swap(Record* Rec1, Record* Rec2);
		bool IsBigger(void *v1, void *v2, CField* Criterium);
		bool IsSmaller(void *v1, void *v2, CField* Criterium);
		Record* ReadRecord(unsigned long RecNum);
		Record* CreateNewRecord();
		Record* GetRecord(unsigned long RecordNum);
		void Append(Record* Rec, Record* Tail=NULL);
		void* GetFieldValue(Record* Rec, CField* Field);
		std::string getText(Record* Rec, CField* Field);
		void SetFieldValue(Record* Rec, CField* Field, void* Value);
		void DeleteRecord(Record *Rec);
		void SortAllRecords(Record *Head, Record *Tail,
								 CField* Criter1/*,CField* Criter2*/);
		bool WriteHeader(char* Path=NULL);
		void DeleteVoidPointer(void* Pointer, CField* Field);

		};
	}
}

#endif // SYNTHESE_util_cdbfile_h__
