#pragma once

#include <string>
#include <map>

using std::map;
using std::string;

class cHelpDb {
public:
	cHelpDb() { }
	~cHelpDb() { }

	void add(string& key, string& help) {
		if (key.empty())
			return;
		m_db[key] = help;
	}

	bool lookup(string& help, string& key) {
		help.clear();
		if (key.empty())
			return true;

		// make key lower case
		string lkey = key;
		for (int i=0; i<(int)key.length(); ++i) {
			lkey[i] = tolower(key[i]);
		}

		// search
		map<string,string>::iterator it = m_db.find(lkey);
		if (it != m_db.end()) {
			help = it->second;
			return false;
		}
		// did not find key
		return true;
	}

private:
	map<string,string> m_db;
};
