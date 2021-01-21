#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <chrono>
using namespace std;
using namespace std::chrono;

struct arg_struct
{
	int l;
	int r;
	double *array;
};

void *dosum(void *argument)
{
	struct arg_struct *args = (struct arg_struct *) argument;
	int l = args->l;
	int r = args->r;
	double *array = args->array;

	double sum = 0;
	for (int i = l; i < r; i++)
	{
		sum += array[i];
	}
	double *ret = (double*) malloc(sizeof(double));
	*ret = sum;
	return (void*) ret;
}
int main()
{
	int n = 1000;
	double *array = new double[n];
	for (int i = 0; i < n; i++)
		array[i] = i;

	int numT = 8;
	cout << "enter number of threads" << endl;
	cin >> numT;

	double avgTime = 0;

	int numIter = 10;
	double sum = 0;

	while (numIter--)
	{
		auto start = high_resolution_clock::now();

		int rangeCompleted = 0;
		pthread_t threads[numT];
		for (int i = 0; i < numT; i++)
		{

			int l = rangeCompleted;
			int r = rangeCompleted + (n / numT);
			if (i == numT - 1)
			{
				r = n;
			}
			struct arg_struct *args = (struct arg_struct *) malloc(sizeof(arg_struct));
			args->l = l;
			args->r = r;
			args->array = array;

			pthread_create(&threads[i], NULL, dosum, (void*) args);
			rangeCompleted += (n / numT);
		}

		sum = 0;
		for (int i = 0; i < numT; i++)
		{
			double *ret;
			pthread_join(threads[i], (void **) &ret);
			sum = sum + (*ret);
		}

		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds> (stop - start);

		//cout<<"time taken="<<duration.count()<<endl;
		avgTime += duration.count();
	}

	avgTime /= 10;
	cout << "total Time Taken=" << avgTime << endl;

	double formula_sum = (double) n *(n - 1) / 2;
	std::cout << "N: " << n << std::endl;
	std::cout << "Loop Sum: " << sum << std::endl;
	std::cout << "Formula Sum: " << formula_sum <<
		std::endl;
	delete[] array;
	return 0;
}


/******************************************************

A.
model name	: AMD A6-7310 APU with AMD Radeon R4 Graphics
cpu cores	: 4

B.
time taken(in micro seconds) by corresponding number of threads

1 thread-116.2
2 thread- 178.3
4 thread-281.5
8 thread-565.7
16 thread-1111.2

C.
It seems the cost of our taks is very small so we can not leverage the multithreading because
creating and joining the threads are more expensive than the real task’s cost.


******************************************************/
