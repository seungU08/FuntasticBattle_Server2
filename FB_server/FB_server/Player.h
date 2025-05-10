#pragma once


enum PLAYER_STATE{P_FREE,P_ALLOC, P_LOGIN, P_READY};
enum PLAYER_MOVE_STATE{ P_STAND, P_MOVE, P_JUMP, P_RUSH, P_DIE };

class PLAYER {

public:
	SOCKET client_socket;
	char client_ID[20];
	int playerNUM;
	std::mutex player_lock;
	int playerID;
	int _prev_remain;

	char playerName[NAME_SIZE];

	float Xpos;
	float Ypos;
	float Zpos;

	PLAYER_STATE p_state;
	PLAYER_MOVE_STATE p_m_state;
	float p_health;
	float p_stamina;

public:
	PLAYER();
	~PLAYER();

	void collision();

	void send_remove_player_packet(int);

	void do_recv();

};