// cquate.cpp : Defines the entry point for the console application.
//

#include "targetver.h"
#include "options.h"
#include "state.h"

cStorage T;
cState prog_state;

int main(int argc, const char* argv[])
{
	cOptions opts;
	if (opts.parseOptions(argc,argv)) {
		if (argc > 1) {
			std::cout << "Unacceptable argument(s) given.\n";
			return 0;
		}
		return 0;
	}

	if (opts.m_nFmtDecimals > 0) {
		prog_state.SetFormatDecimals(opts.m_nFmtDecimals);
	}

	// first, update any changed scripts
	prog_state.m_lang.updateAllScripts();
	prog_state.MultiLineManager(opts.m_inFile);

	return 0;
}
