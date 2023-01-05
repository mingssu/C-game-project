#pragma warning (disable:4996)

#include<stdio.h>
#include<windows.h>
#include<time.h>
#include<conio.h>
#include<math.h>
#pragma comment (lib, "winmm.lib")
#include <mmsystem.h>

//size of gameboard

#define GBOARD_WIDTH 36
#define GBOARD_HEIGHT 21
//starting point of gameboard
#define GBOARD_ORIGIN_X 4
#define GBOARD_ORIGIN_Y 5

//SelectedCard_window
#define CARD_WIDTH 36
#define CARD_HEIGHT 10
#define CARD_ORIGIN_X 4
#define CARD_ORIGIN_Y 28

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define ENTER 13
#define CANCEL 99
#define SPACE 32

#define QKEY 113
#define WKEY 119
#define EKEY 101
#define RKEY 114
#define AKEY 97
#define DKEY 100
#define SKEY 115

COORD curPos;
int PickConsolePos_X;
int PickConsolePos_Y;
int PlayerConsolePos_X;
int PlayerConsolePos_Y;
int NpcConsolePos_X;
int NpcConsolePos_Y;

int stage = 1;
int screen = 0;
int player_index = 0;
int npc_index = 0;
int first_check = 0; // 처음에만 스토리 뜨게하기 위한 변수
int cleared_check = 0; //클리어해서 타이틀로 돌아왔는지 확인
char p_card[3];
char n_card[3];
int npc_action[10] = { RIGHT, LEFT, UP, DOWN, AKEY, QKEY, WKEY, EKEY, RKEY, DKEY }; //차례로 상하좌우 (랜덤으로 뽑기 위해 인덱스로 관리함)

char Gameboardinfo[5][6];
COORD playerPos;
COORD npcPos;

int playerHP = 100;
int playerMP = 100;
int npcHP = 100;
int npcMP = 100;

int flag_stage1 = 0; // stage1 첫 턴의 첫 이동 구별용
int flag_stage3 = 0; // stage3 첫 턴의 첫 이동 구별용
int checkPlayerLastCard; // 플레이어의 마지막 카드가 공격카드면 1 아니면 0

int isPlayerLeft = 1;	//npc에 대한 player의 상대위치 1: player 왼쪽, 0: 오른쪽
int overlap = 0;		// 겹쳐있는지 안겹쳐 있는지 0: 안겹쳐있음, 1: 겹쳐있음

int doubleJumpDirect = 0;
int result_flag = 0; // 승부 확인용, 0은 게임 진행중 1은 승리 2는 패배 3은 부승부
void drawTitle();
void drawcredit();
void drawmanaul();

typedef struct Character {
	char name[10];

	int q_dm;
	int q_mp;
	int q_scope[3][3];

	int w_dm;
	int w_mp;
	int w_scope[3][3];

	int e_dm;
	int e_mp;
	int e_scope[3][3];

	int r_dm;
	int r_mp;
	int r_scope[3][3];

	int ch[4][4];	//오른쪽 보는 모습
	int re_ch[4][4];	//왼쪽 보는 모습

	int guard;

	char effect;

	int color;
}Character;

Character character[4] = {
   {   //도크
	"Dogk",
	30, 15, { {0, 1, 0}, {0, 1, 0}, {0, 1, 0} },
	50, 50, { {0, 0, 0}, {0, 1, 0}, {1, 1, 1} },
	25, 25, { {0, 1, 0}, {1, 1, 1}, {0, 1, 0} },
	25, 35, { {1, 0, 1}, {0, 1, 0}, {1, 0, 1} },
	{ {'&',0,'&',0},
	 {0,'o',0,'o'},
	 {0,0,'P','P'},
	 {0,'f','f',0} },

	{ {0,'&',0,'&'},
	 {'o',0,'o',0},
	 {'P','P',0,0},
	 {0,'f','f',0} },
	 0, '#',12
   },

   {   //캐럿
	"Carat",
	60, 70, { {0, 0, 0}, {1, 0, 1}, {0, 0, 0} },
	20, 15, { {0, 0, 0}, {1, 1, 1}, {1, 1, 1} },
	40, 50, { {0, 1, 0}, {1, 1, 1}, {0, 1, 0} },
	25, 20, { {1, 0, 1}, {0, 1, 0}, {1, 0, 1} },
	{ {'t',0,'t',0},
	 {0,'u',0,'u'},
	 {0,0,'>','>'},
	 {0,'T','T',0} },

	{ {0,'t',0,'t'},
	 {'u',0,'u',0},
	 {'<','<',0,0},
	 {0,'T','T',0} },
	 0, '&',13
   },


   {   //바니
	"Bunny",
	50, 50, { {1, 0, 1}, {0, 1, 0}, {0, 0, 0} },
	15, 15, { {1, 1, 1}, {1, 1, 1}, {1, 1, 1} },
	25, 15, { {0, 0, 0}, {1, 1, 1}, {0, 0, 0} },
	25, 25, { {0, 1, 0}, {1, 1, 1}, {0, 1, 0} },
	{ {'A',0,'A',0},
	 {0,'b',0,'b'},
	 {0,0,'B','B'},
	 {0,'v','v',0} },

	{ {0,'A' ,0,'A'},
	 {'d',0,'d',0},
	 {'B','B',0,0},
	 {0,'v','v',0} },
	 0, '/',11
   },

   {   //너츠
	"Nuts",
	40, 50, { {1, 0, 1}, {1, 1, 1}, {1, 0, 1} },
	25, 30, { {0, 1, 0}, {1, 1, 1}, {0, 1, 0} },
	25, 15, { {0, 0, 0}, {1, 1, 1}, {0, 0, 0} },
	25, 30, { {1, 0, 1}, {0, 1, 0}, {1, 0, 1} },
	{ {'Y',0,'Y',0},
	 {0,'o',0,'o'},
	 {0,0,'r','i'},
	 {0,'L','L',0} },

	{ {0,'Y',0,'Y'},
	 {'o',0,'o',0},
	 {'i','r',0,0},
	 {0,'L','L',0} },
	 0, '@',9
   }

};

COORD GetCurrentCursorPos()
{
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;

	return curPoint;
}
void DeleteSelectedMoveCard(int n);
void SetCurrentCursorPos(int x, int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void RemoveCursor()
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

//글자 색 바꾸기 위한 함수 핸들선언
void textcolor(int color_number)

{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_number);
}

//게임 판 길이가 가로 36, 세로 21이면 실제 내부 한 칸의 크기는 가로 8 세로 6임 
void DrawGameBoard()
{
	COORD cur = GetCurrentCursorPos();

	int x, y;

	for (x = 0; x <= GBOARD_WIDTH; x++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y);
		if (x == 0)
			printf("┌");
		else if (x == GBOARD_WIDTH)
			printf("┐");
		else if (x % 9 == 0)
			printf("┬");
		else
			printf("─");
	}
	for (x = 0; x <= GBOARD_WIDTH; x++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT);
		if (x == 0)
			printf("└");
		else if (x == GBOARD_WIDTH)
			printf("┘");
		else if (x % 9 == 0)
			printf("┴");
		else
			printf("─");
	}
	for (y = 1; y < GBOARD_HEIGHT; y++)
	{
		for (x = 0; x <= GBOARD_WIDTH; x += 9)
		{
			SetCurrentCursorPos(GBOARD_ORIGIN_X + (2 * x), GBOARD_ORIGIN_Y + y);
			if (y == GBOARD_HEIGHT - 1)
				printf("│");
			else if (y == 1)
				printf("│");
			else if (y % 7 == 0 && x == 0)
				printf("├");
			else if (y % 7 == 0 && x == GBOARD_WIDTH)
				printf("┤");
			else if (y % 7 == 0)
				printf("┼");
			else
				printf("│");
		}
	}

	for (x = 1; x < GBOARD_WIDTH; x++)
	{
		for (y = 1; y < GBOARD_HEIGHT - 1; y++)
		{
			SetCurrentCursorPos(GBOARD_ORIGIN_X + (2 * x), GBOARD_ORIGIN_Y + y);
			if (y % 7 == 0 && x % 9 != 0)
				printf("─");
		}
	}


	SetCurrentCursorPos(cur.X, cur.Y);
}

void DrawSelectedCard()
{
	int x, y;

	for (x = 0; x <= CARD_WIDTH; x++)
	{
		SetCurrentCursorPos(CARD_ORIGIN_X + x * 2, CARD_ORIGIN_Y);
		if (x == 0)
			printf("┌");
		else if (x == CARD_WIDTH)
			printf("┐");
		else
			printf("─");
	}
	for (x = 0; x <= CARD_WIDTH; x++)
	{
		SetCurrentCursorPos(CARD_ORIGIN_X + x * 2, CARD_ORIGIN_Y + CARD_HEIGHT - 1);
		if (x == 0)
			printf("└");
		else if (x == CARD_WIDTH)
			printf("┘");

		else
			printf("─");
	}

	for (y = 1; y < CARD_HEIGHT - 1; y++)
	{
		SetCurrentCursorPos(CARD_ORIGIN_X, CARD_ORIGIN_Y + y);
		printf("│");
	}

	for (y = 1; y < CARD_HEIGHT - 1; y++)
	{
		SetCurrentCursorPos(CARD_ORIGIN_X + (2 * CARD_WIDTH), CARD_ORIGIN_Y + y);
		printf("│");
	}

	//----내부 사각형----

	for (x = 1; x < CARD_WIDTH; x++)
	{
		if ((x == 3) || (x == 11) || (x == 14) || (x == 22) || (x == 25) || (x == 33))
		{
			for (y = 2; y <= CARD_HEIGHT - 2; y++)
			{
				SetCurrentCursorPos(CARD_ORIGIN_X + (2 * x), CARD_ORIGIN_Y + y);
				printf("│");
			}
		}
	}

	for (y = 1; y <= CARD_HEIGHT; y++)
	{
		if (y == 1 || y == 8)
		{
			for (x = 3; x <= 11; x++)
			{
				SetCurrentCursorPos(CARD_ORIGIN_X + (2 * x), CARD_ORIGIN_Y + y);
				if (y == 1 && x == 3)
				{
					printf("┌");
				}
				else if (y == 1 && x == 11)
				{
					printf("┐");
				}
				else if (y == 8 && x == 3)
				{
					printf("└");
				}
				else if (y == 8 && x == 11)
				{
					printf("┘");
				}
				else
					printf("─");
			}

			for (x = 14; x <= 22; x++)
			{
				SetCurrentCursorPos(CARD_ORIGIN_X + (2 * x), CARD_ORIGIN_Y + y);
				if (y == 1 && x == 14)
				{
					printf("┌");
				}
				else if (y == 1 && x == 22)
				{
					printf("┐");
				}
				else if (y == 8 && x == 14)
				{
					printf("└");
				}
				else if (y == 8 && x == 22)
				{
					printf("┘");
				}
				else
					printf("─");
			}
			for (x = 25; x <= 33; x++)
			{
				SetCurrentCursorPos(CARD_ORIGIN_X + (2 * x), CARD_ORIGIN_Y + y);
				if (y == 1 && x == 25)
				{
					printf("┌");
				}
				else if (y == 1 && x == 33)
				{
					printf("┐");
				}
				else if (y == 8 && x == 25)
				{
					printf("└");
				}
				else if (y == 8 && x == 33)
				{
					printf("┘");
				}
				else
					printf("─");
			}
		}
	}

}

void fence1(int Origin_X, int Origin_Y) { // 작은 테두리 ( 이동, Recovery, Defense 테두리)
	SetCurrentCursorPos((Origin_X + 2) * 2, Origin_Y + 2);
	printf("┌");
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 2);
	printf("─");
	SetCurrentCursorPos((Origin_X + 4) * 2, Origin_Y + 2);
	printf("┐");
	SetCurrentCursorPos((Origin_X + 2) * 2, Origin_Y + 3);
	printf("│");
	SetCurrentCursorPos((Origin_X + 4) * 2, Origin_Y + 3);
	printf("│");
	SetCurrentCursorPos((Origin_X + 2) * 2, Origin_Y + 4);
	printf("└");
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 4);
	printf("─");
	SetCurrentCursorPos((Origin_X + 4) * 2, Origin_Y + 4);
	printf("┘");
}

//Draw_Defense/Draw_Recovery는 origin위치를 받아서 종합할 때 수정
void Draw_Defense()
{
	int Origin_X = 40;
	int Origin_Y = 20;
	SetCurrentCursorPos(Origin_X * 2 + 8, Origin_Y);
	printf("Defense");
	SetCurrentCursorPos(Origin_X * 2, Origin_Y + 3);
	printf("A");

	fence1(Origin_X, Origin_Y);
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 3);
	printf("▣");

	SetCurrentCursorPos((Origin_X + 6) * 2, Origin_Y + 3);
	printf("DM -15");


}

void HP(int Origin_X, int Origin_Y) {
	SetCurrentCursorPos(Origin_X * 2, Origin_Y + 3);
	printf("S");

	fence1(Origin_X, Origin_Y);
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 3);
	printf("♥");

	SetCurrentCursorPos((Origin_X + 6) * 2, Origin_Y + 3);
	printf("HP + 30");
}

void MP(int Origin_X, int Origin_Y) {
	SetCurrentCursorPos(Origin_X * 2, Origin_Y + 3);
	printf("D");

	fence1(Origin_X, Origin_Y);
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 3);
	printf("★");

	SetCurrentCursorPos((Origin_X + 6) * 2, Origin_Y + 3);
	printf("MP + 15");
}

void HPEffect(int plus) {
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 8, 1);
	textcolor(12);
	int num_playerHPbar = playerHP / 5;
	for (int i = 0; i < num_playerHPbar; i++)
	{
		printf("l");
	}
	textcolor(13);
	if (playerHP + plus > 100) {
		int dif = 100 - playerHP;
		for (int i = 0; i < dif / 5; i++) {
			printf("l");
			Sleep(100);
		}
	}
	else {
		for (int i = 0; i < plus / 5; i++) {
			printf("l");
			Sleep(100);
		}
	}
}   //plus만큼 회복 되는 이펙트

void MPEffect_player(int plus) {
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 8, 2);
	textcolor(9);
	int num_playerMPbar = playerMP / 5;
	for (int i = 0; i < num_playerMPbar; i++)
	{
		printf("l");
	}
	textcolor(11);
	if (playerMP + plus > 100) {
		int dif = 100 - playerMP;
		for (int i = 0; i < dif / 5; i++) {
			printf("l");
			Sleep(100);
		}
	}
	else {
		for (int i = 0; i < plus / 5; i++) {
			printf("l");
			Sleep(100);
		}
	}

	textcolor(15);
}   //plus만큼 회복 되는 이펙트

void MPEffect_npc(int plus) {
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 45, 2);
	textcolor(9);
	int num_npcMPbar = npcMP / 5;
	for (int i = 0; i < num_npcMPbar; i++)
	{
		printf("l");
	}
	textcolor(11);
	if (npcMP + plus > 100) {
		int dif = 100 - npcMP;
		for (int i = 0; i < dif / 5; i++) {
			printf("l");
			Sleep(100);
		}
	}
	else {
		for (int i = 0; i < plus / 5; i++) {
			printf("l");
			Sleep(100);
		}
	}
	textcolor(15);
}   //plus만큼 회복 되는 이펙트

void Draw_Recovery()
{
	int Origin_X = 40;
	int Origin_Y = 28;

	SetCurrentCursorPos(Origin_X * 2 + 8, Origin_Y);
	printf("Recovery");
	SetCurrentCursorPos(Origin_X * 2, Origin_Y + 8);
	printf("C	CANCEL");
	MP(Origin_X, Origin_Y);
}

void Draw_Special_dogk()
{
	int Origin_X = 110;
	int Origin_Y = 30;
	int width = 5;
	int height = 4;

	int x, y;
	SetCurrentCursorPos(Origin_X + 8, Origin_Y - 1);
	printf("Special");
	SetCurrentCursorPos(Origin_X, Origin_Y + 3);
	printf("S");
	SetCurrentCursorPos(Origin_X + 2 * width + 6, Origin_Y + 2);
	printf("Recovery");
	SetCurrentCursorPos(Origin_X + 2 * width + 6, Origin_Y + 4);
	printf("HP +30");
	SetCurrentCursorPos(Origin_X + 2 * width + 6, Origin_Y + 6);
	printf("MP -50");

	for (x = 1; x <= width + 1; x++)
	{
		SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + 1);
		if (x == 1)
			printf("┌");
		else if (x == width + 1)
		{
			printf("┐");
		}
		else
			printf("─");
		SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + height + 2);
		if (x == width + 1)
			printf("┘");
		else
			printf("─");
	}

	for (y = 1; y <= height + 1; y++)
	{
		SetCurrentCursorPos(Origin_X + 2, Origin_Y + y + 1);
		if (y == height + 1)
			printf("└");

		else
			printf("│");

		SetCurrentCursorPos(Origin_X + 2 * (width + 1), Origin_Y + y + 1);
		if (y != height + 1)
			printf("│");

	}


	SetCurrentCursorPos(Origin_X + 2 + 2, Origin_Y + 4);
	printf("●");
	SetCurrentCursorPos(Origin_X + 2 + 2, Origin_Y + 5);
	printf("┠");
	SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 2);
	printf("│");
	SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 3);
	printf("│");
	SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 4);
	printf("┼");



	SetCurrentCursorPos(Origin_X + 2 + 6, Origin_Y + 2);
	printf("─");
	SetCurrentCursorPos(Origin_X + 2 + 8, Origin_Y + 2);
	printf("┐");

	SetCurrentCursorPos(Origin_X + 2 + 6, Origin_Y + 3);
	textcolor(12);
	printf("♥");
	textcolor(15);

	SetCurrentCursorPos(Origin_X + 2 + 8, Origin_Y + 3);
	printf("│");

	SetCurrentCursorPos(Origin_X + 2 + 8, Origin_Y + 4);
	printf("┘");


}

