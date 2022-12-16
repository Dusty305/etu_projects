#include "core.h"


/*
	Получение информации о вычислительной системе.

	Функция Win32 API – GetSystemInfo.
*/

string get_processor_architecture(WORD arch);

void get_system_info()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	string arch = get_processor_architecture(si.wProcessorArchitecture);

	cout 
		<< "OEM ID: "											   << si.dwOemId					 << endl
		<< "Архитектура процессора: "							   << arch							 << endl
		<< "Размер страницы: "									   << si.dwPageSize					 << endl
		<< "Нижняя граница адресов (base адрес): "				   << si.lpMinimumApplicationAddress << endl
		<< "Верхняя граница адресов (bound адрес): "			   << si.lpMaximumApplicationAddress << endl
		<< "Количество логических процессоров в текущей группе: "  << si.dwNumberOfProcessors		 << endl
		<< "Маска активных процессоров: "						   << si.dwActiveProcessorMask		 << endl;
}

string get_processor_architecture(WORD arch)
{
	switch (arch)
	{
	case PROCESSOR_ARCHITECTURE_AMD64:
		return "x64 (AMD or Intel)";
	case PROCESSOR_ARCHITECTURE_ARM:
		return "ARM";
	case PROCESSOR_ARCHITECTURE_ARM64:
		return "ARM64";
	case PROCESSOR_ARCHITECTURE_IA64:
		return "Intel Itanium-based";
	case PROCESSOR_ARCHITECTURE_INTEL:
		return "x86";
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
		return "Unknown architecture";
	}
}

/*
	Определение статуса виртуальной памяти.

	Функция Win32 API – GlobalMemoryStatus.
*/
void get_vm_status()
{
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(ms);
	if (!GlobalMemoryStatusEx(&ms))
	{
		print_winapi_error();
		return;
	}

	cout
		<< "Процент используемой физической памяти: " << ms.dwMemoryLoad << endl
		<< "Объём физической памяти: " << ms.ullTotalPhys << " байтов" << endl
		<< "Доступно физической памяти: " << ms.ullAvailPhys << " байтов" << endl
		<< "Предел памяти для процесса: " << ms.ullTotalPageFile << endl
		<< "Максимальный объём памяти, который можно зафиксировать: " << ms.ullAvailPageFile << " байтов" << endl
		<< "Размер ВАП для процесса: " << ms.ullTotalVirtual << endl
		<< "Объем незарезервированного и незафиксированного объема памяти в ВАП для процесса: " << ms.ullAvailVirtual << endl;
}

/*
	Определение состояния конкретного участка памяти 
	по заданному с клавиатуры адресу.

	Функция Win32 API – VirtualQuery.
*/
void get_pm_status()
{
	MEMORY_BASIC_INFORMATION mbi;
	LPVOID address;
	cout << "Введите адрес участка памяти, состояние которого нужно определить: ";
	wcin >> hex >> address >> dec;
	if (!VirtualQuery(address, &mbi, sizeof(mbi)))
	{
		print_winapi_error();
		return;
	}

	string state;
	switch (mbi.State)
	{
	case MEM_COMMIT:
		state = "физическая память выделена для страниц";
		break;
	case MEM_FREE:
		state = "освобожденные страницы";
		break;
	case MEM_RESERVE:
		state = "страны зарезервированны";
		break;
	}

	string type;
	switch (mbi.Type)
	{
	case MEM_IMAGE:
		type = "страницы сопоставляются с представлением раздела изображения";
		break;
	case MEM_MAPPED:
		type = "страницы сопоставляются с представлением раздела";
		break;
	case MEM_PRIVATE:
		type = "страницы являются частными";
		break;
	}

	cout
		<< "Указатель на базовый адрес области страниц: " << mbi.BaseAddress << endl
		<< "Указатель на базовый адрес диапазона страниц: " << mbi.AllocationBase << endl
		<< "Параметр защиты памяти при первоначальном выделении региона: " << hex << mbi.AllocationProtect << endl
		<< "Настоящий параметр защиты памяти: " << hex << mbi.Protect << endl
		<< "Размер региона страниц: " << dec << mbi.RegionSize << " байтов" << endl
		<< "Состояние страниц в регионе: " << state << endl
		<< "Тип страниц в регионе: " << type << endl;
}

/*
	Резервирование региона (и передача ему физической памяти, если commit == true)
	в автоматическом режиме и в режиме ввода адреса начала региона.

	Функция Win32 API – VirtualAlloc.
*/
void alloc_vm(DWORD alloc_type)
{
	LPVOID address = nullptr;
	SIZE_T size = 0;

	string answer;
	cout << "Если вы хотите ввести начальный адрес региона для резервирования, введите 1, иначе 0.\n";
	cin >> answer;
	if (answer[0] == '1')
	{
		cout << "Введите начальный адрес: ";
		cin >> hex >> address >> dec;
	}

	cout << "Введите размер региона для резервирования: ";
	wcin >> size;

	address = VirtualAlloc(address, size, alloc_type, PAGE_READWRITE);
	if (!address)
		print_winapi_error();
	else
		cout << "Резервирование региона прошло успешно.\n";
		
}

/*
	Запись данных в ячейки памяти по заданным с клавиатуры
	адресам.
*/
void write_to_pm()
{
	// Исправить??
	MEMORY_BASIC_INFORMATION mbi;
	LPVOID address = nullptr;

	cout << "Введите адрес ячейки памяти, куда вы желаете записать данные: ";
	wcin >> hex >> address >> dec;

	VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	// error check?

	if (mbi.State != MEM_COMMIT)
	{
		cout << "Память не была выделена. Запись данных невозможна.\n";
		return;
	}

	SIZE_T max_size = mbi.RegionSize - ((SIZE_T)address - (SIZE_T)mbi.BaseAddress);
	SIZE_T size = 0;
	cout << "Максимальное число байтов, доступных для записи: " << max_size << endl;
	cout << "Введите количество байтов для ввода: ";
	wcin >> size;

	unsigned b;
	byte* data = (byte*)address;
	cout << "Введите через пробел байты для записи: ";
	for (SIZE_T i = 0; i < size; i++)
	{
		cin >> b;
		*(data++) = b;
	}

	cout << "Вы ввели ";
	for (SIZE_T i = 0; i < size; i++)
	{
		b = *((byte*)address + i);
		cout << b << " ";
	}
}

/*
	Установку защиты доступа для заданного (с клавиатуры) региона
	памяти и ее проверку.

	Функция Win32 API – VirtualProtect.
*/
void set_pm_protection()
{
	MEMORY_BASIC_INFORMATION mbi;
	LPVOID address = nullptr;
	DWORD new_protection, old_protection;

	cout << "Введите адрес региона для которого нужно изменить защиту доступа: ";
	wcin >> hex >> address >> dec;
	
	VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	//error check?

	cout << "Введите флаг для доступа защиты: ";
	wcin >> hex >> new_protection >> dec;
	if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, new_protection, &old_protection))
		print_winapi_error();
	else
		cout << "Флаг доступа защиты был успешно изменен.\n";
}

/*
	Возврат физической памяти и освобождение региона адресного
	пространства заданного (с клавиатуры) региона памяти.

	Функция Win32 API – VirtualFree.
*/
void free_pm()
{
	MEMORY_BASIC_INFORMATION mbi;
	LPVOID address = nullptr;

	cout << "Введите адрес региона, который надо освободить: ";
	wcin >> hex >> address >> dec;

	VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

	if (!VirtualFree(mbi.BaseAddress, 0, MEM_RELEASE))
		print_winapi_error();
	else
		cout << "Память была успешно освобождена.\n";
}