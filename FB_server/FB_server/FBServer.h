#pragma once

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND };
class OVER_EXP {
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	COMP_TYPE _comp_type;
	OVER_EXP()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		_comp_type = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}
	OVER_EXP(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		_comp_type = OP_SEND;
		memcpy(_send_buf, packet, packet[0]);
	}
};

class FBServer {
public:
	HANDLE h_iocp;
	SOCKADDR_IN server_addr, cl_addr;
	SOCKET FB_s_socket, FB_c_socket;

	OVER_EXP FB_a_over;


	std::array<SESSION, MAX_SESSION> sessions;
	std::vector<ROOM> rooms;
	std::vector<std::thread> worker_threads;


public:

	void init();

	void disconnect(int);

	int get_new_client_id();

	void find_sessionID_playerindex_by_clientID(int);

	void do_send();
	void do_recv();

	void ThreadWork();

public:
	FBServer();
	~FBServer();

	

};