void Draw_Special_carrot()
{
	int Origin_X = 110;
	int Origin_Y = 30;
	int width = 5;
	int height = 4;

	int x, y;
	SetCurrentCursorPos(Origin_X + 8, Origin_Y - 1);
	printf("Special");
	SetCurrentCursorPos(Origin_X, Origin_Y + 3);
	printf("S");
	SetCurrentCursorPos(Origin_X + 2 * width + 6, Origin_Y + 2);
	printf("Double Jump");
	SetCurrentCursorPos(Origin_X + 2 * width + 6, Origin_Y + 4);
	printf("S를 누른 후에");
	SetCurrentCursorPos(Origin_X + 2 * width + 6, Origin_Y + 6);
	printf("방향키로 방향 설정");

	for (x = 1; x <= width + 1; x++)
	{
		SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + 1);
		if (x == 1)
			printf("┌");
		else if (x == width + 1)
		{
			printf("┐");
		}
		else
			printf("─");
		SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + height + 2);
		if (x == width + 1)
			printf("┘");
		else
			printf("─");
	}

	for (y = 1; y <= height + 1; y++)
	{
		SetCurrentCursorPos(Origin_X + 2, Origin_Y + y + 1);
		if (y == height + 1)
			printf("└");

		else
			printf("│");

		SetCurrentCursorPos(Origin_X + 2 * (width + 1), Origin_Y + y + 1);
		if (y != height + 1)
			printf("│");

	}
	SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 4);
	printf("/");
	SetCurrentCursorPos(Origin_X + 2 + 6, Origin_Y + 4);
	printf("＼");
	SetCurrentCursorPos(Origin_X + 2 + 2, Origin_Y + 5);
	printf("↙");
	SetCurrentCursorPos(Origin_X + 2 + 6, Origin_Y + 5);
	printf("↙");
	SetCurrentCursorPos(Origin_X + 2 + 5, Origin_Y + 2);
	printf("○");
	SetCurrentCursorPos(Origin_X + 2 + 5, Origin_Y + 3);
	printf("＃");
	SetCurrentCursorPos(Origin_X + 2 + 3, Origin_Y + 3);
	printf("─");
	SetCurrentCursorPos(Origin_X + 2 + 3, Origin_Y + 2);
	printf("＼");
}

void Draw_Special_bunny()
{
	int Origin_X = 110;
	int Origin_Y = 30;
	int width = 5;
	int height = 4;

	int x, y;
	SetCurrentCursorPos(Origin_X + 8, Origin_Y - 1);
	printf("Special");
	SetCurrentCursorPos(Origin_X, Origin_Y + 3);
	printf("S");
	SetCurrentCursorPos(Origin_X + 2 * width + 6, Origin_Y + 2);
	printf("arrow rain");
	SetCurrentCursorPos(Origin_X + 2 * width + 6, Origin_Y + 4);
	printf("DM 40");
	SetCurrentCursorPos(Origin_X + 2 * width + 6, Origin_Y + 6);
	printf("MP 70");

	for (x = 1; x <= width + 1; x++)
	{
		SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + 1);
		if (x == 1)
			printf("┌");
		else if (x == width + 1)
		{
			printf("┐");
		}
		else
			printf("─");
		SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + height + 2);
		if (x == width + 1)
			printf("┘");
		else
			printf("─");
	}

	for (y = 1; y <= height + 1; y++)
	{
		SetCurrentCursorPos(Origin_X + 2, Origin_Y + y + 1);
		if (y == height + 1)
			printf("└");
		else
			printf("│");

		SetCurrentCursorPos(Origin_X + 2 * (width + 1), Origin_Y + y + 1);
		if (y != height + 1)
			printf("│");
	}

	for (int i = 1; i <= 4; i++)
		for (int j = 1; j <= 4; j++)
		{
			if ((i + j) % 2 == 0)
			{
				SetCurrentCursorPos(Origin_X + 2 + (i * 2), Origin_Y + 1 + j);
				printf("↙");
			}
		}
}

void Draw_Special_nuts()
{
	int Origin_X = 110;
	int Origin_Y = 30;
	int width = 5;
	int height = 4;

	int x, y;
	SetCurrentCursorPos(Origin_X + 8, Origin_Y - 1);
	printf("Special");
	SetCurrentCursorPos(Origin_X, Origin_Y + 3);
	printf("S");
	SetCurrentCursorPos(Origin_X + 2 * width + 6, Origin_Y + 2);
	printf("Power Recovery MP");
	SetCurrentCursorPos(Origin_X + 2 * width + 6, Origin_Y + 4);
	printf("HP -20");
	SetCurrentCursorPos(Origin_X + 2 * width + 6, Origin_Y + 6);
	printf("MP +60");

	for (x = 1; x <= width + 1; x++)
	{
		SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + 1);
		if (x == 1)
			printf("┌");
		else if (x == width + 1)
		{
			printf("┐");
		}
		else
			printf("─");
		SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + height + 2);
		if (x == width + 1)
			printf("┘");
		else
			printf("─");
	}

	for (y = 1; y <= height + 1; y++)
	{
		SetCurrentCursorPos(Origin_X + 2, Origin_Y + y + 1);
		if (y == height + 1)
			printf("└");

		else
			printf("│");

		SetCurrentCursorPos(Origin_X + 2 * (width + 1), Origin_Y + y + 1);
		if (y != height + 1)
			printf("│");

	}


	SetCurrentCursorPos(Origin_X + 2 + 2, Origin_Y + 4);
	printf("●");
	SetCurrentCursorPos(Origin_X + 2 + 2, Origin_Y + 5);
	printf("┠");
	SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 3);
	printf("？");
	SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 4);
	printf("│");
	SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 5);
	printf("│");


	SetCurrentCursorPos(Origin_X + 2 + 6, Origin_Y + 2);
	printf("─");
	SetCurrentCursorPos(Origin_X + 2 + 8, Origin_Y + 2);
	printf("┐");

	SetCurrentCursorPos(Origin_X + 2 + 6, Origin_Y + 3);
	textcolor(9);
	printf("★");
	textcolor(15);

	SetCurrentCursorPos(Origin_X + 2 + 8, Origin_Y + 3);
	printf("│");

	SetCurrentCursorPos(Origin_X + 2 + 8, Origin_Y + 4);
	printf("┘");

}

void shiftRight(int Origin_X, int Origin_Y) {
	SetCurrentCursorPos(Origin_X * 2, Origin_Y + 3);
	printf("→");

	fence1(Origin_X, Origin_Y);
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 3);
	printf("→");

	SetCurrentCursorPos((Origin_X + 6) * 2, Origin_Y + 2);
	printf("Shift Right");
}

void shiftLeft(int Origin_X, int Origin_Y) {
	SetCurrentCursorPos(Origin_X * 2, Origin_Y + 3);
	printf("←");

	fence1(Origin_X, Origin_Y);
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 3);
	printf("←");

	SetCurrentCursorPos((Origin_X + 6) * 2, Origin_Y + 2);
	printf("Shift Left");
}

void shiftUp(int Origin_X, int Origin_Y) {
	SetCurrentCursorPos(Origin_X * 2, Origin_Y + 3);
	printf("↑");

	fence1(Origin_X, Origin_Y);
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 3);
	printf("↑");

	SetCurrentCursorPos((Origin_X + 6) * 2, Origin_Y + 2);
	printf("Shift Up");
}

void shiftDown(int Origin_X, int Origin_Y) {
	SetCurrentCursorPos(Origin_X * 2, Origin_Y + 3);
	printf("↓");

	fence1(Origin_X, Origin_Y);
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 3);
	printf("↓");

	SetCurrentCursorPos((Origin_X + 6) * 2, Origin_Y + 2);
	printf("Shift Down");
}

void Draw_Move() {
	int Origin_X = 40;
	int Origin_Y = 3;

	SetCurrentCursorPos(Origin_X * 2 + 10, Origin_Y);
	printf("Move");

	shiftRight(Origin_X, Origin_Y + 1);
	shiftLeft(Origin_X, Origin_Y + 4);
	shiftUp(Origin_X, Origin_Y + 7);
	shiftDown(Origin_X, Origin_Y + 10);
}

void fence2(int Origin_X, int Origin_Y) {    // 큰 테두리 ( 스킬 아이콘 테두리 )
	SetCurrentCursorPos((Origin_X + 2) * 2, Origin_Y + 1);
	printf("┌");
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 1);
	printf("─");
	SetCurrentCursorPos((Origin_X + 4) * 2, Origin_Y + 1);
	printf("─");
	SetCurrentCursorPos((Origin_X + 5) * 2, Origin_Y + 1);
	printf("─");
	SetCurrentCursorPos((Origin_X + 6) * 2, Origin_Y + 1);
	printf("┐");

	SetCurrentCursorPos((Origin_X + 2) * 2, Origin_Y + 2);
	printf("│");
	SetCurrentCursorPos((Origin_X + 6) * 2, Origin_Y + 2);
	printf("│");

	SetCurrentCursorPos((Origin_X + 2) * 2, Origin_Y + 3);
	printf("│");
	SetCurrentCursorPos((Origin_X + 6) * 2, Origin_Y + 3);
	printf("│");

	SetCurrentCursorPos((Origin_X + 2) * 2, Origin_Y + 4);
	printf("│");
	SetCurrentCursorPos((Origin_X + 6) * 2, Origin_Y + 4);
	printf("│");

	SetCurrentCursorPos((Origin_X + 2) * 2, Origin_Y + 5);
	printf("└");
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 5);
	printf("─");
	SetCurrentCursorPos((Origin_X + 4) * 2, Origin_Y + 5);
	printf("─");
	SetCurrentCursorPos((Origin_X + 5) * 2, Origin_Y + 5);
	printf("─");
	SetCurrentCursorPos((Origin_X + 6) * 2, Origin_Y + 5);
	printf("┘");
}

void Q(int Origin_X, int Origin_Y) {
	SetCurrentCursorPos(Origin_X * 2, Origin_Y + 3);
	printf("Q");

	fence2(Origin_X, Origin_Y);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			SetCurrentCursorPos((Origin_X + 3 + j) * 2, Origin_Y + 2 + i);
			if (character[player_index].q_scope[i][j] == 0) {
				printf("□");
			}
			if (character[player_index].q_scope[i][j] == 1) {
				printf("■");
			}
		}
	}

	SetCurrentCursorPos((Origin_X + 8) * 2, Origin_Y + 2);
	printf("DM %d", character[player_index].q_dm);
	SetCurrentCursorPos((Origin_X + 8) * 2, Origin_Y + 4);
	printf("Mp %d", character[player_index].q_mp);
}

void W(int Origin_X, int Origin_Y) {
	SetCurrentCursorPos(Origin_X * 2, Origin_Y + 3);
	printf("W");

	fence2(Origin_X, Origin_Y);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			SetCurrentCursorPos((Origin_X + 3 + j) * 2, Origin_Y + 2 + i);
			if (character[player_index].w_scope[i][j] == 0) {
				printf("□");
			}
			if (character[player_index].w_scope[i][j] == 1) {
				printf("■");
			}
		}
	}

	SetCurrentCursorPos((Origin_X + 8) * 2, Origin_Y + 2);
	printf("DM %d", character[player_index].w_dm);
	SetCurrentCursorPos((Origin_X + 8) * 2, Origin_Y + 4);
	printf("Mp %d", character[player_index].w_mp);
}

void E(int Origin_X, int Origin_Y) {
	SetCurrentCursorPos(Origin_X * 2, Origin_Y + 3);
	printf("E");

	fence2(Origin_X, Origin_Y);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			SetCurrentCursorPos((Origin_X + 3 + j) * 2, Origin_Y + 2 + i);
			if (character[player_index].e_scope[i][j] == 0) {
				printf("□");
			}
			if (character[player_index].e_scope[i][j] == 1) {
				printf("■");
			}
		}
	}

	SetCurrentCursorPos((Origin_X + 8) * 2, Origin_Y + 2);
	printf("DM %d", character[player_index].e_dm);
	SetCurrentCursorPos((Origin_X + 8) * 2, Origin_Y + 4);
	printf("Mp %d", character[player_index].e_mp);
}

void R(int Origin_X, int Origin_Y) {
	SetCurrentCursorPos(Origin_X * 2, Origin_Y + 3);
	printf("R");

	fence2(Origin_X, Origin_Y);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			SetCurrentCursorPos((Origin_X + 3 + j) * 2, Origin_Y + 2 + i);
			if (character[player_index].r_scope[i][j] == 0) {
				printf("□");
			}
			if (character[player_index].r_scope[i][j] == 1) {
				printf("■");
			}
		}
	}

	SetCurrentCursorPos((Origin_X + 8) * 2, Origin_Y + 2);
	printf("DM %d", character[player_index].r_dm);
	SetCurrentCursorPos((Origin_X + 8) * 2, Origin_Y + 4);
	printf("Mp %d", character[player_index].r_mp);
}

void Draw_Attack() {
	int Origin_X = 55;
	int Origin_Y = 3;

	SetCurrentCursorPos(Origin_X * 2 + 10, Origin_Y);
	printf("Attack");

	Q(Origin_X, Origin_Y + 1);
	W(Origin_X, Origin_Y + 7);
	E(Origin_X, Origin_Y + 13);
	R(Origin_X, Origin_Y + 19);
}

void setGameInfo()
{
	//게임판을 모두 0으로 초기화
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 6; j++)
			Gameboardinfo[i][j] = 1;
	for (int i = 1; i < 4; i++)
		for (int j = 1; j < 5; j++)
			Gameboardinfo[i][j] = 0;

	//플레이어의 초기 위치를 1,2 으로 지정
	playerPos.X = 1;
	playerPos.Y = 2;
	playerHP = 100;
	playerMP = 100;

	//npc의 초기 위치를 4,2 으로 지정
	npcPos.X = 4;
	npcPos.Y = 2;
	npcHP = 100;
	npcMP = 100;

	Gameboardinfo[playerPos.Y][playerPos.X] = 2;
	Gameboardinfo[npcPos.Y][npcPos.X] = 3;
}

void drawNpc();
void deleteNpc();

