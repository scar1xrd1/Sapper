#include <iostream>
using namespace std;

// Паттерны: посредник

class Dot
{
	int state; // 0 - мина, 1 - флаг, 2 - закрытая клетка, 3 - открытая клетка, Дальше - цифры
	string _state;
	string states[4] = { "*", "$", "o", "." };

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

	bool inRange(int value) { return value >= 0 && value <= 3; }
};

class Field
{
	Dot field[10][10];
	bool mine[10][10];

public:
	Field()
	{
		
	}

	void show()
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (field[j][i].get_state() <= 3) cout << field[j][i].view_state() << " ";
				else cout << to_string(field[j][i].get_state()-3) << " ";
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

					if (field[r1][r2].get_state() != 0) { field[r1][r2].set_state(0); break; }
				}				
			}
		}
	}

	void calculate_numbers()
	{
		int w, h, w1, h1;
		w = h = 0;
		w1 = h1 = 1;

		// field[9][9].set_state(3); // [w][h] 

		for (int i = 0; i < 10; i++)
		{
			if (field[w][h].get_state() == 2)
			{
				for (int j = 0; j < 10; j++)
				{
					if (w + w1 != 9)
					{
						if (field[w + w1][h].get_state() == 0)
						{
							field[w + w1][h].nearby_mine();
						}
					}
				}				
			}
		}
	}
};

int main()
{
	setlocale(LC_ALL, "ru");

	Field field;

	field.generate_mines(10);
	field.calculate_numbers();
	field.show();
}