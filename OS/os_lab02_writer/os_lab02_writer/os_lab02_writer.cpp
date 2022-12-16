#include <Windows.h>
#include <iostream>


using namespace std;

void print_winapi_error() noexcept
{
	DWORD error = GetLastError();
	string message;
	if (error == 0)
		message = string();
	else
	{
		LPSTR message_buffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPSTR)&message_buffer, 0, NULL);
		message = string(message_buffer, size);
		LocalFree(message_buffer);
	}

	cout << "Ошибка: " << message << "(Код ошибки: 0x" << hex << GetLastError() << ")\n";
}

void create_file(HANDLE&);
void create_file_mapping(HANDLE&, HANDLE&);
void map_view_of_file(HANDLE&, LPVOID&);
void unmap_view_of_file(LPVOID&);
void write_to_map(LPVOID&);

int main()
{
	HANDLE file = NULL, file_map = NULL;
	LPVOID map_address = nullptr;

	while (true)
	{
		string answer;
		cout
			<< "1. Создание файла." << endl
			<< "2. Создание объекта сопоставления для созданного файла" << endl
			<< "3. Сопоставление представления сопоставления файла" << endl
			<< "4. Отменить сопоставление представления файла" << endl
			<< "5. Запись данных в сопоставление" << endl
			<< "6. Закрыть приложение" << endl;
		cin >> answer;
		switch (answer[0])
		{
		case '1':
			create_file(file);
			break;
		case '2':
			create_file_mapping(file, file_map);
			break;
		case '3':
			map_view_of_file(file_map, map_address);
			break;
		case '4':
			unmap_view_of_file(file_map);
			break;
		case '5':
			write_to_map(map_address);
			break;
		case '6':
			return;
		}
		system("pause");
	}
}

void create_file(HANDLE& file)
{
	if (file != NULL and file != INVALID_HANDLE_VALUE)
		CloseHandle(file);
	wstring file_path;
	cout << "Введите путь до файла: ";
	wcin >> file_path;
	file = CreateFile(
		file_path.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (file == INVALID_HANDLE_VALUE)
		print_winapi_error();
	else
		cout << "Файл был создан.\n";
}

void create_file_mapping(HANDLE& file, HANDLE& file_map)
{
	LARGE_INTEGER size;
	
	if (file_map != NULL)
		CloseHandle(file_map);

	cout << "Введите количество байтов для сопоставления: ";
	wcin >> size.QuadPart;

	file_map = CreateFileMapping(
		file,
		NULL,
		PAGE_READWRITE,
		size.HighPart,
		size.LowPart,
		NULL
	);

	if (file_map == NULL)
		print_winapi_error();
	else
		cout << "Сопоставление было создано.\n";
}

void map_view_of_file(HANDLE& file_map, LPVOID& map_address)
{
	SIZE_T size;

	cout << "Введите количество байтов для сопоставления: ";
	wcin >> size;

	map_address = MapViewOfFile(
		file_map,
		FILE_MAP_WRITE,
		NULL,
		NULL,
		size
	);

	if (map_address == NULL)
		print_winapi_error();
	else
		cout << "Сопоставление было создано.\n";
}

void unmap_view_of_file(LPVOID& map_address)
{
	if (map_address == NULL)
		cout << "Сопоставления нет.\n";
	else
	{
		UnmapViewOfFile(map_address);
		map_address = NULL;
		cout << "Сопоставление было отменено.\n";
	}
}

void write_to_map(LPVOID&)
{
	LARGE_INTEGER offset;
	SIZE_T size;

	cout << "Введите байт с которого начинать запись сопоставления: ";
	wcin >> offset.QuadPart;

	cout << "Введите количество байт для ввода: ";
	wcin >> size;


}
