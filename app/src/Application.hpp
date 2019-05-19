#pragma once

#include <string>

class Application
{
public:
	struct Options
	{
		bool verbose;
		std::string ifile;
		std::string ofile;
	};

	Application(Options options);

	int exec();

private:
	Options m_options;
};