void drawPlayer()   // 플레이어를 현재 위치에 맞게 화면에 그림
{
	int x, y;
	PlayerConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((playerPos.X - 1) * 9 * 2);
	PlayerConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((playerPos.Y - 1) * 7);

	SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);

	COORD curPos = GetCurrentCursorPos();


	textcolor(character[player_index].color);

	if ((playerPos.X == npcPos.X) && (playerPos.Y == npcPos.Y))
		overlap = 1;
	else
		overlap = 0;

	if (playerPos.X < npcPos.X)
		isPlayerLeft = 1;
	else if (playerPos.X > npcPos.X)
		isPlayerLeft = 0;

	if (overlap == 1) //겹쳐 있을때
	{
		for (y = 0; y < 4; y++)	//기존에 있던 npc 지움
		{
			for (x = 0; x < 4; x++)
			{
				SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
				printf(" ");
			}
		}

		if (isPlayerLeft == 1) //플레이어 전 위치가 NPC기준 좌측일때
		{
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X - 4 + (x * 2), curPos.Y + y);
					if (character[player_index].ch[y][x] != 0)
						printf("%c", character[player_index].ch[y][x]);
					else
						printf(" ");
				}
			}

			textcolor(character[npc_index].color);
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X + 4 + (x * 2), curPos.Y + y);
					if (character[npc_index].re_ch[y][x] != 0)
						printf("%c", character[npc_index].re_ch[y][x]);
					else
						printf(" ");
				}
			}

		}
		else     //플레이어 전 위치가 NPC기준 우측일때
		{
			textcolor(character[npc_index].color);
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X - 4 + (x * 2), curPos.Y + y);
					if (character[npc_index].ch[y][x] != 0)
						printf("%c", character[npc_index].ch[y][x]);
					else
						printf(" ");
				}
			}

			textcolor(character[player_index].color);
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X + 4 + (x * 2), curPos.Y + y);
					if (character[player_index].re_ch[y][x] != 0)
						printf("%c", character[player_index].re_ch[y][x]);
					else
						printf(" ");
				}
			}
		}

	}
	else   //안겹쳐있을 때
	{
		textcolor(character[player_index].color);

		if (isPlayerLeft == 1)	//플레이어가 npc보다 왼쪽에 있으면
		{
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
					if (character[player_index].ch[y][x] != 0)
						printf("%c", character[player_index].ch[y][x]);
					else
						printf(" ");
				}
			}
		}
		else   //플레이어가 npc보다 오른쪽에 있으면
		{
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
					if (character[player_index].re_ch[y][x] != 0)
						printf("%c", character[player_index].re_ch[y][x]);
					else
						printf(" ");
				}
			}
		}
	}

	textcolor(15);
	Gameboardinfo[playerPos.Y][playerPos.X] = 2;
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void deletePlayer()   // 플레이어를 현재 위치에서 화면에 삭제함
{
	int x, y;
	PlayerConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((playerPos.X - 1) * 9 * 2);
	PlayerConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((playerPos.Y - 1) * 7);

	SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
	COORD curPos = GetCurrentCursorPos();

	if (overlap == 1)	//겹쳐져 있었다면
	{
		for (y = 0; y < 4; y++)
		{
			for (x = 0; x < 8; x++)
			{
				SetCurrentCursorPos(curPos.X - 4 + (x * 2), curPos.Y + y);
				printf(" ");
			}
		}
		overlap = 0;

		//npc를 다시 그림

		textcolor(character[npc_index].color);
		if (isPlayerLeft == 1)	//npc가 플레이어 보다 우측에 있으면
		{
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
					if (character[npc_index].re_ch[y][x] != 0)
						printf("%c", character[npc_index].re_ch[y][x]);
					else
						printf(" ");
				}
			}
		}
		else   //npc가 플레이어보다 왼쪽에 있으면
		{
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
					if (character[npc_index].ch[y][x] != 0)
						printf("%c", character[npc_index].ch[y][x]);
					else
						printf(" ");
				}
			}
		}

		textcolor(15);
	}

	else  //안겹쳐져있었다면
	{
		for (y = 0; y < 4; y++)
		{
			for (x = 0; x < 4; x++)
			{
				SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
				printf(" ");
			}
		}
	}

	Gameboardinfo[playerPos.Y][playerPos.X] = 0;

	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void drawNpc()   // 플레이어를 현재 위치에 맞게 화면에 그림
{
	int x, y;
	NpcConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((npcPos.X - 1) * 9 * 2);
	NpcConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((npcPos.Y - 1) * 7);

	SetCurrentCursorPos(NpcConsolePos_X, NpcConsolePos_Y);
	COORD curPos = GetCurrentCursorPos();

	textcolor(character[npc_index].color);

	if ((playerPos.X == npcPos.X) && (playerPos.Y == npcPos.Y))
		overlap = 1;
	else
		overlap = 0;

	if (playerPos.X < npcPos.X)
		isPlayerLeft = 1;
	else if (playerPos.X > npcPos.X)
		isPlayerLeft = 0;


	if (overlap == 1) //겹쳐 있을때
	{
		for (y = 0; y < 4; y++)	//기존에 있던 플레이어 지움
		{
			for (x = 0; x < 4; x++)
			{
				SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
				printf(" ");
			}
		}

		if (isPlayerLeft == 1) //npc 전 위치가 플레이어 기준 우측일 때
		{
			textcolor(character[player_index].color);
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X - 4 + (x * 2), curPos.Y + y);
					if (character[player_index].ch[y][x] != 0)
						printf("%c", character[player_index].ch[y][x]);
					else
						printf(" ");
				}
			}

			textcolor(character[npc_index].color);
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X + 4 + (x * 2), curPos.Y + y);
					if (character[npc_index].re_ch[y][x] != 0)
						printf("%c", character[npc_index].re_ch[y][x]);
					else
						printf(" ");
				}
			}

		}
		else     //npc 전 위치가 플레이어 기준 좌측일 때
		{

			textcolor(character[npc_index].color);
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X - 4 + (x * 2), curPos.Y + y);
					if (character[npc_index].ch[y][x] != 0)
						printf("%c", character[npc_index].ch[y][x]);
					else
						printf(" ");
				}
			}

			textcolor(character[player_index].color);
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X + 4 + (x * 2), curPos.Y + y);
					if (character[player_index].re_ch[y][x] != 0)
						printf("%c", character[player_index].re_ch[y][x]);
					else
						printf(" ");
				}
			}
		}

	}
	else   //안겹쳐있을 때
	{

		if (isPlayerLeft == 1)	//npc가 플레이어 보다 우측에 있으면
		{
			textcolor(character[npc_index].color);
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
					if (character[npc_index].re_ch[y][x] != 0)
						printf("%c", character[npc_index].re_ch[y][x]);
					else
						printf(" ");
				}
			}
		}
		else   //npc가 플레이어보다 왼쪽에 있으면
		{
			textcolor(character[npc_index].color);
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
					if (character[npc_index].ch[y][x] != 0)
						printf("%c", character[npc_index].ch[y][x]);
					else
						printf(" ");
				}
			}
		}

	}

	textcolor(15);
	Gameboardinfo[npcPos.Y][npcPos.X] = 3;
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void deleteNpc()   // 플레이어를 현재 위치에서 화면에 삭제함
{
	int x, y;
	NpcConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((npcPos.X - 1) * 9 * 2);
	NpcConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((npcPos.Y - 1) * 7);

	SetCurrentCursorPos(NpcConsolePos_X, NpcConsolePos_Y);
	COORD curPos = GetCurrentCursorPos();


	if (overlap == 1)	//겹쳐져 있었다면
	{
		for (y = 0; y < 4; y++) //기존의 플레이어도 지움
		{
			for (x = 0; x < 8; x++)
			{
				SetCurrentCursorPos(curPos.X - 4 + (x * 2), curPos.Y + y);
				printf(" ");
			}
		}
		overlap = 0;

		//플레이어 다시 그림
		if (isPlayerLeft == 1)	//플레이어가 npc보다 왼쪽에 있으면
		{
			textcolor(character[player_index].color);
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
					if (character[player_index].ch[y][x] != 0)
						printf("%c", character[player_index].ch[y][x]);
					else
						printf(" ");
				}
			}
		}
		else   //플레이어가 npc보다 오른쪽에 있으면
		{
			textcolor(character[player_index].color);
			for (y = 0; y < 4; y++)
			{
				for (x = 0; x < 4; x++)
				{
					SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
					if (character[player_index].re_ch[y][x] != 0)
						printf("%c", character[player_index].re_ch[y][x]);
					else
						printf(" ");
				}
			}
		}
	}
	else  //안겹쳐져있었다면
	{
		for (y = 0; y < 4; y++)
		{
			for (x = 0; x < 4; x++)
			{
				SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);
				printf("  ");
			}
		}
	}

	textcolor(15);

	Gameboardinfo[npcPos.Y][npcPos.X] = 0;
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void drawstatus(int dif_playerHP, int dif_playerMP, int dif_npcHP, int dif_npcMP)
{
	//player hp,mp,이름 
	SetCurrentCursorPos(GBOARD_ORIGIN_X, 1);
	textcolor(12);
	printf("HP: %3d", playerHP);
	textcolor(9);
	SetCurrentCursorPos(GBOARD_ORIGIN_X, 2);
	printf("MP: %3d", playerMP);
	textcolor(15);
	SetCurrentCursorPos(GBOARD_ORIGIN_X, 3);

	if (player_index == 0) //도크
	{
		textcolor(4);
	}

	else if (player_index == 1) //캐럿
	{
		textcolor(6);
	}
	else if (player_index == 2) //바니
	{
		textcolor(10);
	}
	else //너츠
	{
		textcolor(3);
	}

	textcolor(character[player_index].color);
	printf("%s", character[player_index].name);
	textcolor(15);

	//npc hp,mp,이름 
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 * (GBOARD_WIDTH - 3), 1);
	textcolor(12);
	printf("HP: %3d", npcHP);
	textcolor(9);
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 * (GBOARD_WIDTH - 3), 2);
	printf("MP: %3d", npcMP);
	textcolor(15);
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 * (GBOARD_WIDTH - 2), 3);
	textcolor(10);


	if (npc_index == 0) //도크
	{
		textcolor(4);
	}

	else if (npc_index == 1) //캐럿
	{
		textcolor(6);
	}
	else if (npc_index == 2) //바니
	{
		textcolor(10);
	}
	else //너츠
	{
		textcolor(3);
	}

	textcolor(character[npc_index].color);
	printf("%s", character[npc_index].name);
	textcolor(15);

	SetCurrentCursorPos(GBOARD_ORIGIN_X + 8, 1);
	for (int i = 0; i < 20; i++)
	{
		printf(" ");
	}
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 8, 2);
	for (int i = 0; i < 20; i++)
	{
		printf(" ");
	}

	/// player HP,MP bar 
	int num_playerHPbar = playerHP / 5;
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 8, 1);
	textcolor(12);
	for (int i = 0; i < num_playerHPbar; i++)
	{
		printf("l");
	}
	COORD PlayerHpPos = GetCurrentCursorPos();


	SetCurrentCursorPos(GBOARD_ORIGIN_X + 8, 2);
	textcolor(9);
	int num_playerMPbar = playerMP / 5;
	for (int i = 0; i < num_playerMPbar; i++)
	{
		printf("l");
	}
	COORD PlayerMpPos = GetCurrentCursorPos();

	//HP소모 표현
	textcolor(13);
	SetCurrentCursorPos(PlayerHpPos.X, 1);
	for (int i = 0; i < (dif_playerHP / 5); i++)
	{
		printf("l");
	}
	if (dif_playerHP > 0)
		Sleep(700);
	SetCurrentCursorPos(PlayerHpPos.X, 1);
	for (int i = 0; i < (dif_playerHP / 5); i++)
	{
		printf(" ");
	}
	//MP 소모 표현
	textcolor(11);
	SetCurrentCursorPos(PlayerMpPos.X, 2);
	for (int i = 0; i < (dif_playerMP / 5); i++)
	{
		printf("l");
	}
	if (dif_playerMP > 0)
		Sleep(700);
	SetCurrentCursorPos(PlayerMpPos.X, 2);
	for (int i = 0; i < (dif_playerMP / 5); i++)
	{
		printf(" ");
	}


	//npc HP,MP bar 
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 45, 1);
	for (int i = 0; i < 20; i++)
	{
		printf(" ");
	}
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 45, 2);
	for (int i = 0; i < 20; i++)
	{
		printf(" ");
	}


	int num_npcHPbar = npcHP / 5;
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 45, 1);
	textcolor(12);
	for (int i = 0; i < num_npcHPbar; i++)
	{
		printf("l");
	}
	COORD NpcHpPos = GetCurrentCursorPos();

	SetCurrentCursorPos(GBOARD_ORIGIN_X + 45, 2);
	textcolor(9);
	int num_npcMPbar = npcMP / 5;
	for (int i = 0; i < num_npcMPbar; i++)
	{
		printf("l");
	}
	COORD NpcMpPos = GetCurrentCursorPos();


	//npc Hp소모 표현
	textcolor(13);
	SetCurrentCursorPos(NpcHpPos.X, 1);
	for (int i = 0; i < (dif_npcHP / 5); i++)
	{
		printf("l");
	}
	if (dif_npcHP > 0)
		Sleep(700);

	SetCurrentCursorPos(NpcHpPos.X, 1);
	for (int i = 0; i < (dif_npcHP / 5); i++)
	{
		printf(" ");
	}

	//npc MP소모 표현
	textcolor(11);
	SetCurrentCursorPos(NpcMpPos.X, 2);
	for (int i = 0; i < (dif_npcMP / 5); i++)
	{
		printf("l");
	}
	if (dif_npcMP > 0)
		Sleep(700);

	SetCurrentCursorPos(NpcMpPos.X, 2);
	for (int i = 0; i < (dif_npcMP / 5); i++)
	{
		printf(" ");
	}

	//stage
	textcolor(15);
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 34, 1);
	if (stage == 1) {
		printf("STAGE 1");
	}
	else if (stage == 2) {
		printf("STAGE 2");
	}
	else if (stage == 3) {
		printf("STAGE 3");
	}
}

void drawPlayerSkill(int skill)
{
	COORD curPos = GetCurrentCursorPos();

	textcolor(character[player_index].color);

	if (skill == 1) //Q스킬
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].q_scope[y + 1][x + 1] == 1 && Gameboardinfo[playerPos.Y + y][playerPos.X + x] != 1) {
					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					PlayerConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					PlayerConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(PlayerConsolePos_X + 2 * j, PlayerConsolePos_Y + i);
							printf("%c", character[player_index].effect);
						}
					}
				}
			}
		}
	}

	else if (skill == 2) //W스킬
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].w_scope[y + 1][x + 1] == 1 && Gameboardinfo[playerPos.Y + y][playerPos.X + x] != 1) {
					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					PlayerConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					PlayerConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(PlayerConsolePos_X + 2 * j, PlayerConsolePos_Y + i);
							printf("%c", character[player_index].effect);
						}
					}
				}
			}
		}
	}
	else if (skill == 3)//E스킬
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].e_scope[y + 1][x + 1] == 1 && Gameboardinfo[playerPos.Y + y][playerPos.X + x] != 1) {
					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					PlayerConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					PlayerConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(PlayerConsolePos_X + 2 * j, PlayerConsolePos_Y + i);
							printf("%c", character[player_index].effect);
						}
					}
				}
			}
		}
	}

	else if (skill == 4)//R스킬
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].r_scope[y + 1][x + 1] == 1 && Gameboardinfo[playerPos.Y + y][playerPos.X + x] != 1) {
					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					PlayerConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					PlayerConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(PlayerConsolePos_X + 2 * j, PlayerConsolePos_Y + i);
							printf("%c", character[player_index].effect);
						}
					}
				}
			}
		}
	}
	else if (skill == 5)   //궁수 스페셜 스킬에만 불림
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[2].w_scope[y + 1][x + 1] == 1 && Gameboardinfo[playerPos.Y + y][playerPos.X + x] != 1) {
					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					PlayerConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					PlayerConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 5; j++) {
							SetCurrentCursorPos(PlayerConsolePos_X - 2 + (2 * j), PlayerConsolePos_Y + i);
							if ((i + j) % 2 == 0)
								printf("↙");
							else
								printf(" ");
						}
					}
				}
			}
		}
	}

	textcolor(15);

	SetCurrentCursorPos(curPos.X, curPos.Y);

}

void deletePlayerSkill(int skill)
{
	SetCurrentCursorPos(0, 0);
	COORD curPos = GetCurrentCursorPos();
	if (skill == 1)
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].q_scope[y + 1][x + 1] == 1 && Gameboardinfo[playerPos.Y + y][playerPos.X + x] != 1) {
					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					PlayerConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					PlayerConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(PlayerConsolePos_X + 2 * j, PlayerConsolePos_Y + i);
							printf("  ");
						}
					}
				}
			}
		}
	}
	else if (skill == 2)
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].w_scope[y + 1][x + 1] == 1 && Gameboardinfo[playerPos.Y + y][playerPos.X + x] != 1) {
					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					PlayerConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					PlayerConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(PlayerConsolePos_X + 2 * j, PlayerConsolePos_Y + i);
							printf("  ");
						}
					}
				}
			}
		}
	}

	else if (skill == 3)
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].e_scope[y + 1][x + 1] == 1 && Gameboardinfo[playerPos.Y + y][playerPos.X + x] != 1) {
					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					PlayerConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					PlayerConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(PlayerConsolePos_X + 2 * j, PlayerConsolePos_Y + i);
							printf("  ");
						}
					}
				}
			}
		}
	}

	else if (skill == 4)
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].r_scope[y + 1][x + 1] == 1 && Gameboardinfo[playerPos.Y + y][playerPos.X + x] != 1) {
					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					PlayerConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					PlayerConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(PlayerConsolePos_X + 2 * j, PlayerConsolePos_Y + i);
							printf("  ");
						}
					}
				}
			}
		}
	}
	else if (skill == 5)
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].w_scope[y + 1][x + 1] == 1 && Gameboardinfo[playerPos.Y + y][playerPos.X + x] != 1) {
					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					PlayerConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					PlayerConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 5; j++) {
							SetCurrentCursorPos(PlayerConsolePos_X - 2 + (2 * j), PlayerConsolePos_Y + i);
							printf("  ");
						}
					}
				}
			}
		}
	}

	deletePlayer();
	deleteNpc();
	drawPlayer();
	drawNpc();
	SetCurrentCursorPos(0, 0);
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void CheckSkill_Hit(int skill)
{
	int ex_playerHP = playerHP;
	int ex_playerMP = playerMP;
	int ex_npcHP = npcHP;
	int ex_npcMP = npcMP;

	COORD curpos = GetCurrentCursorPos();

	if (skill == 1) //q스킬
	{
		playerMP -= character[player_index].q_mp; //q 스킬 마나 소모량 만큼 마나 감소 (전역변수 playermp)

		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].q_scope[y + 1][x + 1] == 1) {

					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					if (_x == npcPos.X && _y == npcPos.Y) //hit
					{
						if (character[npc_index].guard == 1) {
							npcHP -= (character[player_index].q_dm - 15);
							PlaySound(TEXT("guardsuccess.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
						}
						else
							npcHP -= character[player_index].q_dm;
					}

				}
			}
		}
	}
	else if (skill == 2) //w스킬
	{
		playerMP -= character[player_index].w_mp; //w 스킬 마나 소모량 만큼 마나 감소 (전역변수 playermp)

		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].w_scope[y + 1][x + 1] == 1) {

					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					if (_x == npcPos.X && _y == npcPos.Y) //hit
					{
						if (character[npc_index].guard == 1) {
							npcHP -= (character[player_index].w_dm - 15);
							PlaySound(TEXT("guardsuccess.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
						}
						else
							npcHP -= character[player_index].w_dm;
					}

				}
			}
		}
	}

	else if (skill == 3) //e스킬
	{
		playerMP -= character[player_index].e_mp; //e 스킬 마나 소모량 만큼 마나 감소 (전역변수 playermp)

		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].e_scope[y + 1][x + 1] == 1) {

					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					if (_x == npcPos.X && _y == npcPos.Y) //hit
					{
						if (character[npc_index].guard == 1) {
							npcHP -= (character[player_index].e_dm - 15);
							PlaySound(TEXT("guardsuccess.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
						}
						else
							npcHP -= character[player_index].e_dm;
					}

				}
			}
		}
	}

	else if (skill == 4) //r스킬
	{
		playerMP -= character[player_index].r_mp; //r 스킬 마나 소모량 만큼 마나 감소 (전역변수 playermp)

		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].r_scope[y + 1][x + 1] == 1) {

					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					if (_x == npcPos.X && _y == npcPos.Y) //hit
					{
						if (character[npc_index].guard == 1) {
							npcHP -= (character[player_index].r_dm - 15);
							PlaySound(TEXT("guardsuccess.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
						}
						else
							npcHP -= character[player_index].r_dm;
					}
				}
			}
		}
	}
	else if (skill == 5)   //궁수 스페셜 스킬 때에만 불림
	{
		playerMP -= 70; //w 스킬 마나 소모량 만큼 마나 감소 (전역변수 playermp)

		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[player_index].w_scope[y + 1][x + 1] == 1) {

					int _x = playerPos.X + x;
					int _y = playerPos.Y + y;
					if (_x == npcPos.X && _y == npcPos.Y) //hit
					{
						if (character[npc_index].guard == 1) {
							npcHP -= 25;
							PlaySound(TEXT("guardsuccess.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
						}
						else
							npcHP -= 40;
					}

				}
			}
		}
	}

	drawstatus(ex_playerHP - playerHP, ex_playerMP - playerMP, ex_npcHP - npcHP, ex_npcMP - npcMP);
}

