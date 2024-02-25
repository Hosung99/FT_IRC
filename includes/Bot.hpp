#ifndef BOT_HPP
#define BOT_HPP

#include "../main.hpp"

class Bot
{
	private:
		Bot(const Bot &src);
		Bot &operator=(const Bot &src);
		std::map<std::string, std::string> _commands;
	public:
		Bot();
		~Bot();
		std::string introduce();
		void addCommand(std::string command, std::string response);
		void delCommand(std::string command);
		std::vector<std::string> listCommand();
		std::string doCommand(std::string command);
};

#endif