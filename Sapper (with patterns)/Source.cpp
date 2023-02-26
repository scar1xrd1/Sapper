#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>
using namespace std;

// Паттерны: посредник

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

void set_color(int text)
{
	SetConsoleTextAttribute(h, (0 << 4) + text);
}

class Dot
{
	int state = 2; // 0 - мина, 1 - флаг, 2 - закрытая клетка, 3 - открытая клетка, Дальше - цифры
	string _state;
	string states[4] = { "*", "$", "o", "." };
	int w, h;
	int color = 7;
	bool calculated = false;

public:
	Dot() { state = 2; _state = states[state]; }

	Dot(int value) { if (value >= 0 && value <= 3) state = value; }

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

	bool inRange(int value) { return value >= 0 && value <= 3; }

	bool get_calc() { return calculated; }
	void calculate() { calculated = true; }
};

class Field
{
	Dot* field[10][10];
	bool mine[10][10];

	int w, h;

	vector<int> made_empty;

	string str_pos[10][10];
	string str_pos_rus[10][10];
	string word[10] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
	string word_rus[10] = {"ф", "и", "с", "в", "у", "а", "п", "р", "ш", "о"};

public:
	Field()
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				field[j][i] = new Dot();
			}
		}

		generate_mines(10);

		for (int i = 1; i <= 10; i++)
		{
			for (int j = 1; j <= 10; j++)
			{
				str_pos[j - 1][i - 1] = word[i-1] + to_string(j);
				str_pos_rus[j - 1][i - 1] = word_rus[i-1] + to_string(j);
			}
		}
	}

	void show()
	{
		cout << "\ta b c d e f g h i j\n\n";

		for (int i = 0; i < 10; i++)
		{
			cout << i + 1 << "\t";
			for (int j = 0; j < 10; j++)
			{
				if (field[j][i]->get_state() <= 3) cout << field[j][i]->view_state() << " ";

				else
				{
					set_color(field[j][i]->get_color()); cout << to_string(field[j][i]->get_state() - 3); set_color(7); cout << " ";
				}
			}
			cout << endl;
		}
	}

	void generate_mines(int quantity)
	{
		int r1, r2;

		if (quantity <= 90)
		{
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

	void accept_input(string user)
	{
		if (get_index(user))
		{
			calculate_numbers(w, h);
		}
		else
		{
			cout << "Неверная координата!\n\n";
			return;
		}
	}

	bool check_mines(int x, int y)
	{
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
		return false;
	}

	void calculate_numbers(int x, int y) 
	{
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
			return;
		}

		check_mines(x, y);

		// ------------------------------------------------------------------------------------------------------------------------------------------------------------//

		int state = field[x][y]->get_state();

		cout << "this x y is " << x << " " << y << endl;

		cout << "field[x][y]->get_state() = " << field[x][y]->get_state() << endl;

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
};

int main()
{
	system("chcp 1251");
	system("cls");
	string user;

	Field field;

	cout << "Управление: Вы должны ввести координату, например A1 или J7, но не 1A или 7J\n\n";

	while (true)
	{
		field.show();

		cout << "\nВведите координату -> ";
 		cin >> user;
		system("cls");

		transform(user.begin(), user.end(), user.begin(), tolower);

		field.accept_input(user);
	}
}