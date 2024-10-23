﻿#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <fstream>
#include <Windows.h>
using namespace std;

vector<int> vowels = { 'а','о','я','е','ё','и','у','ы' };
vector<vector<string>> sentences = {};
vector<string> exceptions;
map<string, vector<pair<string, vector<int>>>> endings;
map<pair<int, string>, vector<string>> syllables_to_words; // ключ - кол-во слогов + окончание, значение - вектор слов

void initialize_map()   //загрузка слов в словарь из файла
{

	ifstream in("endings.txt");
	string line;
	if (in.is_open())
	{
		while (getline(in, line))
		{
			endings.insert(pair<string, vector<pair<string, vector<int>>>>(line, {}));
		}
	}
	in.close();
}

void initialize_exceptions()  //загрузка исключений из файла
{
	string line = "";
	ifstream in("exceptions.txt");

	if (in.is_open())
	{
		while (getline(in, line))
		{
			exceptions.push_back(line);
		}
	}
	in.close();
}

string to_lower_case_and_plain_word(string str)  //функция понижения заглавной буквы
{
	for (int i = 0; i < str.length(); i++)
	{
		str[i] = tolower(str[i]);  //понизить регистр
	}


	if (str.size() > 1)
	{
		if (!(((str[0] >= -64) && (str[0] <= -33)) || ((str[0] >= -32) && (str[0] <= -1)) || (str[0] == -88) || (str[0] == -72))) // если первый элемент строки не буква
		{
			str = str.substr(1, str.size());
		}
		if (!(((str[str.size() - 1] >= -64) && (str[str.size() - 1] <= -33)) || ((str[str.size() - 1] >= -32) && (str[str.size() - 1] <= -1)) || (str[str.size() - 1] == -88) || (str[str.size() - 1] == -72))) // если последний элемент строки не буква
		{
			str = str.substr(0, str.size() - 1);
		}
	}
	return str;
}

void highlight()   //вывод с нумерацией предложений
{
	string line;
	string word = "";
	ifstream in("test.txt");
	if (in.is_open())
	{
		int index = 0;
		while (getline(in, line))
		{
			vector<string> current_sentence;
			for (int i = 0; i < line.size(); i++)
			{
				if (line[i] != '.' && line[i] != '!' && line[i] != '?')
				{
					if (line[i] == ' ')
					{
						if (word != "")
						{
							current_sentence.push_back(word);
						}
						word = "";
					}
					else
					{
						word += line[i]; 
					}
				}
				else
				{
					if (word != "")
					{
						current_sentence.push_back(word);
					}
					sentences.push_back(current_sentence);
					current_sentence.clear();
					word = "";
				}
			}
		}
	}
	in.close();
}

void count_syllables(string word, string ending) //Функция подсчёта слогов в слове
{
	int count_syllables = 0;
	for (int i = 0; i < word.size(); i++)
	{
		if (find(vowels.begin(), vowels.end(), word[i]) != vowels.end()) //если символ - гласная
		{
			count_syllables++; //увеличение числа слогов
		}
	}

	if (syllables_to_words.find(pair<int, string>(count_syllables, ending)) == syllables_to_words.end()) //если ключа еще нет в мапе
	{
		syllables_to_words.insert(pair<pair<int,string>, vector<string>>(pair<int, string>(count_syllables, ending), {word})); // создаём ключ и пустой вектор
	}
	else //если ключ есть в мапе
	{
		syllables_to_words[pair<int, string>(count_syllables, ending)].push_back(word); //просто добавляем в список слов
	}
}

int find_element_in_vector_pair(vector<pair<string, vector<int>>> pair_vector, string word)
{
	for (int i = 0; i < pair_vector.size(); i++)
	{
		if (pair_vector[i].first == word)
		{
			return i;
		}
	}
	return -1;
}

bool is_element_in_vector_pair(vector<pair<string, vector<int>>> pair_vector, string word)
{
	for (int i = 0; i < pair_vector.size(); i++)
	{
		if (pair_vector[i].first == word)
		{
			return true;
		}
	}
	return false;
}

