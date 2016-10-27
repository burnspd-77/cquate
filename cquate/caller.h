#pragma once

#include<string>

using std::string;

class cCaller
{
public:
	cCaller();
	~cCaller();

	bool prepCall(string& argsin, string& argsout, string& name);

private:

};

