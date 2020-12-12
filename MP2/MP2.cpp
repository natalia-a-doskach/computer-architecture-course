#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h> 
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <iostream>

#define N 5  //number of philosophers
#define THINKING 0
#define HUNGRY 1 
#define EATING 2 

int state[N]; //state of each philosopher (thinking, hungry, eating)
int counter[N]; //counts amount of meals each philosopher is having
int workingTime; //how long each philosopher sits at the table

sem_t mutex;
sem_t S[N];

void test(int n)
{
	if (state[n] == HUNGRY
		&& state[(n + 4) % N] != EATING
		&& state[(n + 1) % N] != EATING) {
		state[n] = EATING;
		sem_post(&S[n]);

		printf("Philosopher %d obtained forks\n", n);
	}
}

// take up forks
void obtainForks(int n)
{
	sem_wait(&mutex);
	state[n] = HUNGRY;
	test(n);
	sem_post(&mutex);
	//if still hasn't eaten - wait until neibours finish eating and test you -> resulting  in you eating
	//otherwise, skip this
	sem_wait(&S[n]);
	printf("Philosopher %d started eating\n", n);
	int timeEating = rand() % 2000 + 1000; //each meal is between 1 & 3 seconds
#ifdef _WIN32
	Sleep(timeEating);
#else
	sleep(timeEating);
#endif
	counter[n]++;

	printf("Philosopher %d finished eating\n", n);
}

// put down forks
void putForksDown(int n)
{
	sem_wait(&mutex);
	state[n] = THINKING;
	printf("Philosopher %d put forks down & started thinking\n", n);
	test((n + 4) % N);
	test((n + 1) % N);
	sem_post(&mutex);
	int timeThinking = rand() % 2000 + 1000; //each thinking pause is between 1 & 3 seconds
#ifdef _WIN32
	Sleep(timeThinking);
#else
	sleep(timeThinking);
#endif
	printf("Philosopher %d finished thinking\n", n);
}

void* func(void* param)
{

	int n = (int)param;
	printf("Philosopher %d entered the dining room\n", n);
	clock_t start_time1 = clock();
	while (1) {
		if ((clock() - start_time1) / CLOCKS_PER_SEC >= workingTime) //check if philosopher worked for more than his workingTime
		{
			printf("Philosopher %d went home. He had %d meals.\n", n, counter[n]);
			break;
		}
		obtainForks(n);
		putForksDown(n);
	}
	return nullptr;
}

int getTime()
{
	int time;
	std::cout << "Input for how long you want each philosopher to sit at the table in seconds (from 5 to 1000 seconds):";
	std::cin >> time;
	if (time < 5 || time > 1000)
	{
		std::cout << "n cannot be less than 5 or more than 1000 ";
		return getTime();
	}
	else
		return time;
}

int main()
{
	//print intro information
	std::cout << "This is a dining philosopher problem. There are 5 philosophers and 5 forks.\n Each philosopher tries to obtain both forks, eats for 1-3 seconds (each time exast duration is a new random number),\n puts forks down, thinks for 1-3 seconds(each time exast duration is a new random number) and so on.\n";
	//get for how long each thread must work from the user
	workingTime = getTime();

	// initialize the semaphores 
	sem_init(&mutex, 0, 1);
	for (int i = 0; i < N; i++)
		sem_init(&S[i], 0, 0);

	//create threads
	pthread_t thread_id[N];
	for (int i = 0; i < N; i++) {
		pthread_create(&thread_id[i], NULL, func, (void*)i);
	}

	for (int i = 0; i < N; i++)
		pthread_join(thread_id[i], NULL);
}
