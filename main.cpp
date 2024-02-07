#include "Server.hpp"

void	error_handling(std::string str)
{
	std::cout << str << std::endl;
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cout << "Warning :: Argument is only two" << std::endl;
		return (1);
	}
	try{
		Server server(argv[1], argv[2]);
		server.run();
	}catch(std::exception &e){
		std::cout << e.what() << std::endl;
	}


	// server는 => irssi인증 되도록.
	// pong을 보내줘야야함.
	// client 객체 만들기 => 멤버변수로 뭐가 필요하지.. 버퍼.. 어케할지 몰랑
	// server객체가 std::map<fd, client> client 멤버변수로;
	// --토욜까지 성호
	// 승준형 -> 담주 화욜까지 공부(에코서버 만들어보기)
	// 승준형이 나랑 클라이언트객체 버퍼 좀 정리..
	// 지현 -> 담주화욜까지 09 끝내기

	// client객체가 read,write버퍼존재.
	// "/r/n"
	// 연결된 client를 다돌면서 체크해야하는 부분이 존재.
	// Channel 객체 만들기 => 그냥 모르겠음.. 커맨드 할 때.. 같이 고려
	// 커맨드 파싱해서 실행해야댐.
	return (0);
}