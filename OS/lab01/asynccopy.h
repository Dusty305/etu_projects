#pragma once

#include "diskutility.h"

struct AsyncCopyHandle
{
	const DWORD overlapped_multiplier;
	const DWORD bytes_tc;
	const HANDLE in_file;
	const HANDLE out_file;
	const LARGE_INTEGER file_size;
	char* buffer;

	AsyncCopyHandle(DWORD om, DWORD btc, HANDLE in, HANDLE out, LARGE_INTEGER s)
		: overlapped_multiplier(om), bytes_tc(btc), in_file(in), out_file(out), file_size(s)
	{
		buffer = new char[btc + 1];
	}
	~AsyncCopyHandle()
	{
		delete[] buffer;
	}
};

VOID WINAPI async_write(DWORD code, DWORD byres, LPOVERLAPPED lpOv);
VOID WINAPI async_read(DWORD code, DWORD bytes, LPOVERLAPPED out_lpOv);
void async_end(HANDLE, HANDLE, LPOVERLAPPED, AsyncCopyHandle**);

DWORD async_copy(const wstring& in, const wstring& out, const DWORD btc, const DWORD ovm)
{
	AsyncCopyHandle** ac_handles = nullptr;
	LARGE_INTEGER file_size = { 0 };
	HANDLE in_file = nullptr;
	HANDLE out_file = nullptr;
	DWORD start_time, end_time;
	LPOVERLAPPED overlapped = nullptr;
	LARGE_INTEGER offset = { 0 };

	//OPEN FILES
	in_file = CreateFile(in.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
	out_file = CreateFile(out.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
	if (out_file == INVALID_HANDLE_VALUE || in_file == INVALID_HANDLE_VALUE)
	{
		print_winapi_error();
		async_end(in_file, out_file, overlapped, ac_handles);
		return -1;
	}
	GetFileSizeEx(in_file, &file_size);

	ac_handles = new AsyncCopyHandle*[ovm];
	overlapped = new OVERLAPPED[ovm];
	for (int i = 0; i < ovm; i++)
	{
		ac_handles[i] = new AsyncCopyHandle(ovm, btc, in_file, out_file, file_size);

		overlapped[i].hEvent = ac_handles[i];
		overlapped[i].Offset = offset.LowPart;
		overlapped[i].OffsetHigh = offset.HighPart;

		offset.QuadPart += btc;
	}

	start_time = GetTickCount64();
	for (int i = 0; i < ovm; i++)
	{
		AsyncCopyHandle* ach = (AsyncCopyHandle*)overlapped[i].hEvent;
		ReadFileEx(ach->in_file, ach->buffer, ach->bytes_tc, &overlapped[i], async_write);
	}
	while (SleepEx(100, true));
	end_time = GetTickCount64();
	
	async_end(in_file, out_file, overlapped, ac_handles);

	return end_time - start_time;
}

VOID WINAPI async_write(DWORD code, DWORD bytes, LPOVERLAPPED lpOv)
{
	if(bytes > 0)
	{
		AsyncCopyHandle* ach = (AsyncCopyHandle*)lpOv->hEvent;
		WriteFileEx(ach->out_file, ach->buffer, bytes, lpOv, async_read);
	}
}

VOID WINAPI async_read(DWORD code, DWORD bytes, LPOVERLAPPED lpOv)
{
	AsyncCopyHandle* ach = (AsyncCopyHandle*)lpOv->hEvent;
	LARGE_INTEGER new_offset;
	new_offset.LowPart = lpOv->Offset;
	new_offset.HighPart = lpOv->OffsetHigh;
	new_offset.QuadPart += (LONGLONG)(ach->overlapped_multiplier * ach->bytes_tc);

	if (new_offset.QuadPart < ach->file_size.QuadPart)
	{
		lpOv->Offset = new_offset.LowPart;
		lpOv->OffsetHigh = new_offset.HighPart;
		ReadFileEx(ach->in_file, ach->buffer, ach->bytes_tc, lpOv, async_write);
	}
}

void async_end(HANDLE in_file, HANDLE out_file, LPOVERLAPPED ov, AsyncCopyHandle** ac_handles)
{
	if (in_file != nullptr or in_file != INVALID_HANDLE_VALUE)
		CloseHandle(in_file);
	if (out_file != nullptr or out_file != INVALID_HANDLE_VALUE)
		CloseHandle(out_file);
	if (ac_handles != nullptr)
	{
		DWORD ovm = ac_handles[0]->overlapped_multiplier;
		for (int i = 0; i < ovm; i++)
			delete ac_handles[i];
		delete[] ac_handles;
	}
	delete ov;
}