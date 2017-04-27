#include "snake.h"



void FixConsoleWindow() {
	HWND console = GetConsoleWindow();
	LONG style = GetWindowLong(console, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(console, GWL_STYLE, style);
}

void GotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
bool isValid(int x, int y, Snake snk) {
	for (int i = 0;i < snk.size;i++)
		if (snk.body[i].x == x && snk.body[i].y == y)
			return false;
	return true;
}

void GenerateFood(Snake snk, Food &fd, Portal &port) {
	int x, y;
	srand(time(NULL));
	//Generate Food
	for (int i = 0;i < MAX_SIZE_FOOD;i++) {
		do {
			x = rand() % (WIDTH_CONSOLE - 1) + 1;
			y = rand() % (HEIGH_CONSOLE - 1) + 1;
		} while (!isValid(x, y, snk));
		fd.pos[i] = { x,y };
	}
	//Generate Portal
	{
		do {
			x = rand() % (WIDTH_CONSOLE - 5) + 3;
			y = rand() % (HEIGH_CONSOLE - 5) + 3;
		} while (!isValid(x, y, snk));
		port.port = { x,y };
		int a = rand() % 4;
		char b[] = { 'A','S','D','W' };
		port.direction = b[a];
		int dx[] = { 0,0,1,1,-1,1,-1,1,0,0,-1,-1,-1,1,-1,1 };
		int dy[] = { -1,1,-1,1, 0,0,-1,-1,-1,1,-1,1,0,0,1,1 };
		for (int j = 0;j < 4;j++)
			port.wall[j] = { x + dx[4 * a + j],y + dy[4 * a + j] };
	}
}

void ResetData(Snake &snk, Food &fd, Portal &port) {
	snk.body.clear();
	snk.body.resize(6);
	Score = 0;
	snk.charlock = 'A', snk.moving = 'D', snk.speed = 1, snk.size = 6;
	fd.idx = 0;
	WIDTH_CONSOLE = INIT_WIDTH,	HEIGH_CONSOLE = INIT_HEIGH;
	snk.body[0] = { 10,5 };
	snk.body[1] = { 11,5 };
	snk.body[2] = { 12,5 };
	snk.body[3] = { 13,5 };
	snk.body[4] = { 14,5 };
	snk.body[5] = { 15,5 };
	GenerateFood(snk, fd, port);
}

void StartGame(Snake &snk, Food &fd, Portal &port) {
	system("cls");
	ResetData(snk, fd, port);
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE);
	snk.alive = 1;
}
void DrawBoard(int x, int y, int width, int height, int curPosX, int curPosY) {
	GotoXY(x, y); cout << 'X';
	for (int i = 1;i < width;i++) cout << 'X';
	cout << 'X';
	GotoXY(x, height + y); cout << 'X';
	for (int i = 1;i < width;i++) cout << 'X';
	cout << 'X';
	for (int i = y + 1;i < height + y;i++) {
		GotoXY(x, i); cout << 'X';
		GotoXY(x + width, i); cout << 'X';
	}
	GotoXY(WIDTH_CONSOLE + 2, 5); cout << "Score: " << Score;
	GotoXY(curPosX, curPosY);
}

void ExitGame(Snake &snk, thread t) {
	// Nothing to see here
}

void PauseGame(HANDLE t) {
	GotoXY(0, HEIGH_CONSOLE + 5);
	cout << "Game Paused! Press A, S, D, W to resume the game.";
	SuspendThread(t);
}

void Eat(Snake &snk, Food &fd, Portal &port) {
	GotoXY(WIDTH_CONSOLE + 9, 5); cout << ++Score;
	snk.body.resize(snk.size + 1);
	snk.body[snk.size] = fd.pos[fd.idx];
	if (fd.idx == MAX_SIZE_FOOD - 1) {
		snk.portal = true;
		//fd.idx = 0;
		//if (snk.speed == MAX_SPEED) {
		//	snk.speed = 1;
		//	snk.size = 6;
		//}
		//else snk.speed++;
		snk.size++;
		snk.snklen++;
	}
	else {
		fd.idx++;
		snk.size++;
		snk.snklen++;
	}
}

