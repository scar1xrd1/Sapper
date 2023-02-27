#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <thread>
#include <chrono>
using namespace std;

// Паттерны: посредник

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
float _time = 0;

void set_color(int text)
{
	SetConsoleTextAttribute(h, (0 << 4) + text);
}

void set_color(int text, int background)
{
	SetConsoleTextAttribute(h, (background << 4) + text);
}

void timer()
{
	while (true)
	{
		this_thread::sleep_for(chrono::milliseconds(100));
		_time+=0.1;
	}	
}

class Dot
{
	int state = 2; // 0 - мина, 1 - флаг, 2 - закрытая клетка, 3 - открытая клетка, Дальше - цифры
	string _state;
	string states[4] = { "*", "$", "o", "." };
	int w, h;
	int color = 7;
	bool calculated, show, flag;

public:
	Dot()
	{ 
		state = 2; 
		_state = states[state];
		calculated = show = flag = false;
	}

	Dot(int value)
	{
		if (value >= 0 && value <= 3) state = value; 
		calculated = show = flag = false;
	}

	void set_state(int value)
	{
		if(inRange(value))
		{
			state = value;
			_state = states[value];
		}
	}

	void nearby_mine()
	{
		if (inRange(state)) state = 4;
		else state++;
	}

	int get_state() { return state; }
	string view_state() { return _state; }

	void change_show() { show = !show; }
	void change_flag() { flag = !flag; }

	int get_color()
	{
		if (state <= 3) return 7;

		int c = state - 3;

		if (c == 1) return 9;
		if (c == 2) return 2;
		if (c == 3) return 4;
		if (c == 4) return 1;
		if (c == 5) return 13;
		if (c == 6) return 3;
		if (c == 7) return 8;
		if (c == 8) return 15;
	}

	bool true_flag()
	{
		if (state == 0 && flag) return true;
		return false;
	}

	bool inRange(int value) { return value >= 0 && value <= 3; }

	bool get_calc() { return calculated; }
	bool get_show() { return show; }
	bool get_flag() { return flag; }
	void calculate() { calculated = true; }
};

class Field
{
	Dot* field[10][10];
	bool mine[10][10];

	int w, h, mines, core_mines, flags;

	vector<int> made_empty;

	bool last, flag, stop;

