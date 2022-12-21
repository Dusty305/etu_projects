#include <Windows.h>


void count_pi(void* in_parameters)
{
	PVOID** parameters = (PVOID**)in_parameters;
	double* pi = (double*)parameters[0];
	UINT current_block = *(int*)(parameters[1]);
	const HANDLE mutex = *(HANDLE*)(parameters[2]);
	const UINT thread_n = *(int*)(parameters[3]);
	const UINT iterations_per_block = *(UINT*)(parameters[4]);
	const UINT precesion = *(UINT*)(parameters[5]);

	double sum = 0;
	while (current_block < precesion)
	{
		for(int i = current_block; i < current_block + iterations_per_block; i++)
			sum += 4 / (1 + ((i + 0.5) / precesion) * ((i + 0.5) / precesion));
		current_block += thread_n * iterations_per_block;
	}

	WaitForSingleObject(mutex, INFINITE);
	*pi += sum;
	ReleaseMutex(mutex);
}

ULONGLONG calculate_pi(UINT thread_n, double& pi)
{
	UINT iterations_per_block = 10;
	UINT precesion = 100000000;
	ULONGLONG start_time = 0, end_time = 0;
	HANDLE* threads = nullptr;
	HANDLE mutex = nullptr;
	UINT* starting_iterations = nullptr;
	PVOID** parameters = nullptr;

	threads = new HANDLE[thread_n];
	starting_iterations = new UINT[thread_n];
	parameters = new PVOID*[thread_n];
	mutex = CreateMutex(NULL, FALSE, NULL);
	
	for (int i = 0; i < thread_n; i++) {
		parameters[i] = new PVOID[6];
		starting_iterations[i] = i * iterations_per_block;
		parameters[i][0] = &pi;
		parameters[i][1] = &starting_iterations[i];
		parameters[i][2] = &mutex;
		parameters[i][3] = &thread_n;
		parameters[i][4] = &iterations_per_block;
		parameters[i][5] = &precesion;

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
	pi /= precesion;
	end_time = GetTickCount64();

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