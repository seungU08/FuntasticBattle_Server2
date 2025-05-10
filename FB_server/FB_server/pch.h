#pragma once
#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_set>
#include "protocol.h"
#include "Player.h"
#include "Room.h"
#include "FBServer.h"
#include "FBSession.h"
#include "utils.h"


#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")