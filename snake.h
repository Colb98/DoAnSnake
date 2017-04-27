#pragma once
#pragma pack(1)
#include <time.h>
#include <iostream>
#include <Windows.h>
#include <vector>
#include <thread>

using namespace std;

#define MAX_SIZE_SNK 10
#define MAX_SIZE_FOOD 4
#define MAX_SPEED 5
#define MAX_NAME 100
#define INIT_WIDTH 70
#define INIT_HEIGH 20
#define SNAKE_STR "1612343"


extern int HEIGH_CONSOLE, WIDTH_CONSOLE;
extern int Score;

struct Point {
	int x, y;
};

struct Snake {
	vector<Point> body;
	int size;
	char charlock;
	char moving;
	int speed;
	int snklen;
	bool alive;
	bool GameOn = true;
	bool portal = false;
	bool porting = false;
	bool pause = false;
};

struct Food {
	Point pos[MAX_SIZE_FOOD];
	int idx;
};

struct Portal {
	Point port;
	Point wall[4];
	int direction; //
};

void FixConsoleWindow();
void GotoXY(int x, int y);
bool isValid(int x, int y, Snake snk);
void GenerateFood(Snake snk, Food &fd, Portal &port);
void ResetData(Snake &snk, Food &fd, Portal &port);
void StartGame(Snake &snk, Food &fd, Portal &port);
void DrawBoard(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0);
void ExitGame(Snake &snk, thread t);
void PauseGame(HANDLE t);
void Eat(Snake &snk, Food &fd, Portal &port);
void ProcessDead(Snake &snk);
void DrawSnkFd(Snake snk, Food fd, char* s1, char s2);
void MoveRight(Snake &snk, Food &fd, Portal &port);
void MoveLeft(Snake &snk, Food &fd, Portal &port);
void MoveUp(Snake &snk, Food &fd, Portal &port);
void MoveDown(Snake &snk, Food &fd, Portal &port);
void ThreadFunc(Snake *snk, Food *fd, Portal *port);
bool isBiteTail(Snake snk, char Direction);
void Clash(Snake snk);
void DrawSnkPortal(Snake snk, Point port, char* s1, char s2);
void SaveFile(Snake snk, Portal port, Food fd, char *filename);
void Save(Snake snk, Portal port, Food fd);
void OpenFile(Snake &snk, Portal &port, Food &fd, char *filename);
void Open(Snake &snk, Portal &port, Food &fd);
void DrawSnk(Snake snk, char* s);
void DrawPortal(Portal port, char s2);
void DrawFd(Food fd, char s2);
bool isCrashPortal(Snake snk, Portal port, char dir);