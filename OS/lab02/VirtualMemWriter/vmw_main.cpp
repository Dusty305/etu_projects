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

	cout << "������: " << message << "(��� ������: 0x" << hex << GetLastError() << ")\n";
}

class VirtualMemoryWriter
{
private:
	HANDLE file		= NULL;
	HANDLE file_map = NULL;
	LPVOID address  = NULL;
	SIZE_T size		= NULL;

public:
	~VirtualMemoryWriter()
	{
		if (address != NULL)
			UnmapViewOfFile(address);
		if (file_map != NULL)
			CloseHandle(file_map);
		if (file != NULL and file != INVALID_HANDLE_VALUE)
			CloseHandle(file);
	}

	BOOL create_file_mapping()
	{
		if (address != NULL)
		{
			UnmapViewOfFile(address);
			address = NULL;
			size = NULL;
		}
		if (file_map != NULL)
			CloseHandle(file_map);
		if (file != NULL)
			CloseHandle(file);

		wstring file_path;
		cout << "������� ���� �� �����: ";
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
		{
			print_winapi_error();
			file = NULL;
			return FALSE;;
		}

		LARGE_INTEGER size;
		wstring mapping_name;
		
		cout << "������� ���������� ������ ��� �������� ������� �������������� �����: ";
		wcin >> size.QuadPart;

		cout << "������� ��� ������� �������� �����: ";
		wcin >> mapping_name;

		file_map = CreateFileMapping(
			file,
			NULL,
			PAGE_READWRITE,
			size.HighPart,
			size.LowPart,
			mapping_name.c_str()
		);

		if (file_map == NULL)
		{
			print_winapi_error();
			CloseHandle(file);
			file = NULL;
			return FALSE;
		}
		else
			return TRUE;
	}
	
	BOOL map_view_of_file()
	{
		if (file == NULL or file_map == NULL)
		{
			cout << "������. ��������� ������� ������������ ���� ����� �������������� ��� ��������� � ������.\n";
			return FALSE;
		}
		
		if (address != NULL)
		{
			UnmapViewOfFile(address);
			address = NULL;
			size = NULL;
		}

		cout << "������� ���������� ������ ��� ��������: ";
		wcin >> size;

		address = MapViewOfFile(
			file_map,
			FILE_MAP_WRITE,
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

	BOOL write_to_map()
	{
		if (file == NULL or file_map == NULL)
		{
			cout << "������. ����� ������� ������ � ������������ ���� ��� ���� �������.\n";
			return FALSE;
		}
		else if (address == NULL)
		{
			cout << "������. ����� ������� ������ � ������������ ���� ���� ������� �������� ��� ���������.\n";
			return FALSE;
		}

		SIZE_T size;
		cout << "������� ���������� ���� ��� �����: ";
		wcin >> size;

		if (size > this->size)
		{
			cout << "����������� ���� ��� ����� ��������� ������ �������������.\n";
			return FALSE;
		}

		byte* b = (byte*)address;

		cout << "������� ����� ��� ������ ����� ������: ";
		unsigned n;
		for (SIZE_T i = 0; i < size; i++)
		{
			cin >> n;
			*(b++) = n;
		}

		return TRUE;
	}
};

int main()
{
	setlocale(LC_ALL, "ru");

	VirtualMemoryWriter vm_writer;

	while (true)
	{
		string answer;
		cout
			<< "1. ������� ������������ ����."
			<< "2. ������������ �������� ����� � ������.\n"
			<< "3. ������ ������ � ������������ ����.\n"
			<< "4. ������� ����������.\n";
		cin >> answer;
		switch (answer[0])
		{
		case '1':
			if (vm_writer.create_file_mapping())
				cout << "������������ ���� ��� ������� ������.\n";
			break;
		case '2':
			if (vm_writer.map_view_of_file())
				cout << "�������� ��������� ����� � ������ ���� ������� �������.\n";
			break;
		case '3':
			if (vm_writer.write_to_map())
				cout << "������ ��������� �������.\n";
			break;
		case '4':
			return 0;
		}
		system("pause");
	}
}