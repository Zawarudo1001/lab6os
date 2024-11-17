#include <iostream>
#include <queue>
#include <map>
#include <string>
#include <random>
#include <vector>
#include <Windows.h>
#include <fstream>


std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> dist6(1000, 9999);

using namespace std;

typedef unsigned char byte;

byte memory[65536];
map<int, int> procSize;


class proc {
public:
	string path;
	int id;
	int size;

	proc(string fpath) {
		path = fpath;
		id = dist6(rng);
		cout << "Был создан процесс с идентификатором: " << id << endl;
		size = getsize(path);
		procSize.insert(make_pair(id, size));
	}

	long getsize(string path) {
		ifstream in(path.c_str(), ifstream::ate | ifstream::binary);
		return in.tellg();
	}

};


class MemoryManager{
public:
	queue<proc> procQueue;
	map<int, int> sectionEmul;

	int sections;
	int sectionsize;

	MemoryManager() {
		cout << "Введите количество разделов: ";
		cin >> sections;
		cout << endl;
		sectionsize = sizeof(memory) / sections;
		cout << "Размер одного раздела составляет: " << sectionsize << " байт" << endl;
		for (int i = 0; i < sections; i++) {
			sectionEmul.insert(make_pair(i,0));
		}
	}

	int sectionParse() {
		for (int i = 0; i < sections; i++) {
			if (sectionEmul[i] == 0) {
				return i;
			}
		}
		return -1;
	}

	int procParse(int id) {
		for (int i = 0; i < sections; i++) {
			if (sectionEmul[i] == id) {
				return i;
			}
		}
		return -1;
	}
	
	void loadProc(proc A) {
		if (A.size <= sectionsize) {
			int temp = sectionParse();
			if (temp >= 0) {
				sectionEmul[temp] = A.id;
				cout << "Процесс с идентификатором " << A.id << " был загружен в раздел " << temp << endl;
				return;
			}
			else {
				procQueue.push(A);
				cout << "Процесс с идентификатором " << A.id << " был добавлен в очередь\n" << endl;
			}
		}
		else {
			cout << "Невозможно загрузить процесс из-за его размера\n" << endl;
		}
	}

	void delProc(int id) {
		int temp = procParse(id);
		if (temp < 0) { cout << "Данный процесс еще не выгружен ни в какой раздел" << endl; return; }
		else {
			sectionEmul[temp] = 0;
			cout << "Процесс с идентификатором " << id << " был выгружен из памяти разделов" << endl;
		}

		temp = sectionParse();
		if (!procQueue.empty() and temp >= 0) {
			proc Next = procQueue.front();
			cout << "В очереди есть процесс " << Next.id << " требующий загрузки" << endl;
			procQueue.pop();
			loadProc(Next);
		}
	}

	void display() {
		for (int i = 0; i < sections; i++) {
			if (sectionEmul[i] == 0) cout << "Раздел " << i << " пуст" << endl;
			else {
				cout << "Раздел " << i << " занят процессом " << sectionEmul[i] << endl;
				cout << "Размер процесса: " << sectionEmul[i] << " составляет " << procSize[sectionEmul[i]] << " байт" << endl;
			}
		}
		cout << "В очереди ожидают " << procQueue.size() << " процессов\n" << endl;
	}

};

class Menu : MemoryManager{
public:
	
	int parse() {
		int code;
		cout << "Введите 0 для загрузки процесса" << endl;
		cout << "Введите 1 для выгрузки процесса" << endl;
		cout << "Введите 2 для вывода состояния памяти" << endl;
		cin >> code;
		cout << endl;
		return code;
	}

	void task(int code) {
		if (code == 0) {
			cout << "Введите путь для загрузки процесса: "; 
			string path;
			cin >> path;
			proc A(path);
			loadProc(A);
		}

		if (code == 1) {
			cout << "Введите идентификатор выгружаемого процесса: " << endl;
			int id;
			cin >> id;
			delProc(id);
		}

		if (code == 2) {
			display();
		}
	}

};



int main(int argc, char** argv)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	Menu menu;
	while (true) {
		int c = menu.parse();
		menu.task(c);
	}
	return 0;
}
