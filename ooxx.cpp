#include <vector>
#include <iostream>
#include <string>

namespace scpo_ooxx
{
	const short
		SIZE = 4;
	const std::string
		CHESS[4]{ "○", "  ", "×", "无人" };
	short
		i, j, k,
		board[SIZE][SIZE]{ 0 },
		count[SIZE + SIZE + 2]{ 0 },
		cursor[2]{ 0, 0 },
		per_now = 1,
		step = 0;
	std::vector<short*>
		count_board[SIZE][SIZE];
	typedef const struct
	{
		bool type;
		short power;
	} move_way;
	move_way
		M_UP{ false, 0 },
		M_DOWN{ true, 0 },
		M_LEFT{ false, 1 },
		M_RIGHT{ true, 1 };
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
namespace operation
{
	const HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	const HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	INPUT_RECORD what_key;
	DWORD save_key;
	char in_scan()
	{
		ReadConsoleInput(handle_in, &what_key, 1, &save_key);
		if (what_key.EventType == KEY_EVENT && what_key.Event.KeyEvent.bKeyDown == TRUE)
		{
			switch (what_key.Event.KeyEvent.wVirtualKeyCode)
			{
			case 87:
				return 'w';
			case 65:
				return 'a';
			case 83:
				return 's';
			case 68:
				return 'd';
			case VK_RETURN:
				return '\n';
			}
		}
		return '\0';
	}
	void out_hide()
	{
		CONSOLE_CURSOR_INFO cursor;
		cursor.bVisible = false;
		cursor.dwSize = sizeof(cursor);
		SetConsoleCursorInfo(handle_out, &cursor);
	}
	void out_pos(short x, short y)
	{
		SetConsoleCursorPosition(handle_out, { x, y });
	}
	void out_color(short type)
	{
		switch (type)
		{
		case 0:
			type = 0xb0;
			break;
		case 1:
			type = 0x07;
			break;
		}
		SetConsoleTextAttribute(handle_out, type);
	}
	void init()
	{
		system("cls");
		out_hide();
	}
	void end()
	{
		out_pos(8, scpo_ooxx::SIZE * 2 + 4);
		system("pause");
		CloseHandle(handle_in);
		CloseHandle(handle_out);
	}
}
#else
#include <termios.h>
namespace operation
{
	void out_hide()
	{
		std::cout << "\033[?25l";
	}
	void out_pos(short x, short y)
	{
		std::cout << "\033[" << y + 1 << ';' << x + 1 << 'H';
	}
	void out_color(short type)
	{
		switch (type)
		{
		case 0:
			std::cout << "\033[43m\033[30m";
			break;
		case 1:
			std::cout << "\033[40m\033[37m";
			break;
		}
	}
	char in_scan()
	{
		out_pos(0, scpo_ooxx::SIZE * 2 + 4);
		struct termios new_settings, stored_settings;
		tcgetattr(0, &stored_settings);
		new_settings = stored_settings;
		new_settings.c_lflag &= (~ICANON);
		new_settings.c_cc[VTIME] = 0;
		tcgetattr(0, &stored_settings);
		new_settings.c_cc[VMIN] = 1;
		tcsetattr(0, TCSANOW, &new_settings);
		int in = getchar();
		tcsetattr(0, TCSANOW, &stored_settings);
		std::cout << "\r\033[K";
		return in;
	}
	void init()
	{
		system("clear");
		out_hide();
	}
	void end()
	{
		out_pos(0, scpo_ooxx::SIZE * 2 + 4);
	}
}
#endif

namespace scpo_ooxx
{
	void move(move_way way)
	{
		cursor[way.power] = (cursor[way.power] + SIZE + way.type * 2 - 1) % SIZE;
	}
	void out_board()
	{
		using namespace std;
		operation::out_pos(0, 0);
		for (i = 0; i < SIZE; i++)
		{
			cout << "╬";
			for (j = 0; j < SIZE; j++) cout << "═══╬";
			cout << " \n║ ";
			for (j = 0; j < SIZE; j++)
			{
				if (i == cursor[0] && j == cursor[1])
				{
					operation::out_color(0);
					cout << CHESS[1 + board[i][j]];
					operation::out_color(1);
				}
				else cout << CHESS[1 + board[i][j]];
				cout << "║ ";
			}
			cout << "\n";
		}
		cout << "╬";
		for (j = 0; j < SIZE; j++) cout << "═══╬";
		cout << " ";
	}
	void out_tip(std::string tip)
	{
		operation::out_pos(8, SIZE * 2 + 2);
		std::cout << tip;
	}
	void out_cn()
	{
		out_tip("当前是" + CHESS[1 + per_now] + "的回合");
	}
	short put()
	{
		short& bn = board[cursor[0]][cursor[1]];
		if (bn != 0) return 0;
		bn = per_now, step++;
		std::vector<short*>& count_now = count_board[cursor[0]][cursor[1]];
		int len = count_now.size();
		for (i = 0; i < len; i++)
		{
			*count_now[i] += per_now;
			if (*count_now[i] * per_now == SIZE) return per_now;
		}
		if (step == SIZE * SIZE) return 2;
		return (per_now = -per_now, out_cn(), 0);
	}
	void init()
	{
		for (i = 0; i < SIZE; i++) for (j = 0; j < SIZE; j++)
		{
			count_board[i][j].push_back(&count[i]);
			count_board[i][j].push_back(&count[SIZE + j]);
		}
		for (i = 0; i < SIZE; i++)
		{
			count_board[i][i].push_back(&count[SIZE * 2]);
			count_board[i][2 - i].push_back(&count[SIZE * 2 + 1]);
		}
		out_board();
		out_cn();
	}
}
int main()
{
	using namespace scpo_ooxx;
	bool re;
	short win;
	operation::init();
	init();
	while (true)
	{
		re = true, win = 0;
		switch (operation::in_scan())
		{
		case 'w':
			move(M_UP);
			break;
		case 'a':
			move(M_LEFT);
			break;
		case 's':
			move(M_DOWN);
			break;
		case 'd':
			move(M_RIGHT);
			break;
		case '\n':
			win = put();
			break;
		default:
			re = false;
			break;
		}
		if (re) out_board();
		if (win != 0)
		{
			out_tip("游戏结束。" + CHESS[1 + win] + "胜利");
			break;
		}
	}
	operation::end();
	return 0;
}
