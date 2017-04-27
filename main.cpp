#include <conio.h>
#include "snake.h"

int HEIGH_CONSOLE, WIDTH_CONSOLE;
int Score = 0;

int main() {
	char t;
	Snake snk;
	Food fd;
	Portal port;
	FixConsoleWindow();
	StartGame(snk, fd, port);
	GotoXY(8, 5); cout << "Press T to load save file, press any key to start new game";
	GotoXY(21, 7); cout << "Use W, A, S, D to control the snake";
	t = toupper(_getch());
	GotoXY(8, 5); 
	for (int i = 0;i < 59;i++) cout << ' ';
	GotoXY(21, 7);
	for (int i = 0;i < 36;i++) cout << ' ';
	if (t == 'T') 
		Open(snk, port, fd);
	if (snk.GameOn == 0) {
		GotoXY(8, 5);
		cout << "Invalid file name!! Press Enter to exit.";
		while (_getch() != 13)
			;
		return 0;
	}
	GotoXY(WIDTH_CONSOLE + 9, 5); cout << Score;
	thread t1(ThreadFunc, &snk, &fd, &port);
	HANDLE handle_t1 = t1.native_handle();
	while (true) {
		char tmp;
		tmp = toupper(_getch());
		if (snk.alive) {
			if (tmp == 'P') {
				PauseGame(handle_t1);
				snk.pause = true;
			}
			else if (tmp == 'L') {				
				snk.GameOn = 0;
				t1.join();
				Save(snk, port, fd);
				system("cls");
				return 0;
			}
			else if (tmp == 27) {
				snk.GameOn = 0;
				system("cls");
				t1.join();
				return 0;
			}
			else {
				ResumeThread(handle_t1);
				if (snk.pause) {
					snk.pause = false;
					GotoXY(0, HEIGH_CONSOLE + 5);
					for (int i = 0;i<100;i++) cout << ' ';
				}
				if ((tmp != snk.charlock && tmp != snk.moving && !snk.porting) && (tmp == 'A' || tmp == 'S' || tmp == 'D' || tmp == 'W')) {
					if (tmp == 'D') snk.charlock = 'A';
					else if (tmp == 'A') snk.charlock = 'D';
					else if (tmp == 'S') snk.charlock = 'W';
					else snk.charlock = 'S';
					snk.moving = tmp;
				}
			}
		}
		else {
			if (tmp == 'Y') StartGame(snk, fd, port);
			else {
				snk.GameOn = 0;
				system("cls");
				t1.join();
				return 0;
			}
		}
	}
	return 0;
}