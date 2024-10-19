#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <fstream>
#include <Windows.h>
using namespace std;


map<string, vector<string>> endings;

void initialize_map()   //загрузка слов в словарь из файла
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

vector<string> initialize_exceptions()  //загрузка исключений из файла
{
	vector<string> exception;
	string line = "";
	ifstream in("exceptions.txt");

	if (in.is_open())
	{
		while (getline(in, line))
		{
			exception.push_back(line);
		}
	}
	in.close();
	return exception;
}



vector<string> parse_line(string line)  //функция избавления от знаков препинания в строке
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
				clean_words.push_back(word);   //поместить элемент в конец вектора
			}
			word = "";
		}
	}


	return clean_words;   //возвращаем вектор, в котором хранятся только слова из одной строки

}

string to_lower_case(string str)  //функция понижения заглавной буквы
{
	for (int i = 0; i < str.length(); i++)
	{
		str[i] = tolower(str[i]);  //понизить регистр
	}
	return str;
}


void parse_word(vector<string> words)   //функция поиска и добавления глаголов в map (словарь)
{


	for (int i = 0; i < words.size(); i++)
	{
		string current_word = to_lower_case(words[i]);   //понижаем регистр букв слова (элемента) в векторе
		vector<string> exception;

		for (auto iterator = endings.begin(); iterator != endings.end(); iterator++)  //Перебираем ключи словаря
		{
			string current_key = iterator->first;   //берем окончание


			if ((current_word.size() > 1) && (current_word.size() >= current_key.size()))
			{
				if (current_word.substr(current_word.size() - current_key.size()) == current_key)  //проверка совпадения окончания с ключом
				{
					exception = initialize_exceptions();

					if ((find(endings[current_key].begin(), endings[current_key].end(), current_word) == endings[current_key].end()) &&
						(find(exception.begin(), exception.end(), current_word) == exception.end()))  //Проверка на дубликаты и исключения
					{
						endings[current_key].push_back(current_word);
					}
				}
			}
		}
	}
}


vector <string> highlight()   //вывод с нумерацией предложений
{
	string line;
	string word = "";
	vector<string> sentence;
	ifstream in("test.txt");
	if (in.is_open())
	{
		while (getline(in, line))
		{

			for (int i = 0; i < line.size(); i++)
			{
				if (line[i] != '.')
				{
					if (word[0] == ' ')
					{
						word = "";
					}
					word += line[i];
				}
				else
				{
					word += ".";

					sentence.push_back(word);   //поместить элемент в конец вектора
					word = "";
				}
			}
		}
	}
	in.close();
	return sentence;
}


void rhyme()  //вывод рифмы в файл
{
	int i = 0;
	string str;
	ofstream fout("rhyme.txt");
	if (fout.is_open())
	{
		for (auto iterator = endings.begin(); iterator != endings.end(); iterator++)
		{
			vector<string> current_vector = iterator->second;
			if (!current_vector.empty())
			{
				if (current_vector.size() > 1)
				{
					fout << ++i << ")" << " ";
					str = "";
					for (int i = 0; i < current_vector.size(); i++)
					{
						str += current_vector.at(i) + " - ";
					}
					fout << str.erase(str.size() - 3);
					fout << endl << endl;
				}
			}
		}
	}
	fout.close();

}


//TODO: каждый вектор должен быть проверен на список исключений 
//мужской род прошедшее время - принес, прилег, отвез, сжег и т.д. - придумать как учитываться
//добавить что глаголы не = 1 букве 
int main()
{

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "");
	int t = 0;
	initialize_map();  //загрузка слов в словарь из файла
	string line;

	ifstream in("test.txt");
	if (in.is_open())
	{
		while (getline(in, line))
		{
			vector<string> output = parse_line(line);   //"чистка" строки от пробелов и знаков препинания (только слова на русском языке)

			parse_word(output);   //добавление в словарь
		}
	}
	in.close();

	/*for (auto iterator = endings.begin(); iterator != endings.end(); iterator++)
	{
		cout << "KEY: " << iterator->first << endl;
		vector<string> current_vector = iterator->second;
		for (int i = 0; i < current_vector.size(); i++)
		{
			cout << current_vector.at(i) << ", ";
		}
		cout << endl << endl;
	}*/
	vector<string> text = highlight();

	for (auto iterator = text.begin(); iterator != text.end(); iterator++)
	{
		cout << "(" << ++t << ")" << *iterator << endl;
	}
	rhyme();
}