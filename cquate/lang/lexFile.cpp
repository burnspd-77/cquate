
#include "lex.h"
#include "state.h"

#ifndef _WINDOWS_
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>
#endif

extern cState prog_state;

bool cLex::initializeFiles()
{
	if (getFileList()) return true;		// something went wrong...

	for (int i=0; i<(int)m_fileList.size(); ++i) {
		if ( updateFunctions( m_fileList[i], false) ) return true;
	}

	addToScriptTable( &m_fileList );

	for (int i=0; i<(int)m_fileList.size(); ++i)
	{
		if ( addFile(m_fileList[i]) ) { /* NOP if error */ }
		m_pSymbolTable->reset();
	}
	return false;
}

bool cLex::setPath(vector<string>& userPath)
{
	m_path = m_DefaultAutoPaths;
	m_path.insert( m_path.end(), userPath.begin(), userPath.end() );
	return false;
}

bool cLex::setPath()
{
	m_path = m_DefaultAutoPaths;
	return false;
}

bool cLex::addPath(string& dir)
{
	for (int i=0; i<(int)m_path.size(); ++i)
	{
		if (m_path[i].compare(dir)==0)
			return false;				// dir already in path
	}

	m_path.push_back(dir);
	return false;
}

bool cLex::delPath(string& dir)
{
	for (int i=0; i<(int)m_path.size(); ++i) {
		if (m_path[i].compare(dir)==0) {
			m_path.erase( m_path.begin()+i );
			return false;
		}
	}

	return true;	// dir not found in path
}

bool cLex::isInPath(string& dir)
{
	for (int i=0; i<(int)m_path.size(); ++i)
	{
		if (m_path[i].compare(dir)==0) return true;
	}
	return false;
}

bool cLex::getFileList()
{
	vector<string> scriptFiles;

	m_fileList.clear();

	// first, take care of files in working directory
	string cwd = prog_state.m_WorkingDir;

	if (!isInPath(cwd)) {
		if (getScriptFilesInDir( scriptFiles, cwd )) return true;
		insertFiles( scriptFiles, cwd );
	}
	
	// now do all other files in path
	for (int i=0; i<(int)m_path.size(); ++i)
	{
		if (getScriptFilesInDir( scriptFiles, m_path[i] )) return true;
		insertFiles( scriptFiles, m_path[i] );
	}
	return false;
}

bool cLex::insertFiles(vector<string>& newFiles, string& dir)
{
	string fullFile;

	for (int i=0; i<(int)newFiles.size(); ++i)
	{
		// next file
		fullFile = dir + '\\';	fullFile.append( newFiles[i] );
		m_fileList.push_back( fullFile );
	}
	return false;
}


bool cLex::getScriptFilesInDir(vector<string>& files, string& dir)
{
	files.clear();
	string fname, tag;

#ifdef _WINDOWS_
	// windows directory interrogation library

	int ANY_FILE =  _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH;

	struct _finddatai64_t data;
	// First create the filename that will be use to initialize the find.
	// "*.*" are wild card characters that tells the find function to return a 
	// list of all the files and directories.  You can limit this if you wish
	// to just file with specific extensions, for example "*.txt".  If you do that
	// then finder will not return any directory names.

	// start the finder -- on error _findfirsti64() will return -1, otherwise if no
	// error it returns a handle greater than -1.

	//if (dir.at(dir.length()-1)==SLASH)
	//	fname = dir + wildcard;
	//else
	//{
	//	fname = dir + SLASH;
	//	fname += wildcard;
	//}
	fname = dir + "\\*";
	fname.append( SCRIPT_EXTENSION );

	long h = _findfirsti64(fname.c_str(),&data);
	if (h >= 0)
	{
		do {
			if( (data.attrib & ANY_FILE) )
			{
				// its a file
				fname = data.name;
				files.push_back( fname );
			}
			else
			{
				// ignore
			}
		} while ( _findnexti64(h,&data) == 0 );
		// close the find handle.  
		_findclose(h);

	}
#else
	// unix directory interrogation library

	tag = SCRIPT_EXTENSION;
	string::size_type nTag = tag.size();

    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(dir.c_str())) == NULL) {
		return true;
    }

	string str;
    while ((dirp = readdir(dp)) != NULL)
	{
		str = string(dirp->d_name);

		if (str.size()>0)
		{
			if (str.at(0)!='.')
			{
				struct stat st;
				size_t size;
				if ( !stat( dirp->d_name, &st ) )
					size = st.st_size;

				if (str.find(tag)==str.size()-nTag)
				{
					files.push_back( dirp->d_name );
				}
			}
		}
    }
    closedir(dp);