void parse_text()   //функция поиска и добавления глаголов в map (словарь)
{
	for (int i = 0; i < sentences.size(); i++)
	{
		vector<string> current_sentence = sentences[i];
		for (int j = 0; j < current_sentence.size(); j++)
		{
			string current_word = to_lower_case_and_plain_word(current_sentence[j]);
			for (auto iterator = endings.begin(); iterator != endings.end(); iterator++)  //Перебираем ключи словаря
			{
				string current_key = iterator->first;   //берем окончание

				if ((current_word.size() > 1) && (current_word.size() >= current_key.size()))
				{
					if (current_word.substr(current_word.size() - current_key.size()) == current_key)  //проверка совпадения окончания с ключом
					{
						if ((is_element_in_vector_pair(endings[current_key], current_word) == false) 
							&& (find(exceptions.begin(), exceptions.end(), current_word) == exceptions.end()))  //Проверка на дубликаты и исключения
						{
							endings[current_key].push_back(pair<string, vector<int>>(current_word, {i + 1}));
							count_syllables(current_word, current_key); //подсчёт слогов текущего слова
						}
						else if (find(exceptions.begin(), exceptions.end(), current_word) == exceptions.end())
						{
							int index = find_element_in_vector_pair(endings[current_key], current_word);
							endings[current_key][index].second.push_back(i + 1);
						}
					}
				}
			}
		}
	}
}

void print_examples_in_text(vector<int> examples_int_text, ofstream& fout) 
{
	fout << '[';
	for (int i = 0; i < examples_int_text.size(); i++)
	{
		if (i == examples_int_text.size() - 1)
		{
			fout << examples_int_text[i];
		}
		else
		{
			fout << examples_int_text[i] << ", ";
		}
	}
	fout << ']';
}

int print_words_according_to_syllables(string ending, ofstream& fout, int print_num)
{
	bool is_start = true;
	for (auto iterator = syllables_to_words.begin(); iterator != syllables_to_words.end(); iterator++)
	{
		int current_number_of_syllables = iterator->first.first; // получили количество слогов
		string current_ending = iterator->first.second; // получили окончание
		vector<string> current_words = iterator->second;
		if (current_ending == ending) // если совпало с переданным окончанием, выводим
		{
			if (is_start == true)
			{
				fout << "[Группа " << ++print_num << "]" << endl;
				is_start = false;
			}

			fout << "Кол-во слогов - " << current_number_of_syllables << " : ";
			for (int i = 0; i < current_words.size(); i++)
			{
				vector<int> examples_int_text;
				if (i == current_words.size() - 1)
				{
					fout << current_words[i];
					int index = find_element_in_vector_pair(endings[ending], current_words[i]);
					print_examples_in_text(endings[ending][index].second, fout);
					fout << endl;
				}
				else
				{
					fout << current_words[i];
					int index = find_element_in_vector_pair(endings[ending], current_words[i]);
					print_examples_in_text(endings[ending][index].second, fout);
					fout << " - ";
				}
			}
			fout << endl;
		}
	}
	return print_num;
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
			vector<pair<string, vector<int>>> current_vector = iterator->second;
			if (!current_vector.empty())
			{
				if (current_vector.size() > 1)
				{
					i = print_words_according_to_syllables(iterator->first, fout, i); //вызов функции вывода с текущим окончанием 
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
	initialize_map();  //загрузка слов в словарь из файла
	initialize_exceptions(); //загрузка исключений из файла
	highlight();
	parse_text();

	for (int i = 0; i < sentences.size(); i++)
	{
		cout << "(" << i + 1 << ")";
		vector<string> current_sentence = sentences[i];

		for (int j = 0; j < current_sentence.size(); j++)
		{
			if (j == current_sentence.size() - 1)
			{
				cout << current_sentence[j] << "." << endl;
			}
			else
			{
				cout << current_sentence[j] << " ";
			}
		}
	}
	rhyme();
}