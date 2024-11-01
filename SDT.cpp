#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <fstream>
#include <Windows.h>
#include <iomanip>
using namespace std;

vector<int> vowels = { 'а','о','я','е','ё','и','у','ы','ю','э'};
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

bool open_file(string& filename, ifstream& in)  //проверка файла на наличие и пустоту
{
	bool f = true;
	do
	{
		f = true;
		system("cls");
		in.open(filename);

		if (!in.is_open())  //проверка на открытие
		{
			cout << endl << "Ошибка! Файл не найден. Проверьте нахождение файла <text.txt> в папке с программой." << endl << endl;
			f = false;
			system("pause");
			continue;
		}

		if (in.peek() == -1)  //проверка файла на пустоту 
		{
			cout << endl << "Ошибка! Файл пустой. Проверьте наличие текста в файле <text.txt>" << endl << endl;
			f = false;
			in.close();
			system("pause");
			continue;
		}

	} while (f != true);

	return true;
	
}

void parse_sentences()   //разделение текста на предложения
{
	string line;
	string word = "";
	string filename = "text.txt";
	ifstream in;
	bool quote_is_open = false; //флаг открытия кавычек

	if (open_file(filename, in))
	{
		int index = 0;
		while (getline(in, line))
		{
			vector<string> current_sentence;
			for (int i = 0; i < line.size(); i++)
			{
				if (line[i] != '.' && line[i] != '!' && line[i] != '?' && line[i] != '…')
				{
					if (line[i] == '«')  //если встретились открывающие кавычки
					{
						quote_is_open = true;
					}

					if (line[i] == '»')  //если встретились закрывающие кавычки
					{
						quote_is_open = false;

						if (line[i - 1] == '?!' || line[i - 1] == '!' || line[i - 1] == '?' || line[i - 1] == '…')  //проверка, не идет ли перед кавычками завершающий знак предложения; если идет, то тогда записываем завершающие кавычки и завершаем предложение; Пример: "Привет!"                                                                                     
						{
							word += line[i];
							current_sentence.push_back(word);
							sentences.push_back(current_sentence);
							current_sentence.clear();
							word = "";
							continue;
						}
					}

					if (line[i] == ' ') //если встретился пробел
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
						//if (is_letter(line[i]))
						//{
							//word += line[i];
						//}
					}
				}

				else   //если встретился завершающий знак
				{
					if (quote_is_open == true)  //продолжаем записывать предложение, если кавычки ОТКРЫТЫ, но встретился завершающий знак предложения (вследствие чего и попали в этот else)              
					{                           //(это означает, что внутри прямой речи встречаются ?!. но предложение НЕ может быть закончено, пока не закроются кавычки)  (Пример:  "Я пришел!")
						word += line[i];
						continue;
					}

					if (line[i] == '…' && i != line.size() - 1 && i != line.size() - 2)  //если троеточие встретилось внутри предложения, не в конце строки, то продолжаем записывать (Пример: Тихо... хорошо)
					{
						if (line[i + 2] >= 'а' && line[i + 2] <= 'я') //если буква через пробел маленькая, то это одно предложение (можем сделать эту проверку, так как убедились, что троеточие не последний и не предпоследний символ в строке, иначе не сможем обратиться без проверки к символам после троеточия)
						{
							word += line[i];
							continue;
						}
						
					}

					if (line[i] == '.' && i != line.size() - 1)  //если точка встретилась внутри скобок, продолжаем записывать  (предложение(69,70) - оно в скобках и скобка записывалась в следующее предложение)
					{
						if (line[i + 1] == ')')                  //если следующий после точки символ ) , то записываем скобку последним символом в текущее предложение. (Раньше было вот так: (70)) - две скобки подряд)
						{
							word += line[i];
							word += line[i+1];
							current_sentence.push_back(word);
							sentences.push_back(current_sentence);
							current_sentence.clear();
							word = "";
							i++;      //увеличиваем самостоятельно счетчик, так как записали сразу скобку, идущую после точки
							continue;
						}

					}

					if ((line[i] == '!' || line[i] == '?' || line[i] == '.' || line[i] == '…') &&                          //для диалога: если после ?; ...; !; . идет тире со словами автора с маленькой буквы, продолжаем предложение; проверяем, что после заверш знака идет еще минимум 4 символа
						(i != line.size() - 1 && i != line.size() - 2 && i != line.size() - 3 && i != line.size() - 4))
					{
						if (line[i + 2] == '–' && (line[i + 4] >= 'а' && line[i + 4] <= 'я'))   //проверка, что через пробел идет "-", а потом через пробел маленькая буква  (Пример: - Привет! - сказала она.)
						{
							word += line[i];
							continue;
						}
					}


					word += line[i];
					current_sentence.push_back(word);
					sentences.push_back(current_sentence);
					current_sentence.clear();
					word = "";
				}
			}
		}
	}
	in.close();
}

