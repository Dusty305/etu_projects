#pragma once

#include "diskutility.h"

char** buffers;
DWORD overlapped_multiplier;
DWORD bytes_tc;

HANDLE in_file;
HANDLE out_file;
LARGE_INTEGER file_size;


VOID WINAPI async_write(DWORD code, DWORD byres, LPOVERLAPPED lpOv);
VOID WINAPI async_read(DWORD code, DWORD bytes, LPOVERLAPPED out_lpOv);
void async_end(LPOVERLAPPED);

DWORD async_copy(const wstring& in, const wstring& out, const DWORD btc, const DWORD ovm)
{
	overlapped_multiplier = ovm;
	bytes_tc = btc;
	buffers = new char* [ovm];
	for (int i = 0; i < ovm; i++)
		buffers[i] = new char[btc + 1];

	DWORD start_time, end_time;
	LPOVERLAPPED overlapped = nullptr;
	LARGE_INTEGER offset = { 0 };

	//OPEN FILES
	in_file = CreateFile(in.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
	out_file = CreateFile(out.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
	if (out_file == INVALID_HANDLE_VALUE || in_file == INVALID_HANDLE_VALUE)
	{
		print_winapi_error();
		async_end(overlapped);
		return -1;
	}
	GetFileSizeEx(in_file, &file_size);

	overlapped = new OVERLAPPED[ovm];
	for (int i = 0; i < ovm; i++)
	{
		overlapped[i].hEvent = buffers + i * sizeof(char**);

		overlapped[i].Offset = offset.LowPart;
		overlapped[i].OffsetHigh = offset.HighPart;
		offset.QuadPart += bytes_tc;
	}

	start_time = GetTickCount64();
	for (int i = 0; i < ovm; i++)
	{
		char* buffer = *((char**)overlapped[i].hEvent);
		ReadFileEx(in_file, buffer, bytes_tc, &overlapped[i], async_write);
	}
	while (SleepEx(100, true));

	end_time = GetTickCount64();
	
	async_end(overlapped);

	return end_time - start_time;
}

VOID WINAPI async_write(DWORD code, DWORD byres, LPOVERLAPPED lpOv)
{
	char* buffer = *((char**)lpOv->hEvent);
	WriteFileEx(out_file, buffer, bytes_tc, lpOv, async_read);
}

VOID WINAPI async_read(DWORD code, DWORD bytes, LPOVERLAPPED lpOv)
{
	LARGE_INTEGER new_offset;
	new_offset.LowPart = lpOv->Offset;
	new_offset.HighPart = lpOv->OffsetHigh;
	new_offset.QuadPart += (LONGLONG)(overlapped_multiplier * bytes_tc);

	if (new_offset.QuadPart < file_size.QuadPart)
	{
		lpOv->Offset = new_offset.LowPart;
		lpOv->OffsetHigh = new_offset.HighPart;
		char* buffer = *((char**)lpOv->hEvent);
		ReadFileEx(in_file, buffer, bytes_tc, lpOv, async_write);
	}
}

void async_end(LPOVERLAPPED ov)
{
	if (in_file != INVALID_HANDLE_VALUE)
		CloseHandle(in_file);
	if (out_file != INVALID_HANDLE_VALUE)
		CloseHandle(out_file);

	for (int i = 0; i < overlapped_multiplier; i++)
		delete[] buffers[i];
	delete[] buffers;
	delete ov;
}