// ��Ʈ��ũ �⺻ �ڵ� ������ ������..

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>	
#include <winsock2.h>	
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void main()
{
	WSADATA wsadata;

	if ( WSAStartup( MAKEWORD(2,2), &wsadata) != 0 )
	{
		printf("Can't Initialize Socket !\n");
		return ;
	}
	//--------------------------------------------
	char url[256] = { 0 };
	printf("������ URL �� �־��ּ��� >> ");
	scanf("%s", url );

	// URL -> IP �� �����ϱ�.
	HOSTENT* p = gethostbyname( url );
	
	int index = 0;

	while ( p->h_addr_list[index] )
	{
		in_addr addr = *((in_addr*)(p->h_addr_list[index]));

		printf("IP Address : %s\n", inet_ntoa( addr ) );

		index++;
	}
	//------------------------------------------------
	in_addr serverip = *((in_addr*)(p->h_addr_list[0]));

	SOCKET s = socket( AF_INET, SOCK_STREAM, 0);

	// ���� ���� ������ ���� �Ŀ��� TIME_WAIT ��ŭ ��� �ϹǷ� 
	// ������ ��� �ٽ� ���������� bind()�� ���� �Ѵ�.(������/unix ����)

	// ���� ������ �ٷ� ��õ� �Ҽ� �ְ� �Ϸ��� �Ʒ� Option �� �����ؾ� �Ѵ�.
	BOOL b = TRUE;

	setsockopt( s,  SOL_SOCKET,  // option level
					SO_REUSEADDR, // option name
					(char*)&b,    // option ���� �� 
					sizeof(b) );  // option ���� ũ��.


	// ���� ���۸� Ű����� ���� option�� �����Ѵ�.
	int sz = 8192;
	setsockopt( s, SOL_SOCKET,	SO_RCVBUF, (char*)&sz, sizeof(sz));

	// UDP client �� BROADCAST �� �����ϰ� �ϴ� option(x.x.x.255)
	BOOL bBroad = TRUE;
	setsockopt( s, SOL_SOCKET, SO_BROADCAST, (char*)&bBroad, sizeof( bBroad));


	SOCKADDR_IN addr = { 0 };
	addr.sin_family		= AF_INET;
	addr.sin_port		= htons(80);	// web server port ��ȣ.
	addr.sin_addr		= serverip;


	if ( connect( s, (SOCKADDR*)&addr, sizeof( addr ) ) == -1 )
	{
		printf("������ �����Ҽ� �����ϴ�.\n");
		closesocket( s );
		return ;
	}
	printf("������ ���ӵǾ����ϴ�.\n");

	// ������ HTTP ��û ��Ŷ�� ������.
	char buf[256] = "GET / HTTP/1.0\r\n\r\n";  // \r\n\r\n �� ������ �׷����ϴ�.
												// �̷��� ���� ������.

	send( s, buf, strlen(buf)+1, 0);

	// �������� ���� data �� �����Ѵ�.
	char data[8192] = { 0 };
	recv( s, data, 8192, 0);

	printf("%s\n", data );

	closesocket( s );
	//--------------------------------------------
	WSACleanup();
}











