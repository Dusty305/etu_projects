#pragma once

#include "diskutility.h"

VOID WINAPI async_read(DWORD code, DWORD bytes, LPOVERLAPPED out_lpOv);
VOID WINAPI async_write(DWORD code, DWORD bytes, LPOVERLAPPED in_lpOv);
void async_end(LPOVERLAPPED in_overlapped, LPOVERLAPPED out_overlapped);

unordered_map<HANDLE, LPOVERLAPPED>* in_to_out_lpOv, * out_to_in_lpOv;
unordered_map<HANDLE, char*>* in_buffers, * out_buffers;

HANDLE* async_events;
LPOVERLAPPED in_overlapped;
LPOVERLAPPED out_overlapped;

HANDLE in_file, out_file;
LARGE_INTEGER file_size;
LARGE_INTEGER in_pos;

DWORD bytes_tc, ov_multiplier;
DWORD start_time, end_time;
bool read_is_over;

DWORD async_copy(const wstring& in, const wstring& out, const DWORD btc, const DWORD ovm)
{
	//INIT VARIABLES
	bytes_tc = btc;
	ov_multiplier = ovm;

	in_overlapped = new OVERLAPPED[ov_multiplier];
	out_overlapped = new OVERLAPPED[ov_multiplier];
	async_events = new HANDLE[ov_multiplier * 2];

	in_to_out_lpOv = new unordered_map<HANDLE, LPOVERLAPPED>;
	out_to_in_lpOv = new unordered_map<HANDLE, LPOVERLAPPED>;

	in_buffers = new unordered_map<HANDLE, char*>;
	out_buffers = new unordered_map<HANDLE, char*>;
	for (int i = 0; i < ov_multiplier; i++)
	{
		(*in_buffers)[in_overlapped[i].hEvent] = new char[bytes_tc];
		(*out_buffers)[out_overlapped[i].hEvent] = (*in_buffers)[&in_overlapped[i]];
	}


	file_size = { 0 };
	in_pos = { 0 };

	//OPEN FILES
	in_file = CreateFile(in.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
	out_file = CreateFile(out.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
	if (out_file == INVALID_HANDLE_VALUE || in_file == INVALID_HANDLE_VALUE)
	{
		print_winapi_error();
		async_end(in_overlapped, out_overlapped);
		return -1;
	}

	GetFileSizeEx(in_file, &file_size);

	read_is_over = false;

	//INIT EVENTS
	for (int i = 0; i < ov_multiplier; i++)
	{
		async_events[i] = CreateEvent(NULL, true, false, nullptr);
		async_events[i + ov_multiplier] = CreateEvent(NULL, true, false, nullptr);
		if (!async_events[i] or !async_events[i + ov_multiplier])
		{
			print_winapi_error();
			async_end(in_overlapped, out_overlapped);
			return -1;
		}
		in_overlapped[i].hEvent = async_events[i];
		out_overlapped[i].hEvent = async_events[i + ov_multiplier];

		(*in_to_out_lpOv)[in_overlapped[i].hEvent] = &out_overlapped[i];
		(*out_to_in_lpOv)[out_overlapped[i].hEvent] = &in_overlapped[i];

		in_overlapped[i].Offset = in_pos.LowPart;
		in_overlapped[i].OffsetHigh = in_pos.HighPart;

		in_pos.QuadPart += bytes_tc;
	}

	//START
	start_time = GetTickCount64();

	for (int i = 0; i < ov_multiplier; i++)
	{
		ReadFileEx(in_file, (*in_buffers)[in_overlapped[i].hEvent], bytes_tc, &in_overlapped[i], async_write);
	}

	while (!read_is_over)
		SleepEx(INFINITE, true);
	WaitForMultipleObjects(ov_multiplier, async_events, true, 0);

	//END
	end_time = GetTickCount64();
	async_end(in_overlapped, out_overlapped);

	return end_time - start_time;
}

VOID WINAPI async_read(DWORD code, DWORD bytes, LPOVERLAPPED out_lpOv)
{
	if (!read_is_over)
	{
		LPOVERLAPPED in_lpOv = (*out_to_in_lpOv)[out_lpOv];
		char* buffer = (*out_buffers)[out_lpOv];

		LARGE_INTEGER next_pos;
		next_pos.LowPart = out_lpOv->Offset;
		next_pos.HighPart = out_lpOv->OffsetHigh;
		next_pos.QuadPart += (LONGLONG)(ov_multiplier * bytes_tc);

		if (next_pos.QuadPart < file_size.QuadPart)
		{
			in_lpOv->Offset = next_pos.LowPart;
			in_lpOv->OffsetHigh = next_pos.HighPart;

			ReadFileEx(in_file, buffer, bytes_tc, in_lpOv, async_write);
		}
		else
		{
			read_is_over = true;
		}
	}

}

VOID WINAPI async_write(DWORD code, DWORD bytes, LPOVERLAPPED in_lpOv)
{
	LPOVERLAPPED out_lpOv = (*in_to_out_lpOv)[in_lpOv];
	char* buffer = (*out_buffers)[in_lpOv];

	out_lpOv->Offset = in_lpOv->Offset;
	out_lpOv->OffsetHigh = in_lpOv->OffsetHigh;

	WriteFileEx(out_file, buffer, bytes_tc, out_lpOv, async_read);
}

void async_end(LPOVERLAPPED in_overlapped, LPOVERLAPPED out_overlapped)
{
	if (in_file != INVALID_HANDLE_VALUE)
		CloseHandle(in_file);
	if (out_file != INVALID_HANDLE_VALUE)
		CloseHandle(out_file);

	for (int i = 0; i < ov_multiplier * 2; i++)
		if (async_events[i])
			CloseHandle(async_events[i]);

	for (int i = 0; i < ov_multiplier; i++)
		delete[](*in_buffers)[&in_overlapped[i]];

	delete[] in_overlapped;
	delete[] out_overlapped;
	delete[] async_events;

	delete in_to_out_lpOv;
	delete out_to_in_lpOv;
}