
#include "SocketStuff.h"

#include <iostream>
#include <iphlpapi.h>


// Link the project with "Ws2_32.lib".
#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib") //used by get adapters


void getAdapterInfo()
{
    IP_ADAPTER_INFO* pAdapterInfo;
    ULONG            ulOutBufLen;
    DWORD            dwRetVal;

    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    ulOutBufLen = sizeof(IP_ADAPTER_INFO);

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS) { //first call is meant to fail
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != ERROR_SUCCESS) {
        printf("GetAdaptersInfo call failed with %d\n", dwRetVal);
    }

    int anum = 0;
    PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
    while (pAdapter) {
        printf("Adapter # %02d\r\n", anum++);

        printf("Adapter Name: %s\n", pAdapter->AdapterName);
        printf("Adapter Desc: %s\n", pAdapter->Description);
        printf("\tAdapter Addr: \t");
        for (UINT i = 0; i < pAdapter->AddressLength; i++) {
            if (i == (pAdapter->AddressLength - 1))
                printf("%.2X\n", (int)pAdapter->Address[i]);
            else
                printf("%.2X-", (int)pAdapter->Address[i]);
        }
        printf("IP Address: %s\n", pAdapter->IpAddressList.IpAddress.String);
        printf("IP Mask: %s\n", pAdapter->IpAddressList.IpMask.String);
        printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
        printf("\t***\n");

        if (pAdapter->DhcpEnabled) {
            printf("\tDHCP Enabled: Yes\n");
            printf("\t\tDHCP Server: \t%s\n", pAdapter->DhcpServer.IpAddress.String);
        }
        else
            printf("\tDHCP Enabled: No\n");

        pAdapter = pAdapter->Next;
        Sleep(1);
    }


    if (pAdapterInfo)  free(pAdapterInfo);

}

bool initialise_winsock()
{
    WSADATA ws;
    printf("Initialising Winsock...\r\n");
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
    {
        printf("Failed. Error Code: %d\r\n", WSAGetLastError());
        return true;
    }
    printf("         Winsock Initialized...\r\n");
    return false;
}

void closeandclean_winsock()
{
    WSACleanup();
}

int isValidIp4(char* str) {
    int segs = 0;   /* Segment count. */
    int chcnt = 0;  /* Character count within segment. */
    int accum = 0;  /* Accumulator for segment. */

    /* Catch NULL pointer. */
    if (str == NULL)
        return 0;
    /* Process every character in string. */
    while (*str != '\0') {
        /* Segment changeover. */
        if (*str == '.') {
            /* Must have some digits in segment. */
            if (chcnt == 0)
                return 0;
            /* Limit number of segments. */
            if (++segs == 4)
                return 0;
            /* Reset segment values and restart loop. */
            chcnt = accum = 0;
            str++;
            continue;
        }
        /* Check numeric. */
        if ((*str < '0') || (*str > '9'))
            return 0;
        /* Accumulate and check segment. */
        if ((accum = accum * 10 + *str - '0') > 255)
            return 0;
        /* Advance other segment specific stuff and continue loop. */
        chcnt++;
        str++;
    }
    /* Check enough segments and enough characters in last segment. */
    if (segs != 3)
        return 0;
    if (chcnt == 0)
        return 0;
    /* Address okay. */
    return 1;
}

static wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}

SOCKET OpenTCPServerSocket(char* host, u_short port)
{
    printf("Trying to open TCP Socket %s:%d...\r\n", host, port);
    SOCKET TCPClientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (TCPClientSocket == INVALID_SOCKET) {
        printf("Error, cannot create TCP socket in OpenTCPServerSocket()\r\n");
        return EXIT_FAILURE;
    }

    // setup address structure
    SOCKADDR_IN  addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host);
    addr.sin_port = htons(port);

    // Attemp to connect to server and exit on failure. 
    int connval = connect(TCPClientSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (connval == SOCKET_ERROR) {
        printf("Error: cannot connect to server in OpenTCPServerSocket()\r\n");
        //Returns status code other than zero
        return EXIT_FAILURE;
    }
    printf("Created and connected to TCP Server at %s:%d\r\n", host, port);
    return TCPClientSocket;
}


