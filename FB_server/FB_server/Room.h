#pragma once
#include "pch.h"


enum ROOM_STATE {r_wait,  r_start, r_play, r_finish};

class ROOM {
public:
	ROOM_STATE room_state;


public:
	ROOM();
	~ROOM();

	void update();

};