void ProcessDead(Snake &snk) {
	snk.alive = 0;
	GotoXY(0, HEIGH_CONSOLE + 2);
	cout << "Dead, type y to continue or anykey to exit." << endl;
}

void DrawSnkFd(Snake snk, Food fd, char* s1, char s2) {
	GotoXY(fd.pos[fd.idx].x, fd.pos[fd.idx].y);
	cout << s2;
	for (int i = 0;i < snk.size;i++) {
		GotoXY(snk.body[i].x, snk.body[i].y);
		cout << s1[i % strlen(s1)]; 
	}
}

void DrawSnk(Snake snk, char* s) {
	for (int i = 0;i < snk.snklen;i++) {
		GotoXY(snk.body[i].x, snk.body[i].y);
		cout << s[i%strlen(s)];
	}
}
void DrawPortal(Portal port, char s2) {
	GotoXY(port.port.x, port.port.y);
	cout << s2;
	for (int i = 0;i < 4;i++) {
		GotoXY(port.wall[i].x, port.wall[i].y);
		cout << s2;
	}
}
void DrawFd(Food fd, char s2) {
	GotoXY(fd.pos[fd.idx].x, fd.pos[fd.idx].y);
	cout << s2;
}

bool isCrashPortal(Snake snk, Portal port, char dir) {
	if (snk.portal == false) return false;
	Point head = snk.body[snk.size - 1];
	int dx, dy; dx = dy = 0;
	switch (dir) {
	case 'A': dx = -1; break;
	case 'D': dx = 1; break;
	case 'S': dy = 1; break;
	case 'W': dy = -1; break;
	}
	head.x += dx;
	head.y += dy;
	if (head.x == port.port.x && head.y == port.port.y && dir != port.direction) 
		return true;
	for (int i = 0;i < 4;i++)
		if (head.x == port.wall[i].x && head.y == port.wall[i].y)
			return true;
	return false;
}

void MoveRight(Snake &snk, Food &fd, Portal &port) {
	int size = snk.size;
	bool isEat = false;
	if ((snk.body[size - 1].x + 1 == WIDTH_CONSOLE || isBiteTail(snk, 'D') || isCrashPortal(snk,port,'D')) && snk.porting==false) {
		ProcessDead(snk);
	}
	else {
		if (snk.body[size - 1].x + 1 == fd.pos[fd.idx].x && snk.body[size - 1].y == fd.pos[fd.idx].y) {
			Eat(snk, fd, port);
			isEat = true;
		}
		if (!isEat) {
			for (int i = 0;i < size - 1;i++) {
				snk.body[i] = snk.body[i + 1];
			}
			snk.body[size - 1].x++;
		}
	}
}

void MoveLeft(Snake &snk, Food &fd, Portal &port) {
	int size = snk.size;
	bool isEat = false;
	if ((snk.body[size - 1].x - 1 == 0 || isBiteTail(snk, 'A') || isCrashPortal(snk, port, 'A')) && snk.porting == false) {
		ProcessDead(snk);
	}
	else {
		if (snk.body[size - 1].x - 1 == fd.pos[fd.idx].x && snk.body[size - 1].y == fd.pos[fd.idx].y) {
			Eat(snk, fd, port);
			isEat = true;
		}
		if (!isEat) {
			for (int i = 0;i < size - 1;i++) {
				snk.body[i] = snk.body[i + 1];
			}
			snk.body[size - 1].x--;
		}
	}
}

void MoveDown(Snake &snk, Food &fd, Portal &port) {
	int size = snk.size;
	bool isEat = false;
	if ((snk.body[size - 1].y + 1 == HEIGH_CONSOLE || isBiteTail(snk, 'S') || isCrashPortal(snk, port, 'S')) && snk.porting == false) {
		ProcessDead(snk);
	}
	else {
		if (snk.body[size - 1].x == fd.pos[fd.idx].x && snk.body[size - 1].y + 1 == fd.pos[fd.idx].y) {
			Eat(snk, fd, port);
			isEat = true;
		}
		if (!isEat) {
			for (int i = 0;i < size - 1;i++) {
				snk.body[i] = snk.body[i + 1];
			}
			snk.body[size - 1].y++;
		}
	}
}