void CheckNpcSkill_Hit(int skill)
{
	int ex_playerHP = playerHP;
	int ex_playerMP = playerMP;
	int ex_npcHP = npcHP;
	int ex_npcMP = npcMP;

	COORD curpos = GetCurrentCursorPos();

	if (skill == 1) //q스킬
	{
		npcMP -= character[npc_index].q_mp; //q 스킬 마나 소모량 만큼 마나 감소 (전역변수 npcmp)

		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].q_scope[y + 1][x + 1] == 1) {

					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					if (_x == playerPos.X && _y == playerPos.Y) //hit
					{
						if (character[player_index].guard == 1) {
							playerHP -= (character[npc_index].q_dm - 15);
							PlaySound(TEXT("guardsuccess.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
						}

						else
							playerHP -= character[npc_index].q_dm;
					}

				}
			}
		}
	}

	else if (skill == 2) //w스킬
	{
		npcMP -= character[npc_index].w_mp; //w 스킬 마나 소모량 만큼 마나 감소 (전역변수 npcmp)

		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].w_scope[y + 1][x + 1] == 1) {

					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					if (_x == playerPos.X && _y == playerPos.Y) //hit
					{
						if (character[player_index].guard == 1) {
							playerHP -= (character[npc_index].w_dm - 15);
							PlaySound(TEXT("guardsuccess.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
						}
						else
							playerHP -= character[npc_index].w_dm;
					}

				}
			}
		}
	}

	else if (skill == 3) //e스킬
	{
		npcMP -= character[npc_index].e_mp; //e 스킬 마나 소모량 만큼 마나 감소 (전역변수 npcmp)

		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].e_scope[y + 1][x + 1] == 1) {

					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					if (_x == playerPos.X && _y == playerPos.Y) //hit
					{
						if (character[player_index].guard == 1) {
							playerHP -= (character[npc_index].e_dm - 15);
							PlaySound(TEXT("guardsuccess.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
						}
						else
							playerHP -= character[npc_index].e_dm;
					}

				}
			}
		}
	}

	else if (skill == 4) //r스킬
	{
		npcMP -= character[npc_index].r_mp; //r 스킬 마나 소모량 만큼 마나 감소 (전역변수 npcmp)

		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].r_scope[y + 1][x + 1] == 1) {

					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					if (_x == playerPos.X && _y == playerPos.Y) //hit
					{
						if (character[player_index].guard == 1) {
							playerHP -= (character[npc_index].r_dm - 15);
							PlaySound(TEXT("guardsuccess.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
						}
						else
							playerHP -= character[npc_index].r_dm;
					}

				}
			}
		}
	}

	else if (skill == 5)   //궁수 스페셜 스킬 때에만 불림
	{
		npcMP -= 70; //w 스킬 마나 소모량 만큼 마나 감소 (전역변수 playermp)

		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].w_scope[y + 1][x + 1] == 1) {

					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					if (_x == playerPos.X && _y == playerPos.Y) //hit
					{
						if (character[player_index].guard == 1) {
							playerHP -= 25;
							PlaySound(TEXT("guardsuccess.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
						}
						else
							playerHP -= 40;
					}

				}
			}
		}
	}

	drawstatus(ex_playerHP - playerHP, ex_playerMP - playerMP, ex_npcHP - npcHP, ex_npcMP - npcMP);
}

void PlayerSpecialSkill()
{
	if (player_index == 0)   //도크
	{
		playerMP -= 50;
		drawstatus(0, 50, 0, 0);

		HPEffect(30);
		playerHP += 30;
		if (playerHP > 100)
			playerHP = 100;

		drawstatus(0, 0, 0, 0);
	}
	else if (player_index == 1)   //캐럿
	{
		//playerMP -= 10;
		//drawstatus(0, 10, 0, 0);

		if (doubleJumpDirect == UP)
		{
			for (int i = 1; i <= 2; i++)
			{
				SetCurrentCursorPos(0, 0);
				if (Gameboardinfo[playerPos.Y - 1][playerPos.X] == 1)
					return;
				Sleep(300);
				PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
				deletePlayer();
				playerPos.Y -= 1;
				drawPlayer();
				SetCurrentCursorPos(0, 0);
			}
		}
		else if (doubleJumpDirect == DOWN)
		{
			for (int i = 1; i <= 2; i++)
			{
				SetCurrentCursorPos(0, 0);
				if (Gameboardinfo[playerPos.Y + 1][playerPos.X] == 1)
					return;
				Sleep(300);
				PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
				deletePlayer();
				playerPos.Y += 1;
				drawPlayer();
				SetCurrentCursorPos(0, 0);
			}
		}
		else if (doubleJumpDirect == RIGHT)
		{
			for (int i = 1; i <= 2; i++)
			{
				SetCurrentCursorPos(0, 0);
				if (Gameboardinfo[playerPos.Y][playerPos.X + 1] == 1)
					return;
				Sleep(300);
				PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
				deletePlayer();
				playerPos.X += 1;
				drawPlayer();
				SetCurrentCursorPos(0, 0);
			}
		}
		else if (doubleJumpDirect == LEFT)
		{
			for (int i = 1; i <= 2; i++)
			{
				SetCurrentCursorPos(0, 0);
				if (Gameboardinfo[playerPos.Y][playerPos.X - 1] == 1)
					return;
				Sleep(300);
				PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
				deletePlayer();
				playerPos.X -= 1;
				drawPlayer();
				SetCurrentCursorPos(0, 0);
			}
		}
	}
	else if (player_index == 2)   //바니
	{
		drawPlayerSkill(5);
		CheckSkill_Hit(5);
		Sleep(1000);
		deletePlayerSkill(5);
	}
	else if (player_index == 3)   //너츠
	{
		playerHP -= 20;
		drawstatus(20, 0, 0, 0);

		MPEffect_player(60);
		playerMP += 60;
		if (playerMP > 100)
			playerMP = 100;

		drawstatus(0, 0, 0, 0);
	}
}

void drawPlayerAction(int i)   //i번째 턴 플레이어 행동
{
	SetCurrentCursorPos(0, 0);
	if (p_card[i] == RIGHT)
	{
		if (Gameboardinfo[playerPos.Y][playerPos.X + 1] == 1)
			return;
		Sleep(1000);
		deletePlayer();
		playerPos.X += 1;
		PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		drawPlayer();
	}
	else if (p_card[i] == LEFT)
	{
		if (Gameboardinfo[playerPos.Y][playerPos.X - 1] == 1)
			return;
		Sleep(1000);
		deletePlayer();
		playerPos.X -= 1;
		PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		drawPlayer();
	}
	else if (p_card[i] == UP)
	{
		if (Gameboardinfo[playerPos.Y - 1][playerPos.X] == 1)
			return;
		Sleep(1000);
		deletePlayer();
		playerPos.Y -= 1;
		PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		drawPlayer();
	}
	else if (p_card[i] == DOWN)
	{
		if (Gameboardinfo[playerPos.Y + 1][playerPos.X] == 1)
			return;
		Sleep(1000);
		deletePlayer();
		playerPos.Y += 1;
		PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		drawPlayer();
	}
	else if (p_card[i] == AKEY)
	{
		int _X = GBOARD_ORIGIN_X + 1 + 6 + ((playerPos.X - 1) * 9 * 2);
		int _Y = GBOARD_ORIGIN_Y + 1 + 1 + ((playerPos.Y - 1) * 7);

		Sleep(1000);
		PlaySound(TEXT("guard.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		character[player_index].guard = 1;
		//+ guard effect?
		//playerMP -= 15;
		drawstatus(0, 0, 0, 0);

		SetCurrentCursorPos(_X - 5, _Y - 1);
		printf("              ");
		Sleep(1000);
	}
	else if (p_card[i] == DKEY) // MP+
	{
		Sleep(1000);
		PlaySound(TEXT("heal.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		MPEffect_player(15); ///마나회복시 이펙트

		playerMP += 15;
		if (playerMP > 100) {
			playerMP = 100;
		}
		drawstatus(0, 0, 0, 0);

	}
	else if (p_card[i] == QKEY)
	{
		Sleep(1000);
		drawPlayerSkill(1);
		if (player_index == 0)
			PlaySound(TEXT("dogk skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (player_index == 1)
			PlaySound(TEXT("carat skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (player_index == 2)
			PlaySound(TEXT("bunny skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (player_index == 3)
			PlaySound(TEXT("nuts skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		CheckSkill_Hit(1);
		Sleep(1000);
		deletePlayerSkill(1);
	}
	else if (p_card[i] == WKEY)
	{
		Sleep(1000);
		drawPlayerSkill(2);
		if (player_index == 0)
			PlaySound(TEXT("dogk skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (player_index == 1)
			PlaySound(TEXT("carat skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (player_index == 2)
			PlaySound(TEXT("bunny skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (player_index == 3)
			PlaySound(TEXT("nuts skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		CheckSkill_Hit(2);
		Sleep(1000);
		deletePlayerSkill(2);
	}
	else if (p_card[i] == EKEY)
	{
		Sleep(1000);
		drawPlayerSkill(3);
		if (player_index == 0)
			PlaySound(TEXT("dogk skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (player_index == 1)
			PlaySound(TEXT("carat skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (player_index == 2)
			PlaySound(TEXT("bunny skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (player_index == 3)
			PlaySound(TEXT("nuts skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		CheckSkill_Hit(3);
		Sleep(1000);
		deletePlayerSkill(3);
	}
	else if (p_card[i] == RKEY)
	{
		Sleep(1000);
		drawPlayerSkill(4);
		if (player_index == 0)
			PlaySound(TEXT("dogk skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (player_index == 1)
			PlaySound(TEXT("carat skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (player_index == 2)
			PlaySound(TEXT("bunny skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (player_index == 3)
			PlaySound(TEXT("nuts skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		Sleep(1000);
		CheckSkill_Hit(4);
		deletePlayerSkill(4);
	}
	else if (p_card[i] == SKEY)
	{
		Sleep(1000);
		if (player_index == 0) {
			PlaySound(TEXT("dogk SpecialSkill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
			PlayerSpecialSkill();
		}
		else if (player_index == 1) {
			PlayerSpecialSkill();
		}
		else if (player_index == 2) {
			PlaySound(TEXT("bunny SpecialSkill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
			PlayerSpecialSkill();
		}

		else if (player_index == 3) {
			PlaySound(TEXT("nuts SpecialSkill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
			PlayerSpecialSkill();
		}
	}
	SetCurrentCursorPos(0, 0);
}

void drawNpcSkill(int skill)
{
	COORD curPos = GetCurrentCursorPos();


	textcolor(character[npc_index].color);

	if (skill == 1) //Q스킬
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].q_scope[y + 1][x + 1] == 1 && Gameboardinfo[npcPos.Y + y][npcPos.X + x] != 1) {
					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					NpcConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					NpcConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(NpcConsolePos_X + 2 * j, NpcConsolePos_Y + i);
							printf("%c", character[npc_index].effect);
						}
					}
				}
			}
		}
	}

	else if (skill == 2) //W스킬
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].w_scope[y + 1][x + 1] == 1 && Gameboardinfo[npcPos.Y + y][npcPos.X + x] != 1) {
					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					NpcConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					NpcConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(NpcConsolePos_X + 2 * j, NpcConsolePos_Y + i);
							printf("%c", character[npc_index].effect);
						}
					}
				}
			}
		}
	}
	else if (skill == 3)//E스킬
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].e_scope[y + 1][x + 1] == 1 && Gameboardinfo[npcPos.Y + y][npcPos.X + x] != 1) {
					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					NpcConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					NpcConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(NpcConsolePos_X + 2 * j, NpcConsolePos_Y + i);
							printf("%c", character[npc_index].effect);
						}
					}
				}
			}
		}
	}

	else if (skill == 4)//R스킬
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].r_scope[y + 1][x + 1] == 1 && Gameboardinfo[npcPos.Y + y][npcPos.X + x] != 1) {
					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					NpcConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					NpcConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(NpcConsolePos_X + 2 * j, NpcConsolePos_Y + i);
							printf("%c", character[npc_index].effect);
						}
					}
				}
			}
		}
	}

	textcolor(15);
	SetCurrentCursorPos(curPos.X, curPos.Y);

}

void deleteNpcSkill(int skill)
{
	SetCurrentCursorPos(0, 0);
	COORD curPos = GetCurrentCursorPos();
	if (skill == 1)
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].q_scope[y + 1][x + 1] == 1 && Gameboardinfo[npcPos.Y + y][npcPos.X + x] != 1) {
					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					NpcConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					NpcConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(NpcConsolePos_X + 2 * j, NpcConsolePos_Y + i);
							printf("  ");
						}
					}
				}
			}
		}
	}
	else if (skill == 2)
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].w_scope[y + 1][x + 1] == 1 && Gameboardinfo[npcPos.Y + y][npcPos.X + x] != 1) {
					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					NpcConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					NpcConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(NpcConsolePos_X + 2 * j, NpcConsolePos_Y + i);
							printf("  ");
						}
					}
				}
			}
		}
	}

	else if (skill == 3)
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].e_scope[y + 1][x + 1] == 1 && Gameboardinfo[npcPos.Y + y][npcPos.X + x] != 1) {
					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					NpcConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					NpcConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(NpcConsolePos_X + 2 * j, NpcConsolePos_Y + i);
							printf("  ");
						}
					}
				}
			}
		}
	}

	else if (skill == 4)
	{
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (character[npc_index].r_scope[y + 1][x + 1] == 1 && Gameboardinfo[npcPos.Y + y][npcPos.X + x] != 1) {
					int _x = npcPos.X + x;
					int _y = npcPos.Y + y;
					NpcConsolePos_X = GBOARD_ORIGIN_X + 1 + 6 + ((_x - 1) * 9 * 2);
					NpcConsolePos_Y = GBOARD_ORIGIN_Y + 1 + 1 + ((_y - 1) * 7);
					//SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							SetCurrentCursorPos(NpcConsolePos_X + 2 * j, NpcConsolePos_Y + i);
							printf("  ");
						}
					}
				}
			}
		}
	}

	deleteNpc();
	deletePlayer();
	drawNpc();
	drawPlayer();
	SetCurrentCursorPos(0, 0);
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void drawNpcAction(int i)   //i번째 턴 npc 행동
{
	SetCurrentCursorPos(0, 0);
	int _X = GBOARD_ORIGIN_X + 1 + 6 + ((npcPos.X - 1) * 9 * 2);
	int _Y = GBOARD_ORIGIN_Y + 1 + 1 + ((npcPos.Y - 1) * 7);
	int Origin_X = _X;

	if (overlap == 1)
		if (isPlayerLeft == 1)
			_X += 7;


	if (n_card[i] == RIGHT)    //우측이동
	{
		SetCurrentCursorPos(_X - 5, _Y - 1);
		printf("우측 이동");
		Sleep(1000);
		if (Gameboardinfo[npcPos.Y][npcPos.X + 1] == 1) {
			SetCurrentCursorPos(_X - 5, _Y - 1);
			printf("                ");
			return;
		}
		deleteNpc();
		npcPos.X += 1;
		SetCurrentCursorPos(Origin_X - 5, _Y - 1);
		printf("                ");
		PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		drawNpc();

	}
	else if (n_card[i] == LEFT)   //좌측이동
	{
		SetCurrentCursorPos(_X - 5, _Y - 1);
		printf("좌측 이동");
		Sleep(1000);
		if (Gameboardinfo[npcPos.Y][npcPos.X - 1] == 1) {
			SetCurrentCursorPos(_X - 5, _Y - 1);
			printf("                ");
			return;
		}
		deleteNpc();
		npcPos.X -= 1;
		SetCurrentCursorPos(Origin_X - 5, _Y - 1);
		printf("                ");
		PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		drawNpc();

	}
	else if (n_card[i] == UP)   //위로 이동
	{
		SetCurrentCursorPos(_X - 5, _Y - 1);
		printf("위로 이동");
		Sleep(1000);
		if (Gameboardinfo[npcPos.Y - 1][npcPos.X] == 1) {
			SetCurrentCursorPos(_X - 5, _Y - 1);
			printf("                ");
			return;
		}
		deleteNpc();
		npcPos.Y -= 1;
		SetCurrentCursorPos(Origin_X - 5, _Y - 1);
		printf("                ");
		PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		drawNpc();

	}
	else if (n_card[i] == DOWN)   //밑으로 이동
	{
		SetCurrentCursorPos(_X - 5, _Y - 1);
		printf("아래 이동");
		Sleep(1000);
		if (Gameboardinfo[npcPos.Y + 1][npcPos.X] == 1) {
			SetCurrentCursorPos(_X - 5, _Y - 1);
			printf("                ");
			return;
		}
		deleteNpc();
		npcPos.Y += 1;
		SetCurrentCursorPos(Origin_X - 5, _Y - 1);
		printf("                ");
		PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		drawNpc();

	}
	else if (n_card[i] == QKEY) {
		SetCurrentCursorPos(_X - 5, _Y - 1);
		printf("공격!");
		Sleep(1000);
		if (npc_index == 0)
			PlaySound(TEXT("dogk skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (npc_index == 1)
			PlaySound(TEXT("carat skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (npc_index == 2)
			PlaySound(TEXT("bunny skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (npc_index == 3)
			PlaySound(TEXT("nuts skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		drawNpcSkill(1);
		CheckNpcSkill_Hit(1);
		Sleep(1000);
		deleteNpcSkill(1);
		SetCurrentCursorPos(Origin_X - 5, _Y - 1);
		printf("                ");
	}
	else if (n_card[i] == WKEY) {
		SetCurrentCursorPos(_X - 5, _Y - 1);
		printf("공격!");
		Sleep(1000);
		drawNpcSkill(2);
		if (npc_index == 0)
			PlaySound(TEXT("dogk skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (npc_index == 1)
			PlaySound(TEXT("carat skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (npc_index == 2)
			PlaySound(TEXT("bunny skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (npc_index == 3)
			PlaySound(TEXT("nuts skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		CheckNpcSkill_Hit(2);
		Sleep(1000);
		deleteNpcSkill(2);
		SetCurrentCursorPos(Origin_X - 5, _Y - 1);
		printf("                ");
	}
	else if (n_card[i] == EKEY) {
		SetCurrentCursorPos(_X - 5, _Y - 1);
		printf("공격!");
		Sleep(1000);
		drawNpcSkill(3);
		if (npc_index == 0)
			PlaySound(TEXT("dogk skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (npc_index == 1)
			PlaySound(TEXT("carat skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (npc_index == 2)
			PlaySound(TEXT("bunny skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (npc_index == 3)
			PlaySound(TEXT("nuts skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		CheckNpcSkill_Hit(3);
		Sleep(1000);
		deleteNpcSkill(3);
		SetCurrentCursorPos(Origin_X - 5, _Y - 1);
		printf("                ");
	}
	else if (n_card[i] == RKEY) {
		SetCurrentCursorPos(_X - 5, _Y - 1);
		printf("공격!");
		Sleep(1000);
		drawNpcSkill(4);
		if (npc_index == 0)
			PlaySound(TEXT("dogk skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (npc_index == 1)
			PlaySound(TEXT("carat skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (npc_index == 2)
			PlaySound(TEXT("bunny skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		else if (npc_index == 3)
			PlaySound(TEXT("nuts skill.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		CheckNpcSkill_Hit(4);
		Sleep(1000);
		deleteNpcSkill(4);
		SetCurrentCursorPos(Origin_X - 5, _Y - 1);
		printf("                ");
	}
	else if (n_card[i] == DKEY) {
		SetCurrentCursorPos(_X - 5, _Y - 1);
		printf("마나회복!");
		SetCurrentCursorPos(0, 0);
		Sleep(1000);
		MPEffect_npc(15); ///마나회복시 이펙트
		PlaySound(TEXT("heal.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		npcMP += 15;
		if (npcMP > 100) {
			npcMP = 100;
		}
		drawstatus(0, 0, 0, 0);
		SetCurrentCursorPos(Origin_X - 5, _Y - 1);
		printf("                ");
		SetCurrentCursorPos(0, 0);
	}
	else if (n_card[i] == AKEY) {
		SetCurrentCursorPos(_X - 5, _Y - 1);
		printf("방어!");

		Sleep(1500);
		PlaySound(TEXT("guard.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		character[npc_index].guard = 1;
		//+ guard effect?
		//npcMP -= 15;
		drawstatus(0, 0, 0, 0);

		SetCurrentCursorPos(Origin_X - 5, _Y - 1);
		printf("                ");
	}

	SetCurrentCursorPos(0, 0);
}

int isNpcCardOverlap(int i) //중복이면 1반환, 아니면 0반환
{
	if (i == 0)
		return 0;

	if (i == 1 && n_card[i] == n_card[i - 1])
		return 1;

	if (i == 2 && (n_card[i] == n_card[i - 1] || n_card[i] == n_card[i - 2]))
		return 1;

	return 0;
}

void setNpcRunAwayCard(int i)	//현재 player의 위치와 반대 방향으로 이동하는 카드를 반환
{
	srand(time(NULL));
	int random = rand() % 2;

	if (random == 0)	// 1/2 확률로 x축으로 먼저 회피할 것인가 y축으로 먼저 회피할 것인가
	{
		//오버랩 발생하면 -> 다음 회피 방향 탐색 -> 마지막 까지 오버랩이면 랜덤 이동
		if (npcPos.X > playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X + 1] != 1)
		{
			n_card[i] = RIGHT;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.Y >= playerPos.Y && Gameboardinfo[npcPos.Y + 1][npcPos.X] != 1)
					n_card[i] = DOWN;
				else if (npcPos.Y <= playerPos.Y && Gameboardinfo[npcPos.Y - 1][npcPos.X] != 1)
					n_card[i] = UP;
				else   //갈 곳이 없으면 랜덤 이동
					n_card[i] = npc_action[rand() % 4];
			}
		}
		else if (npcPos.X < playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X - 1] != 1)
		{
			n_card[i] = LEFT;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.Y >= playerPos.Y && Gameboardinfo[npcPos.Y + 1][npcPos.X] != 1)
					n_card[i] = DOWN;
				else if (npcPos.Y <= playerPos.Y && Gameboardinfo[npcPos.Y - 1][npcPos.X] != 1)
					n_card[i] = UP;
				else
					n_card[i] = npc_action[rand() % 4];
			}
		}
		else if (npcPos.Y > playerPos.Y && Gameboardinfo[npcPos.Y + 1][npcPos.X] != 1)
		{
			n_card[i] = DOWN;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.X >= playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X + 1] != 1)
					n_card[i] = RIGHT;
				else if (npcPos.X <= playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X - 1] != 1)
					n_card[i] = LEFT;
				else
					n_card[i] = npc_action[rand() % 4];
			}
		}
		else if (npcPos.Y < playerPos.Y && Gameboardinfo[npcPos.Y - 1][npcPos.X] != 1)
		{
			n_card[i] = UP;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.X >= playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X + 1] != 1)
					n_card[i] = RIGHT;
				else if (npcPos.X <= playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X - 1] != 1)
					n_card[i] = LEFT;
				else
					n_card[i] = npc_action[rand() % 4];
			}
		}
		else   //선택지가 없으면 랜덤 이동
		{
			n_card[i] = npc_action[rand() % 4];
		}
	}
	else       //y축으로 먼저 회피
	{
		if (npcPos.Y > playerPos.Y && Gameboardinfo[npcPos.Y + 1][npcPos.X] != 1)
		{
			n_card[i] = DOWN;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.X >= playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X + 1] != 1)
					n_card[i] = RIGHT;
				else if (npcPos.X <= playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X - 1] != 1)
					n_card[i] = LEFT;
				else
					n_card[i] = npc_action[rand() % 4];
			}
		}
		else if (npcPos.Y < playerPos.Y && Gameboardinfo[npcPos.Y - 1][npcPos.X] != 1)
		{
			n_card[i] = UP;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.X >= playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X + 1] != 1)
					n_card[i] = RIGHT;
				else if (npcPos.X <= playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X - 1] != 1)
					n_card[i] = LEFT;
				else
					n_card[i] = npc_action[rand() % 4];
			}
		}
		else if (npcPos.X > playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X + 1] != 1)
		{
			n_card[i] = RIGHT;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.Y >= playerPos.Y && Gameboardinfo[npcPos.Y + 1][npcPos.X] != 1)
					n_card[i] = DOWN;
				else if (npcPos.Y <= playerPos.Y && Gameboardinfo[npcPos.Y - 1][npcPos.X] != 1)
					n_card[i] = UP;
				else   //갈 곳이 없으면 랜덤 이동
					n_card[i] = npc_action[rand() % 4];
			}
		}
		else if (npcPos.X < playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X - 1] != 1)
		{
			n_card[i] = LEFT;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.Y >= playerPos.Y && Gameboardinfo[npcPos.Y + 1][npcPos.X] != 1)
					n_card[i] = DOWN;
				else if (npcPos.Y <= playerPos.Y && Gameboardinfo[npcPos.Y - 1][npcPos.X] != 1)
					n_card[i] = UP;
				else
					n_card[i] = npc_action[rand() % 4];
			}
		}
		else   //선택지가 없으면 랜덤 이동
		{
			n_card[i] = npc_action[rand() % 4];
		}
	}


	//조건을 모두 검사했는데도 오버랩이 발생했을 경우에는 랜덤 이동 카드를 뽑음
	while (isNpcCardOverlap(i))
	{
		n_card[i] = npc_action[rand() % 4];
	}
}

void setNpcTrackingCard(int i)  //현재 player방향으로 추적 이동하는 카드를 반환
{
	srand(time(NULL));
	int random = rand() % 2;

	if (random == 0) //x축 방향으로 먼저 추적
	{
		//오버랩 발생하면 -> 다음 접근 방향 탐색 -> 마지막 까지 오버랩이면 랜덤하게 뽑음
		if (npcPos.X > playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X - 1] != 1)
		{
			n_card[i] = LEFT;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.Y > playerPos.Y && Gameboardinfo[npcPos.Y - 1][npcPos.X] != 1)
					n_card[i] = UP;
				else if (npcPos.Y < playerPos.Y && Gameboardinfo[npcPos.Y + 1][npcPos.X] != 1)
					n_card[i] = DOWN;
				else   //선택지가 없으면 방어
					n_card[i] = AKEY;
			}
		}
		else if (npcPos.X < playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X + 1] != 1)
		{
			n_card[i] = RIGHT;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.Y > playerPos.Y && Gameboardinfo[npcPos.Y - 1][npcPos.X] != 1)
					n_card[i] = UP;
				else if (npcPos.Y < playerPos.Y && Gameboardinfo[npcPos.Y + 1][npcPos.X] != 1)
					n_card[i] = DOWN;
				else
					n_card[i] = AKEY;
			}
		}
		else if (npcPos.Y > playerPos.Y && Gameboardinfo[npcPos.Y - 1][npcPos.X] != 1)
		{
			n_card[i] = UP;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.X > playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X - 1] != 1)
					n_card[i] = LEFT;
				else if (npcPos.X < playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X + 1] != 1)
					n_card[i] = RIGHT;
				else
					n_card[i] = AKEY;
			}
		}
		else if (npcPos.Y < playerPos.Y && Gameboardinfo[npcPos.Y + 1][npcPos.X] != 1)
		{
			n_card[i] = DOWN;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.X > playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X - 1] != 1)
					n_card[i] = LEFT;
				else if (npcPos.X < playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X + 1] != 1)
					n_card[i] = RIGHT;
				else
					n_card[i] = AKEY;
			}
		}
		else  //조건을 다 돌았는데 선택지가 없으면 마나 회복
		{
			n_card[i] = DKEY;
		}
	}
	else   //y축 방향으로 먼저 추적
	{
		//오버랩 발생하면 -> 다음 접근 방향 탐색 -> 마지막 까지 오버랩이면 랜덤하게 뽑음
		if (npcPos.Y > playerPos.Y && Gameboardinfo[npcPos.Y - 1][npcPos.X] != 1)
		{
			n_card[i] = UP;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.X > playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X - 1] != 1)
					n_card[i] = LEFT;
				else if (npcPos.X < playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X + 1] != 1)
					n_card[i] = RIGHT;
				else
					n_card[i] = AKEY;
			}
		}
		else if (npcPos.Y < playerPos.Y && Gameboardinfo[npcPos.Y + 1][npcPos.X] != 1)
		{
			n_card[i] = DOWN;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.X > playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X - 1] != 1)
					n_card[i] = LEFT;
				else if (npcPos.X < playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X + 1] != 1)
					n_card[i] = RIGHT;
				else
					n_card[i] = AKEY;
			}
		}
		else if (npcPos.X > playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X - 1] != 1)
		{
			n_card[i] = LEFT;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.Y > playerPos.Y && Gameboardinfo[npcPos.Y - 1][npcPos.X] != 1)
					n_card[i] = UP;
				else if (npcPos.Y < playerPos.Y && Gameboardinfo[npcPos.Y + 1][npcPos.X] != 1)
					n_card[i] = DOWN;
				else   //선택지가 없으면 방어
					n_card[i] = AKEY;
			}
		}
		else if (npcPos.X < playerPos.X && Gameboardinfo[npcPos.Y][npcPos.X + 1] != 1)
		{
			n_card[i] = RIGHT;
			if (isNpcCardOverlap(i))
			{
				if (npcPos.Y > playerPos.Y && Gameboardinfo[npcPos.Y - 1][npcPos.X] != 1)
					n_card[i] = UP;
				else if (npcPos.Y < playerPos.Y && Gameboardinfo[npcPos.Y + 1][npcPos.X] != 1)
					n_card[i] = DOWN;
				else
					n_card[i] = AKEY;
			}
		}
		else  //조건을 다 돌았는데 선택지가 없으면 마나 회복
		{
			n_card[i] = DKEY;
		}
	}


	//조건을 모두 검사했는데도 오버랩이 발생했을 경우에는 랜덤 이동
	while (isNpcCardOverlap(i))
	{
		n_card[i] = npc_action[rand() % 4];
	}

}

