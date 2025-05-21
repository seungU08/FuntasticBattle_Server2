#include "pch.h"
#include "FBServer.h"

void FBServer::init()
{
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(FB_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(FB_s_socket, SOMAXCONN);

	int addr_size = sizeof(cl_addr);
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(FB_s_socket), h_iocp, 9999, 0);
	FB_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	FB_a_over._comp_type = OP_ACCEPT;
	AcceptEx(FB_s_socket, FB_c_socket, FB_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &FB_a_over._over);

	int num_threads = std::thread::hardware_concurrency();

	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(ThreadWork, h_iocp);
	for (auto& th : worker_threads)
		th.join();
}

void FBServer::disconnect(int c_id)
{
	int session_id = c_id / 4;
	int player_id = c_id % 4;

	PLAYER& player = sessions[session_id].players[player_id];

	// 소켓 닫기
	closesocket(player.client_socket);

	{
		std::lock_guard<std::mutex> ll(player.player_lock);
		player.p_state = P_FREE;
	}

	// 같은 세션 내 다른 플레이어에게 알리기
	for (PLAYER& player : sessions[session_id].players) {
		if (player.playerID == player_id) continue;

		std::lock_guard<std::mutex> ll(player.player_lock);
		if (player.p_state == P_LOGIN)
			player.send_remove_player_packet(player_id); // 또는 player._id
	}
}


int FBServer::get_new_client_id()
{
	for (int i = 0; i < MAX_SESSION; ++i) {
		std::lock_guard <std::mutex> ll{ sessions[i].session_lock };
		if (sessions[i].sessionState != ST_INGAME)
			for (int j = 0; j < SESSION_PLAYER_MAX; ++j) {
				std::lock_guard <std::mutex> pl{ sessions[i].players[j].player_lock };
				if (sessions[i].players[j].p_state == P_FREE) {
					return i * 4 + j;
				}
			}
			
	}
	return -1;
}

void FBServer::process_packet(int key, char* p)	// 패킷처리
{
	const unsigned char packet_type = p[1];
	switch (packet_type) {
	case C_LOGIN_PACKET:
	{
		
		break;
	}
	case C_MOVE_PACKET: {
		
		
		break;
	}
	default:
		std::cout << "Error Invalid Packet Type\n";
		exit(-1);
	}
}

void FBServer::do_send()
{
}

void FBServer::do_recv()
{
}

void FBServer::ThreadWork()
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT) std::cout << "Accept Error";
			else {
				std::cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		if ((0 == num_bytes) && ((ex_over->_comp_type == OP_RECV) || (ex_over->_comp_type == OP_SEND))) {
			disconnect(static_cast<int>(key));
			if (ex_over->_comp_type == OP_SEND) delete ex_over;
			continue;
		}

		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			int client_id = get_new_client_id();
			if (client_id != -1) {
				{
					std::lock_guard<std::mutex> ll(sessions[client_id/4].session_lock);
					sessions[client_id/4].players[client_id%4].p_state = P_ALLOC;
				}
				sessions[client_id / 4].players[client_id % 4].playerID = client_id;
				sessions[client_id / 4].players[client_id % 4].playerName[0] = 0;
				sessions[client_id / 4].players[client_id % 4]._prev_remain = 0;
				sessions[client_id / 4].players[client_id % 4].client_socket = FB_c_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(FB_c_socket),
					h_iocp, client_id, 0);
				sessions[client_id / 4].players[client_id % 4].do_recv();
				FB_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else {
				std::cout << "Max user exceeded.\n";
			}
			ZeroMemory(&FB_a_over._over, sizeof(FB_a_over._over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(FB_s_socket, FB_c_socket, FB_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &FB_a_over._over);
			break;
		}

		case OP_RECV: {
			int remain_data = num_bytes + sessions[key / 4].players[key % 4]._prev_remain;
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);		
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			sessions[key / 4].players[key % 4]._prev_remain = remain_data;
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			sessions[key / 4].players[key % 4].do_recv();
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		}
	}
}

FBServer::FBServer()
{
}

FBServer::~FBServer()
{
	closesocket(FB_s_socket);
	WSACleanup();
}
