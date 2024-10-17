#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <fstream>
#include <Windows.h>
#include <locale>

//Для .json файлов
#include <nlohmann/json.hpp>

using namespace std;

using json = nlohmann::json;

map<string, vector<string>> endings;

void parse_json()
{
	ifstream ifs(R"(endings.json)");
	if (ifs.is_open())
	{
		json Doc{ json::parse(ifs) };
		Doc.get_to(endings);
	}
	ifs.close();
}

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

	//Для корректного считывания русского языка из файла
	locale::global(std::locale{ ".UTF-8" }); // inform standard library application logic uses UTF-8
	cout.imbue(std::locale("")); // use system locale to select output encoding

	parse_json();

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