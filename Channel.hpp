#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "main.hpp"

class Channel
{
private:
	int _channel_fd;
	std::string _channel_name;
	std::vector<int> _client_fd_list;

public:
	Channel(const std::string& channel_name, int fd);
	~Channel();
	void set_channel_fd(int channel_fd);
	void set_channel_name(std::string& channel_name);
	void append_client_fd_list(int client_fd);
	void remove_client_fd_list(int client_fd);
	int get_channel_fd();
	std::string get_channel_name();
	std::vector<int> get_client_fd_list();
	std::vector<int>::iterator findMyClientIt(int fd);
	bool checkClientInChannel(int fd);
};

#endif