void MoveUp(Snake &snk, Food &fd, Portal &port) {
	int size = snk.size;
	bool isEat = false;
	if ((snk.body[size - 1].y - 1 == 0 || isBiteTail(snk, 'W') || isCrashPortal(snk, port, 'W')) && snk.porting == false) {
		ProcessDead(snk);
	}
	else {
		if (snk.body[size - 1].x == fd.pos[fd.idx].x && snk.body[size - 1].y - 1 == fd.pos[fd.idx].y) {
			Eat(snk, fd, port);
			isEat = true;
		}
		if (!isEat) {
			for (int i = 0;i < size - 1;i++) {
				snk.body[i] = snk.body[i + 1];
			}
			snk.body[size - 1].y--;
		}
	}
}

void ThreadFunc(Snake *snk, Food *fd, Portal *port) {
	int len = strlen(SNAKE_STR);
	char *blank = new char[len+1];
	for (int i = 0;i < len;i++) blank[i] = ' ';
	blank[len] = '\0';
	while (snk->GameOn != 0) {
		if (snk->portal == false) snk->snklen = snk->size;
		if ((*snk).alive == 1) {
			if (snk->portal == false) {
				DrawSnk(*snk, blank);
				DrawFd(*fd, ' ');
				if ((*snk).moving == 'A') MoveLeft(*snk, *fd, *port);
				else if ((*snk).moving == 'D') MoveRight(*snk, *fd, *port);
				else if ((*snk).moving == 'S') MoveDown(*snk, *fd, *port);
				else MoveUp(*snk, *fd, *port);
				if (!snk->portal) {
					//DrawSnkFd(*snk, *fd, SNAKE_STR, (char)250);
					DrawSnk(*snk, SNAKE_STR);
					DrawFd(*fd, (char)250);
				}
				if (snk->alive == 0) Clash(*snk);
				GotoXY(WIDTH_CONSOLE + 10, 5);
				Sleep(250 / (*snk).speed);
			}
			else {
				//DrawSnkPortal(*snk, *port, blank, ' ');
				DrawSnk(*snk, blank);
				DrawPortal(*port, ' ');
				if ((*snk).moving == 'A') MoveLeft(*snk, *fd, *port);
				else if ((*snk).moving == 'D') MoveRight(*snk, *fd, *port);
				else if ((*snk).moving == 'S') MoveDown(*snk, *fd, *port);
				else MoveUp(*snk, *fd, *port);
				if (snk->body[snk->snklen - 1].x == port->port.x && snk->body[snk->snklen - 1].y == port->port.y) {
						snk->snklen--;
						snk->porting = true;
						cout << '\a';
					}
				if (!snk->porting) {
					//DrawSnkPortal(*snk, *port, SNAKE_STR, 'o');
					DrawSnk(*snk, SNAKE_STR);
					DrawPortal(*port, 'o');
				}
				else {
					//DrawSnkPortal(*snk, *port, SNAKE_STR, (char)254);
					DrawSnk(*snk, SNAKE_STR);
					DrawPortal(*port, (char)254);
				}
				GotoXY(WIDTH_CONSOLE + 10, 5);
				Sleep(250 / snk->speed);
				if (snk->snklen == 0) {
					snk->portal = false;
					//GotoXY(port->port.x, port->port.y); cout << ' ';
					DrawPortal(*port, ' ');
					//Init Snake
					snk->moving = 'D';
					snk->charlock = 'A';
					snk->porting = false;
					for (int i = 0;i < snk->size;i++) {
						snk->body[i] = { 10 + i, 5 };
					}
					GenerateFood(*snk, *fd, *port);
					fd->idx = 0;
					if (snk->speed == MAX_SPEED) {
						snk->speed = 1;
						snk->size = 6;
					}
					else snk->speed++;
				}
			}
		}
	}
	delete[] blank;
}

