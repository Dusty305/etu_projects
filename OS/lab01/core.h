#pragma once

#include <Windows.h>
#include <unordered_map>
#include <exception>
#include <iostream>
#include <utility>
#include <stdexcept>
#include <format>

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