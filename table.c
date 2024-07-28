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
    a->ordersize=0;

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

    FILE *file;

    // Open the file in read mode
    file = fopen("menu.txt", "r");

    // Check if the file opened successfully
    if (file == NULL)
    {
        fprintf(stderr, "Unable to open the file.\n");
        return 1; // Return an error code
    }

    int tableNumber;
    int numCustomers;

    printf("Enter Table Number: ");
    scanf("%d", &tableNumber);

    key_t key = ftok("dummy.txt", tableNumber);
    int shm_id = shmget(key, sizeof(struct order), 0644 | IPC_CREAT);

    struct order *sharedOrder = (struct order *)shmat(shm_id, NULL, 0);

    orderInitializer(sharedOrder);

    sharedOrder->tno = tableNumber;

    sharedOrder->ter_flag = 0;

    while (1)
    {
        
        printf("Enter Number of Customers at Table (maximum no. of customers can be 5): ");
        scanf("%d", &numCustomers);
        if (numCustomers == -1) // will terminate table process if -1 is entered
        {
            sharedOrder->ter_flag = 1;
            break;
        }

        while (1)
        {
            orderInitializer(sharedOrder);

            sharedOrder->tno = tableNumber;

            sharedOrder->ter_flag = 0;

            
            //---------------------------------DISPLAYING MENU-------------------------------------------------

            // Read and print the contents of the file
            char ch;
            fseek(file, 0, SEEK_SET);
            while ((ch = fgetc(file)) != EOF)
            {
                printf("%c",ch);
                fflush(stdout);
            }

            // Close the file

            //---------------------------------------------------------------------------------------------------

            pid_t pids[numCustomers];
            int pipes[2 * numCustomers];
            int i;
            for (i = 0; i < numCustomers; i++)
            {
                if (pipe(pipes + i * 2) == -1)
                {
                    perror("pipe");
                    return 1;
                }
            }

            pid_t childPid;
            int status;

            int j = 0;
            for (i = 0; i < numCustomers; i++)
            {

                childPid = fork();

                if (childPid < 0)
                {
                    fprintf(stderr, "Fork Failed");
                    return 1;
                }
                else if (childPid == 0)
                {
                    int childOrder[10];
                    close(pipes[i * 2]);
                    int choice = 0;

                    printf("%d th customer: \n\n", i + 1);
                    fflush(stdout);
                    int k = 0;
                    while (choice != -1)
                    {

                        printf("Enter the serial number(s) for customer %d of the item(s) to order from the menu. Enter -1 when done:\n", i+1);
                        fflush(stdout);
                        scanf("%d", &choice);

                        childOrder[k] = choice;
                        
                        k++;

                        if (choice == -1)
                        {
                            write(pipes[(i * 2) + 1], &childOrder, sizeof(childOrder));
                            close(pipes[(i * 2) + 1]);
                        }
                    }

                    // return 0;
                    exit(i + 1);
                }

                else
                { // parent process

                    int custOrder[10];
                    read(pipes[i * 2], &custOrder, sizeof(struct order));
                    int t = 0;
                    while (custOrder[t] != -1)
                    {

                        sharedOrder->ordlis[j] = custOrder[t];
                        t++;
                        j++;
                    }

                    sharedOrder->ordlis[j] = -1;
                    j++;
                    wait(NULL);
                    close(pipes[(i * 2) + 1]);
                }
            }

            for (i = 0; i < numCustomers; i++)
            {

                close(pipes[i * 2]);
            }
            

            sharedOrder->ordersize=j;
            sharedOrder->flag = 1;

            

            while (sharedOrder->flag != 0)
            {
                sleep(1);
            }
            // case of invalid order
            if (sharedOrder->inval == 1)
            {
                printf("YOU HAVE ENTERED INVALID ORDER!!! Please enter order again. \n\n\n\n\n");
                fflush(stdout);
                continue;
            }
            else
            {
                break;
            }
        }
        printf("The total bill amount is %d INR.\n\n\n\n\n", sharedOrder->bill);
        fflush(stdout); 
    }

    fclose(file);

    
    shmdt((void *)sharedOrder);

    if (shmctl(shm_id, IPC_RMID, 0) == -1)
    { // don't delete before reading is done

        perror("Error in shmctl\n");
        return 1;
    }

    return 0;
}
