#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <fstream>
#include <Windows.h>
using namespace std;


map<string, vector<string>> endings;

void initialize_map()
{
	
	ifstream in("endings.txt");
	string line;
	if (in.is_open())
	{
		while (getline(in, line)) 
		{
			endings.insert(pair<string, vector<string>>(line, {}));
		}
	}
	in.close();
}


//функция избавления от знаков препинания в строке
vector<string> parse_line(string line)
{
	string word = "";
	vector<string> clean_words;

	for (int i = 0; i < line.size(); i++)
	{
		if (((line[i] >= -64) && (line[i] <= -33)) || ((line[i] >= -32) && (line[i] <= -1)))
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

	/*for (int i = 0; i < clean_words.size(); i++)
	{
		cout << clean_words[i] << endl;
	}*/
	return clean_words;

}

string to_lower_case(string str)
{
	for (int i = 0; i < str.length(); i++)
	{
		str[i] = tolower(str[i]);
	}
	return str;
}

//функция поиска и добавления глаголов в map (словарь)
void parse_word(vector<string> words)
{

	for (int i = 0; i < words.size(); i++)
	{
		string current_word = to_lower_case(words[i]);
		
		//Перебираем ключи словаря
		for (auto iterator = endings.begin(); iterator != endings.end(); iterator++)
		{
			string current_key = iterator->first;
			//cout << "CURRENT " << current_key << endl;
			if ((current_word.size() > 1) && (current_word.size() >= current_key.size()))
			{
				if (current_word.substr(current_word.size() - current_key.size()) == current_key)
				{
					//Проверка на дубликаты
					if (find(endings[current_key].begin(), endings[current_key].end(), current_word) == endings[current_key].end())
					{
						endings[current_key].push_back(current_word);
					}
					
				}
			}
		}
	}
}



//TODO: каждый вектор должен быть проверен на список исключений 
//мужской род прошедшее время - принес, прилег, отвез, сжег и т.д. - придумать как учитываться
//добавить что глаголы не = 1 букве 
int main()
{

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "");

	initialize_map();
	string line;

	ifstream in("test.txt");
	if (in.is_open())
	{
		while (getline(in, line))
		{
			//"чистка" строки от пробелов и знаков препинания (только слова на русском языке)
			vector<string> output = parse_line(line); 

			//добавление в словарь
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
		cout << endl << endl;
	}
}

void TT()
{

}