#endif
    return false;
}

bool cLex::UpdatePath()
{
	string pathfname = m_RunDir + "\\path.txt";
	string line;
	char buf[256];
	int idx;
	FILE *pathfile = fopen(pathfname.c_str(),"r");
	if (pathfile!=NULL) {
		m_path.clear();
		while (fgets(buf, 250, pathfile)!=NULL) {
			if (strlen(buf)!=0) line = buf;
			idx = line.find_first_of('\n');
			if (idx!=string::npos) line = line.substr(idx);				// remove the newline char
			line = ConvertToLower(line);
			if (IsValidPath(line)) m_path.push_back(line);
		}
	}
	else {
		// create the default path.txt file
		FILE *newpathfile = fopen(pathfname.c_str(), "w");
		fprintf(newpathfile, "\n");
		for (int i=0; i<(int)m_DefaultAutoPaths.size(); i++) {
			line = m_DefaultAutoPaths[i];
			fprintf(newpathfile, "%s\n", line.c_str());
			m_path.push_back(line);
		}
		fclose(newpathfile);

		return false;
	}

	fclose(pathfile);

	return false;
}

string cLex::ConvertToLower(string& strInput)
{
	if (strInput.empty()) return string("");

	string strOutput;

	int L = strInput.length();
	char *pch = new char[L+1];		// leave room for null
	char ch;

	// copy input string to static char array
	strcpy( pch, strInput.c_str() );
	pch[L] = '\0';				// null terminate

	// convert all chars to lower case
	for (int i=0; i<L; i++) {
		ch = tolower(pch[i]);
		pch[i] = ch;
	}

	strOutput = pch;

	// clean upz0r
	delete []pch;

	return strOutput;
}

bool cLex::IsValidPath(string& line)
{
	char test_path[MAX_PATH_BUFFER];
	bool bIsValid;

	if (__GETCWD( test_path, MAX_PATH_BUFFER-1 )==NULL) {
		// hmm, need to rework the API a bit
		return false;
	}

	int iErr = __CHDIR(line.c_str());		// returns 0 if valid path
	if (iErr!=0) bIsValid = false;
	else bIsValid = true;

	chdir(test_path);			// go back to original working directory
	return bIsValid;
}

bool cLex::updateAllScripts()
{
	vector<string>  newFiles;
	return updateAllScripts(newFiles);
}

bool cLex::updateAllScripts(vector<string>& newFiles)
{
	// initialize output
	newFiles.clear();

	vector<string> oldList = m_fileList;
	if (getFileList()) return true;		// hopefully this never happens

	// save lex state
	pushState();

	sBlock block;
	vector<string> delFiles, tmpList = m_fileList;
	getMissingFiles( delFiles, oldList, m_fileList );
	getMissingFiles( newFiles, tmpList, oldList );

	removeScripts( delFiles );

	sBlock b;
	bool bChanged;
	int i;

	// update function table first
	for (i=0; i<(int)newFiles.size(); ++i) {
		if ( updateFunctions(newFiles[i], false) ) { /* NOP if error */ }
	}

	// add the new files
	addToScriptTable( &newFiles );
	for (i=0; i<(int)newFiles.size(); ++i) {
		if ( updateFunctions(newFiles[i], true) ) { /* NOP if error */ };
		if ( updateScript(newFiles[i]) ) { /* NOP if error */
			printf("error occurred when parsing script: %s\n", newFiles[i].c_str());
		}
	}

	// update whole list
	for (i=0; i<(int)m_fileList.size(); ++i) {
		//printf("updating %s\n", m_fileList[i].c_str());
		if ( checkTime(bChanged, m_fileList[i]) ) goto UpdateDone;
		if (bChanged) {
			if ( updateScript(m_fileList[i]) ) { /* NOP if error */ }
			if ( updateFunctions(m_fileList[i], true) ) goto UpdateDone;
		}
	}


UpdateDone:
	// restore lex state
	popState();

	return false;
}

