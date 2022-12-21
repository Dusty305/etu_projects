#include <omp.h>


ULONGLONG calculate_pi_omp(int thread_n, double& pi)
{
	const UINT iterations_per_block = 10;
	const UINT precesion = 100000000;
	const int block_n = precesion / iterations_per_block;
	omp_lock_t lock;
	ULONGLONG start_time = 0, end_time = 0;

	omp_set_num_threads(thread_n);
	omp_init_lock(&lock);

	
	start_time = GetTickCount64();
	#pragma omp parallel
	{
		double sum = 0;

		#pragma omp for
		for(int block = 0; block < block_n; block++)
			for(int i = block * iterations_per_block; i < block * iterations_per_block + iterations_per_block; i++)
				sum += 4 / (1 + ((i + 0.5) / precesion) * ((i + 0.5) / precesion));
		omp_set_lock(&lock);
		pi += sum;
		omp_unset_lock(&lock);
	}
	end_time = GetTickCount64();
	
	pi /= precesion;
	return end_time - start_time;
}