#pragma once

#include <string>

class Application
{
public:
	struct Options
	{
		std::string src_file;
		std::string dst_file;
		std::string config_file;
	};

	Application(Options options);

	int exec();

private:
	Options m_options;
};
