
#pragma once

#include "container.h"
#include "matrix.h"
#include "c_string.h"
#include "lexTypes.h"

// version indication

bool GetVersion(cString& res);


// file support

bool WriteWSHeader(FILE *pf, int nVars);
bool ReadWSHeader(int& nVer, int& nVars, FILE *pf);
bool SaveAll();
bool SaveAll(cString& file);
bool SaveSymbols(cString& file, cContainer& arg);
bool SaveSymbols(cString& file, cContainer& arg1, cContainer& arg2);
bool SaveSymbols(vector<cContainer>& args);
bool SaveSymbols(FILE *pf, vector<string> *pVNames);

// File I/O helpers
int FOpen(string&);
int FOpen(string&, string&);
bool FClose(double fid);
bool FRead( cMatrix& ReadMat, int& fid, int iSize, string& strPrecision );
bool FWrite( cMatrix& WriteMat, int& fid, int& iCount, string& strPrecision );
bool FScanf( vector<cContainer>& ReadVars, int& fid, string& format);
bool FPrintf( int& fid, string& format );
bool FPrintf( int& fid, string& format, vector<cContainer>& vargs );
bool SPrintf( string& Result, string& format, vector<cContainer>& vargs );
bool FGetline( string& vline, int& fid );
int countFmtArgs(string& s);
bool nextFormat(int& ipos, string& fsub, string& s);

void GetFileIDList(void);								// populate m_iFileIDs with current open FIDs
void GetFileIDList(vector<int>& fileIds);				// populate fileIds with current open FIDs

bool Load(cString& vfile);
bool Load(string& fname, vector<cContainer>& vargs);

bool LoadAscii(cMatrix& Result, cString& vfile);
bool SaveAscii(cString& vfile, cContainer& carg);
bool SaveAscii(cString& vfile, cContainer& carg_1, cContainer& carg_2);
bool SaveAscii(vector<cContainer>& carg);
bool LoadWav(cContainer::cUserHash& Result, cString& vfile);

bool LoadImage(cCube& Result, cString& vfile);
bool SaveImage(cCube& image, cString& vfile, cString& fmt);
unsigned int OneBitRowStride(unsigned int w);
unsigned int GetDataSize(int w, int h, int bitdepth);

bool FOpen(cMatrix& Result, cString& vfile, cString& varg);
bool FClose(cMatrix& vid);
bool FRead(cMatrix& Result, cMatrix& vid, cMatrix& vsize, cString& vprecision);
bool FWrite(cMatrix& vid, cMatrix& Mat, cMatrix& vcount, cString& vprecision);

bool FScanf(cContainer& Result, cMatrix& vid, cString& vformat);
bool FScanf(cContainer& Result_1, cContainer& Result_2, cMatrix& vid, cString& vformat);
bool FScanf(cContainer& Result_1, cContainer& Result_2, cContainer& Result_3, cMatrix& vid, cString& vformat);
bool FScanf(vector<cContainer>& Result, cMatrix& vid, cString& vformat);

//bool FPrintf(cMatrix& Result, cContainer& vid, cContainer& vformat);
//bool FPrintf(cMatrix& Result, cContainer& vid, cContainer& vformat, cContainer& varg);
//bool FPrintf(cMatrix& Result, vector<cContainer>& vargs);

bool stripCR(cString& clean, cString& dirty);

bool FPrintf(cMatrix& Result, cMatrix& vid, cString& vformat);
bool FPrintf(cMatrix& Result, cMatrix& vid, cString& vformat, cContainer& varg);
bool FPrintf(cMatrix& Result, cMatrix& vid, cString& vformat, vector<cContainer>& vargs);


bool Sprintf(cString& Result, cString& vformat);
bool Sprintf(cString& Result, cString& vformat, cContainer& varg);
bool Sprintf(cString& Result, cString& vformat, cContainer& varg1, cContainer& varg2);
bool Sprintf(cString& Result, cString& vformat, vector<cContainer>& vargs);

bool Printf(cString& vformat);
bool Printf(cString& vformat, cContainer& varg);
bool Printf(cString& vformat, cContainer& varg1, cContainer& varg2);
bool Printf(vector<cContainer>& vargs);

bool vv2vars(cContainer& Result, vector<cContainer>& vv);
bool vv2vars(cContainer& Result_1, cContainer& Result_2, vector<cContainer>& vv);
bool vv2vars(cContainer& Result_1, cContainer& Result_2, cContainer& Result_3, vector<cContainer>& vv);
bool vv2vars(vector<valueBlob>& Result, vector<cContainer>& vv);

bool Quit(void);
bool Edit();
bool Edit(cString& file);
bool ForceCrash(void);
bool CallHelp(void);
bool CallHelp(cString& topic);
bool Pause(void);
bool ShowPwd(void);
bool ShowPath(void);
bool NewPath(cString& dir);
bool DelPath(cString& dir);
bool Cd(cString& vdir);
bool Pwd(cString& Result);
bool Ls(void);				// file list
bool Ls(cString& arg);		// file list
bool Vls(void);				// variable list
bool Delete(cString& var);
bool Delete(cString& var1, cString& var2);
bool Delete(cString& var1, cString& var2, cString& var3);
bool Delete(vector<cString>& vars);
bool DeleteAll(void);
bool Cleanup(void);
bool Help(cString& arg);
bool Help(void);
bool DbgGo(void);
bool DbgStop(void);
bool DbgStack(void);

bool handleDotDot(string& absDir, string& arg);

bool getFilesInDir(vector<string>& files, string& dir);
bool getFilesInDir(vector<string>& files, vector<string>& subdirs, string& dir);
bool getFilesInDir(vector<string>& files, vector<string>& subdirs, string& dir, string& arg);
bool getFileExtension(string& ext, string& fname);
bool isRelativeDir(string& dir);
bool getAbsoluteDir(string& absdir, string& reldir);
string getParentDir(string& fulldir);
string forwardToBackSlash(string& dir);
