constexpr int PORT_NUM = 7777;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;
constexpr int MAX_PLAYER = 600;
constexpr int SESSION_PLAYER_MAX = 4;
constexpr int MAX_SESSION = MAX_PLAYER / SESSION_PLAYER_MAX;




#pragma pack (push , 1)

struct c_login_Packet
{
	unsigned char size;
	char type;
	char name[NAME_SIZE];
};

struct c_move_Packet
{
	unsigned char size;
	char type;

};

struct c_player_Packet
{
	unsigned char size;
	char type;

};

struct s_player_Packet
{
	unsigned char size;
	char type;

};
struct s_enemy_Packet
{
	unsigned char size;
	char type;

};


#pragma pack (pop)