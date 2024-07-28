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
	key_t key = ftok("dummy.txt", 'B');
	int shm_id = shmget(key, sizeof(struct order), 0644 | IPC_CREAT);

	struct order *sharedOrder = (struct order *)shmat(shm_id, NULL, 0);

	orderInitializer(sharedOrder);
	while (1)
	{
		char input;
		printf("Do you want to close the hotel? Enter Y for Yes and N for No:");
		fflush(stdout);
		scanf("%c",&input);

		if (input == 'y')
		{
			sharedOrder->admin_flag = 1;
			break;
		}
		
	}
	return 0;
}