bool cLex::getMissingFiles(vector<string>& missingFiles, vector<string>& oldList, vector<string>& curList)
{
	missingFiles.clear();
	int i, j, st(0);
	bool bFound = false;

	for (i=0; i<(int)oldList.size(); ++i) {
		for (j=st; j<(int)curList.size(); ++j) {
			if (oldList[i].compare( curList[j] )==0) { bFound = true; break; }
		}
		if (bFound) { bFound = false; continue; }
		for (j=0; j<st; ++j) {
			if (oldList[i].compare( curList[j] )==0) { bFound = true; break; }
		}
		if (!bFound) missingFiles.push_back( oldList[i] );
	}

	return false;
}

bool cLex::removeScripts( vector<string>& files )
{
	sBlock b;

	for (int i=0; i<(int)files.size(); ++i) {
		if ( fullScript2block(b, files[i]) ) return true;
		if ( m_pFcnTable->removeFileAssoc(b) ) return true;
		if ( m_pScriptTable->remove(b) ) return true;
	}

	return false;
}

bool cLex::checkTime(bool& bNewer, string& fullFile)
{
	// initialize output
	bNewer = false;

	sBlock b;

	if (fullScript2block(b, fullFile)) {
		return false;	// cannot check the time, so just return
	}

	cFileBuffer f;
	if (f.CheckTime( fullFile.c_str() )) {
		// file not found, so delete entry from script table
		m_pScriptTable->remove(b);
		return false;
	}
	bNewer = f.t_lastMod > b.t_lastParsed;

	return false;
}

bool cLex::updateFunctions(string& fullFile, bool bParse)
{	// find all fcn declarations in file and add to fcn table (without parsing the fcn code)
	string name, path, ext;
	sBlock b;
	bool bErr(false);

	fileParts( path, name, ext, fullFile );

	b.fileName = name + ext;
	b.pathName = path;

	cFileBuffer f;
	if (f.Load( fullFile.c_str() ))		// file not found, or other error
		return true;

	bool bSavePre = m_bPreParsing;
	m_bPreParsing = true;
	pushBuffer( b );

	setBuffer( f.begin, f.size() );
	lookAhead = nextFunction();
	while (lookAhead!=DONE) {
		if (parseFunction(b, bParse)) { bErr = true; m_bPreParsing = false; break; }

		m_pFcnTable->insert(b);
		lookAhead = nextFunction();
	}

	popBuffer();
	m_bPreParsing = bSavePre;

	f.Delete();
	return bErr;
}

bool cLex::fullScript2block(sBlock& b, string& fullFile)
{
	// convert full file name of script to code block struct, via table lookup
	string name, path, ext;

	if (fileParts(path,name,ext,fullFile)) return true;
	b.fileName = name;
	b.fileName += ext;
	b.name = name;
	b.pathName = path;
	b.nLineOffset = 0;

	return m_pScriptTable->lookup(b);
}

//bool cLex::makeBlock(sBlock& b)
//{
//	// convert full file name of script to code block struct, via table lookup
//	string name, path, ext;
//
//	if (fileParts(path,name,ext,m_curFile)) return true;
//	b.fileName = name;
//	b.fileName += ext;
//	b.name = name;
//	b.pathName = path;
//	b.nLineOffset = 0;
//
//	return false;
//}

// replace existing script parse with updated one
bool cLex::updateScript(string& fullFile)
{
	string name, path;
	sBlock b;

	if (fullScript2block(b, fullFile)) return true;

	cFileBuffer f;
	if (f.Load( fullFile.c_str() ))		// file not found, or other error
		return true;

	pushBuffer( b );

	setBuffer( f.begin, f.size() );
	if (parseFileCode()) { f.Delete(); popBuffer(); return true; }

	// retrieve pieces of block
	b.bError = m_block.bError;
	b.outMsg = m_block.outMsg;
	b.stmts = m_block.stmts;

	popBuffer();

	// update parsed time to now
	time( &b.t_lastParsed );

	// request debug stops from editor


	// add entry to table
	if (m_pScriptTable->insert( b )) { f.Delete(); return true; }

	f.Delete();
	return false;
}

