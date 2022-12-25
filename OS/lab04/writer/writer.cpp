#include <Windows.h>
#include <fstream>
#include <time.h>

#define ITERATIONS_N 10

using namespace std;

DWORD get_page_size()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwPageSize;
}

int main()
{
	srand(time(NULL));

	const DWORD page_size = get_page_size();
	const DWORD page_number = 0 + 3 + 0 + 5 + 0 + 1;
	const DWORD process_id = GetCurrentProcessId();
	const DWORD start_time = GetTickCount64();

	// INIT LOG
	fstream log;
	log.open("E:\\writer_logg.txt", fstream::app);
	log << "PROCESS ID\t\tFILE PAGE\t\tPROCESS STATE\t\tTIME"
		<< " (start time = " << start_time << ")\n";

	// INIT VIEW OF FILE
	HANDLE mapping_object = OpenFileMapping(
		FILE_MAP_WRITE, 
		FALSE, 
		L"Local\\file"
	);
	void* view_of_file = MapViewOfFile(
		mapping_object,
		FILE_MAP_WRITE,
		0,
		0,
		page_number * page_size
	);
	VirtualLock(view_of_file, page_number * page_size);

	// INIT SEMAPHORES
	HANDLE reader_sem[page_number], writer_sem[page_number];
	for (int i = 0; i < page_number; i++)
	{
		// WRITER SEMAPHORE
		wchar_t writer_name[] = L"writer ";
		writer_name[6] = '0' + i;
		writer_sem[i] = OpenSemaphore(SYNCHRONIZE, FALSE, writer_name);

		// READER SEMAPHORE
		wchar_t reader_name[] = L"reader ";
		reader_name[6] = '0' + i;
		reader_sem[i] = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, reader_name);
	}

	// START WRITING
	for (int i = 0; i < ITERATIONS_N; i++)
	{
		int page = WaitForMultipleObjects(page_number, writer_sem, FALSE, INFINITE);
		
		log << process_id << "\t\t" << page << "\t\t" << "WRITE_START" << "\t\t" << GetTickCount64() - start_time << "\n";
		char* buf = (char*)((char*)view_of_file + page * page_size);
		buf[0] = 'a' + i;
		buf[1] = '\0';
		Sleep(500 + rand() % 1000);
		log << process_id << "\t\t" << page << "\t\t" << "WRITE_END" << "\t\t" << GetTickCount64() - start_time << " (wrote " << buf << ")" << "\n";
		
		ReleaseSemaphore(reader_sem[page], 1, NULL);
		log << process_id << "\t\t" << page << "\t\t" << "SEM_RELEASED" << "\t\t" << GetTickCount64() - start_time << "\n";
	}

	// CLOSE HANDLES
	for (int i = 0; i < page_number; i++)
	{
		CloseHandle(reader_sem[i]);
		CloseHandle(writer_sem[i]);
	}
	VirtualUnlock(view_of_file, page_size);
	UnmapViewOfFile(view_of_file);
	log.close();
	//delete[] buffer;

	return 0;
}