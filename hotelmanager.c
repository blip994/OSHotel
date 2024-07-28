#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
struct order
{
	int inval;
	int ordlis[100];
	int ordersize;
	int tno;
	int first;
	int second;
	int third;
	int fourth;
	int bill;
	int flag;
	int ter_flag;
	int man_flag;
	int admin_flag;
};

void orderInitializer(struct order *a)
{
	memset(a->ordlis, 0, sizeof(a->ordlis));
	a->ordersize = 0;
	a->inval = 0;
	a->tno = 0;
	a->bill = 0;
	a->flag = 0;
	a->ter_flag = 0;
	a->man_flag = 0;
	a->admin_flag = 0;
}

int main()
{

	int total_tables;
	printf("Enter the total number of tables at the hotel:");
	scanf("%d", &total_tables);
	
	FILE *file;

	// Open file for writing
	file = fopen("earnings.txt", "w");

	// Check if file is opened successfully
	if (file == NULL)
	{
		printf("Unable to open file.\n");
		return 1; // Exit with an error code
	}
	
	int total_earnings;
	int waiter_wages;
	int profit;

	
	for (int i = 0; i < total_tables; i++)
	{
		
		key_t key = ftok("dummy.txt", 'A');
		int shm_id = shmget(key, sizeof(struct order), 0644 | IPC_CREAT);
		struct order *manOrder = (struct order *)shmat(shm_id, NULL, 0);
		while (manOrder->man_flag != 1)
		{
			
			sleep(1);
		}

		fprintf(file, "Earning from Table %d: %d INR\n", manOrder->tno, manOrder->bill);
		total_earnings += manOrder->bill;
		manOrder->man_flag = 0;
		shmdt((void *)manOrder);
	}
	
	waiter_wages = (0.4 * total_earnings);
	profit = total_earnings - waiter_wages;

	key_t key = ftok("dummy.txt", 'B');
	int shm_id = shmget(key, sizeof(struct order), 0644 | IPC_CREAT);

	struct order *adminclose = (struct order *)shmat(shm_id, NULL, 0);

	while (1)
	{
		sleep(1);
		if (adminclose->admin_flag==1)
		break;
	}
	printf("Total Earnings of Hotel: %d INR\n", total_earnings);
	printf("Total Wages of Waiters: %d INR\n", waiter_wages);
	printf("Total Profit: %d INR\n", profit);
	printf("Thank you for visiting the Hotel!\n");

//writing profit statement into file
	fprintf(file, "Total Earnings of Hotel: %d INR\n", total_earnings);
	fprintf(file, "Total Wages of Waiters: %d INR\n", waiter_wages);
	fprintf(file, "Total Profit: %d INR\n", profit);
	fclose(file);
	return 0;
}