bool cLex::addToScriptTable(vector<string> *pScriptFiles)
{
	sBlock emptyBlock;
	string path, file, ext, fullFile;

	emptyBlock.nLineOffset = 0;
	emptyBlock.bError = false;

	for (int i=0; i<(int)pScriptFiles->size(); ++i)
	{
		if (fileParts( path, file, ext, pScriptFiles->at(i) ))	return true;		// bad file?
		emptyBlock.fileName = emptyBlock.name = file;
		emptyBlock.fileName += ext;
		emptyBlock.pathName = path;
		emptyBlock.t_lastParsed = 0;
		
		m_pScriptTable->insert(emptyBlock);
	}
	return false;
}

bool cLex::addFile(string& fullFile)
{
	// assume for now this is a simple script (not a fcn)
	sBlock block;
	cFileBuffer f;

	string path, file, ext;
	if (fileParts( path, file, ext, fullFile ))
		return true;		// bad file?

	block.name = file;
	block.fileName = file + ext;
	block.pathName = path;
	block.nLineOffset = 0;

	// check script (file) hash table for this entry
	if (m_pScriptTable->lookup( block, file ))
	{
		// not in our hash, so load and parse
		if (f.Load( fullFile.c_str() ))		// file not found, or other error
			return true;

		pushBuffer( block );

		setBuffer( f.begin, f.size() );
		if (parseFileCode()) { f.Delete(); popBuffer(); return true; }

		// retrieve pieces of block
		block.bError = m_block.bError;
		block.outMsg = m_block.outMsg;
		block.stmts = m_block.stmts;
	
		popBuffer();

		// get local time
		time( &block.t_lastParsed );
//		pUtcTime = gmtime( &rawTime );

		// add entry to table
		if (m_pScriptTable->insert( block )) { f.Delete(); return true; }
	}
	else
	{
		// now we know this is the same file, check time stamp for revision
		if (f.CheckTime( fullFile.c_str() ))	return true;	// file not found, or other error

		if (f.t_lastMod > block.t_lastParsed)
		{
			//// we must delete hash entry and reload
			//m_pScriptTable->remove( ix, ixh );

			// not in our hash, so load and parse
			if (f.Load( fullFile.c_str() ))		// file not found, or other error
				return true;

			pushBuffer( block );

			setBuffer( f.begin, f.size() );
			if (parseFileCode()) { f.Delete(); popBuffer(); return true; }

			// retrieve pieces of block
			block.bError = m_block.bError;
			block.outMsg = m_block.outMsg;
			block.stmts = m_block.stmts;
		
			popBuffer();

			// update parsed time to now
			time( &block.t_lastParsed );

			// add entry to table
			if (m_pScriptTable->insert( block )) { f.Delete(); return true; }
		}
		else
		{
			// no change -- use code block in script table
		}	// if script revision time is newer
	}

	f.Delete();
	return false;
}


bool cLex::parseFileCode()
{
	// clear all stmts
	//m_blockStack.back().stmts.clear();
	clearReport();

	bool bErr = false;

	// pre-parsing mode ON
	m_bPreParsing = true;

	//// set temp file settings
	//string fileSave = m_curFile;	m_curFile = m_blockStack.back().fileName;
	//string pathSave = m_curPath;	m_curPath = m_blockStack.back().pathName;
	int lineSave = m_lineno;		m_lineno = 1;

	//m_blockStack.back().bError = false;
	//m_blockStack.back().outMsg.clear();
	//m_blockStack.back().stmts.clear();

	vector<sStatement> stmts;
	m_block.bError = false;

	// change to parseScriptFile !!!
	if (parseScript(stmts)) {
		//m_blockStack.back().outMsg = m_outMsg;
		//m_blockStack.back().bError = true;
		bErr = true;
		m_block.bError = true;
	}

	//m_blockStack.back().stmts = stmts;

	//// restore temp file settings
	//m_curFile = fileSave;
	//m_curPath = pathSave;
	m_lineno = lineSave;

	m_block.stmts = stmts;
	m_block.inArgs.clear();
	m_block.outArgs.clear();
	m_block.outMsg = m_outMsg;

	// pre-parsing mode OFF
	m_bPreParsing = false;

	// clear temp storage
	clearReport();
	resetStmt();
	m_stmt.tokens.clear();

	return false;
}
