#ifndef SOCKETSTUFF_H
#define SOCKETSTUFF_H

#include <winsock2.h>

#include <Ws2tcpip.h>

#include <functional>
#include <thread>
#include <mutex>


constexpr auto BUFFER_SIZE = 1510;

void getAdapterInfo();
bool initialise_winsock();
void closeandclean_winsock();
int isValidIp4(char* str);

SOCKET OpenTCPServerSocket(char* host, u_short port); //SOCKET is unsigned __int64


struct UDPSocket
{
	sockaddr_in m_sockaddr_in;
	SOCKET m_client_socket;
	bool isMulticast = false;
	bool isBroadcast = false;
	bool b_Open = false;

	char	IP[25] = "239.255.1.1";
	int		Port = 7002;

	bool OpenSocket(char* host, int port);
	bool CloseSocket();
	bool SendUDP(char* buffer, int len);
};



class MulticastSocket
{
private:
	SOCKET m_socketHandle{};

	struct sockaddr_in m_bindAddress {};
	struct sockaddr_in m_multicastGroupAddress {};

	struct ip_mreq m_multicastGroupConfiguration {};

	std::mutex m_socketLock{};
	std::thread m_receptionThread{};

	bool m_isSocketInitialized{ false };
	bool m_isSocketCreated{ false };
	bool m_isGroupJoined{ false };
	bool m_isReceptionThreadStarted{ false };
	bool m_threadJoinRequested{ false };

	std::function<void(char* message, int messageSize)> m_onReceive;



public:
	MulticastSocket();
	virtual ~MulticastSocket();
	void CleanUp();

	bool Intialize(
		const char* multicastGroupString,
		int multicastPort,
		unsigned short loopBack,
		unsigned short ttl,
		std::function<void(char* message, int messageSize)> onReceive);

	bool LeaveGroup();
	bool Send(char* message, int messageSize, int& bytesSent);
};







#endif



