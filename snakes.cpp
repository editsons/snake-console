#include <windows.h>
#include <bits/stdc++.h>
using namespace std;

typedef pair<int, int> Vec;

Vec grid_size(30, 30);
int grid_sleep_ms = 100;
 
int direction, direction_desired;
int score;
deque<Vec> snake;
Vec food_pos;

int mod(int a, int mod_b) {
	return ((a % mod_b) + mod_b) % mod_b;
}

void showConsoleCursor(bool flag) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO     cursor_info;
    GetConsoleCursorInfo(out, &cursor_info);
    cursor_info.bVisible = flag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursor_info);
}

void setConsoleFondSize(int width, int height) {
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = width; 	// width of each character in the font
	cfi.dwFontSize.Y = height;	// height of each character in the font
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	std::wcscpy(cfi.FaceName, L"Consolas"); // font
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

void setConsoleCursor(Vec p) {
	COORD pos = {p.first, p.second};
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, pos);
}

void draw(Vec p, char c) {
	setConsoleCursor(p);
    cout << c;
}

void inputThreadRoutine() {
	while(1) {
			 if(direction != VK_DOWN  && GetAsyncKeyState(VK_UP)) 	 { direction_desired = VK_UP;    }
		else if(direction != VK_UP 	  && GetAsyncKeyState(VK_DOWN))  { direction_desired = VK_DOWN;  } 
		else if(direction != VK_LEFT  && GetAsyncKeyState(VK_RIGHT)) { direction_desired = VK_RIGHT; } 
		else if(direction != VK_RIGHT && GetAsyncKeyState(VK_LEFT))  { direction_desired = VK_LEFT;  } 
		this_thread::sleep_for(chrono::milliseconds(1)); 
	}
}

Vec generateFoodPos() {
	vector<vector<int>> grid(grid_size.second, vector<int>(grid_size.first));
	for(int i = 0; i < snake.size(); i++) {
		int x = snake[i].first;
		int y = snake[i].second;
		grid[y][x] = 1;
	}
	vector<Vec> possible_poss(grid_size.first * grid_size.second - snake.size());
	int i_poss = 0;
	for(int y = 0; y < grid_size.second; y++) {
		for(int x = 0; x < grid_size.first; x++) {
			if(grid[y][x] == 0) {
				possible_poss[i_poss++] = Vec(x, y);
			}
		}
	}
	int i_random = rand() % possible_poss.size();
	return possible_poss[i_random];
}

void gameOver() {
	setConsoleCursor(Vec(40, 5));
	cout << "  GAME OVER  ";
	setConsoleCursor(Vec(40, 6));
	cout << "  Score: " << score << "  ";
	setConsoleCursor(Vec(40, 7));
	cout << "  Press R to restart the game.  ";
	GetAsyncKeyState(0x52);
	while(!GetAsyncKeyState(0x52)) { // wait for R key
		this_thread::sleep_for(chrono::milliseconds(10));
	}
}

int main() {

	// initialize console
	showConsoleCursor(false);
	setConsoleFondSize(12, 12);

	// run input thread
	thread inputThread(inputThreadRoutine);

	// main loop
	while(true) {

		// clear console
		system("cls");

		// initialize variables
		bool restart = false;
		bool food_eaten = true;
		direction = VK_RIGHT;
		direction_desired = direction;
		score = 0;

		// initialize snake
		snake.clear();
		Vec s_pos_1(Vec(3, 5)); snake.push_back(s_pos_1); draw(s_pos_1, '-');
		Vec s_pos_2(Vec(4, 5)); snake.push_back(s_pos_2); draw(s_pos_2, '-');
		Vec s_pos_3(Vec(5, 5)); snake.push_back(s_pos_3); draw(s_pos_3, '-');
		Vec s_pos_4(Vec(6, 5)); snake.push_back(s_pos_4); draw(s_pos_4, '-');
		Vec s_pos_5(Vec(7, 5)); snake.push_back(s_pos_5); draw(s_pos_5, '>');

		// draw boundaries
		for(int i = 0; i < grid_size.second; i++) {
			draw(Vec(grid_size.first, i), '|');
		}
		for(int i = 0; i < grid_size.first; i++) {
			draw(Vec(i, grid_size.second), '-');
		}

		// game loop
		while(true) {

			// show score
			setConsoleCursor(Vec(40, 6));
			cout << "  Score: " << score << "  ";

			// generate food
			if(food_eaten) {
				food_pos = generateFoodPos();
				draw(food_pos, 'o');
			}
			food_eaten = false;

			// check for death
			for(int i = 0; i < snake.size() - 1; i++) {
				// biting itself
				if(snake.back().first == snake[i].first && snake.back().second == snake[i].second) {
					gameOver();
					restart = true;
				}
			}
			if(restart) break;

			// check for food
			for(int i = 0; i < snake.size(); i++) {
				// eaten food
				if(snake[i].first == food_pos.first && snake[i].second == food_pos.second) {
					food_eaten = true;
					score += 4;
				}
			}
			
			// pause
			this_thread::sleep_for(chrono::milliseconds(grid_sleep_ms));
			
			// update direction
			direction = direction_desired;

			// remove tail
			if(!food_eaten) {
				draw(snake.front(), ' ');
				snake.pop_front();
			}

			// determine new snake head position, head char and body char
			Vec new_head_pos = snake.back();
			char new_head_char = 'X', new_body_char = 'X';
			switch(direction) {
				case VK_UP: 	new_head_pos.second = mod((new_head_pos.second - 1), grid_size.second); new_head_char = '^'; new_body_char = '|'; break;
				case VK_DOWN: 	new_head_pos.second = mod((new_head_pos.second + 1), grid_size.second); new_head_char = 'v'; new_body_char = '|'; break;
				case VK_RIGHT: 	new_head_pos.first  = mod((new_head_pos.first  + 1), grid_size.first);  new_head_char = '>'; new_body_char = '-'; break;
				case VK_LEFT: 	new_head_pos.first  = mod((new_head_pos.first  - 1), grid_size.first); 	new_head_char = '<'; new_body_char = '-'; break;
				default: cout << "Error: Unknown direction."; break;
			}

			// remove old snake head and add new one
			draw(snake.back(), new_body_char);
			snake.push_back(new_head_pos);
			draw(snake.back(), new_head_char);
		}
	}

	
	return 0;
}
