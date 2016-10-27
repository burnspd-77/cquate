
#include <string>


class cOptions
{
public:
	cOptions()
	{
		m_bVersion = false;
		m_nFmtDecimals = 0;
	};
	~cOptions() {  };

	bool parseOptions(int argc, const char** argv);

public:
	// just make everything accessible for now

	// options
	bool m_bVersion;

	int m_nFmtDecimals;
	
	std::string m_inFile;
};