void setNpcRandomCard(int i)
{
	if (npcMP < 25) {		//마나가 없으면 마나 카드 선택
		n_card[i] = DKEY;
		if (isNpcCardOverlap(i))	//이미 마나 회복을 썼으면 랜덤
		{
			n_card[i] = npc_action[rand() % 10];
		}
	}
	else if (npcMP <= 50) {		//마나가 50이하이면 마나 회복 포함해서 랜덤
		n_card[i] = npc_action[rand() % 10];	//0~9까지 
	}
	else {	//마나가 50보다 많으면 마나 회복 제외하고 랜덤
		n_card[i] = npc_action[rand() % 9];	//0~8까지 (마나 빼고)
	}
}

void setNpcDefiniteSkill(int i)
{
	int max_dm = 0;
	int check = 0;

	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (character[npc_index].q_scope[y + 1][x + 1] == 1) {

				int _x = npcPos.X + x;
				int _y = npcPos.Y + y;
				if (_x == playerPos.X && _y == playerPos.Y) //hit
				{
					if (npcMP >= character[npc_index].q_mp)
					{
						if (character[npc_index].q_dm >= max_dm)
						{
							n_card[i] = QKEY;
							max_dm = character[npc_index].q_dm;
							check = 1;
						}
					}
				}

			}
		}
	}

	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (character[npc_index].w_scope[y + 1][x + 1] == 1) {

				int _x = npcPos.X + x;
				int _y = npcPos.Y + y;
				if (_x == playerPos.X && _y == playerPos.Y) //hit
				{
					if (npcMP >= character[npc_index].w_mp)
					{
						if (character[npc_index].w_dm >= max_dm)
						{
							n_card[i] = WKEY;
							max_dm = character[npc_index].w_dm;
							check = 1;
						}
					}
				}

			}
		}
	}

	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (character[npc_index].e_scope[y + 1][x + 1] == 1) {

				int _x = npcPos.X + x;
				int _y = npcPos.Y + y;
				if (_x == playerPos.X && _y == playerPos.Y) //hit
				{
					if (npcMP >= character[npc_index].e_mp)
					{
						if (character[npc_index].e_dm >= max_dm)
						{
							n_card[i] = EKEY;
							max_dm = character[npc_index].e_dm;
							check = 1;
						}
					}
				}

			}
		}
	}

	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (character[npc_index].r_scope[y + 1][x + 1] == 1) {

				int _x = npcPos.X + x;
				int _y = npcPos.Y + y;
				if (_x == playerPos.X && _y == playerPos.Y) //hit
				{
					if (npcMP >= character[npc_index].r_mp)
					{
						if (character[npc_index].r_dm >= max_dm)
						{
							n_card[i] = RKEY;
							max_dm = character[npc_index].r_dm;
							check = 1;
						}
					}
				}

			}
		}
	}

	if (check == 0)
	{
		n_card[i] = npc_action[(rand() % 4) + 5];
	}

	return;

}

int npcSelectCard_stage1(int i)   //예외사항 검토 후 성공하면 0 반환, 실패하면 1 반환
{
	srand(time(NULL));

	if (i == 0)	 // 첫 번째 카드 이동, 방어 카드로 고정
	{
		if (flag_stage1 == 0) { // 첫 턴 왼쪽 이동 카드로 고정
			n_card[i] = LEFT;
			flag_stage1 = 1;
			return 0;
		}
		n_card[i] = npc_action[rand() % 5];	//이동 또는 방어
	}
	else if (i == 1)	//2,3번째 턴은 랜덤 (마나 없으면 마나 선택 우선)
	{
		setNpcRandomCard(i);
	}
	else if (i == 2)
	{
		setNpcRandomCard(i);
	}


	// -------실패 사항 검사
	if (n_card[i] == LEFT) {
		if (Gameboardinfo[npcPos.Y][npcPos.X - 1] == 1)
			return 1;
	}
	if (n_card[i] == RIGHT) {
		if (Gameboardinfo[npcPos.Y][npcPos.X + 1] == 1)
			return 1;
	}
	if (n_card[i] == UP) {
		if (Gameboardinfo[npcPos.Y - 1][npcPos.X] == 1)
			return 1;
	}
	if (n_card[i] == DOWN) {
		if (Gameboardinfo[npcPos.Y + 1][npcPos.X] == 1)
			return 1;
	}

	//mp가 부족한지 검사
	if (n_card[i] == QKEY) {
		if (npcMP < character[npc_index].q_mp)
			return 1;
	}
	if (n_card[i] == WKEY) {
		if (npcMP < character[npc_index].w_mp)
			return 1;
	}
	if (n_card[i] == EKEY) {
		if (npcMP < character[npc_index].e_mp)
			return 1;
	}
	if (n_card[i] == RKEY) {
		if (npcMP < character[npc_index].r_mp)
			return 1;
	}


	if (isNpcCardOverlap(i))	//카드를 선택 했는데, 카드가 중복이면 실패 반환
		return 1;

	return 0;      //예외사항이 없으면
}

