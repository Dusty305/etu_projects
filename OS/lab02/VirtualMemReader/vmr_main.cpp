#include <Windows.h>
#include <iostream>


using namespace std;

void print_winapi_error() noexcept
{
	DWORD error = GetLastError();
	string message;
	if (error == 0)
		return;

	LPSTR message_buffer = nullptr;
	size_t size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		(LPSTR)&message_buffer,
		0,
		NULL
	);
	message = string(message_buffer, size);
	LocalFree(message_buffer);

	cout << "Ошибка: " << message << "(Код ошибки: 0x" << hex << GetLastError() << ")\n";
}

class VirtualMemoryReader
{
private:
	HANDLE file_map = NULL;
	LPVOID address = NULL;
	SIZE_T size = NULL;
public:
	~VirtualMemoryReader()
	{

	}

	BOOL open_mapped_file()
	{
		if (address != NULL)
		{
			UnmapViewOfFile(address);
			address = NULL;
			size = NULL;
		}
		wstring mapping_name;
		cout << "Введите название проецируемого файла, которое вы ему задали: ";
		wcin >> mapping_name;

		file_map = OpenFileMapping(
			FILE_MAP_READ,
			FALSE,
			mapping_name.c_str()
		);

		if (file_map == NULL)
		{
			print_winapi_error();
			return FALSE;
		}
		else
			return TRUE;
	}

	BOOL map_view_of_file()
	{
		if (file_map == NULL)
		{
			cout << "Ошибка. Для проецирования фрагмента проецирумоего файла его нужно открыть.\n";
			return FALSE;
		}

		if (address != NULL)
		{
			UnmapViewOfFile(address);
			address = NULL;
			size = NULL;
		}

		cout << "Введите размер фрагмента для проецирования: ";
		wcin >> size;

		address = MapViewOfFile(
			file_map,
			FILE_MAP_READ,
			NULL,
			NULL,
			size
		);

		if (address == NULL)
		{
			print_winapi_error();
			size = NULL;
			return FALSE;
		}
		else
			return TRUE;
	}

	void print_mapped_data()
	{
		if (file_map == NULL)
		{
			cout << "Ошибка. Перед выводом содержимого проецируемого файла нужно его открыть.\n";
			return;
		}
		if (address == NULL)
		{
			cout << "Ошибка. Перед выводом фрагмента файла его нужно спроецировать.\n";
			return;
		}

		byte* b = (byte*)address;
		cout << "Байты проецируемого фрагмента файла: ";
		unsigned n;
		for (SIZE_T i = 0; i < size; i++)
		{
			n = *(b++);
			cout << n << " ";
		}
		cout << endl;
	}
};

int main()
{
	setlocale(LC_ALL, "ru");
	VirtualMemoryReader vm_reader;
	while (true)
	{
		string answer;
		cout
			<< "1. Открыть проецируемый файл\n"
			<< "2. Спроецировать фрагмент файла в память\n"
			<< "3. Вывести содержимое из проецируемого файла.\n"
			<< "4. Закрыть приложение.\n";
		cin >> answer;
		switch (answer[0])
		{
		case '1':
			if(vm_reader.open_mapped_file())
				cout << "Проецируемый файл был успешно открыт.\n";
			break;
		case '2':
			if (vm_reader.map_view_of_file())
				cout << "Фрагмент файла в памяти был успешно спроецирован.\n";
			break;
		case '3':
			vm_reader.print_mapped_data();
			break;
		case '4':
			return 0;
		}
		system("pause");
	}
}