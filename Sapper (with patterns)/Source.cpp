#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// Паттерны: посредник

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

	string str_pos[10][10];
	string word[10] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};

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
			}
		}
	}

	void show()
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (field[j][i]->get_state() <= 3) cout << field[j][i]->view_state() << " ";
				else cout << to_string(field[j][i]->get_state()-3) << " ";
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

	void calculate_numbers(string index) 
	{
		if (!get_index(index))
		{
			cout << "Неверная координата!\n\n";
			return;
		}

		if (field[w][h]->get_calc())
		{
			cout << "Вы уже вводили данную координату!\n\n";
			return;
		}

		int w1, h1;

		w1 = h1 = 1;

		vector<int> made_empty;

		// field[9][9].set_state(3); // [w][h]    [j][i]

		if (field[w][h]->get_state() == 0)
		{
			cout << "Вы проиграли!\n";
			return;
		}

		if (h + h1 < 10) 
		{ 
			if (field[w][h + h1]->get_state() == 0) field[w][h]->nearby_mine(); 
			if (w + w1 < 10) { if (field[w + w1][h + h1]->get_state() == 0) field[w][h]->nearby_mine(); }
			if (w - w1 > -1) { if (field[w - w1][h + h1]->get_state() == 0) field[w][h]->nearby_mine(); }
		}
		if (h - h1 > -1) 
		{
			if (field[w][h - h1]->get_state() == 0) field[w][h]->nearby_mine();
			if (w + w1 < 10) { if (field[w + w1][h - h1]->get_state() == 0) field[w][h]->nearby_mine(); }
			if (w - w1 > -1) { if (field[w - w1][h - h1]->get_state() == 0) field[w][h]->nearby_mine(); }
		}

		if (w + w1 < 10) { if (field[w + w1][h]->get_state() == 0) field[w][h]->nearby_mine(); }
		if (w - w1 > -1) { if (field[w - w1][h]->get_state() == 0) field[w][h]->nearby_mine(); } 

		if (field[w][h]->get_state() == 2)
		{
			field[w][h]->set_state(3);

			if (h + h1 < 10)
			{
				if (field[w][h + h1]->get_state() == 2) field[w][h + h1]->set_state(3);
				if (w + w1 < 10) { if (field[w + w1][h + h1]->get_state() == 2) field[w + w1][h + h1]->set_state(3); }
				if (w - w1 > -1) { if (field[w - w1][h + h1]->get_state() == 2) field[w - w1][h + h1]->set_state(3); }
			}
			if (h - h1 > -1)
			{
				if(field[w][h - h1]->get_state() == 2) field[w][h - h1]->set_state(3);
				if (w + w1 < 10) { if (field[w + w1][h - h1]->get_state() == 2) field[w + w1][h - h1]->set_state(3); }
				if (w - w1 > -1) { if (field[w - w1][h - h1]->get_state() == 2) field[w - w1][h - h1]->set_state(3); }
			}

			if (w + w1 < 10) { if (field[w + w1][h]->get_state() == 2) field[w + w1][h]->set_state(3); }
			if (w - w1 > -1) { if (field[w - w1][h]->get_state() == 2) field[w - w1][h]->set_state(3); }
		}

		field[w][h]->calculate();
	}

	bool get_index(string index)
	{
		for (int i = 1; i <= 10; i++)
		{
			for (int j = 1; j <= 10; j++)
			{
				if (index == str_pos[j-1][i-1])
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
	setlocale(LC_ALL, "ru");
	string user;

	Field field;

	while (true)
	{
		field.show();

		cout << "Введите координату -> ";
		cin >> user;
		system("cls");

		transform(user.begin(), user.end(), user.begin(), tolower);
		
		field.calculate_numbers(user);
	}
}