bool isBiteTail(Snake snk, char Direction) {
	Point head = snk.body[snk.size - 1];
	int dx, dy; dx = dy = 0;
	switch (Direction) {
	case 'A': dx = -1; break;
	case 'D': dx = 1; break;
	case 'S': dy = 1; break;
	case 'W': dy = -1; break;
	}
	head.x += dx;
	head.y += dy;
	for (int i = 0;i < snk.size - 1;i++)
		if (head.x == snk.body[i].x && head.y == snk.body[i].y)
			return true;
	return false;
}

void Clash(Snake snk) {
	char str[] = SNAKE_STR;
	int clash = 178;
	cout << '\a';
	//for (int i = 0;i < snk.size - 1;i++) {
	//	GotoXY(snk.body[i].x, snk.body[i].y); 
	//	cout << str[i%strlen(str)];
	//	cout << clash;
	//}
	while (clash >= 176) {
		GotoXY(snk.body[snk.size - 1].x, snk.body[snk.size - 1].y);
		cout << (char)clash--;
	}
}

void DrawSnkPortal(Snake snk, Point port, char* s1, char s2) {
	GotoXY(port.x, port.y); cout << s2;
	for (int i = 0;i < snk.snklen;i++) {
		GotoXY(snk.body[i].x, snk.body[i].y);
		cout << s1[i%strlen(s1)];
	}
}

void SaveFile(Snake snk, Portal port, Food fd, char *filename) {
	FILE *f;
	fopen_s(&f,filename, "wb");
	fwrite(&snk.alive, sizeof(bool), 1, f);
	fwrite(&snk.charlock, sizeof(char), 1, f);
	fwrite(&snk.snklen, sizeof(int), 1, f);
	fwrite(&snk.moving, sizeof(char), 1, f);
	fwrite(&snk.portal, sizeof(bool), 1, f);
	fwrite(&snk.porting, sizeof(bool), 1, f);
	fwrite(&snk.size, sizeof(int), 1, f);
	fwrite(&snk.speed, sizeof(int), 1, f);
	fwrite(&snk.body[0], sizeof(Point), snk.size, f);
	fwrite(&fd, sizeof(Food), 1, f);
	fwrite(&port, sizeof(Portal), 1, f);
	fwrite(&Score, sizeof(int), 1, f);
	fclose(f);
}

void Save(Snake snk, Portal port, Food fd){
	char name[MAX_NAME];
	GotoXY(0, HEIGH_CONSOLE + 5);
	cout << "Enter savefile's name (without spaces): ";
	cin >> name;
	SaveFile(snk, port, fd, name);
}

void OpenFile(Snake &snk, Portal &port, Food &fd, char *filename) {
	FILE *f;
	fopen_s(&f, filename, "rb");
	if (f == NULL) {
		snk.GameOn = 0;
		return;
	}
	fread(&snk.alive, sizeof(bool), 1, f);
	fread(&snk.charlock, sizeof(char), 1, f);
	fread(&snk.snklen, sizeof(int), 1, f);
	fread(&snk.moving, sizeof(char), 1, f);
	fread(&snk.portal, sizeof(bool), 1, f);
	fread(&snk.porting, sizeof(bool), 1, f);
	fread(&snk.size, sizeof(int), 1, f);
	fread(&snk.speed, sizeof(int), 1, f);
	snk.body.resize(snk.size);
	fread(&snk.body[0], sizeof(Point), snk.size, f);
	fread(&fd, sizeof(Food), 1, f);
	fread(&port, sizeof(Portal), 1, f);
	fread(&Score, sizeof(int), 1, f);
	fclose(f);
}

void Open(Snake &snk, Portal &port, Food &fd) {
	char name[MAX_NAME];
	GotoXY(0, HEIGH_CONSOLE + 5);
	cout << "Enter savefile's name (without spaces): ";
	cin >> name;
	OpenFile(snk, port, fd, name);
	GotoXY(0, HEIGH_CONSOLE + 5);
	for(int i=0;i<100;i++) cout << ' ';
}