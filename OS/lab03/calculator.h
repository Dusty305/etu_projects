#include <Windows.h>
#include <iostream>

LPCRITICAL_SECTION lpCriticalSection = new CRITICAL_SECTION;
UINT counter;
double global_pi;

void count_pi(void* in_parameters)
{
	PVOID** parameters = (PVOID**)in_parameters;
	double* pi = (double*)parameters[0];
	const HANDLE mutex = *(HANDLE*)(parameters[1]);
	const UINT iterations_per_block = *(UINT*)(parameters[2]);
	const UINT precesion = *(UINT*)(parameters[3]);
	UINT N = precesion / iterations_per_block;
	double sum = 0;
	EnterCriticalSection(lpCriticalSection);
	UINT iteration = counter++;
	LeaveCriticalSection(lpCriticalSection);
	while (iteration < N)
	{
		UINT block_start = iteration * iterations_per_block;
		UINT block_end = block_start + iterations_per_block;
		for (int i = block_start; i < block_end && i < precesion; i++)
			sum += 4 / (1 + ((i + 0.5) / precesion) * ((i + 0.5) / precesion));
		EnterCriticalSection(lpCriticalSection);
		iteration = ++counter;
		LeaveCriticalSection(lpCriticalSection);
	}
	EnterCriticalSection(lpCriticalSection);
	global_pi += sum;
	LeaveCriticalSection(lpCriticalSection);
}

ULONGLONG calculate_pi(UINT thread_n, double& pi)
{
	UINT iterations_per_block = 1000;
	UINT precesion = 100000000;
	ULONGLONG start_time = 0, end_time = 0;
	HANDLE* threads = nullptr;
	HANDLE mutex = nullptr;
	UINT* starting_iterations = nullptr;
	PVOID** parameters = nullptr;

	threads = new HANDLE[thread_n];
	starting_iterations = new UINT[thread_n];
	parameters = new PVOID * [thread_n];
	mutex = CreateMutex(NULL, FALSE, NULL);
	global_pi = 0; 
	counter = 0;
	InitializeCriticalSection(lpCriticalSection);

	for (int i = 0; i < thread_n; i++) {
		starting_iterations[i] = i;
		parameters[i] = new PVOID[6];
		starting_iterations[i] = i * iterations_per_block;
		parameters[i][0] = &pi;
		parameters[i][1] = &mutex;
		parameters[i][2] = &iterations_per_block;
		parameters[i][3] = &precesion;

		threads[i] = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)count_pi,
			parameters[i],
			CREATE_SUSPENDED,
			NULL
		);
	}

	start_time = GetTickCount64();
	for (int i = 0; i < thread_n; i++)
		ResumeThread(threads[i]);
	WaitForMultipleObjects(thread_n, threads, TRUE, INFINITE);
	pi = global_pi / precesion;
	end_time = GetTickCount64();
	DeleteCriticalSection(lpCriticalSection);

	for (int i = 0; i < thread_n; i++)
	{
		delete[] parameters[i];
		CloseHandle(threads[i]);
	}
	delete[] parameters;
	delete[] threads;
	delete[] starting_iterations;
	CloseHandle(mutex);

	return end_time - start_time;
}