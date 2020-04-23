#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <fstream>

//Logger saves it's output to log_file.txt located in \..\make directory.
//Don't know how to change the file save path eventually.

class Logger
{
public:

	enum class Level { Info, Debug, Error };
	Logger();
	~Logger();
	void Info(std::string message);
	void Debug(std::string message);
	void Error(std::string message);
	void SetLevel(Level level);

private:
	std::ofstream logfile;
	std::string file_text;
	Level new_level = Level::Info;
};
#endif //LOGGER