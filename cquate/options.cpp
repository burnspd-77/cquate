
#include "targetver.h"
#include "options.h"
#include <stdlib.h>
#include <iostream>
#include <string>


bool cOptions::parseOptions (int argc, const char** argv)
{
	if (argc < 2) {
		std::cout << std::endl;
		std::cout << "Usage: " << PROG_NAME << " [options] <in>" << std::endl;
		std::cout << " Where" << std::endl;
		std::cout << "  <cq>              cquate source file or inline statements to run" << std::endl;
		std::cout << std::endl;
		std::cout << " Options:" << std::endl;
		std::cout << "  -d <n>            numeric decimals (formatted output)" << std::endl;
		std::cout << std::endl;
		std::cout << "\nVersion: " << PROG_VER << std::endl;
		std::cout << std::endl;
		std::cout << "." << std::endl;
		std::cout << std::endl;
		return true;
	}

	//Parse argv for required options
	int ifileCount = 0;

	for (int i=1; i<argc; ++i) {
		std::string opt = (char*)argv[i];
		if (opt.at(0)=='-')
		{
			if (opt == "-d") {
				m_nFmtDecimals = atoi( (char*)argv[++i] );
			}
			else
				return true;
		}
		else
		{
			if (ifileCount==0)
			{
				m_inFile = opt;
				++ifileCount;
			}
			else
				return true;
		}
	}

	if (ifileCount != 1)
		return true;		// incorrect number of arguments

	return false;
}
