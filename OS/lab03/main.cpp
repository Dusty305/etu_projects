#include <iostream>
#include "calculator.h"
#include "ompcalculator.h"


using namespace std;

int main()
{
	setlocale(LC_ALL, "Russian");
	string inp;
	ULONGLONG time;
	unsigned int thread_n;
	double pi;

	while (true)
	{
		pi = 0;
		cout 
			<< "Программа вычисляет число pi, используя многопоточность.\n"
			<< "Выберите, какую технологию использовать для вычислений.\n"
			<< "1. Стандартная (Win32 API).\n"
			<< "2. OpenMP.\n"
			<< "3. Выйти из программы\n";
		cin >> inp;
		switch (inp[0])
		{
		case '1':
			cout << "Введите число потоков для вычислений: ";
			cin >> thread_n;
			time = calculate_pi(thread_n, pi);
			cout.precision(100000000);
			cout << "На вычисление pi = " << pi << " ушло " << time << " мс\n";
			break;
		case '2':
			cout << "Введите число потоков для вычислений: ";
			cin >> thread_n;
			time = calculate_pi_omp(thread_n, pi);
			cout.precision(100000000);
			cout << "На вычисление pi = " << pi << " ушло " << time << " мс\n";
			break;
		case '3':
			return 0;
		}
		std::system("pause");
	}
}