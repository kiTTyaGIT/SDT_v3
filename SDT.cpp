#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <fstream>
#include <Windows.h>

using namespace std;

map<string, vector<string>> endings
{

	//+сь
	{"у", {}},

	{"ть", {}},
	{"ться", {}},
	{"тся", {}},

	//+сь
	{"ю", {}},
	{"ую", {}},
	{"ою", {}},
	{"ею", {}},
	{"аю", {}},

	//+ся
	{"ем", {}},
	{"ём", {}},
	{"им", {}},

	//+ся
	{"ешь", {}},
	{"ёшь", {}},
	{"ишь", {}},

	//сь
	{"ете", {}},
	{"ёте", {}},
	{"ите", {}},

	//+ся
	{"ет", {}},
	{"ёт", {}},
	{"ит", {}},

	//+ся
	{"ат", {}},
	{"ят", {}},
	{"ут", {}},
	{"ют", {}},

	//+ся
	{"еть", {}},
	{"ить", {}},
	{"ать", {}},
	{"ять", {}},
	{"оть", {}},
	{"уть", {}},

	//+ся
	{"чь", {}},

	//+сь
	{"ти", {}},
	{"те", {}},
	{"и", {}},

	{"ой", {}},
	{"ай", {}},
	{"ей", {}},
	{"ни", {}},
	{"вь", {}},
	{"сь", {}},

	///////////////
	//+сь ся
	{"л", {}},
	{"ла", {}},
	{"ло", {}},
	{"ли", {}},
};

//функция избавления от знаков препинания в строке
vector<string> parse_line(string line)
{
	string word = "";
	vector<string> clean_words;

	for (int i = 0; i < line.size(); i++)
	{
		if (((line[i] >= 'А') and (line[i] <= 'Я')) or ((line[i] >= 'а') and (line[i] <= 'я')))
		{
			word += line[i];
		}
		else
		{
			if (word != "")
			{ 
				clean_words.push_back(word);
			}
			word = "";
		}
	}
	return clean_words;

}

//функция поиска и добавления глаголов в map (словарь)
void parse_word(vector<string> words)
{

	for (int i = 0; i < words.size(); i++)
	{
		string current_word = words[i];
		//Перебираем ключи словаря
		for (auto iterator = endings.begin(); iterator != endings.end(); iterator++)
		{
			string current_key = iterator->first;
			if (current_word.size() > current_key.size())
			{
				if (current_word.substr(current_word.size() - current_key.size()) == current_key)
				{
					endings[current_key].push_back(current_word);
				}
			}
		}
	}
}


//TODO: каждый вектор должен быть проверен на список исключений и 
//ПРОВЕРКА НА ПОВТОРЕНИЯ
//ДОБАВИТЬ СЛОВА С -ся И ПОДУМАТЬ НАД -ть
int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "");

	string line;

	ifstream in("test.txt");
	if (in.is_open())
	{
		while (getline(in, line))
		{
			vector<string> output = parse_line(line);
			parse_word(output);
		}
	}
	in.close();

	for (auto iterator = endings.begin(); iterator != endings.end(); iterator++)
	{
		cout << "KEY: " << iterator->first << endl;
		vector<string> current_vector = iterator->second;
		for (int i = 0; i < current_vector.size(); i++)
		{
			cout << current_vector.at(i) << ", ";
		}
		cout << endl;
	}
}