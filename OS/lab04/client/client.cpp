#include <Windows.h>
#include <iostream>


using namespace std;

void print_winapi_error() noexcept
{
	std::string message;
	DWORD error = GetLastError();

	if (error == 0)
		message = std::string();
	else
	{
		LPSTR message_buffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPSTR)&message_buffer, 0, NULL);
		message = std::string(message_buffer, size);
		LocalFree(message_buffer);
	}

	std::cout << "Произошла ошибка: " << message << "(Код ошибки: 0x" << std::hex << GetLastError() << ")\n";
}

HANDLE pipe = NULL;
OVERLAPPED overlapped;
wstring pipe_name;
UINT pipe_string_length;

void connect_pipe();
void read_from_pipe();
void output_pipe_str();

int main()
{
	setlocale(LC_ALL, "ru");
	ZeroMemory(&overlapped, sizeof(overlapped));
	while (true)
	{
		string inp;
		cout
			<< "1. Подключиться к каналу.\n"
			<< "2. Начать чтение из канала.\n"
			<< "3. Закрыть программу.\n";
		cin >> inp;

		switch (inp[0])
		{
		case '1':
			cout << "Введите название канала: ";
			wcin >> pipe_name;
			connect_pipe();
			break;
		case '2':
			cout << "Введите длину строки, которую вы хотите прочитать: ";
			cin >> pipe_string_length;
			read_from_pipe();
			break;
		case '3':
			CloseHandle(pipe);
			return 0;
		}
		system("pause");
	}
}

void connect_pipe()
{
	wstring temp_string = L"\\\\.\\pipe\\";
	if (pipe)
		CloseHandle(pipe);
	pipe = CreateFile(
		(temp_string + pipe_name).c_str(),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
		NULL
	);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		pipe = NULL;
		print_winapi_error();
	}
	else
		cout << "Подключение прошло успешно.\n";
}

VOID WINAPI after_read(DWORD error, DWORD bytes, LPOVERLAPPED lpov)
{
	char* str = (char*)lpov->hEvent;
	str[bytes] = '\0';
	if (error)
		print_winapi_error();
	else
		cout << "Чтение завершено. Прочитанная строка: " << str << "\n";
	delete[] str;
}

void read_from_pipe()
{
	ZeroMemory(&overlapped, sizeof(overlapped));
	char* pipe_string = new char[pipe_string_length + 1];
	overlapped.hEvent = pipe_string;
	if (ReadFileEx(pipe, pipe_string, pipe_string_length, &overlapped, after_read))
	{
		cout << "Чтение началось.\n";
		SleepEx(0, TRUE);
	}
	else
		print_winapi_error();
}