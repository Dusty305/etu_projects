#include <omp.h>


ULONGLONG calculate_pi_omp(int thread_n, double& pi)
{
	const UINT iterations_per_block = 10;
	const UINT precesion = 100000000;
	const int block_n = precesion / iterations_per_block;
	omp_lock_t lock;
	ULONGLONG start_time = 0, end_time = 0;
	omp_init_lock(&lock);
	start_time = GetTickCount64();
	double _pi = 0;
	#pragma omp parallel num_threads(thread_n)
	{
		double sum = 0;
		#pragma omp for schedule(dynamic, iterations_per_block) nowait
		for(int i = 0; i < precesion; ++i)
			sum += 4 / (1 + ((i + 0.5) / precesion) * ((i + 0.5) / precesion));
		omp_set_lock(&lock);
		pi += sum;
		omp_unset_lock(&lock);
	}
	end_time = GetTickCount64();
	
	pi = pi / precesion;
	return end_time - start_time;
}