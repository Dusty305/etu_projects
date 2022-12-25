#include <Windows.h>

DWORD get_page_size()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwPageSize;
}

int main()
{
	const DWORD page_size = get_page_size();
	const DWORD page_number = 0 + 3 + 0 + 5 + 0 + 1;

	// INIT SHARED OBJECT
	HANDLE file = CreateFile(
		L"E:\\file.txt",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	HANDLE mapping_object = CreateFileMapping(
		file,
		NULL,
		PAGE_READWRITE,
		0,
		page_size * page_number,
		L"Local\\file"
	);

	// INIT SEMAPHORES
	HANDLE reader_sem[page_number], writer_sem[page_number];
	SECURITY_ATTRIBUTES semaphoreSecurityAttributes = { 
		sizeof(SECURITY_ATTRIBUTES),
		NULL, 
		TRUE 
	};
	for (int i = 0; i < page_number; i++)
	{
		// WRITER SEMAPHORE
		wchar_t writer_name[] = L"writer ";
		writer_name[6] = '0' + i;
		writer_sem[i] = CreateSemaphore(
			&semaphoreSecurityAttributes,
			1,
			1,
			writer_name
			);

		// READER SEMAPHORE
		wchar_t reader_name[] = L"reader ";
		reader_name[6] = '0' + i;
		reader_sem[i] = CreateSemaphore(
			&semaphoreSecurityAttributes,
			0,
			1,
			reader_name
		);
	}

	// GET STARTUP INFO FOR NEW PROCESSES
	STARTUPINFO startup_info;
	ZeroMemory(&startup_info, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);

	// PROCESSES INIT
	HANDLE processes[page_number * 2];
	PROCESS_INFORMATION writer_info[page_number], reader_info[page_number];
	wchar_t buffer[100] = L"";
	for (int i = 0; i < page_number; i++)
	{
		ZeroMemory(&reader_info[i], sizeof(reader_info[i]));
		ZeroMemory(&writer_info[i], sizeof(writer_info[i]));

		CreateProcess(
			L"E:\\Projects\\etu_projects\\OS\\lab04\\x64\\Debug\\reader.exe",
			buffer,
			NULL,
			NULL,
			TRUE,
			CREATE_SUSPENDED,
			NULL,
			NULL,
			&startup_info,
			&reader_info[i]
		);
		CreateProcess(
			L"E:\\Projects\\etu_projects\\OS\\lab04\\x64\\Debug\\writer.exe", 
			buffer,
			NULL,
			NULL,
			TRUE,
			CREATE_SUSPENDED,
			NULL,
			NULL,
			&startup_info,
			&writer_info[i]
		);

		processes[i] = reader_info[i].hProcess;
		processes[page_number * 2 - 1 - i] = writer_info[i].hProcess;
	}

	// PROCESSES STARTUP
	for (int i = 0; i < page_number; i++)
		ResumeThread(writer_info[i].hThread);
	for (int i = 0; i < page_number; i++)
		ResumeThread(reader_info[i].hThread);

	// WAIT FOR PROCESSES END
	WaitForMultipleObjects(page_number * 2, processes, TRUE, INFINITE);

	// END OF PROGRAM
	for (int i = 0; i < page_number; i++)
	{
		CloseHandle(writer_info[i].hProcess);
		CloseHandle(writer_info[i].hThread);

		CloseHandle(reader_info[i].hProcess);
		CloseHandle(reader_info[i].hThread);

		CloseHandle(reader_sem[i]);
		CloseHandle(writer_sem[i]);
	}
	CloseHandle(mapping_object);
	CloseHandle(file);

	return 0;
}