#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <fstream>
#include <Windows.h>
#include <iomanip>
#include <conio.h>
using namespace std;

vector<int> vowels = { 'а','о','я','е','ё','и','у','ы','ю','э'}; //гласные русского алфавита
vector<vector<string>> sentences = {}; //вектор типа строкового вектора - где i элемент = номер предложения, а строковый вектор = предложение (каждый элемент вектора - слово)
vector<string> exceptions; //вектор слов исключений
map<string, vector<pair<string, vector<int>>>> endings; //словарь окончаний: ключ - окончание; значение = слово + вектор предложений, в которых встретилось слово
map<pair<int, string>, vector<string>> syllables_to_words; //словарь слогов слов: ключ = кол-во слогов + окончание; значение - вектор слов

//входные файлы
string const input_endings_file_name = "endings.txt";
string const input_exception_file_name = "exceptions.txt";
string const input_datafile_name = "text.txt";

//выходные файлы
string const rhyme_file_name = "rhyme.txt";
string const count_file_name = "count.txt";
string const numbered_text_file_name = "numbered_text.txt";

///функция загрузки слов в словарь окончаний <endings> из файла 
void initialize_map()
{
	ifstream in(input_endings_file_name);
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

///функция загрузки списка исключений из файла
void initialize_exceptions() 
{
	string line = "";
	ifstream in(input_exception_file_name);

	if (in.is_open())
	{
		while (getline(in, line))
		{
			exceptions.push_back(line);
		}
	}
	in.close();
}

///функция понижения заглавной буквы
string to_lower_case_and_plain_word(string str) 
{
	for (int i = 0; i < str.length(); i++)
	{
		str[i] = tolower(str[i]);
	}

	if (str.size() > 1)
	{
		// от -64 до -1 - русский алфавит (заглавные и строчные), -88 и -72 - бува Ё и ё
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

///функция проверки файла на его наличие и пустоту
bool open_file(string& filename, ifstream& in)
{
	bool f = true; //флаг для продолжения проверки пока ошибки не будут исправлены
	do
	{
		f = true;
		in.open(filename);

		if (!in.is_open())  //проверка на открытие
		{
			cout << endl << endl << "Ошибка открытия файла – отсутствует входной файл. Проверьте нахождение файла <" << input_datafile_name << "> в текущем каталоге." << endl << endl << endl;
			f = false;
			system("pause");
			continue;
		}

		if (in.peek() == -1)  //проверка файла на пустоту 
		{
			cout << endl << endl << "Ошибка открытия файла – входной файл пустой. Проверьте наличие текста в файле <" << input_datafile_name << ">" << endl << endl << endl;
			f = false;
			in.close();
			system("pause");
			continue;
		}

	} while (f != true);

	cout << endl << endl << "Программа успешно завершена!" << endl << endl;

	return true;
	
}

///функция разбивания поступающего текста по предложениям
void parse_sentences()
{
	string line;
	string word = ""; //для побуквенной записи текущего слова
	string filename = input_datafile_name;
	ifstream in;
	bool quote_is_open = false; //флаг открытия кавычек
	vector<string> current_sentence;

	if (open_file(filename, in))
	{
		int index = 0;
		while (getline(in, line))
		{
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

						//если после кавычек идет тире со словами автора с маленькой буквы, продолжаем предложение. "Привет!" - сказал он.
						if ((i < line.size() - 4) && (line[i + 2] == '–' && (line[i + 4] >= 'а' && line[i + 4] <= 'я')))  
						{
							word += line[i];
							continue;
						}

						//проверка, не идет ли перед кавычками завершающий знак предложения; если идет, то тогда записываем завершающие кавычки и завершаем предложение; Пример: "Привет!"
						if (line[i - 1] == '?!' || line[i - 1] == '!' || line[i - 1] == '?' || line[i - 1] == '…' || line[i - 1] == '.')                                                                                       
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
					}
				}

				else   //если встретился завершающий знак
				{
					if (quote_is_open == true)  //продолжаем записывать предложение, если кавычки ОТКРЫТЫ, но встретился завершающий знак предложения  (Пример:  "Я пришел!")         
					{                            
						word += line[i];
						continue;
					}

					if (line[i] == '…' && i < line.size() - 2)  //если троеточие встретилось внутри предложения, не в конце строки, то продолжаем записывать (Пример: Тихо... хорошо)
					{
						if (line[i + 2] >= 'а' && line[i + 2] <= 'я') //если буква через пробел маленькая, то это одно предложение 
						{
							word += line[i];
							continue;
						}
						
					}

					//если точка встретилась внутри скобок, продолжаем записывать  
					if (line[i] == '.' && i < line.size() - 1)  
					{
						if (line[i + 1] == ')')    //если следующий после точки символ ) , то записываем скобку последним символом в текущее предложение              
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

					//для диалога: если после ?; ...; !; . идет тире со словами автора с маленькой буквы, продолжаем предложение
					if ((line[i] == '!' || line[i] == '?' || line[i] == '.' || line[i] == '…') && (i < line.size() - 4)) 
					{
						if (line[i + 2] == '–' && (line[i + 4] >= 'а' && line[i + 4] <= 'я'))   //проверка, что через пробел идет "-", а потом через пробел маленькая буква (Пример: - Привет! - сказала она.)
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

///функция вывода нумерованных предложений в файл
void print_numbered_sentences()
{
	ofstream fout(numbered_text_file_name);
	if (fout.is_open())
	{
		fout << "Данный файл содержит упорядоченные предложения входного файла <" << input_datafile_name << ">\n\n" << endl;
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

///функция подсчёта количества слогов в слове
void count_syllables(string word, string ending)
{
	int count_syllables = 0;
	for (int i = 0; i < word.size(); i++)
	{
		if (find(vowels.begin(), vowels.end(), word[i]) != vowels.end()) //если символ - гласная
		{
			count_syllables++; //увеличение числа слогов
		}
	}

	if (syllables_to_words.find(pair<int, string>(count_syllables, ending)) == syllables_to_words.end()) //если ключа еще нет в словааре окончаний
	{
		syllables_to_words.insert(pair<pair<int,string>, vector<string>>(pair<int, string>(count_syllables, ending), {word})); //создаём ключ и слово в вектор
	}
	else //если ключ есть в мапе
	{
		syllables_to_words[pair<int, string>(count_syllables, ending)].push_back(word); //просто добавляем в список слов
	}
}

///функция нахождения индекса элемента в векторе
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

///функция проверки наличия элемента в векторе пар
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

///функция поиска и добавления глаголов в словарь окончаний
void parse_text()
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
						if ((is_element_in_vector_pair(endings[current_key], current_word) == false) && (find(exceptions.begin(), exceptions.end(), current_word) == exceptions.end()))  //Проверка на дубликаты и исключения
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

///функция вывода номеров предложений
void print_examples_in_text(vector<int> examples_in_text, ofstream& fout)
{
	fout << " [";
    for (int i = 0; i < examples_in_text.size(); i++)
    {
        fout << examples_in_text[i]; //выводим номер предложения
        if (i < examples_in_text.size() - 1)
        {
            fout << ", "; //добавляем запятую, если это не последний номер
        }
    }
    fout << "]"; 
}

///функция вывода рифмованных пар по окончанию и количеству слогов
int print_words_according_to_syllables(string ending, ofstream& fout, int print_num)
{
	bool is_start = true; //флаг начала запаси новой группы слов текущего окончания
	for (auto iterator = syllables_to_words.begin(); iterator != syllables_to_words.end(); iterator++)
	{
		int current_number_of_syllables = iterator->first.first; //текущее количество слогов
		string current_ending = iterator->first.second; //текущее окончание
		vector<string> current_words = iterator->second;
		if (current_ending == ending) //если совпало с переданным окончанием - выводим
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
					int index = find_element_in_vector_pair(endings[ending], current_words[i]); //нахождения индекса текущего слова в векторе окончаний
					print_examples_in_text(endings[ending][index].second, fout); //вывод номеров предложений по найденному индексу
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

///внешняя функция вывода рифмованных пар в файл
void rhyme()
{
	int i = 0;
	int total_count = 0; //общее количество сочетаний всех рифмующихся пар
	ofstream fout(rhyme_file_name);
	if (fout.is_open())
	{
		fout << "Данный текстовый файл содержит найденные рифмованные пары." << endl
			<< "В квадратных скобках справа от найденного члена пары записываются номера предложений, в которых он встречается." << endl
			<< "Рифмующиеся пары записаны в группы и разделены по количеству слогов.\n\n"<< endl;

		for (auto iterator = endings.begin(); iterator != endings.end(); iterator++)
		{
			vector<pair<string, vector<int>>> current_vector = iterator->second;
			if (current_vector.size() > 1)
			{
				int n = current_vector.size();
				int combination_count = (n * (n - 1)) / 2;  //количество сочетаний без повторений для текущего окончания

				total_count += combination_count;  //добавляем количество сочетаний для текущего окончания к итоговому количеству
				
				i = print_words_according_to_syllables(iterator->first, fout, i); //вызов функции вывода с текущим окончанием 
				fout << "--------------------------------" << endl;
				fout << "Количество пар для группы: " << combination_count << endl;
				fout << "--------------------------------" << endl << endl;
				
			}
		}

		fout << endl;
		fout << endl << "Общее число найденных рифмующихся пар: " << total_count << endl;
	}
	fout.close();
}

///функция, определяющая логику сортировки
bool compare(pair < string, vector<int>> a, pair < string, vector<int>> b)
{
	return a.second.size() < b.second.size();  //сортировка по возрастанию
}

///функция вывода слова и числа его появлений в тексте
void count_words()
{
	int print_num = 0;
	ofstream fout(count_file_name);
	if (fout.is_open())
	{
		fout << "Данный текстовый файл содержит найденные члены рифмующихся пар и их количество появлений в тексте.\n\n" << endl;

		for (auto iterator = endings.begin(); iterator != endings.end(); iterator++) //прохождение по map (каждая итерация - новый вектор слов к новому окончанию)
		{
			vector<pair<string, vector<int>>> ending_words = iterator->second;

			if (ending_words.size() > 1)  //выводим только те слова, которые составляют пары рифм (если одно слово в группе окончания - не выводим)
			{
				sort(ending_words.begin(), ending_words.end(), compare);  //сортировка
				fout << "[Группа " << ++print_num << "]" << endl << endl;
				fout << left << setw(20) << "Слово:" << "Число появлений в тексте:" << endl;
				for (int i = 0; i < ending_words.size(); i++)
				{
					fout << left << setw(20) << ending_words[i].first << ending_words[i].second.size() << endl;
				}
				fout << endl << endl;
			}
		}
	}
	fout.close();
}

///функция вывода приветственной информации пользователю
void print_message()
{
	cout << "Здравствуйте! Вы запустили программу, которая ищет однородные рифмы из текстового файла.\n";
	cout << "Часть речи, к которой относятся слова в рифмующихся парах -  глагол.\n\nСледующие текстовые файлы содержатся в текущем каталоге:" << endl;
	cout << " <text.txt> - входной файл, содержащий текст на русском языке;" << endl;
	cout << " <numbered_text.txt> - выходной файл, содержащий пронумерованные предложения входного файла;" << endl;
	cout << " <rhyme.txt> - выходной файл, содержащий найденные рифмованные пары;" << endl;
	cout << " <count.txt> - выходной файл, содержащий найденные члены рифмующихся пар и количество появлений каждого из слов." << endl << endl;
	cout << "\nОбратите внимание, что при повторном запуске программы существующие текстовые файлы будут перезаписаны! " << endl << endl << endl;
}

int main()
{
	setlocale(LC_ALL, "");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	print_message(); //приветственная информация	
	system("pause");
	initialize_map();  //загрузка слов в словарь из файла
	initialize_exceptions(); //загрузка исключений из файла
	parse_sentences(); //разбивание текста по предложениям
	parse_text(); //поиск и добавления глаголов в словарь
	rhyme(); //вывод рифмованных пар в файл <rhyme.txt>
	count_words(); //вывод слов и числа его появлений в тексте в файл <count.txt>
	print_numbered_sentences(); //функция вывода нумерованных предложений в файл
	int c = _getch(); //чтобы не закрывался exe-файл
}