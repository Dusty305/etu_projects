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
HANDLE event = NULL;
OVERLAPPED overlapped;
wstring pipe_name;

void create_pipe();
void connect_pipe();
void disconnect_pipe();
void write_to_pipe();
void wait_writing_to_pipe();

int main()
{
	setlocale(LC_ALL, "ru");

	event = CreateEvent(NULL, TRUE, FALSE, NULL);
	ZeroMemory(&overlapped, sizeof(overlapped));
	overlapped.hEvent = event;

	while (true)
	{
		string inp;
		cout
			<< "1. Создать канал.\n"
			<< "2. Подключить канал.\n"
			<< "3. Отключить канал.\n"
			<< "4. Произвести запись в канал.\n"
			<< "5. Подождать конца записи.\n"
			<< "6. Закрыть программу.\n";
		cin >> inp;

		switch (inp[0])
		{
		case '1':
			cout << "Введите название канала: ";
			wcin >> pipe_name;
			create_pipe();
			break;
		case '2':
			connect_pipe();
			break;
		case '3':
			disconnect_pipe();
			break;
		case '4':
			write_to_pipe();
			break;
		case '5':
			wait_writing_to_pipe();
			break;
		case '6':
			CloseHandle(event);
			CloseHandle(pipe);
			return 0;
		}
		system("pause");
	}
}

void create_pipe()
{
	wstring temp_string = L"\\\\.\\pipe\\";
	if (pipe)
		CloseHandle(pipe);
	pipe = CreateNamedPipe(
		(temp_string + pipe_name).c_str(),
		PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE,
		2,
		MAX_PATH,
		MAX_PATH,
		0,
		NULL
	);

	if (pipe == INVALID_HANDLE_VALUE)
	{
		pipe = NULL;
		print_winapi_error();
	}
	else
		cout << "Канал был успешно создан.\n";
}

void connect_pipe()
{
	if (ConnectNamedPipe(pipe, &overlapped))
		cout << "Подключение прошло успешно.\n";
	else
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			while (HasOverlappedIoCompleted(&overlapped));
			ResetEvent(event);
			cout << "Подключение прошло успешно.\n";
		}
		else
			print_winapi_error();
	}
}

void disconnect_pipe()
{
	if (DisconnectNamedPipe(pipe))
		cout << "Канал был успешно отключен.\n";
	else
		print_winapi_error();
}

VOID WINAPI after_write(DWORD error, DWORD bytes, LPOVERLAPPED lpov) {
	if (error)
		print_winapi_error();
	SetEvent(lpov->hEvent);
}

void write_to_pipe()
{
	string s;
	cout << "Введите строку для записи в канал: ";
	cin >> s;

	ZeroMemory(&overlapped, sizeof(overlapped));
	overlapped.hEvent = event;
	if (WriteFileEx(pipe, s.c_str(), s.length(), &overlapped, after_write))
		cout << "Операция записи началась.\n";
	else
		print_winapi_error();
}

void wait_writing_to_pipe()
{
	SleepEx(0, TRUE);
	if (WaitForSingleObject(event, 2000) == WAIT_TIMEOUT)
		cout << "Время ожидания истекло, операция записи не завершилась.\n";
	else
		cout << "Ожидание завершенно.\n";
	ResetEvent(event);
}