	string str_pos[10][10];
	string str_pos_rus[10][10];
	string word[10] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
	string word_rus[10] = {"ф", "и", "с", "в", "у", "а", "п", "р", "ш", "о"};

public:
	Field(int bombs)
	{
		last = flag = stop = false;
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				field[j][i] = new Dot();
			}
		}

		generate_mines(bombs);

		for (int i = 1; i <= 10; i++)
		{
			for (int j = 1; j <= 10; j++)
			{
				str_pos[j - 1][i - 1] = word[i-1] + to_string(j);
				str_pos_rus[j - 1][i - 1] = word_rus[i-1] + to_string(j);
			}
		}
	}

	~Field()
	{
		cout << "destructor field\n";

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				delete field[j][i];
			}
		}
	}

	bool stop_game() { return stop; }

	void show()
	{
		cout << "Осталось мин: " << mines - flags << endl << endl;
		cout << "\ta b c d e f g h i j\n\n";

		for (int i = 0; i < 10; i++)
		{
			cout << i + 1 << "\t";
			for (int j = 0; j < 10; j++)
			{
				if (field[j][i]->get_state() == 0)
				{
					if (field[j][i]->get_flag()) { set_color(112, 112); cout << "&"; set_color(7, 0); cout << " "; }
					else cout << "o ";
				}
				else if (field[j][i]->get_flag()) { set_color(112, 112); cout << "&"; set_color(7, 0); cout << " "; }
				else if (field[j][i]->get_state() <= 3) { cout << field[j][i]->view_state() << " "; }
				else if (field[j][i]->get_state() > 3) { set_color(field[j][i]->get_color()); cout << to_string(field[j][i]->get_state() - 3); set_color(7); cout << " "; }
			}
			cout << endl;
		}
	}

	void generate_mines(int quantity)
	{
		int r1, r2;

		if (quantity <= 50)
		{
			mines = quantity;
			core_mines = quantity;
			srand(time(NULL));
			for (int i = 0; i < quantity; i++)
			{
				while (true)
				{
					r1 = 0 + rand() % 9;
					r2 = 0 + rand() % 9;

					if (field[r1][r2]->get_state() != 0) { field[r1][r2]->set_state(0); break; }
				}				
			}
		}
	}

	void check_win()
	{
		int opens = 0;
		int true_flags = 0;

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (field[j][i]->get_state() >= 3) opens++;
				if (field[j][i]->true_flag()) true_flags++;
			}
		}

		if (opens + core_mines == 100)
		{
			cout << "Вы выиграли!\n\n";
			stop = true;
			return;
		}

		if (true_flags == core_mines)
		{
			cout << "Вы выиграли!\n\n";
			stop = true;
			return;
		}
	}

	void accept_input(string user)
	{	
		if (get_index(user))
		{
			if (flag)
			{
				put_flag(w, h);
			}
			else
			{
				if (field[w][h]->get_state() == 2)
				{
					Beep(100, 200);
					calculate_numbers(w, h);
				}
				else if (field[w][h]->get_state() == 0)
				{
					cout << "Вы проиграли!\n\n";
					stop = true;
					return;
				}
			}				
		}
		else
		{
			cout << "Неверная координата!\n\n";
			return;
		}

		check_win();
	}

	void put_flag(int x, int y)
	{
		if (mines - 1 - flags < 0)
		{
			cout << "Упс! Похоже, под каким-то флагом нет мины '-'\n\n";
			return;
		}

		if (field[x][y]->get_state() <= 2)
		{
			if (field[x][y]->get_flag()) { field[x][y]->change_flag(); flags--; cout << "Флаг убран!\n\n"; }
			else { field[x][y]->change_flag(); flags++; cout << "Флаг поставлен!\n\n"; }
		}

		else if (field[x][y]->get_flag()) { field[x][y]->change_flag(); flags--; cout << "Флаг убран!\n\n"; }
	}

	bool check_mines(int x, int y)
	{
		if (field[x][y]->get_state() > 2 && field[x][y]->get_flag()) { field[x][y]->change_flag(); flags--; }

		if (field[x][y]->get_state() <= 3)
		{
			if (y + 1 < 10)
			{
				if (field[x][y + 1]->get_state() == 0) field[x][y]->nearby_mine();
				if (x + 1 < 10) { if (field[x + 1][y + 1]->get_state() == 0) field[x][y]->nearby_mine(); }
				if (x - 1 > -1) { if (field[x - 1][y + 1]->get_state() == 0) field[x][y]->nearby_mine(); }
			}
			if (y - 1 > -1)
			{
				if (field[x][y - 1]->get_state() == 0) field[x][y]->nearby_mine();
				if (x + 1 < 10) { if (field[x + 1][y - 1]->get_state() == 0) field[x][y]->nearby_mine(); }
				if (x - 1 > -1) { if (field[x - 1][y - 1]->get_state() == 0) field[x][y]->nearby_mine(); }
			}

			if (x + 1 < 10) { if (field[x + 1][y]->get_state() == 0) field[x][y]->nearby_mine(); }
			if (x - 1 > -1) { if (field[x - 1][y]->get_state() == 0) field[x][y]->nearby_mine(); }
		}		

		if (field[x][y]->get_state() > 3) return true;
		else
		{
			field[x][y]->set_state(3);
			return false;
		}
	}
	
	bool chck_mns(int x, int y)
	{
		if (field[x][y]->get_state() > 2 && field[x][y]->get_flag()) { field[x][y]->change_flag(); flags--; }

		int mine = 0;

		if (field[x][y]->get_state() <= 3)
		{
			if (y + 1 < 10)
			{
				if (field[x][y + 1]->get_state() == 0) ++mine;
				if (x + 1 < 10) { if (field[x + 1][y + 1]->get_state() == 0) ++mine; }
				if (x - 1 > -1) { if (field[x - 1][y + 1]->get_state() == 0) ++mine; }
			}
			if (y - 1 > -1)
			{
				if (field[x][y - 1]->get_state() == 0) ++mine;
				if (x + 1 < 10) { if (field[x + 1][y - 1]->get_state() == 0) ++mine; }
				if (x - 1 > -1) { if (field[x - 1][y - 1]->get_state() == 0) ++mine; }
			}

			if (x + 1 < 10) { if (field[x + 1][y]->get_state() == 0) ++mine; }
			if (x - 1 > -1) { if (field[x - 1][y]->get_state() == 0) ++mine; }
		}		

		if (mine > 0) return true;
		else return false;
	}

	void calc_nums(int x, int y)
	{
		if (field[x][y]->get_state() > 2 && field[x][y]->get_flag()) { field[x][y]->change_flag(); flags--; }

		if (y < 0) return;
		if (y > 9) return;

		if (field[x][h]->get_calc())
		{
			return;
		}

		if (field[x][y]->get_state() == 0)
		{
			return;
		}

		if (field[x][y]->get_state() > 3) return;

		if (!chck_mns(x, y)) last = false;
		else return;

		if (check_mines(x, y))
		{
			last = true;
			return;
		}

		int state = field[x][y]->get_state();

		//cout << "this x y is " << x << " " << y << endl;

		//cout << "field[x][y]->get_state() = " << field[x][y]->get_state() << endl;

		if (state == 3 || state == 2)
		{
			bool up, down, left, right;
			up = down = left = right = true;

			for (int i = 0; i < 10; i++)
			{
				if (up && y - i > -1 && field[x][y - i]->get_state() != 0)
				{
					if (check_mines(x, y - i)) up = false;
				}
				if (down && y + i < 10 && field[x][y + i]->get_state() != 0)
				{
					if (check_mines(x, y + i)) down = false;
				}

				if (left && x - i > -1 && field[x - i][y]->get_state() != 0)
				{
					if (check_mines(x - i, y)) left = false;
				}
				if (right && x + i < 10 && field[x + i][y]->get_state() != 0)
				{
					if (check_mines(x + i, y)) right = false;
				}
			}	
		}
	}

	void calculate_numbers(int x, int y) 
	{
		if (field[x][y]->get_flag()) return;

		if (field[x][y]->get_state() > 2 && field[x][y]->get_flag()) { field[x][y]->change_flag(); flags--; }

		if (y < 0) return;
		if (y > 9) return;

		if (field[x][h]->get_calc())
		{
			//cout << "Вы уже вводили данную координату!\n\n";
			return;
		}		

		// field[9][9].set_state(3); // [w][h]    [j][i]

		if (field[x][y]->get_state() == 0)
		{
			cout << "Вы проиграли!\n";
			stop = true;
			return;
		}

		check_mines(x, y);

		// ------------------------------------------------------------------------------------------------------------------------------------------------------------//

		int state = field[x][y]->get_state();

		//cout << "this x y is " << x << " " << y << endl;

		//cout << "field[x][y]->get_state() = " << field[x][y]->get_state() << endl;

		if (state == 3 || state == 2)
		{
			bool up, down, left, right;
			up = down = left = right = true;

			for (int i = 0; i < 10; i++)
			{
				if (up && y - i > -1 && field[x][y - i]->get_state() != 0)
				{
					if (check_mines(x, y - i)) up = false;
				}
				if (down && y + i < 10 && field[x][y + i]->get_state() != 0) 
				{
					if(check_mines(x, y + i)) down = false;
				}
				
				if (left && x - i > -1 && field[x - i][y]->get_state() != 0)
				{
					if (check_mines(x - i, y)) left = false;
				}
				if (right && x + i < 10 && field[x + i][y]->get_state() != 0)
				{
					if (check_mines(x + i, y)) right = false;
				}		

				if (y - i > -1)
				{
					calc_nums(x, y - i);
				}

				if (y + i < 10)
				{
					calc_nums(x, y + i);
				}

				if(x - i > -1)
				{
					calc_nums(x - i, y);
				}

				if (x + i < 10)
				{
					calc_nums(x + i, y);
				}
			}
			////cout << "y + 1 = " << y + 1 << " y - 1 = " << y - 1;	
			//if (y - 1 > -1) { check_mines(x, y - 1); /*calculate_numbers(x, y - 1);*/ }
			//if (y + 1 < 10) { /*calculate_numbers(x, y + 1);*/ }					
		}

		// ДОДЕЛАТЬ

		//if (mode == 1 && field[x][y]->get_state() == 2 || field[x][y]->get_state() == 3)
		//{
		//	if (h + h1 < 10)
		//	{
		//		if (field[x][h + h1]->get_state() == 0) field[x][h]->nearby_mine();
		//		if (x + w1 < 10) { if (field[x + w1][y + h1]->get_state() == 0) field[x][y]->nearby_mine(); }
		//		if (x - w1 > -1) { if (field[x - w1][y + h1]->get_state() == 0) field[x][y]->nearby_mine(); }
		//	}
		//	if (h - h1 > -1)
		//	{
		//		if (field[w][h - h1]->get_state() == 0) field[w][h]->nearby_mine();
		//		if (x + w1 < 10) { if (field[x + w1][y - h1]->get_state() == 0) field[x][y]->nearby_mine(); }
		//		if (x - w1 > -1) { if (field[x - w1][y - h1]->get_state() == 0) field[x][y]->nearby_mine(); }
		//	}

		//	if (x + w1 < 10) { if (field[x + w1][y]->get_state() == 0) field[x][y]->nearby_mine(); }
		//	if (x - w1 > -1) { if (field[x - w1][y]->get_state() == 0) field[x][y]->nearby_mine(); }

		//	// if (field[x][y]->get_state() > 3) return;

		//	field[x][y]->set_state(3);

		//	if (y + h1 < 10)
		//	{
		//		if (field[x][y + h1]->get_state() == 2 || field[x][y + h1]->get_state() == 3) calculate_numbers(x, y + h1, 1);
		//		if (x + w1 < 10) { if (field[x + w1][y + h1]->get_state() == 2 || field[x + w1][y + h1]->get_state() == 3) calculate_numbers(x + w1, y + h1, 1); }
		//		if (x - w1 > -1) { if (field[x - w1][y + h1]->get_state() == 2 || field[x - w1][y + h1]->get_state() == 3) calculate_numbers(x - w1, y + h1, 1); }
		//	}
		//	if (h - h1 > -1)
		//	{
		//		if (field[x][h - h1]->get_state() == 2 || field[x][h - h1]->get_state() == 3) calculate_numbers(x, y - h1, 1);
		//		if (w + w1 < 10) { if (field[x + w1][y - h1]->get_state() == 2 || field[x + w1][y - h1]->get_state() == 3) calculate_numbers(x + w1, y - h1, 1); }
		//		if (w - w1 > -1) { if (field[x - w1][y - h1]->get_state() == 2 || field[x - w1][y - h1]->get_state() == 3) calculate_numbers(x - w1, y - h1, 1); }
		//	}

		//	if (x + w1 < 10) { if (field[x + w1][y]->get_state() == 2 || field[x + w1][y]->get_state() == 3) calculate_numbers(x + w1, y, 1); }
		//	if (x - w1 > -1) { if (field[x - w1][y]->get_state() == 2 || field[x - w1][y]->get_state() == 3) calculate_numbers(x - w1, y, 1); }
		//}
		//else if(field[w][h]->get_state() == 2 || field[w][h]->get_state() == 3)
		//{
		//	field[w][h]->set_state(3);

		//	if (h + h1 < 10)
		//	{
		//		if (field[w][h + h1]->get_state() == 2 || field[w][h + h1]->get_state() == 3) calculate_numbers(w, h + h1, 1);
		//		if (w + w1 < 10) { if (field[w + w1][h + h1]->get_state() == 2 || field[w + w1][h + h1]->get_state() == 3) calculate_numbers(w + w1, h + h1, 1); }
		//		if (w - w1 > -1) { if (field[w - w1][h + h1]->get_state() == 2 || field[w - w1][h + h1]->get_state() == 3) calculate_numbers(w - w1, h + h1, 1); }
		//	}
		//	if (h - h1 > -1)
		//	{
		//		if(field[w][h - h1]->get_state() == 2 || field[w][h - h1]->get_state() == 3) calculate_numbers(w, h - h1, 1);
		//		if (w + w1 < 10) { if (field[w + w1][h - h1]->get_state() == 2 || field[w + w1][h - h1]->get_state() == 3) calculate_numbers(w + w1, h - h1, 1); }
		//		if (w - w1 > -1) { if (field[w - w1][h - h1]->get_state() == 2 || field[w - w1][h - h1]->get_state() == 3) calculate_numbers(w - w1, h - h1, 1); }
		//	}

		//	if (w + w1 < 10) { if (field[w + w1][h]->get_state() == 2 || field[w + w1][h]->get_state() == 3) calculate_numbers(w + w1, h, 1); }
		//	if (w - w1 > -1) { if (field[w - w1][h]->get_state() == 2 || field[w - w1][h]->get_state() == 3) calculate_numbers(w - w1, h, 1); }
		//}

		made_empty.clear();
		field[w][h]->calculate();
	}

	bool get_index(string index)
	{
		for (int i = 1; i <= 10; i++)
		{
			for (int j = 1; j <= 10; j++)
			{
				if (index == str_pos[j-1][i-1] || index == str_pos_rus[j-1][i-1])
				{
					w = i - 1;
					h = j - 1;
					return true;
				}
			}
		}
		return false;
	}

	string get_mode()
	{
		if (flag) return "флаг";
		else return "лопата";
	}
	void change_mode()
	{
		flag = !flag;
	}
};

