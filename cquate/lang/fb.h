
#pragma once


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

using std::string;

class cFileBuffer
{
public:

	cFileBuffer() { begin = end = 0; };
	~cFileBuffer() { Delete(); };

	char* begin;
	char* end;
	time_t t_lastMod;
	string strError;

	bool Load( const char* name )
	{
		size_t size;

		if ( name == 0 || *name == '\0' )
		{
			strError = "no file name given";
			return true;
		}

		struct stat st;
		if ( !stat( name, &st ) )
			size = st.st_size;
		else
		{
			strError = "file not found: ";
			strError.append( name );
			return true;
		}

		delete [] begin;
		begin = new char[ size + 1 ];
		if ( !begin )
		{
			strError = "insufficient memory to load file: ";
			strError.append( name );
			return true;
		}

		FILE* fp = fopen( name, "rb" );
		if ( !fp )
		{
			strError = "error opening file: ";
			strError.append( name );
			return true;
		}
		size = fread( begin, 1, size, fp);
		if ( ferror(fp) )
		{
			strError = "error reading file: ";
			strError.append( name );
			return true;
		}
		fclose( fp );

		begin[ size ] = '\0';
		end = begin + size;

		// set time
		t_lastMod = st.st_mtime;

		return false;
	}

	bool CheckTime( const char *name )
	{
		struct stat st;
		if ( !stat( name, &st ) ) { t_lastMod = st.st_mtime; return false; }
		else { return true; }
	}

	void Delete()
	{
		if (begin != NULL) {
			delete []begin;
			begin = NULL;
		}
	}

	int size()
	{
		return end-begin;
	}
};