int npcSelectCard_stage2(int i)   //예외사항 검토 후 성공하면 0 반환, 실패하면 1 반환
{
	srand(time(NULL));
	double dist = sqrt(((playerPos.X - npcPos.X) * (playerPos.X - npcPos.X)) + ((playerPos.Y - npcPos.Y) * (playerPos.Y - npcPos.Y))); //플레이어와 npc 사이의 거리


	if (i == 0) //NPC가 상황에 맞는 카드를 선택 
	{
		if (dist <= sqrt(2))	//위험지역에 있을 때 1/3 확률로 플레이어의 스킬을 방어
		{
			int randomGuard = rand() % 3;
			if (randomGuard == 0 && (p_card[i] == QKEY || p_card[i] == WKEY || p_card[i] == EKEY || p_card[i] == RKEY || (player_index == 2 && p_card[i] == SKEY)))
			{
				n_card[i] = AKEY;
				if (isNpcCardOverlap(i))
					return 1;
			}
			else
			{
				//범위 안에 있는데 마나가 별로 없다면 후퇴 or 방어
				if (npcMP < 35)  //후퇴
				{
					setNpcRunAwayCard(i);
				}
				else   //마나가 충분하면 스킬 사용
				{
					int random = rand() % 4;

					if (random == 0) //마나가 충분해도 1/4 확률로 회피 함
					{
						setNpcRunAwayCard(i);
					}
					else // 3/4확률로는 공격
					{
						n_card[i] = npc_action[(rand() % 4) + 5];
					}
				}
			}

		}
		else   //위험지역 밖에 있다면
		{
			if (npcMP < 40) //마나가 적으면
				n_card[i] = DKEY; //마나 회복

			else if (npcHP >= 40) //마나가 충분하고 피가 많으면 플레이어 한테 접근
			{
				setNpcTrackingCard(i);
			}
			else  // 마나가 충분하나 피가 40보다 적으면
			{
				if (npcMP < 70)
					n_card[i] = DKEY; //마나 회복
				else  //마나를 충분히 채웠으면 접근
				{
					setNpcTrackingCard(i);
				}
			}

		}

	}
	else if (i == 1) //상황에 맞는 스킬
	{
		if (dist <= sqrt(2))	//위험지역에 있을 때 1/3 확률로 플레이어의 스킬을 방어
		{
			int randomGuard = rand() % 3;
			if (randomGuard == 0 && (p_card[i] == QKEY || p_card[i] == WKEY || p_card[i] == EKEY || p_card[i] == RKEY || (player_index == 2 && p_card[i] == SKEY)))
			{
				n_card[i] = AKEY;
				if (isNpcCardOverlap(i))
					return 1;
			}
			else
			{
				//범위 안에 있는데 마나가 별로 없다면 후퇴 or 방어
				if (npcMP < 35)  //후퇴
				{
					setNpcRunAwayCard(i);
				}
				else   //마나가 충분하면 스킬 사용
				{
					int random = rand() % 4;

					if (random == 0) //마나가 충분해도 1/4 확률로 회피 함
					{
						setNpcRunAwayCard(i);
					}
					else // 3/4확률로는 공격
					{
						n_card[i] = npc_action[(rand() % 4) + 5];
					}
				}
			}

		}
		else   //위험지역 밖에 있다면
		{
			if (npcMP < 40) //마나가 적으면
				n_card[i] = DKEY; //마나 회복

			else if (npcHP >= 40) //마나가 충분하고 피가 많으면 플레이어 한테 접근
			{
				//추적 방향 카드
				setNpcTrackingCard(i);
			}
			else  // 마나가 충분하나 피가 40보다 적으면
			{
				if (npcMP < 70)
					n_card[i] = DKEY; //마나 회복
				else  //마나를 충분히 채웠으면 접근
				{
					//추적 방향 카드
					setNpcTrackingCard(i);
				}
			}

		}

	}
	else if (i == 2) //상황에 맞는 스킬
	{
		if (dist <= sqrt(2))	//위험지역에 있을 때 1/3 확률로 플레이어의 스킬을 방어
		{
			int randomGuard = rand() % 3;
			if (randomGuard == 0 && (p_card[i] == QKEY || p_card[i] == WKEY || p_card[i] == EKEY || p_card[i] == RKEY || (player_index == 2 && p_card[i] == SKEY)))
			{
				n_card[i] = AKEY;
				if (isNpcCardOverlap(i))
					return 1;
			}
			else
			{
				//범위 안에 있는데 마나가 별로 없다면 후퇴 or 방어
				if (npcMP < 35)  //후퇴
				{
					setNpcRunAwayCard(i);
				}
				else   //마나가 충분하면 스킬 사용
				{
					int random = rand() % 4;

					if (random == 0) //마나가 충분해도 1/4 확률로 회피 함
					{
						setNpcRunAwayCard(i);
					}
					else // 3/4확률로는 공격
					{
						n_card[i] = npc_action[(rand() % 4) + 5];
					}
				}
			}

		}
		else   //위험지역 밖에 있다면
		{
			if (npcMP < 40) //마나가 적으면
				n_card[i] = DKEY; //마나 회복

			else if (npcHP >= 40) //마나가 충분하고 피가 많으면 플레이어 한테 접근
			{
				//추적 방향 카드
				setNpcTrackingCard(i);
			}
			else  // 마나가 충분하나 피가 40보다 적으면
			{
				if (npcMP < 70)
					n_card[i] = DKEY; //마나 회복
				else  //마나를 충분히 채웠으면 접근
				{
					//추적 방향 카드
					setNpcTrackingCard(i);
				}
			}

		}

	}


	//---------루프방지--------
	if (isNpcCardOverlap(i) || n_card[i] == 0)
		n_card[i] = npc_action[rand() % 10];
	//-------------------------


	// -------실패 사항 검사--------

	//벽에 부딪히는지 검사
	if (n_card[i] == LEFT) {
		if (Gameboardinfo[npcPos.Y][npcPos.X - 1] == 1)
			return 1;
	}
	if (n_card[i] == RIGHT) {
		if (Gameboardinfo[npcPos.Y][npcPos.X + 1] == 1)
			return 1;
	}
	if (n_card[i] == UP) {
		if (Gameboardinfo[npcPos.Y - 1][npcPos.X] == 1)
			return 1;
	}
	if (n_card[i] == DOWN) {
		if (Gameboardinfo[npcPos.Y + 1][npcPos.X] == 1)
			return 1;
	}

	//mp가 부족한지 검사
	if (n_card[i] == QKEY) {
		if (npcMP < character[npc_index].q_mp)
			return 1;
	}
	if (n_card[i] == WKEY) {
		if (npcMP < character[npc_index].w_mp)
			return 1;
	}
	if (n_card[i] == EKEY) {
		if (npcMP < character[npc_index].e_mp)
			return 1;
	}
	if (n_card[i] == RKEY) {
		if (npcMP < character[npc_index].r_mp)
			return 1;
	}


	if (isNpcCardOverlap(i))	//카드를 선택 했는데, 카드가 중복이면 실패 반환
		return 1;


	return 0;      //예외사항이 없으면
}
int npcSelectCard_stage3(int i)   //예외사항 검토 후 성공하면 0 반환, 실패하면 1 반환
{
	srand(time(NULL));
	double dist = sqrt(((playerPos.X - npcPos.X) * (playerPos.X - npcPos.X)) + ((playerPos.Y - npcPos.Y) * (playerPos.Y - npcPos.Y))); //플레이어와 npc 사이의 거리


	if (i == 0) //NPC가 상황에 맞는 카드를 선택 
	{
		if (dist <= sqrt(2))	//위험지역에 있을 때 1/2 확률로 플레이어의 스킬을 방어
		{
			int randomGuard = rand() % 2;
			if (randomGuard == 0 && (p_card[i] == QKEY || p_card[i] == WKEY || p_card[i] == EKEY || p_card[i] == RKEY || (player_index == 2 && p_card[i] == SKEY)))
			{
				n_card[i] = AKEY;
				if (isNpcCardOverlap(i))
					return 1;
			}
			else
			{
				//범위 안에 있는데 마나가 별로 없다면 후퇴 or 방어
				if (npcMP < 35)  //후퇴
				{
					setNpcRunAwayCard(i);
				}
				else   //마나가 충분하면 스킬 사용
				{
					int random = rand() % 4;

					if (random == 0) //마나가 충분해도 1/4 확률로 회피 함
					{
						setNpcRunAwayCard(i);
					}
					else // 3/4확률로는 공격
					{
						setNpcDefiniteSkill(i);
					}
				}
			}

		}
		else   //위험지역 밖에 있다면
		{
			if (npcMP < 40) //마나가 적으면
				n_card[i] = DKEY; //마나 회복

			else if (npcHP >= 40) //마나가 충분하고 피가 많으면 플레이어 한테 접근
			{
				setNpcTrackingCard(i);
			}
			else  // 마나가 충분하나 피가 40보다 적으면
			{
				if (npcMP < 70)
					n_card[i] = DKEY; //마나 회복
				else  //마나를 충분히 채웠으면 접근
				{
					setNpcTrackingCard(i);
				}
			}

		}

	}
	else if (i == 1) //상황에 맞는 스킬
	{
		if (dist <= sqrt(2))	//위험지역에 있을 때 1/2 확률로 플레이어의 스킬을 방어
		{
			int randomGuard = rand() % 2;
			if (randomGuard == 0 && (p_card[i] == QKEY || p_card[i] == WKEY || p_card[i] == EKEY || p_card[i] == RKEY || (player_index == 2 && p_card[i] == SKEY)))
			{
				n_card[i] = AKEY;
				if (isNpcCardOverlap(i))
					return 1;
			}
			else
			{
				//범위 안에 있는데 마나가 별로 없다면 후퇴 or 방어
				if (npcMP < 35)  //후퇴
				{
					setNpcRunAwayCard(i);
				}
				else   //마나가 충분하면 스킬 사용
				{
					int random = rand() % 4;

					if (random == 0) //마나가 충분해도 1/4 확률로 회피 함
					{
						setNpcRunAwayCard(i);
					}
					else // 3/4확률로는 공격
					{
						setNpcDefiniteSkill(i);
					}
				}
			}

		}
		else   //위험지역 밖에 있다면
		{
			if (npcMP < 40) //마나가 적으면
				n_card[i] = DKEY; //마나 회복

			else if (npcHP >= 40) //마나가 충분하고 피가 많으면 플레이어 한테 접근
			{
				//추적 방향 카드
				setNpcTrackingCard(i);
			}
			else  // 마나가 충분하나 피가 40보다 적으면
			{
				if (npcMP < 70)
					n_card[i] = DKEY; //마나 회복
				else  //마나를 충분히 채웠으면 접근
				{
					//추적 방향 카드
					setNpcTrackingCard(i);
				}
			}

		}

	}
	else if (i == 2) //상황에 맞는 스킬
	{
		if (dist <= sqrt(2))	//위험지역에 있을 때 1/2 확률로 플레이어의 스킬을 방어
		{
			int randomGuard = rand() % 2;
			if (randomGuard == 0 && (p_card[i] == QKEY || p_card[i] == WKEY || p_card[i] == EKEY || p_card[i] == RKEY || (player_index == 2 && p_card[i] == SKEY)))
			{
				n_card[i] = AKEY;
				if (isNpcCardOverlap(i))
					return 1;
			}
			else
			{
				//범위 안에 있는데 마나가 별로 없다면 후퇴 or 방어
				if (npcMP < 35)  //후퇴
				{
					setNpcRunAwayCard(i);
				}
				else   //마나가 충분하면 스킬 사용
				{
					int random = rand() % 4;

					if (random == 0) //마나가 충분해도 1/4 확률로 회피 함
					{
						setNpcRunAwayCard(i);
					}
					else // 3/4확률로는 공격
					{
						setNpcDefiniteSkill(i);
					}
				}
			}

		}
		else   //위험지역 밖에 있다면
		{
			if (npcMP < 40) //마나가 적으면
				n_card[i] = DKEY; //마나 회복

			else if (npcHP >= 40) //마나가 충분하고 피가 많으면 플레이어 한테 접근
			{
				//추적 방향 카드
				setNpcTrackingCard(i);
			}
			else  // 마나가 충분하나 피가 40보다 적으면
			{
				if (npcMP < 70)
					n_card[i] = DKEY; //마나 회복
				else  //마나를 충분히 채웠으면 접근
				{
					//추적 방향 카드
					setNpcTrackingCard(i);
				}
			}

		}

	}


	//---------루프방지--------
	if (isNpcCardOverlap(i) || n_card[i] == 0)
		n_card[i] = npc_action[rand() % 10];
	//-------------------------


	// -------실패 사항 검사--------

	//벽에 부딪히는지 검사
	if (n_card[i] == LEFT) {
		if (Gameboardinfo[npcPos.Y][npcPos.X - 1] == 1)
			return 1;
	}
	if (n_card[i] == RIGHT) {
		if (Gameboardinfo[npcPos.Y][npcPos.X + 1] == 1)
			return 1;
	}
	if (n_card[i] == UP) {
		if (Gameboardinfo[npcPos.Y - 1][npcPos.X] == 1)
			return 1;
	}
	if (n_card[i] == DOWN) {
		if (Gameboardinfo[npcPos.Y + 1][npcPos.X] == 1)
			return 1;
	}

	//mp가 부족한지 검사
	if (n_card[i] == QKEY) {
		if (npcMP < character[npc_index].q_mp)
			return 1;
	}
	if (n_card[i] == WKEY) {
		if (npcMP < character[npc_index].w_mp)
			return 1;
	}
	if (n_card[i] == EKEY) {
		if (npcMP < character[npc_index].e_mp)
			return 1;
	}
	if (n_card[i] == RKEY) {
		if (npcMP < character[npc_index].r_mp)
			return 1;
	}


	if (isNpcCardOverlap(i))	//카드를 선택 했는데, 카드가 중복이면 실패 반환
		return 1;


	return 0;      //예외사항이 없으면
}


void selectNpcCard(int i) {	//해당 턴의 i번째 카드를 뽑는다 / 성공하면 0, 실패하면 1 반환

	int _X = GBOARD_ORIGIN_X + 1 + 6 + ((npcPos.X - 1) * 9 * 2);
	int _Y = GBOARD_ORIGIN_Y + 1 + 1 + ((npcPos.Y - 1) * 7);
	SetCurrentCursorPos(_X - 5, _Y - 1);
	printf("카드 선택 중..");
	Sleep(500);

	if (stage == 1)
		while (npcSelectCard_stage1(i));

	else if (stage == 2)
		while (npcSelectCard_stage2(i));

	else if (stage == 3)
		while (npcSelectCard_stage3(i));


	SetCurrentCursorPos(_X - 5, _Y - 1);
	printf("               ");
}

