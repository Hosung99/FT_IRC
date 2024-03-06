#ifndef BOT_HPP
# define BOT_HPP

# include "../main.hpp"

class Bot
{
public:
	/* OCCF */
	Bot();
	~Bot();

	/* member functions */
	std::string							introduce();
	void								addCommand(std::string, std::string);
	void								delCommand(std::string);
	std::vector<std::string>			listCommand();
	std::string							doCommand(std::string);

private:
	/* OCCF */
	Bot(const Bot &src);
	Bot &operator=(const Bot &src);

	/* member variables */
	std::map<std::string, std::string>	_commands;
};

#endif
