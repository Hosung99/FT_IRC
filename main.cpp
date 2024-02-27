#include "includes/Server.hpp"

void a() {
	system("leaks ircserv");
}

int main(int argc, char *argv[])
{
	// atexit(a);
	if (argc != 3)
	{
		std::cout << "Warning :: Argument is only two" << std::endl;
		return (1);
	}
	try
	{
		Server server(argv[1], argv[2]);
		server.run();
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	return (0);
}