void ShowResult_Victory() {
	COORD cur = GetCurrentCursorPos();
	system("cls"); //콘솔창 초기화
	SetCurrentCursorPos(cur.X + 500, cur.Y + 200);
	PlaySound(TEXT("victory.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
	printf("\n\n\n\n\n\n\n\n								  승리!!\n\n\n\n\n\n							Press Enter to go next stage!");
	result_flag = 1; // 승리
	while (1) {
		while (!_kbhit());
		int key = _getch();
		if (key == ENTER) {
			stage++;
			npc_index++;
			//player_index++; // 발표용
			if (npc_index == 4)
				npc_index = 0;
			//if (player_index == 4)
			//	player_index = 0;
			system("cls"); //콘솔창 초기화
			break;
		}
	}

}
void ShowResult_Lose() {
	COORD cur = GetCurrentCursorPos();
	system("cls"); //콘솔창 초기화
	SetCurrentCursorPos(cur.X + 500, cur.Y + 200);
	PlaySound(TEXT("lose.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
	printf("\n\n\n\n\n\n\n\n								  패배..\n\n\n\n\n\n							Press Enter to revenge");
	result_flag = 2; // 패배
	while (1) {
		while (!_kbhit());
		int key = _getch();
		if (key == ENTER) {
			//stage = 1;
			//npc_index = player_index + 1;
			system("cls"); //콘솔창 초기화
			break;
		}
	}

}
void ShowResult_Tie() {
	COORD cur = GetCurrentCursorPos();
	system("cls"); //콘솔창 초기화
	SetCurrentCursorPos(cur.X + 500, cur.Y + 200);
	//PlaySound(TEXT("lose.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
	printf("\n\n\n\n\n\n\n\n								 무승부!\n\n\n\n\n\n							Press Enter to revenge");
	result_flag = 3; // 무승부
	while (1) {
		while (!_kbhit());
		int key = _getch();
		if (key == ENTER) {
			system("cls"); //콘솔창 초기화
			break;
		}
	}

}
void gameclear() {

	COORD cur = GetCurrentCursorPos();
	//system("cls"); //콘솔창 초기화
	SetCurrentCursorPos(cur.X + 500, cur.Y + 200);
	PlaySound(TEXT("clear.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
	printf("\n\n\n\n\n\n\n\n								  게임 클리어!!\n\n\n\n\n\n							   Press Enter to play again!!");
	while (1) {
		while (!_kbhit());
		int key = _getch();
		if (key == ENTER) {
			system("cls"); //콘솔창 초기화
			break;
		}
	}


}

void updateGameBoard()
{
	for (int i = 0; i < 3; i++)
	{

		selectNpcCard(i);			//npc가 한 카드를 뽑음

		//캐릭터의 이동이 먼저 실행
		if ((p_card[i] == QKEY || p_card[i] == WKEY || p_card[i] == EKEY || p_card[i] == RKEY || (player_index == 2 && p_card[i] == SKEY)) &&
			(n_card[i] == RIGHT || n_card[i] == LEFT || n_card[i] == UP || n_card[i] == DOWN || n_card[i] == AKEY)) {

			drawNpcAction(i);
			drawPlayerAction(i);

			if (npcHP <= 0 && playerHP <= 0) {
				Sleep(3000);
				ShowResult_Tie();
				break;
			}
			else if (npcHP <= 0) {
				Sleep(3000);
				ShowResult_Victory();
				break;
			}
			else if (playerHP <= 0) {
				Sleep(3000);
				ShowResult_Lose();
				break;
			}

		}
		else if ((n_card[i] == QKEY || n_card[i] == WKEY || n_card[i] == EKEY || n_card[i] == RKEY) &&
			(p_card[i] == RIGHT || p_card[i] == LEFT || p_card[i] == UP || p_card[i] == DOWN || p_card[i] == AKEY)) {

			drawPlayerAction(i);
			drawNpcAction(i);

			if (npcHP <= 0 && playerHP <= 0) {
				Sleep(3000);
				ShowResult_Tie();
				break;
			}
			else if (npcHP <= 0) {
				Sleep(3000);
				ShowResult_Victory();
				break;
			}
			else if (playerHP <= 0) {
				Sleep(3000);
				ShowResult_Lose();
				break;
			}
		}
		else {	//조건이 없으면 그냥 플레이어 부터 카드 실행
			drawPlayerAction(i);
			drawNpcAction(i);
			if (npcHP <= 0 && playerHP <= 0) {
				Sleep(3000);
				ShowResult_Tie();
				break;
			}
			else if (npcHP <= 0) {
				Sleep(3000);
				ShowResult_Victory();
				break;
			}
			else if (playerHP <= 0) {
				Sleep(3000);
				ShowResult_Lose();
				break;
			}
		}

		//한 턴이 끝났을 때, 가드가 걸려있으면 가드 초기화
		character[player_index].guard = 0;
		character[npc_index].guard = 0;
	}

	//왼쪽 하단 카드 선택 창 초기화, n_card 초기화
	for (int i = 0; i < 3; i++) {
		DeleteSelectedMoveCard(i);
		n_card[i] = 0;
	}

	//턴이 끝나면 캐릭터 모두 mp15 획득
	Sleep(1000);

	if (result_flag == 0) {
		MPEffect_player(15);
		MPEffect_npc(15);

		playerMP += 15;
		if (playerMP > 100) {
			playerMP = 100;
		}
		npcMP += 15;
		if (npcMP > 100) {
			npcMP = 100;
		}
		drawstatus(0, 0, 0, 0);
	}
}

void DrawSelectedRight(int n)   //n번째 카드 선택란에 그림을 그림
{
	int x, y;

	SetCurrentCursorPos(CARD_ORIGIN_X + 8 + (n * 22), CARD_ORIGIN_Y + 4);
	COORD cur = GetCurrentCursorPos();


	for (x = 0; x < 7; x++)
	{
		SetCurrentCursorPos(cur.X + (x * 2), cur.Y);
		printf("■");

		if (x == 6)
		{
			SetCurrentCursorPos(cur.X + (x * 2) - 2, cur.Y - 1);
			printf("■");
			SetCurrentCursorPos(cur.X + (x * 2) - 4, cur.Y - 2);
			printf("■");
			SetCurrentCursorPos(cur.X + (x * 2) - 2, cur.Y + 1);
			printf("■");
			SetCurrentCursorPos(cur.X + (x * 2) - 4, cur.Y + 2);
			printf("■");
		}

	}

}

void DrawSelectedLeft(int n)
{
	int x, y;

	SetCurrentCursorPos(CARD_ORIGIN_X + 8 + (n * 22), CARD_ORIGIN_Y + 4);
	COORD cur = GetCurrentCursorPos();


	for (x = 0; x < 7; x++)
	{
		SetCurrentCursorPos(cur.X + (x * 2), cur.Y);
		printf("■");

		if (x == 0)
		{
			SetCurrentCursorPos(cur.X + (x * 2) + 2, cur.Y + 1);
			printf("■");
			SetCurrentCursorPos(cur.X + (x * 2) + 4, cur.Y + 2);
			printf("■");
			SetCurrentCursorPos(cur.X + (x * 2) + 2, cur.Y - 1);
			printf("■");
			SetCurrentCursorPos(cur.X + (x * 2) + 4, cur.Y - 2);
			printf("■");
		}

	}
}

void DrawSelectedUp(int n)
{
	int x, y;

	SetCurrentCursorPos(CARD_ORIGIN_X + 14 + (n * 22), CARD_ORIGIN_Y + 1);
	COORD cur = GetCurrentCursorPos();


	for (y = 1; y < 7; y++)
	{
		SetCurrentCursorPos(cur.X, cur.Y + y);
		printf("■");

		if (y == 5)
		{
			SetCurrentCursorPos(cur.X + 2, cur.Y + 2);
			printf("■");
			SetCurrentCursorPos(cur.X + 4, cur.Y + 3);
			printf("■");
			SetCurrentCursorPos(cur.X - 2, cur.Y + 2);
			printf("■");
			SetCurrentCursorPos(cur.X - 4, cur.Y + 3);
			printf("■");
		}

	}
}
void DrawSelectedDown(int n)
{
	int x, y;

	SetCurrentCursorPos(CARD_ORIGIN_X + 14 + (n * 22), CARD_ORIGIN_Y + 1);
	COORD cur = GetCurrentCursorPos();


	for (y = 1; y < 7; y++)
	{
		SetCurrentCursorPos(cur.X, cur.Y + y);
		printf("■");

		if (y == 1)
		{
			SetCurrentCursorPos(cur.X - 2, cur.Y + 5);
			printf("■");
			SetCurrentCursorPos(cur.X - 4, cur.Y + 4);
			printf("■");
			SetCurrentCursorPos(cur.X + 2, cur.Y + 5);
			printf("■");
			SetCurrentCursorPos(cur.X + 4, cur.Y + 4);
			printf("■");
		}

	}
}
void DrawSelectedMP(int n)
{
	int x, y;

	SetCurrentCursorPos(CARD_ORIGIN_X + 14 + (n * 22), CARD_ORIGIN_Y + 1);
	COORD cur = GetCurrentCursorPos();

	int Origin_X = CARD_ORIGIN_X + 11 * n + 2;
	int Origin_Y = CARD_ORIGIN_Y + 2;

	fence1(Origin_X, Origin_Y);
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 3);
	printf("★");

	SetCurrentCursorPos((Origin_X + 2) * 2, Origin_Y + 1);
	printf("MP + 15");
}
void DrawSelectedDefense(int n)
{
	int x, y;

	SetCurrentCursorPos(CARD_ORIGIN_X + 14 + (n * 22), CARD_ORIGIN_Y + 1);
	COORD cur = GetCurrentCursorPos();

	int Origin_X = CARD_ORIGIN_X + 11 * n + 2;
	int Origin_Y = CARD_ORIGIN_Y + 2;

	fence1(Origin_X, Origin_Y);
	SetCurrentCursorPos((Origin_X + 3) * 2, Origin_Y + 3);
	printf("▣");

	SetCurrentCursorPos((Origin_X + 2) * 2, Origin_Y + 1);
	printf("DM - 15");

}
void DrawSelectedQ(int n)
{
	int Origin_X = CARD_ORIGIN_X + 11 * n + 1;
	int Origin_Y = CARD_ORIGIN_Y + 2;

	SetCurrentCursorPos((Origin_X + 1) * 2, Origin_Y);
	printf("Dm %d    Mp %d", character[player_index].q_dm, character[player_index].q_mp);

	fence2(Origin_X, Origin_Y);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			SetCurrentCursorPos((Origin_X + 3 + j) * 2, Origin_Y + 2 + i);
			if (character[player_index].q_scope[i][j] == 0) {
				printf("□");
			}
			if (character[player_index].q_scope[i][j] == 1) {
				printf("■");
			}
		}
	}
}
void DrawSelectedW(int n)
{
	int Origin_X = CARD_ORIGIN_X + 11 * n + 1;
	int Origin_Y = CARD_ORIGIN_Y + 2;

	SetCurrentCursorPos((Origin_X + 1) * 2, Origin_Y);
	printf("Dm %d    Mp %d", character[player_index].w_dm, character[player_index].w_mp);

	fence2(Origin_X, Origin_Y);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			SetCurrentCursorPos((Origin_X + 3 + j) * 2, Origin_Y + 2 + i);
			if (character[player_index].w_scope[i][j] == 0) {
				printf("□");
			}
			if (character[player_index].w_scope[i][j] == 1) {
				printf("■");
			}
		}
	}
}
void DrawSelectedE(int n)
{
	int Origin_X = CARD_ORIGIN_X + 11 * n + 1;
	int Origin_Y = CARD_ORIGIN_Y + 2;

	SetCurrentCursorPos((Origin_X + 1) * 2, Origin_Y);
	printf("Dm %d    Mp %d", character[player_index].e_dm, character[player_index].e_mp);

	fence2(Origin_X, Origin_Y);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			SetCurrentCursorPos((Origin_X + 3 + j) * 2, Origin_Y + 2 + i);
			if (character[player_index].e_scope[i][j] == 0) {
				printf("□");
			}
			if (character[player_index].e_scope[i][j] == 1) {
				printf("■");
			}
		}
	}
}
void DrawSelectedR(int n)
{
	int Origin_X = CARD_ORIGIN_X + 11 * n + 1;
	int Origin_Y = CARD_ORIGIN_Y + 2;

	SetCurrentCursorPos((Origin_X + 1) * 2, Origin_Y);
	printf("Dm %d    Mp %d", character[player_index].r_dm, character[player_index].r_mp);

	fence2(Origin_X, Origin_Y);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			SetCurrentCursorPos((Origin_X + 3 + j) * 2, Origin_Y + 2 + i);
			if (character[player_index].r_scope[i][j] == 0) {
				printf("□");
			}
			if (character[player_index].r_scope[i][j] == 1) {
				printf("■");
			}
		}
	}
}
void DrawSelectedS(int n)
{
	int Origin_X = (CARD_ORIGIN_X + 11 * n + 2) * 2;
	int Origin_Y = CARD_ORIGIN_Y + 1;

	if (player_index == 0)
	{
		int width = 5;
		int height = 4;
		int x, y;

		for (x = 1; x <= width + 1; x++)
		{
			SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + 1);
			if (x == 1)
				printf("┌");
			else if (x == width + 1)
				printf("┐");
			else
				printf("─");
			SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + height + 2);
			if (x == width + 1)
				printf("┘");
			else
				printf("─");
		}

		for (y = 1; y <= height + 1; y++)
		{
			SetCurrentCursorPos(Origin_X + 2, Origin_Y + y + 1);
			if (y == height + 1)
				printf("└");
			else
				printf("│");
			SetCurrentCursorPos(Origin_X + 2 * (width + 1), Origin_Y + y + 1);
			if (y != height + 1)
				printf("│");
		}
		SetCurrentCursorPos(Origin_X + 2 + 2, Origin_Y + 4);
		printf("●");
		SetCurrentCursorPos(Origin_X + 2 + 2, Origin_Y + 5);
		printf("┠");
		SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 2);
		printf("│");
		SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 3);
		printf("│");
		SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 4);
		printf("┼");
		SetCurrentCursorPos(Origin_X + 2 + 6, Origin_Y + 2);
		printf("─");
		SetCurrentCursorPos(Origin_X + 2 + 8, Origin_Y + 2);
		printf("┐");
		SetCurrentCursorPos(Origin_X + 2 + 6, Origin_Y + 3);
		textcolor(12);
		printf("♥");
		textcolor(15);
		SetCurrentCursorPos(Origin_X + 2 + 8, Origin_Y + 3);
		printf("│");
		SetCurrentCursorPos(Origin_X + 2 + 8, Origin_Y + 4);
		printf("┘");
	}
	else if (player_index == 1)
	{
		int width = 5;
		int height = 4;
		int x, y;

		for (x = 1; x <= width + 1; x++)
		{
			SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + 1);
			if (x == 1)
				printf("┌");
			else if (x == width + 1)
			{
				printf("┐");
			}
			else
				printf("─");
			SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + height + 2);
			if (x == width + 1)
				printf("┘");
			else
				printf("─");
		}

		for (y = 1; y <= height + 1; y++)
		{
			SetCurrentCursorPos(Origin_X + 2, Origin_Y + y + 1);
			if (y == height + 1)
				printf("└");

			else
				printf("│");

			SetCurrentCursorPos(Origin_X + 2 * (width + 1), Origin_Y + y + 1);
			if (y != height + 1)
				printf("│");

		}
		SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 4);
		printf("/");
		SetCurrentCursorPos(Origin_X + 2 + 6, Origin_Y + 4);
		printf("＼");
		SetCurrentCursorPos(Origin_X + 2 + 2, Origin_Y + 5);
		printf("↙");
		SetCurrentCursorPos(Origin_X + 2 + 6, Origin_Y + 5);
		printf("↙");
		SetCurrentCursorPos(Origin_X + 2 + 5, Origin_Y + 2);
		printf("○");
		SetCurrentCursorPos(Origin_X + 2 + 5, Origin_Y + 3);
		printf("＃");
		SetCurrentCursorPos(Origin_X + 2 + 3, Origin_Y + 3);
		printf("─");
		SetCurrentCursorPos(Origin_X + 2 + 3, Origin_Y + 2);
		printf("＼");
	}
	else if (player_index == 2)
	{
		int width = 5;
		int height = 4;
		int x, y;

		for (x = 1; x <= width + 1; x++)
		{
			SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + 1);
			if (x == 1)
				printf("┌");
			else if (x == width + 1)
				printf("┐");
			else
				printf("─");
			SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + height + 2);
			if (x == width + 1)
				printf("┘");
			else
				printf("─");
		}

		for (y = 1; y <= height + 1; y++)
		{
			SetCurrentCursorPos(Origin_X + 2, Origin_Y + y + 1);
			if (y == height + 1)
				printf("└");
			else
				printf("│");
			SetCurrentCursorPos(Origin_X + 2 * (width + 1), Origin_Y + y + 1);
			if (y != height + 1)
				printf("│");
		}

		for (int i = 1; i <= 4; i++)
			for (int j = 1; j <= 4; j++)
			{
				if ((i + j) % 2 == 0)
				{
					SetCurrentCursorPos(Origin_X + 2 + (i * 2), Origin_Y + 1 + j);
					printf("↙");
				}
			}
	}
	else if (player_index == 3)
	{
		int width = 5;
		int height = 4;
		int x, y;

		for (x = 1; x <= width + 1; x++)
		{
			SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + 1);
			if (x == 1)
				printf("┌");
			else if (x == width + 1)
			{
				printf("┐");
			}
			else
				printf("─");
			SetCurrentCursorPos(Origin_X + x * 2, Origin_Y + height + 2);
			if (x == width + 1)
				printf("┘");
			else
				printf("─");
		}

		for (y = 1; y <= height + 1; y++)
		{
			SetCurrentCursorPos(Origin_X + 2, Origin_Y + y + 1);
			if (y == height + 1)
				printf("└");
			else
				printf("│");

			SetCurrentCursorPos(Origin_X + 2 * (width + 1), Origin_Y + y + 1);
			if (y != height + 1)
				printf("│");

		}

		SetCurrentCursorPos(Origin_X + 2 + 2, Origin_Y + 4);
		printf("●");
		SetCurrentCursorPos(Origin_X + 2 + 2, Origin_Y + 5);
		printf("┠");
		SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 3);
		printf("？");
		SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 4);
		printf("│");
		SetCurrentCursorPos(Origin_X + 2 + 4, Origin_Y + 5);
		printf("│");
		SetCurrentCursorPos(Origin_X + 2 + 6, Origin_Y + 2);
		printf("─");
		SetCurrentCursorPos(Origin_X + 2 + 8, Origin_Y + 2);
		printf("┐");
		SetCurrentCursorPos(Origin_X + 2 + 6, Origin_Y + 3);
		textcolor(9);
		printf("★");
		textcolor(15);
		SetCurrentCursorPos(Origin_X + 2 + 8, Origin_Y + 3);
		printf("│");
		SetCurrentCursorPos(Origin_X + 2 + 8, Origin_Y + 4);
		printf("┘");
	}
}
void DeleteSelectedMoveCard(int n) //선택된 키 그림 지우기
{

	int Origin_X = CARD_ORIGIN_X + 11 * n + 1;
	int Origin_Y = CARD_ORIGIN_Y + 2;

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			SetCurrentCursorPos((Origin_X + 1 + j) * 2, Origin_Y + i);
			printf("  ");
		}
	}
}
void DrawDoubleJumpDirect(int n)
{
	while (1)
	{
		while (!_kbhit());

		int key = _getch();

		if (key == UP)
		{
			doubleJumpDirect = UP;
			break;
		}
		else if (key == DOWN)
		{
			doubleJumpDirect = DOWN;
			break;
		}
		else if (key == RIGHT)
		{
			doubleJumpDirect = RIGHT;
			break;
		}
		else if (key == LEFT)
		{
			doubleJumpDirect = LEFT;
			break;
		}
	}


	int Origin_X = (CARD_ORIGIN_X + 11 * n + 2) * 2;
	int Origin_Y = CARD_ORIGIN_Y + 1;

	if (doubleJumpDirect == UP)
	{
		SetCurrentCursorPos(Origin_X, Origin_Y + 1);
		printf("U");
		SetCurrentCursorPos(Origin_X, Origin_Y + 2);
		printf("P");
	}
	else if (doubleJumpDirect == DOWN)
	{
		SetCurrentCursorPos(Origin_X, Origin_Y + 1);
		printf("D");
		SetCurrentCursorPos(Origin_X, Origin_Y + 2);
		printf("O");
		SetCurrentCursorPos(Origin_X, Origin_Y + 3);
		printf("W");
		SetCurrentCursorPos(Origin_X, Origin_Y + 4);
		printf("N");
	}
	else if (doubleJumpDirect == RIGHT)
	{
		SetCurrentCursorPos(Origin_X, Origin_Y + 1);
		printf("R");
		SetCurrentCursorPos(Origin_X, Origin_Y + 2);
		printf("I");
		SetCurrentCursorPos(Origin_X, Origin_Y + 3);
		printf("G");
		SetCurrentCursorPos(Origin_X, Origin_Y + 4);
		printf("H");
		SetCurrentCursorPos(Origin_X, Origin_Y + 5);
		printf("T");
	}
	else if (doubleJumpDirect == LEFT)
	{
		SetCurrentCursorPos(Origin_X, Origin_Y + 1);
		printf("L");
		SetCurrentCursorPos(Origin_X, Origin_Y + 2);
		printf("E");
		SetCurrentCursorPos(Origin_X, Origin_Y + 3);
		printf("F");
		SetCurrentCursorPos(Origin_X, Origin_Y + 4);
		printf("T");
	}

	SetCurrentCursorPos(0, 0);
}
void selectPlayerCard()
{
	int cnt = 0;
	int abstract_MP = playerMP;

	while (1)
	{
		while (!_kbhit());

		int key = _getch();
		if (cnt > 0) {
			if (cnt == 1 && key == p_card[cnt - 1]) {
				continue;
			}
			if (cnt == 2 && (key == p_card[cnt - 1] || key == p_card[cnt - 2])) {
				continue;
			}
		}

		if (cnt < 3 && key == RIGHT) // 오른쪽
		{
			p_card[cnt] = RIGHT;
			DrawSelectedRight(cnt);
			cnt += 1;
		}
		if (cnt < 3 && key == LEFT) // 왼쪽
		{
			p_card[cnt] = LEFT;
			DrawSelectedLeft(cnt);
			cnt += 1;
		}
		if (cnt < 3 && key == UP) // 위쪽
		{
			p_card[cnt] = UP;
			DrawSelectedUp(cnt);
			cnt += 1;
		}
		if (cnt < 3 && key == DOWN) // 아래쪽
		{
			p_card[cnt] = DOWN;
			DrawSelectedDown(cnt);
			cnt += 1;
		}
		if (cnt < 3 && key == AKEY)
		{
			p_card[cnt] = AKEY;
			DrawSelectedDefense(cnt);
			cnt += 1;
		}
		if (cnt < 3 && key == DKEY) // D (MP+)
		{
			p_card[cnt] = DKEY;
			DrawSelectedMP(cnt);
			abstract_MP += 15;
			cnt += 1;
		}
		if (cnt < 3 && key == QKEY) // Q
		{
			if (abstract_MP < character[player_index].q_mp)
				continue;
			abstract_MP -= character[player_index].q_mp;
			p_card[cnt] = QKEY;
			DrawSelectedQ(cnt);
			cnt += 1;
		}
		if (cnt < 3 && key == WKEY) // W
		{
			if (abstract_MP < character[player_index].w_mp)
				continue;
			abstract_MP -= character[player_index].w_mp;
			p_card[cnt] = WKEY;
			DrawSelectedW(cnt);
			cnt += 1;
		}
		if (cnt < 3 && key == EKEY) // E
		{
			if (abstract_MP < character[player_index].e_mp)
				continue;
			abstract_MP -= character[player_index].e_mp;
			p_card[cnt] = EKEY;
			DrawSelectedE(cnt);
			cnt += 1;
		}
		if (cnt < 3 && key == RKEY) // R
		{
			if (abstract_MP < character[player_index].r_mp)
				continue;
			abstract_MP -= character[player_index].r_mp;
			p_card[cnt] = RKEY;
			DrawSelectedR(cnt);
			cnt += 1;
		}
		if (cnt < 3 && key == SKEY) // 특수키
		{
			if (player_index == 0)   //도크
			{
				if (abstract_MP < 50)
					continue;
				abstract_MP -= 50;
			}
			else if (player_index == 1)   //캐럿
			{
				/*if (abstract_MP < 10)
					continue;
				abstract_MP -= 10;*/
			}
			else if (player_index == 2)   //바니
			{
				if (abstract_MP < 70)
					continue;
				abstract_MP -= 70;
			}
			else if (player_index == 3)   //너츠
			{
				if (playerHP < 20)
					continue;
				abstract_MP += 60;
			}
			p_card[cnt] = SKEY;
			DrawSelectedS(cnt);
			if (player_index == 1)   //캐럿 더블 점프는 방향키를 한 번 더 받음
				DrawDoubleJumpDirect(cnt);

			cnt += 1;
		}

		if (cnt > 0 && key == CANCEL) // 취소
		{
			if (p_card[cnt - 1] == QKEY)
				abstract_MP += character[player_index].q_mp;
			else if (p_card[cnt - 1] == WKEY)
				abstract_MP += character[player_index].w_mp;
			else if (p_card[cnt - 1] == EKEY)
				abstract_MP += character[player_index].e_mp;
			else if (p_card[cnt - 1] == RKEY)
				abstract_MP += character[player_index].r_mp;
			else if (p_card[cnt - 1] == DKEY)
				abstract_MP -= 15;
			//else if (p_card[cnt - 1] == AKEY)
				//abstract_MP += 15;
			else if (p_card[cnt - 1] == SKEY)
			{
				if (player_index == 0)   //도크
					abstract_MP += 50;
				//else if (player_index == 1)   //캐럿
					//abstract_MP += 10;
				else if (player_index == 2)   //바니
					abstract_MP += 70;
			}

			cnt -= 1;
			DeleteSelectedMoveCard(cnt);
		}
		SetCurrentCursorPos(0, 0);
		if (cnt == 3 && key == ENTER) //카드 확정
		{
			SetCurrentCursorPos(0, 0);
			//커서가 선택란으로 가있기 때문에 다시 플레이어 쪽으로 옮겨줌
			SetCurrentCursorPos(PlayerConsolePos_X, PlayerConsolePos_Y);
			break;
		}
		SetCurrentCursorPos(0, 0);

	}
}
void DrawCommonSkillMenual() {
	Draw_Move();
	Draw_Defense();
	Draw_Recovery();
	Draw_Attack();
}
void DrawSpecialSkillMenual() {
	if (player_index == 0)
		Draw_Special_dogk();
	else if (player_index == 1)
		Draw_Special_carrot();
	else if (player_index == 2)
		Draw_Special_bunny();
	else if (player_index == 3)
		Draw_Special_nuts();
}
void drawGame(int player_index)
{
	DrawGameBoard();
	DrawSelectedCard();
	DrawCommonSkillMenual();
	DrawSpecialSkillMenual();

	SetCurrentCursorPos(0, 0);
}
void drawStory()
{
	printf("Space bar : 다음\nEnter : 스킵");

	int storyline = 0;

	const char* str[7];
	int i = 1;
	str[0] = "\n\n\n                    4종류의 동물들이 있다.";
	str[1] = "\n\n\n                    그들은 예로부터 힘도 비슷하고 사이도 좋지 않았다.";
	str[2] = "\n\n\n                    팽팽한 긴장감을 유지하던 와중, 결국 서열을 정하는 싸움을 하기로 한다.";
	str[3] = "\n\n\n                    그러나, 만약 전쟁을 하게 되면 모두에게 피해가 크므로";
	str[4] = "\n\n\n                    동물별로 가장 강한 한 마리가 영웅이 되어 대표로 싸움에 나선다.";
	str[5] = "\n\n\n                    정정당당한 싸움 후, 그 결과에는 이의를 제기할 수 없고 서열이 정해진다.";
	str[6] = "\n\n\n                    동물의 왕국의 리더가 될 동물은 누가 될 것인가?\n\n\n\n\n                      엔터 키를 누르세요.";
	printf("%s", str[0]);

	while (1) {
		while (!_kbhit());
		int key = _getch();
		if (key == ENTER) {

			break;
		}

		if (key == SPACE) {
			if (i >= 7)
				continue;
			PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
			printf("%s", str[i]);
			i++;
		}
		else
			continue;
	}

}
void drawcredit() {
	printf("\n\n\n\n\n                              세종대학교 소프트웨어학과 SW설계기초 3조\n");
	printf("\n\n\n\n\n                              17011680 강민수");
	printf("\n\n\n\n\n                              18011667 신무성");
	printf("\n\n\n\n\n                              18011674 김진호");
	printf("\n\n\n\n\n                              18011681 임성근");
	printf("\n\n\n\n\n                              엔터 키를 눌러 타이틀로 돌아갑니다.\n");
	while (1) {

		while (!_kbhit());

		int key = _getch();
		if (key == ENTER) {
			system("cls"); //콘솔창 초기화
			screen = 0;
			break;
		}
	}
}
void loading() {
	PlaySound(TEXT("loading.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
	printf("\n\n\n\n\n\n\n                                                         Now Loading . . . . . .");

	printf("\n\n\n\n\n\n\n                                                           %s     VS     %s", character[player_index].name, character[npc_index].name);
	Sleep(3000);
	system("cls"); //콘솔창 초기화
}
void drawmanual() {
	printf("\n\n              - 동물의 왕국은 ‘턴제 전략 게임’ 입니다.");
	printf("\n\n\n              - 게임의 상황을 예측하면서 매 턴마다 3개의 카드를 고르고, 고른 카드로 공격과 방어를 주고 받으며 전투합니다.");
	printf("\n\n\n              - 플레이어와 npc가 고른 카드는 번갈아 가며 한 번씩 실행됩니다.");
	printf("\n\n\n              - 그러나, 방어 카드와 이동 카드는 공격 방어 및 회피를 위해 공격보다 먼저 실행됩니다.");
	printf("\n\n\n              - 4개의 캐릭터가 있고, 캐릭터들이 사용할 수 있는 카드는 각자 다릅니다.");
	printf("\n\n\n              - HP가 0이 되면 게임에서 패배하고, npc의 HP가 0이 되면 게임에서 승리합니다.");
	printf("\n\n\n              - MP를 사용하는 카드의 경우, MP가 부족하다면 선택할 수 없습니다.");
	printf("\n\n\n              - 게임 중 오른쪽에 있는 미니 메뉴얼에서 선택할 수 있는 카드를 확인할 수 있습니다.");
	printf("\n\n\n              - 스테이지는 3스테이지까지 존재하며, 게임에서 승리하면 다음 스테이지로 넘어갑니다.");
	printf("\n\n\n              - 게임에서 패배하거나 무승부일 경우 현재 스테이지를 한번 더 진행합니다.");

	printf("\n\n\n              - 높은 스테이지일수록 npc가 더욱 전략적인 전투를 합니다.");

	printf("\n\n\n\n\n				                  엔터 키를 눌러 타이틀로 돌아갑니다.");
	while (1) {

		while (!_kbhit());

		int key = _getch();
		if (key == ENTER) {
			system("cls"); //콘솔창 초기화
			screen = 0;
			break;
		}
	}
}
void drawTitle() {
	const char* title;
	const char* manual;
	const char* credit;
	const char* temp = " ";
	const char* none = " ";
	printf("                                                                                                                                                \n");
	printf("                                                                                                                                                \n");
	printf("         @@@@@@@@@@@@@@@       @@@@@@@@@@@@@@@@          @@@@@@@@        @@                @@@@@@@@         @@       @@@@@@@@@@@@@@@@@@@        \n");
	printf("         @@                    @@            @@        @@        @@      @@              @@        @@       @@                        @@        \n");
	printf("         @@                    @@            @@       @@          @@     @@             @@          @@      @@                        @@        \n");
	printf("         @@                    @@            @@      @@            @@    @@            @@            @@     @@                        @@        \n");
	printf("         @@@@@@@@@@@@@@@       @@            @@       @@          @@     @@             @@          @@      @@                        @@        \n");
	printf("               @@               @@@@@@@@@@@@@@@@        @@        @@     @@               @@       @@       @@                        @@        \n");
	printf("               @@                                         @@@@@@@@       @@                 @@@@@@@         @@@@@                     @@        \n");
	printf("        @@@@@@@@@@@@@@@@@@@@  @@@@@@@@@@@@@@@@@@@@@@                     @@                   @@            @@     @@@@@@@@@@@@@@@@@@@@@@@@     \n");
	printf("                                       @@                                @@                   @@            @@                @@                \n");
	printf("             @@@@@                     @@             @@@@@@@@@@@@@@@@@@@@@         @@@@@@@@@@@@@@@@@@@@@@@@@@                @@                \n");
	printf("           @@     @@             @@@@@@@@@@@@@@@@                        @@                                 @@         @@@@@@@@@@@@@@@@@        \n");
	printf("          @@      @@                          @@                         @@                  @@@@@@@@                                 @@        \n");
	printf("           @@    @@             @@@@@@@@@@@@@@@@                         @@                @@        @@                               @@        \n");
	printf("            @@@@@               @@                                       @@                @@        @@                               @@        \n");
	printf("                                 @@@@@@@@@@@@@@@@                                           @@      @@                                @@        \n");
	printf("                                                                                              @@@@@@                                  @@        \n\n\n\n\n");

	title = "                                                        엔터 : 시작 !!";
	manual = "                                                        Q : 설명서";
	credit = "                                                        W : 제작진";
	printf("%s\n\n\n", title);
	printf("%s\n\n\n", manual);
	printf("%s", credit);

	while (1) {

		while (!_kbhit());

		int key = _getch();
		if (key == ENTER) {
			screen = 3;
			break;
		}
		else if (key == QKEY) {
			system("cls"); //콘솔창 초기화
			screen = 1; // 메뉴얼로
			drawmanual();
			break;
		}
		else if (key == WKEY) {
			system("cls"); //콘솔창 초기화
			screen = 2; // 제작진
			drawcredit();
			break;
		}
	}
}

void drawCharacters() {
	printf("                                                           Select Character !!                                            \n\n\n\n");
	printf("                                                좌우 방향키로 캐릭터를 선택 후 엔터를 누르세요\n\n\n\n");
	printf("      #                #                  #                  #                   ###            ###                 # # #             # # #       \n");
	printf("     ##               ##                 # #                #  #                # # #          # # #               #      #          #     #      \n");
	printf("    #  ############### ##               #  #################    #               # # #          # # #               #      # #  #  # #      #      \n");
	printf("   ###                  #        #     #                         #              # # #          # # #               #                   #   #  ### \n");
	printf("   ##                   ##      ##     #                          #             #   #          #   #              #  ##################    #  ### \n");
	printf("  ## ########   ######### #    # #     #  #######        #######   #            #   ############   #       #    ###  ###    ####   ####   ##   #  \n");
	printf(" ##  ####  ##    ###  ##  #   #  #     #                           #           #                    #     ###    #  ####################    ## #  \n");
	printf(" #     ####       ###     #  #   #     #             ###           #         #    #### #     #### #  #     #   ###  #####          #####   #   #  \n");
	printf(" #            ##          # #    #   ############     #     #############   #      ####       ####    #    #    #            ###           ### #  \n");
	printf(" #            #           ##    #       #             #           #         #              #          #    #   ###        #   #   #       ##   #  \n");
	printf(" ##          ##          # #   #     ############ #  # #  # #############    #          ########      #    #      #        ### ###         #   #  \n");
	printf("   ##       #  #         #########       #         ##   ##       #           ##          # # #       #     #    ###                      ###   #  \n");
	printf("     ##    #    #     ##   #  #           #                     #             ##         ## ##      #      #       ##                  ###     #  \n");
	printf("       ##            ##    ####            ##                 ##                ###              ###      # #        ####            ##        #  \n");
	printf("         ############                        #################                     ##############        #   #           ###########            \n\n");
	printf("            도크                                  캐럿                                  바니                                  너츠                \n");
	printf("            Dogk                                 Carat                                 Bunny                                  Nuts        \n\n\n\n\n");


}
void drawPick() {
	printf("▲");
}
void deletePick() {
	curPos = GetCurrentCursorPos();
	SetCurrentCursorPos(curPos.X - 2, curPos.Y);
	printf("  ");
}

void selectCharacter() {
	printf("\n");
	drawCharacters();

	COORD cp = GetCurrentCursorPos();
	SetCurrentCursorPos(cp.X + 13, cp.Y);
	cp = GetCurrentCursorPos();
	drawPick();


	while (1) {
		while (!_kbhit());
		int key = _getch();

		if (key == RIGHT && player_index < 3) {
			deletePick();
			SetCurrentCursorPos(cp.X + 38, cp.Y);
			cp = GetCurrentCursorPos();
			drawPick();
			PlaySound(TEXT("selecting.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
			player_index++;
		}
		if (key == LEFT && player_index > 0) {
			deletePick();
			SetCurrentCursorPos(cp.X - 38, cp.Y);
			cp = GetCurrentCursorPos();
			drawPick();
			PlaySound(TEXT("selecting.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
			player_index--;
		}
		if (key == ENTER) {
			if (player_index == 0) {
				PlaySound(TEXT("Pick.wav"), NULL, SND_SYNC | SND_NODEFAULT);
				PlaySound(TEXT("Dogk.wav"), NULL, SND_SYNC | SND_NODEFAULT);
				npc_index = 1;
			}
			else if (player_index == 1) {
				PlaySound(TEXT("Pick.wav"), NULL, SND_SYNC | SND_NODEFAULT);
				PlaySound(TEXT("carat.wav"), NULL, SND_SYNC | SND_NODEFAULT);
				npc_index = 2;
			}
			else if (player_index == 2) {
				PlaySound(TEXT("Pick.wav"), NULL, SND_SYNC | SND_NODEFAULT);
				PlaySound(TEXT("bunny.wav"), NULL, SND_SYNC | SND_NODEFAULT);
				npc_index = 3;
			}
			else if (player_index == 3) {
				PlaySound(TEXT("Pick.wav"), NULL, SND_SYNC | SND_NODEFAULT);
				PlaySound(TEXT("nuts.wav"), NULL, SND_SYNC | SND_NODEFAULT);
				npc_index = 0;
			}
			Sleep(1000);
			break;
		}
	}
}

int main()
{
	while (1) {
		if (first_check == 0) {
			system("mode con cols=150 lines=40");
			RemoveCursor();
			drawStory();

			first_check = 1;
			system("cls"); //콘솔창 초기화
		}

		PlaySound(TEXT("titleBGM.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
		while (1) {

			drawTitle();
			if (screen == 3) { // 픽창으로 가기
				screen = 0;
				break;
			}
		}
		PlaySound(NULL, NULL, SND_ASYNC | SND_PURGE);
		system("cls"); //콘솔창 초기화
		selectCharacter();
		system("cls"); //콘솔창 초기화


		while (1)
		{
			//if (result_flag == 2) { //패배했을 경우
			//	player_index = 0;
			//	result_flag = 0;
			//	break;
			//}
			result_flag = 0;


			if (stage == 4) {
				gameclear();
				player_index = 0;
				stage = 1;
				cleared_check = 1;
				break;

			}
			system("cls"); //콘솔창 초기화
			loading();
			playerHP = 100;
			playerMP = 100;
			npcHP = 100;
			npcMP = 100;

			drawGame(player_index);   //게임 판 /공통 스킬 메뉴얼그리는거 묶어놈

			setGameInfo();   //초기 게임 정보 (게임 판 배열 초기화)
			drawstatus(0, 0, 0, 0);
			drawPlayer();
			drawNpc();

			while (1) {
				SetCurrentCursorPos(0, 0);
				selectPlayerCard();
				SetCurrentCursorPos(0, 0);
				updateGameBoard();
				SetCurrentCursorPos(0, 0);
				if (result_flag != 0)
					break;
			}
		}
		//getchar();
	}
}
