#pragma once

enum SESSION_STATE { ST_FREE, ST_WAIT, ST_INGAME };
class SESSION {
	OVER_EXP session_recv_over;

public:
	std::mutex session_lock;
	SESSION_STATE sessionState;

	int sessionID;

	
	std::vector<PLAYER> players;


public:
	

};