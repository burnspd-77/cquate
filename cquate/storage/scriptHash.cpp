
#include "scriptHash.h"

void cScriptHash::initTable()
{
	clear();

	string s = "";
	sBlock b;
	b.fileName = b.pathName = s;
	b.nType = LEX_BLOCK_NULL;
}

	//bool lookup(char *pchName) const;
	//bool lookup(string& name) const;
	//bool lookup(sBlock& t, char *pchName) const;
	//bool lookup(sBlock& t, string& name) const;
	//bool lookup(sBlock& t) const;

bool cScriptHash::lookup(char *pchName) const
{
	string name = pchName;
	return lookup(name);
}

bool cScriptHash::lookup(string& name) const
{
	return m_table.find(name) == m_table.end();
}

bool cScriptHash::lookup(sBlock& b, string& name) const
{
	map<string,sBlock>::const_iterator it = m_table.find(name);

	if (it != m_table.end()) {
		b = it->second;
		return false;
	}
	return true;
}

bool cScriptHash::lookup(sBlock& t) const
{
	return lookup(t, t.name);
}

bool cScriptHash::which(string& fullFile, string& name)
{
	fullFile.clear();
	sBlock b;
	b.name = name;

	if (!lookup(b)) {
		fullFile = b.pathName;
		fullFile.append("\\");
		fullFile.append( b.fileName );
		return false;
	}
	return true;
}

bool cScriptHash::insert(sBlock& b)
{
	string name = b.name;
	m_table[name] = b;

	return false;
}

bool cScriptHash::remove(sBlock& b)
{
	return remove(b.name);
}

bool cScriptHash::remove(string& name)
{
	map<string,sBlock>::iterator it = m_table.find(name);

	if (it != m_table.end())
		m_table.erase( it );

	return false;
}


void cScriptHash::clear()
{
	m_table.clear();
}


int cScriptHash::count()
{
	return (int) m_table.size();
}

vector<string> cScriptHash::getAll()
{
	map<string,sBlock>::iterator it = m_table.begin();
	vector<string> names;
	string fname;

	while (it != m_table.end())
	{
		fname = it->second.fileName;
		
		names.push_back( fname );

		++it;
	}

	return names;
}