void print_numbered_sentences() //запись в файл пронумерованных предложений
{
	ofstream fout("numbered_text.txt");
	if (fout.is_open())
	{
		for (int i = 0; i < sentences.size(); i++)
		{
			fout << "(" << i + 1 << ")";
			vector<string> current_sentence = sentences[i];

			for (int j = 0; j < current_sentence.size(); j++)
			{
				if (j == current_sentence.size() - 1)
				{
					fout << current_sentence[j] << endl;
				}
				else
				{
					fout << current_sentence[j] << " ";
				}
				
			}
		}
	}

	fout.close();
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

void print_examples_in_text(vector<int> examples_int_text, ofstream& fout) //вывод номеров предложений
{
	fout << " [";
    for (int i = 0; i < examples_int_text.size(); i++)
    {
        fout << examples_int_text[i]; //выводим номер предложения
        if (i < examples_int_text.size() - 1)
        {
            fout << ", "; //добавляем запятую, если это не последний номер
        }
    }
    fout << "]";
}

int print_words_according_to_syllables(string ending, ofstream& fout, int print_num)  //функция вывода с текущим окончанием
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
	int total_count = 0;  // переменная для хранения итогового количества элементов в векторах
	ofstream fout("rhyme.txt");
	if (fout.is_open())
	{
		for (auto iterator = endings.begin(); iterator != endings.end(); iterator++)
		{
			vector<pair<string, vector<int>>> current_vector = iterator->second;
			if (current_vector.size() > 1)
			{
				int n = current_vector.size();
				int combination_count = (n * (n - 1)) / 2;  // количество сочетаний без повторений для текущего окончания

				total_count += combination_count;  // добавляем количество сочетаний для текущего окончания к итоговому количеству
				
				i = print_words_according_to_syllables(iterator->first, fout, i); // вызов функции вывода с текущим окончанием 
				fout << "--------------------------------" << endl;
				fout << "Количество пар для группы: " << combination_count << endl;
				fout << "--------------------------------" << endl << endl;
				
			}
		}
		// выводим итоговое количество сочетаний для всех окончаний
		fout << endl;
		fout << endl << "Общее число найденных рифмующихся пар: " << total_count << endl;
	}
	fout.close();
}

void count_words()  //вывод числа появлений каждого из слов
{
	ofstream fout("count.txt");
	if (fout.is_open())
	{
		fout << left << setw(20) << "Слово:" << "Число появлений в тексте:" << endl;
		for (auto iterator = endings.begin(); iterator != endings.end(); iterator++)
		{
			vector<pair<string, vector<int>>> pair_vector = iterator->second;
			if (pair_vector.size() > 1)
			{
				for (int i = 0; i < pair_vector.size(); i++)
				{
					fout << left << setw(20) << pair_vector[i].first << pair_vector[i].second.size() << endl;
				}
			}
		}
	}
	fout.close();
}
 
void title()  //анонс назначения программы
{
	cout << endl << endl << endl << setw(12) << " " << "ПРОГРАММА ДЛЯ ФОРМИРОВАНИЯ РИФМОВАННЫХ ПАР ГЛАГОЛОВ" << endl << endl << endl << endl << endl;
}

void print_message()
{
	cout << "Рифмованные пары из поступающего текста записаны в файл: <rhyme.txt>" << endl << endl
		<< "Количество появлений каждого из слов, составляющих рифмованные пары, содержатся в файле: <count.txt>" << endl << endl
		<< "Пронумерованные предложения поступающего на вход программы текста находятся в файле: <numbered_text.txt>" << endl << endl;
}

//TODO: каждый вектор должен быть проверен на список исключений 
//мужской род прошедшее время - принес, прилег, отвез, сжег и т.д. - придумать как учитываться
//добавить что глаголы не = 1 букве 
int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	title();
	system("pause");
	setlocale(LC_ALL, "");
	initialize_map();  //загрузка слов в словарь из файла
	initialize_exceptions(); //загрузка исключений из файла
	parse_sentences_to_process();
	parse_sentences();
	parse_text();
	rhyme();
	count_words();
	print_numbered_sentences();
	print_message();
}