class Mediator
{
public:
	Mediator() {}

	void start()
	{
		string user;
		int u = 0;

		while (true)
		{
			cout << "Введите количество мин (до 50) -> ";
			while (!(cin >> u) || (cin.peek() != '\n')) { cin.clear(); while (cin.get() != '\n'); cout << "\nТолько цифрами!\n\n--> "; }
			if (u < 1 || u > 50) cout << "От 1 до 50!\n\n";
			else break;
		}
		system("cls");

		thread th(timer);
		Field field(u);

		cout << "Управление: Вы должны ввести координату, например A1 или J7, но не 1A или 7J\nНемного о интерфейсе\no - Закрытая клетка\n. - Открытая клетка\n* - Мина\n& - Флаг\n\n";

		while (true)
		{
			field.show();

			cout << "\nВведите координату (режим: " << field.get_mode() << ", чтобы сменить режим введите F) -> ";
			cin >> user;
			system("cls");

			transform(user.begin(), user.end(), user.begin(), tolower);

			if (user == "f") field.change_mode();
			else field.accept_input(user);

			if (field.stop_game()) break;
		}

		cout << "\n\nВремя игры: " << _time << " сек" << endl;
		th.detach();
	}
};

int main()
{
	system("chcp 1251");
	system("cls");
	
	Mediator mediator;
	mediator.start();
}