bool UDPSocket::OpenSocket(char *host, int port)
{
    if ((m_client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) // <<< UDP socket
    {
        printf("socket() failed with error code: %d", WSAGetLastError());
        return true;
    }
    // setup address structure
    memset((char*)&m_sockaddr_in, 0, sizeof(m_sockaddr_in));
    m_sockaddr_in.sin_family = AF_INET;
    m_sockaddr_in.sin_port = htons(port);
    m_sockaddr_in.sin_addr.S_un.S_addr = inet_addr(host);
    b_Open = true;
    return false;
}

bool UDPSocket::CloseSocket()
{
    closesocket(m_client_socket);
    b_Open = false;
    return false;
}

bool UDPSocket::SendUDP(char* buffer, int len)
{
    if (sendto(m_client_socket, (const char*)buffer, len, 0, (sockaddr*)&m_sockaddr_in, sizeof(sockaddr_in)) == SOCKET_ERROR)
    {
        printf("sendto() failed to send packet to %d with error code: %d\r\n", Port,  WSAGetLastError());
        return true;
    }
    return false;
}



MulticastSocket::MulticastSocket()
{
    printf("In MulticastSocket::Constructor\r\n");
}

MulticastSocket::~MulticastSocket()
{
    printf("In MulticastSocket::Destructor()\r\n");
    CleanUp();
}


void MulticastSocket::CleanUp()
{
    printf("In MulticastSocket::CleanUp()\r\n");

    // Join Reception Thread with Main Thread.
    if (m_isReceptionThreadStarted == true)
    {
        m_threadJoinRequested = true;
        m_receptionThread.join();
    }

    // Leave Multicast Group.
    LeaveGroup();

    // Close Socket.
    if (m_isSocketCreated) closesocket(m_socketHandle);

    // Clean-up Windows Sockets.
    //if (m_isSocketInitialized == true) WSACleanup();

    // Reset flags.
    m_isReceptionThreadStarted = false;
    m_isSocketCreated = false;
    m_isSocketInitialized = false;
}



bool MulticastSocket::Intialize(const char* multicastGroupString, int multicastPort, unsigned short loopBack, unsigned short ttl,
    std::function<void(char* message, int messageSize)> onReceive)
{

    /*
    // Initialize Windows Sockets v2.2.
    WSADATA wsaData{};

    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        std::cout << "WSAStartup() failed with error " << GetLastError() << std::endl;
        return false;
    }
    */

    m_isSocketInitialized = false;

    // Create Socket Handle: (Family: IPv4, Socket Type: Datagram, Protocol: UDP)
    m_socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (m_socketHandle == SOCKET_ERROR)
    {
        std::cout << "socket() failed with error " << GetLastError() << std::endl;
        CleanUp();
        return false;
    }

    // Socket is created.
    m_isSocketCreated = true;

    // Allow address reusability for other Apps.
    unsigned int allowReuse{ 1 };

    int result = setsockopt(
        m_socketHandle,
        SOL_SOCKET,
        SO_REUSEADDR,
        (char*)&allowReuse,
        sizeof(allowReuse));

    if (result == SOCKET_ERROR)
    {
        std::cout << "setsockopt() failed for socket reuse with error " << WSAGetLastError() << std::endl;
        CleanUp();
        return false;
    }

    // Bind socket with any Interface Address.
    ZeroMemory(&m_bindAddress, sizeof(m_bindAddress));

    m_bindAddress.sin_family = AF_INET;
    m_bindAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    m_bindAddress.sin_port = htons((unsigned short)multicastPort);

    result = bind(m_socketHandle, (struct sockaddr*)&m_bindAddress, sizeof(m_bindAddress));

    if (result == SOCKET_ERROR) {
        std::cout << "bind() failed with error " << WSAGetLastError() << std::endl;
        CleanUp();
        return false;
    }

    // Join Multicast Group.
    unsigned long ipV4address{};
    inet_pton(AF_INET, multicastGroupString, &ipV4address);

    m_multicastGroupConfiguration.imr_multiaddr.s_addr = ipV4address;
    m_multicastGroupConfiguration.imr_interface.s_addr = htonl(INADDR_ANY);

    result = setsockopt(
        m_socketHandle,
        IPPROTO_IP,
        IP_ADD_MEMBERSHIP,
        (char*)&m_multicastGroupConfiguration,
        sizeof(m_multicastGroupConfiguration));

    if (result == SOCKET_ERROR) {
        std::cout << "setcsockopt() failed for joining group with error " << WSAGetLastError() << std::endl;
        CleanUp();
        return false;
    }

    // Set Loop-Back Value.
    result = setsockopt(
        m_socketHandle,
        IPPROTO_IP,
        IP_MULTICAST_LOOP,
        (char*)&loopBack,
        sizeof(loopBack));

    if (result == SOCKET_ERROR) {
        std::cout << "setcsockopt() failed for loop-back with error " << WSAGetLastError() << std::endl;
        CleanUp();
        return false;
    }

    // Set TTL.
    result = setsockopt(
        m_socketHandle,
        IPPROTO_IP,
        IP_MULTICAST_TTL,
        (char*)&ttl,
        sizeof(ttl));

    if (result == SOCKET_ERROR) {
        std::cout << "setcsockopt() failed for TTL with error " << WSAGetLastError() << std::endl;
        CleanUp();
        return 1;
    }

    // Assign Callback.
    m_onReceive = onReceive;

    // Create Reception Thread.
    m_receptionThread = std::thread([this](void* threadParams) {
        // Register Notification for Network Event: FD_READ.
        WSAEVENT networkEvent = WSACreateEvent();

        int result = WSAEventSelect(
            m_socketHandle,
            networkEvent,
            FD_READ);

        if (result == SOCKET_ERROR)
        {
            std::cout << "WSAEventSelect() failed with error " << WSAGetLastError() << std::endl;
            std::cout << "Termintating Reception Thread after failing to Select Network Events." << std::endl;
            return;
        }

        // Handle Network Events.
        while (true)
        {
            // If Main Thread is waiting for Reception Thread to Join.
            if (m_threadJoinRequested == true)
            {
                std::cout << "Termintating Reception Thread after Join Requested." << std::endl;
                break;
            }

            // Receive Notification of Network Events.
            WSANETWORKEVENTS networkEvents{};
            WSAEnumNetworkEvents(m_socketHandle, networkEvent, &networkEvents);

            // Check if FD_READ has been notified.
            if (networkEvents.lNetworkEvents & FD_READ)
            {
                if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
                {
                    std::cout << "FD_READ failed with error " << networkEvents.iErrorCode[FD_READ_BIT] << std::endl;
                    continue;
                }
                else
                {
                    // Lock Socket Handle while receving data.
                    m_socketLock.lock();

                    char messageBuffer[BUFFER_SIZE];

                    struct sockaddr_in sourceAddress {};
                    int addressLength = sizeof(sourceAddress);

                    // Receive bytes.
                    int bytesRead = recvfrom(
                        m_socketHandle,
                        messageBuffer,
                        BUFFER_SIZE,
                        0,
                        (struct sockaddr*)&sourceAddress,
                        &addressLength);

                    if (bytesRead < 0) {
                        std::cout << "recvfrom() failed with error " << GetLastError() << std::endl;
                        m_socketLock.unlock();
                        continue;
                    }
                    else
                    {
                        // Terminate Byte Stream.
                        messageBuffer[std::abs(bytesRead)] = '\0';
                    }

                    // Reception Callback.
                    if (m_onReceive)
                        m_onReceive(messageBuffer, bytesRead);

                    // Unlock Socket Handle.
                    m_socketLock.unlock();
                }
            }
        }
        }, nullptr);

    m_isReceptionThreadStarted = true;

    // Create Multicast Group Address.
    ZeroMemory(&m_multicastGroupAddress, sizeof(m_multicastGroupAddress));

    m_multicastGroupAddress.sin_family = AF_INET;
    m_multicastGroupAddress.sin_addr.s_addr = ipV4address;
    m_multicastGroupAddress.sin_port = htons(multicastPort);

    return true;
}

bool MulticastSocket::LeaveGroup()
{
    // Return if Socket is not created.
    if (m_isSocketCreated == false) return false;

    // Return if Multicast Group is not joined.
    if (m_isGroupJoined == false) return false;

    // Leave Group.
    int result = setsockopt(
        m_socketHandle,
        IPPROTO_IP,
        IP_DROP_MEMBERSHIP,
        (char*)&m_multicastGroupConfiguration,
        sizeof(m_multicastGroupConfiguration));

    if (result == SOCKET_ERROR)
    {
        std::cout << "setcsockopt() failed for leaving group with error " << WSAGetLastError() << std::endl;
        m_isGroupJoined = false;
        return false;
    }

    // Update flag.
    m_isGroupJoined = false;

    return true;
}

bool MulticastSocket::Send(char* message, int messageSize, int& bytesSent)
{
    // Return if socket is not created.
    if (m_isSocketCreated == false)  return false;

    // Lock Socket Handle for transmission.
    m_socketLock.lock();

    // Send bytes.
    bytesSent = sendto(
        m_socketHandle,
        message,
        messageSize,
        0,
        (struct sockaddr*)&m_multicastGroupAddress,
        (int)sizeof(m_multicastGroupAddress));


    if (bytesSent < 0) {
        std::cout << "sendto() failed with error " << WSAGetLastError() << std::endl;
        m_socketLock.unlock();
        return false;
    }

    std::cout << "Bytes Sent: " << bytesSent << std::endl;
    std::cout << "Message: " << message << std::endl << std::endl;

    // Unlock Socket Handle.
    m_socketLock.unlock();

    return true;
}
