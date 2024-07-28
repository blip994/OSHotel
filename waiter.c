#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define MAX_ITEMS 100

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
	int MAX_SIZE = 50;

	FILE *fp;
	char menu;
	int ch;
	int number, count = 0;
	int numbers[MAX_ITEMS];

	//memset(numbers, 0, sizeof(numbers)); // Array to store extracted numbers

	// Open the text file in read mode
	fp = fopen("menu.txt", "r");

	// Check if file opened successfully
	if (fp == NULL)
	{
		printf("Error opening file!\n");
		return 1;
	}

	// Read characters one by one
	while ((ch = fgetc(fp)) != EOF)
	{
		// Skip non-digit characters
		if (!isdigit(ch))
		{
			continue;
		}

		// Extract the number
		number = 0;
		while (isdigit(ch))
		{
			number = number * 10 + (ch - '0');
			ch = fgetc(fp);
		}

		// Store the extracted number in the array (check for array bounds)
		if (count < MAX_ITEMS)
		{
			numbers[count++] = number;
		}
		else
		{
			printf("Warning: Maximum number of items (%d) exceeded!\n", MAX_ITEMS);
		}
	}

	int total_orders = count / 2;

	int prices[MAX_SIZE];
	memset(prices, -50, sizeof(prices));

	for (int i = 0; i < total_orders; i++)
	{
		prices[i + 1] = numbers[2 * i + 1];
	}

	// In case the File does not open
	do
	{
		menu = fgetc(fp);
		//printf("%c", menu);
	} while (menu != EOF);

	

	// above code to extract prices from menu

	// prices of the various items on menu
	// int prices[10] = {0, 20, 30, 40, 50, -50, -50, -50, -50, -50}; 

	int w_id;
	printf("Enter Waiter ID:");
	scanf("%d", &w_id);

	int totalBill = 0;

	key_t key = ftok("dummy.txt", w_id);
	int shm_id = shmget(key, sizeof(struct order), 0644 | IPC_CREAT);
	struct order *receivedOrder = (struct order *)shmat(shm_id, NULL, 0);
	receivedOrder->ter_flag = 0;

	// for current set of customers
	while (1)
	{

		while (receivedOrder->flag != 1)
		{
			if (receivedOrder->ter_flag == 1)
				break;
			sleep(1);
		}
		if (receivedOrder->ter_flag == 1)
			break;
		// verifying order and adding to bill
		int invalid_flag = 0;

		for (int i = 0; i < receivedOrder->ordersize; i++)
		{

			if (receivedOrder->ordlis[i] > 0 && receivedOrder->ordlis[i] < 10)
			{

				if (prices[receivedOrder->ordlis[i]] != -825307442)
				{

					receivedOrder->bill += prices[receivedOrder->ordlis[i]];
				}
				else
				{

					invalid_flag = 1;
					// invalid order
				}
			}
			else if (receivedOrder->ordlis[i] != -1 && receivedOrder->ordlis[i] != 0)
			{

				invalid_flag = 1;
				// invalid order
			}
		}

		if (invalid_flag == 1)
		{
			// case of invalid order
			receivedOrder->inval = 1;
			receivedOrder->flag = 0;
			receivedOrder->bill = 0;
			continue;
		}
		else
		{
			// case of valid order
			printf("Bill Amount for Table %d: %d INR \n\n", w_id, receivedOrder->bill);
			fflush(stdout);
			receivedOrder->flag = 0;
			totalBill += receivedOrder->bill;
		}
	}

	shmdt((void *)receivedOrder);
	// MANAGER PART STARTS HERE

	key_t man_key = ftok("dummy.txt", 'A');
	int man_shm_id = shmget(man_key, sizeof(struct order), 0644 | IPC_CREAT);
	struct order *sharedOrder = (struct order *)shmat(man_shm_id, NULL, 0);

	orderInitializer(sharedOrder); // initializing sharedOrder with defualt values

	sharedOrder->tno = w_id;
	sharedOrder->bill = totalBill;
	sharedOrder->man_flag = 1;

	shmdt((void *)sharedOrder);

	if (shmctl(man_shm_id, IPC_RMID, 0) == -1)

	{ // don't delete before reading is done

		perror("Error in shmctl\n");
		return 1;
	}

	return 0;
}
