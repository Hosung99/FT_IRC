#include "Server.hpp"
#include "Client.hpp"

unsigned short int Server::set_port_num(char *str)
{
	int ret;
	int str_index = 0;
	while (str[str_index])
	{
		if (!(str[str_index] >= '0' || str[str_index] <= '9'))
			throw(std::out_of_range("ERROR:: Port Number is only number"));
		str_index++;
	}
	ret = atoi(str);
	if (!(ret >= 1024 && ret <= 65535))
		throw(std::logic_error("ERROR:: PORT number range is 1024~65535"));
	return ((unsigned short int)ret);
}

std::string Server::set_password(char *str)
{
	int str_index = 0;
	while (str[str_index])
		str_index++;
	if (str_index >= 9)
		throw(std::logic_error("ERROR:: Password is under 9 digit"));
	return (str);
}

Server::Server(char *port_num, char *password)
{
	this->port_num = set_port_num(port_num);
	this->password = set_password(password);
	fd_cnt = 1;
	client_addr_size = sizeof(this->client_addr);
	this->command = new Command(*this);
}

Server::~Server()
{
	close(server_sock);
}

void Server::run()
{
	set_server_sock();
	set_server_addr();
	set_server_bind();
	set_server_listen();
	memset(fds, 0, sizeof(fds));
	fds[0].fd = server_sock;
	fds[0].events = POLLIN;
	execute();
}

void Server::set_server_sock()
{
	int option = 1;
	server_sock = socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if (server_sock == -1)
		throw(std::logic_error("ERROR :: socket() error"));
}

void Server::set_server_addr()
{
	memset(&this->server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(this->port_num);
}

void Server::set_server_bind()
{
	if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		throw(std::logic_error("ERROR:: bind() error"));
}

void Server::set_server_listen()
{
	if (listen(server_sock, 15) == -1)
		throw std::logic_error("ERROR:: listen() error");
}

int Server::get_server_sock()
{
	return (this->server_sock);
}

std::map<int, Client *> Server::get_clients()
{
	return (this->Clients);
}

std::string Server::get_password()
{
	return (this->password);
}

std::string Server::get_message(int fd)
{
	return (this->message[fd]);
}

int Server::recv_message(int fd)
{
	char buf[2];
	ssize_t read_len;
	int ret = 0;
	while ((read_len = recv(fd, buf, 1, 0)) == 1)
	{
		ret++;
		message[fd] += buf[0];
		if (buf[0] == '\n')
			break;
	}
	return (ret);
}

bool Server::check_message_ends(int fd)
{
	if (message[fd].length() == 1 && message[fd][0] == '\n')
		message[fd] = "";
	if (message[fd][message[fd].length() - 1] == '\n' && message[fd][message[fd].length() - 2] == '\r')
		return (true);
	return (false);
}

void Server::do_command(int fd)
{
	command->run(fd);
}

void Server::add_client(int fd)
{
	Clients.insert(std::make_pair(fd, new Client(fd)));
}

void Server::execute()
{
	while (1)
	{
		int ret_poll = poll(fds, fd_cnt, -1);
		if (ret_poll > 0)
		{
			for (int i = 0; i < fd_cnt; i++)
			{
				if (fds[i].revents == 0)
					continue;
				if (fds[i].fd == server_sock)
				{
					client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
					fcntl(client_sock, F_SETFL, O_NONBLOCK);
					fds[fd_cnt].fd = client_sock;
					fds[fd_cnt].events = POLLIN;
					fd_cnt++;
					add_client(client_sock);
					std::cout << "fd[" << client_sock << "] is connected" << std::endl;
				}
				else
				{
					if (fds[i].revents & POLLIN)
					{
						str_len = recv_message(fds[i].fd);
						if (str_len <= 0)
						{
							close(fds[i].fd);
							std::cout << "fd " << fds[i].fd << " is quit connect" << std::endl;
							fds[i].fd = -1;
						}
						else
						{
							if (check_message_ends(fds[i].fd))
							{
								std::cout << "fd[" << fds[i].fd << "]: " << message[fds[i].fd];
								do_command(fds[i].fd);
								message[fds[i].fd] = "";
							}
						}
					}
				}
			}
			std::map<int, Client *>::iterator iter = Clients.begin();
			for (; iter != Clients.end(); iter++)
			{
				if (iter->second->get_client_recv_buf().length() > 0)
				{
					send(iter->first, iter->second->get_client_recv_buf().c_str(), iter->second->get_client_recv_buf().length(), 0);
					iter->second->clear_client_recv_buf();
				}
			}
		}
		else if (ret_poll < 0)
		{
			throw(std::logic_error("ERROR:: poll() errror"));
		}
	}
}