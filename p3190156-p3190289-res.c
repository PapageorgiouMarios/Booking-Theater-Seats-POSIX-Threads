#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "p3190156-p3190289-res.h"

//---------Arguments for main-----------------
int number_of_customers; //first argument 
unsigned int seed; //second argument 
//--------------------------------------------

unsigned int sleep(unsigned int seconds); //sleep method for threads

//------------------------------------------------Variables------------------------------------------------------------
unsigned int total_revenue = 0; // Total revenue, sum of all successful transactions

int available_cashiers = N_CASH; // Number of all available cashiers
int available_operators = N_TEL; // Number of all available operators

//------------------------------Variables to calculate average times-------------------
float average_waiting_time = 0; //average time of customer's waiting time printed at the end of all transactions
float average_transaction_time = 0; //average time for all transactions printed at the end of all transactions
double sum_waiting_time = 0; // Total waiting time for all customers

int number_of_transactions = 0; // Number of transactions
int sum_transaction_time = 0; // Total transactions time
//--------------------------------------------------------------------------------------

//------------------------------------Variables for percentage--------------------------
int successful_transactions = 0; //number of all completed transactions
int failed_due_to_unavailble_seats = 0; //number of all transactions due to unavailable seats
int failed_due_to_payment = 0; //number of all transaction if payment failed
//--------------------------------------------------------------------------------------


//----------------------------------------------Theater seats--------------------------------------------------------------
int theater_seats[N_SEAT * (N_ZONE_A + N_ZONE_B)] = { 0 }; // Array of all theater seats (A rows + B rows)* one_row_seats, values inside are 0 (available) or 3 (taken)
int available_seats = N_SEAT * (N_ZONE_A + N_ZONE_B); // All seats are available in the beginning
//---------------------------------------------------------------------------------------------------------------------

//--------------------------Mutexes----------------------------------------
pthread_mutex_t mutex_operators; //used for telephones
pthread_mutex_t mutex_seats; //used for seats
pthread_mutex_t mutex_payment; //used for payment
pthread_mutex_t mutex_cashiers; //used for cashiers
pthread_mutex_t mutex_screen; //used to make sure that one value from one global variable increases from all running threads
//-------------------------------------------------------------------------

//--------------------------Condition variables----------------------------
pthread_cond_t condition_operators; //condition variable for the 3 operators
pthread_cond_t condition_cashiers; //condition variable for the 2 cashiers
//-------------------------------------------------------------------------


//---------------------Mutex Initialization, Destruction, Lock, Unlock------------------------
void mutex_operation(pthread_mutex_t* mutex, int operation) 
{
    int rc; 

    if (operation == INITIALIZATION) 
    {
        rc = pthread_mutex_init(mutex, NULL);

        if (rc != 0) 
        {
            printf("\nError...Mutex Initialization failed\n");
            exit(-1);
        }
    }
    else if (operation == DESTRUCTION) 
    {
        rc = pthread_mutex_destroy(mutex);

        if (rc != 0) 
        {
            printf("\nError...Mutex Destruction failed\n");
            exit(-1);
        }
    }
    else if (operation == LOCK) 
    {
        rc = pthread_mutex_lock(mutex);

        if (rc != 0) 
        {
            printf("\nError...Mutex Lock failed\n");
            pthread_exit(&rc);
        }
    }
    else if (operation == UNLOCK) 
    {
        rc = pthread_mutex_unlock(mutex);

        if (rc != 0) 
        {
            printf("\nError...Mutex Unlock failed\n");
            pthread_exit(&rc);
        }
    }
}
//-------------------------------------------------------------------------------------------


//----------------------------Condition Variable Initialization, Destruction--------------------------
void condition_operation(pthread_cond_t* condition, int operation) 
{ 

    int rc;
    if (operation == INITIALIZATION) 
    {
        rc = pthread_cond_init(condition, NULL);

        if (rc != 0) 
        {
            printf("\nError..Condition Variable Initialization failed\n");
            exit(-1);
        }
    }
    else if (operation == DESTRUCTION) 
    {
        rc = pthread_cond_destroy(condition);

        if (rc != 0) 
        {
            printf("\nError..Condition Variable Destruction failed\n");
            exit(-1);
        }
    }

}
//-------------------------------------------------------------------------------------------------------


//--------------------------Random number Generator-------------------------------------------------
int generate_probabilty(float percentage) 
{  
    float generated_probability;

    generated_probability = rand_r(&seed) % (100 + 1);

    if(generated_probability < (1 - percentage))
    {
        return FAIL;
    }

    else
    {
        return SUCCESS;
    }
         
}
//--------------------------------------------------------------------------------------------------


//-------------------------------------Operators----------------------------------------------------
void operators(CUSTOMER_REQUEST* customer_transaction, int operation) 
{  
    if (operation == LOCK) 
    {
        mutex_operation(&mutex_operators, LOCK);
 
        while (1) 
        {
            if (available_operators > 0) 
            {
                 
                available_operators--;
                (*customer_transaction).transaction_number = number_of_transactions++;
                break;
            }
            else 
            {
                pthread_cond_wait(&condition_operators, &mutex_operators);
            }
        }
        mutex_operation(&mutex_operators, UNLOCK);

    }
    else if (operation == UNLOCK) 
    {
        mutex_operation(&mutex_operators, LOCK);
        available_operators++;
        mutex_operation(&mutex_operators, UNLOCK);
        pthread_cond_broadcast(&condition_operators);
    }

}
//--------------------------------------------------------------------------------------------------


//----------------------------------------Cashiers--------------------------------------------------
void cashiers(int operation) 
{  
    if (operation == LOCK) 
    {
        mutex_operation(&mutex_cashiers, LOCK);

        while (1) 
        {
            if (available_cashiers > 0) 
            {
                available_cashiers--;
                break;
            }
            else 
            {
                pthread_cond_wait(&condition_cashiers, &mutex_cashiers);
            }
        }
        mutex_operation(&mutex_cashiers, UNLOCK);
    }
    else if (operation == UNLOCK) 
    {
        mutex_operation(&mutex_cashiers, LOCK);
        available_cashiers++;
        mutex_operation(&mutex_cashiers, UNLOCK);
        pthread_cond_broadcast(&condition_cashiers);
    }
}
//---------------------------------------------------------------------------------------------------


//-----------------------------------------------------Ask customer for number of seats--------------------------------------------------------------
int ask_seats(int customer_id) 
{ 
    int requested_seats;
 
    printf("\nCustomer <%d>:: Pick a seat from %d to %d. \nHow many seats do you want for the show? ", customer_id, N_SEATLOW, N_SEATHIGH);
  
    requested_seats = rand_r(&seed) % (N_SEATHIGH + 1);

    if (requested_seats < N_SEATLOW)
    {
        requested_seats = N_SEATLOW;
    }
         
    return requested_seats;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------Ask customer which of the two zones they wish--------------------------------------------------
int ask_zone(int customer_id) 
{  
    int prob;

    printf("\nCustomer <%d>:: Do you want zone A or zone B? ", customer_id);    

    prob = rand_r(&seed) % (100 + 1);

    if (prob < P_ZONE_A * 100) 
    {
        printf("\nCustomer <%d>: A", customer_id);

        return ZONE_A;
    }
    else
    {
        printf("\nCustomer <%d>: B", customer_id);
    
        return ZONE_B;
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------Search seats available for the customer------------------------------------------------
int search_seats_for_customer(int clientID, CUSTOMER_REQUEST* info) 
{

    int searching_time_for_seats = rand_r(&seed) % (T_SEATHIGH + 1);

    if (searching_time_for_seats < T_SEATLOW)
    {
        searching_time_for_seats = T_SEATLOW;
    }

    sleep(searching_time_for_seats);

    int i;
    int j;
    int start;
    int end;
    int k = 0;

    if (info->requested_zone == ZONE_A)
    {
        start = 0;
        end = N_ZONE_A * N_SEAT;
    }
    else if (info->requested_zone == ZONE_B)
    {
        start = N_ZONE_A * N_SEAT;
        end = (N_ZONE_A + N_ZONE_B) * N_SEAT;
    }
     
    for (i = start; i < end - info->requested_seats + 1; i = j + 1)
    {
        j = i;
        mutex_operation(&mutex_seats, LOCK);

        if ((theater_seats[i] == AVAILABLE) && (N_SEAT - i % N_SEAT >= info->requested_seats)) 
        {
            for (j = i; j < i + info->requested_seats; j++)
            {
                if (theater_seats[j] != AVAILABLE) 
                {
                    while (theater_seats[j++] != AVAILABLE) {}
                    break;
                }

                theater_seats[j] = TAKEN;
                info->seats[k++] = j;
            }

            if (k != info->requested_seats) 
            {
                for (j = i; j < i + k - 1; j++)
                {
                    theater_seats[j] = AVAILABLE;
                }
            }
        }

        mutex_operation(&mutex_seats, UNLOCK);
        if (k == info->requested_seats)
            break;
    }

    if (k < info->requested_seats)
    {
        return FAIL;
    }
    else
    {
        return SUCCESS;
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------Pay for the seats----------------------------------------------------------------------
int payment_probability_generator(int price) 
{
    int res = generate_probabilty(P_CARDSUCCESS);

    if (res == SUCCESS) 
    {
        mutex_operation(&mutex_payment, LOCK);
        total_revenue += price;
        mutex_operation(&mutex_payment, UNLOCK);

        return SUCCESS;
    }
    return FAIL;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------After the seats are paid their state must be change from AVAILABLE to TAKEN--------------------
void change_to_available(int availability, CUSTOMER_REQUEST* info) 
{
    mutex_operation(&mutex_seats, LOCK);

    for (int i = 0; i < info->requested_seats; i++)
    {
        theater_seats[info->seats[i]] = availability;
    }
    if (availability != AVAILABLE) 
    {
        available_seats -= info->requested_seats;

    }
    mutex_operation(&mutex_seats, UNLOCK);
}
//------------------------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------The operator is searching for seats available for customer---------------------
int find_seats(int customer_id, CUSTOMER_REQUEST* info) 
{
    info->requested_seats = ask_seats(customer_id);
    info->requested_zone = ask_zone(customer_id);

 
    printf("\nCustomer <%d>:: Number of desired seats: %d. Procceed to checking...", customer_id, info->requested_seats);
 
    //check seats availability in theater

    int checking = search_seats_for_customer(customer_id, info);

    if (checking == SUCCESS) 
    {
         
        printf("\nCustomer <%d>:: Seats found:", customer_id);
       
        for (int i = 0; i < info->requested_seats; i++)
        {
             
            printf("\nCustomer <%d>:: No. %d. ", customer_id, info->seats[i] + 1);
       
        }

        printf("\nCustomer <%d>:: Found your seats. Procceed to ticket payment...", customer_id);
       
    }
    else if (checking == FAIL)
    {
        mutex_operation(&mutex_screen, LOCK);

        printf("\nCustomer <%d>:: Unfortunately, there are no seats available...", customer_id);

        failed_due_to_unavailble_seats++;

        mutex_operation(&mutex_screen, UNLOCK);
        
        return FAIL;
    }

    return SUCCESS;
}
//-------------------------------------------------------------------------------------------------------------------------------------------

char print_zone(CUSTOMER_REQUEST* info) {

    char zone = ' ';

    if (info->requested_zone == 0)
    {
        zone = 'A';
    }
         
    else if (info->requested_zone == 1)
    {
        zone = 'B';
    }

    return zone;
        
}

//-----------------------------------------------------------------Payment procedure-------------------------------------------------------------------
void payment(int customer_id, CUSTOMER_REQUEST* info) {

    struct timespec req_start;
    struct timespec req_end;

    clock_gettime(CLOCK_REALTIME, &req_start);

    int realize_payment_time = rand_r(&seed) % (T_CASHHIGH + 1);

    if (realize_payment_time < T_CASHLOW)
    {
        realize_payment_time = T_CASHLOW;
    }

    sleep(realize_payment_time);

    cashiers(LOCK);

    clock_gettime(CLOCK_REALTIME, &req_end);

    int cashier_waiting_time = req_end.tv_sec - req_start.tv_sec;

    info->waited += cashier_waiting_time;
    
    sum_waiting_time += info->waited;
    
    if (info->requested_zone == ZONE_A) 
    {
        info->cost = info->requested_seats * C_ZONE_A;
    }
    else if (info->requested_zone == ZONE_B) 
    {
        info->cost = info->requested_seats * C_ZONE_B;
    }

    printf("\nCustomer <%d>:: Total cost for seats: %d", customer_id, info->cost);
    
    int payment_probability = payment_probability_generator(info->cost);

    if (payment_probability == SUCCESS) 
    {
        change_to_available(customer_id, info);

        info->state = 1;
       
        if (info->requested_zone == 0)
        {
           
            printf("\nCustomer <%d>:: Your seats are at zone A", customer_id);
          
        }
        else if (info->requested_zone == 1)
        {
            
            printf("\nCustomer <%d>:: Your seats are at zone B", customer_id);
            
        }
 
        printf("\nCustomer <%d>:: Your row is: %d", customer_id, (int)info->seats[0]/10 + 1);
   

        printf("\nCustomer <%d>:: Total Cost: %d", customer_id, info->cost);
  
        for (int i = 0; i < info->requested_seats; i++)
        {
 
            printf("\nCustomer <%d>:: Booked Seat: %d", customer_id, info->seats[i] + 1);
 
        }
 
    }
    else 
    {
        change_to_available(AVAILABLE, info);
        printf("\nCustomer <%d>:: Payment failed. Your booking is canceled", customer_id);
        info->state = 3;
    
    }
    cashiers(UNLOCK);

}
//----------------------------------------------------------------------------------------------------------------------------------------------------------

void* transaction(void* customer_id) 
{
    int search_seats;
    int* cid = (int*)customer_id;
    CUSTOMER_REQUEST info;

    struct timespec req_start;
    struct timespec req_end;
    struct timespec trans_end;

    printf("\nCustomer <%d> is calling. \n", *cid);

    clock_gettime(CLOCK_REALTIME, &req_start);

    operators(&info, LOCK); 

    clock_gettime(CLOCK_REALTIME, &req_end);

    int operator_waiting_time = req_end.tv_sec - req_start.tv_sec;

    info.waited += operator_waiting_time;

    sum_waiting_time += info.waited;

    search_seats = FAIL;
    if (available_seats == 0) 
    {
        printf("\nCustomer <%d>:: There are no seats available for booking", *cid);
        info.state = 2;
    }
    else 
    {
        search_seats = find_seats(*cid, &info);
    }

    operators(&info, UNLOCK);

    if (search_seats == SUCCESS) 
    {
        payment(*cid, &info);

    }

    clock_gettime(0, &trans_end);

    float print_time = trans_end.tv_sec - req_start.tv_sec;

    mutex_operation(&mutex_screen, LOCK);
    sum_transaction_time += trans_end.tv_sec - req_start.tv_sec;
    mutex_operation(&mutex_screen, UNLOCK);


    if(info.state == 1)
    {
        mutex_operation(&mutex_screen, LOCK);
        successful_transactions = successful_transactions + 1;
        mutex_operation(&mutex_screen, UNLOCK);
    }
    else if(info.state == 2)
    {
        mutex_operation(&mutex_screen, LOCK);
        failed_due_to_unavailble_seats = failed_due_to_unavailble_seats + 1;
        mutex_operation(&mutex_screen, UNLOCK);
    }
    else if(info.state == 3)
    {
        mutex_operation(&mutex_screen, LOCK);
        failed_due_to_payment = failed_due_to_payment + 1;
        mutex_operation(&mutex_screen, UNLOCK);
    }

    printf("\nCustomer <%d>:: Total transaction time: %lf seconds", *cid, print_time);
    printf("\nCustomer <%d>:: Total waiting time: %lf seconds", *cid, info.waited);
    
    pthread_exit(NULL);
}

void arguments_check(int argc, char* argv[]) 
{
    if (argc != 3) //3 because ./a.out 1, number of customers 2, seed 3
    {
        printf("\nError...Give 2 arguments.\n");
        exit(-1);
    }
    number_of_customers = atoi(argv[1]);
    seed = abs(atoi(argv[2]));

    if (number_of_customers <= 0) 
    {
        printf("\nError...The number of customers must be positive.\n");
        exit(-1);
    }
}

int main(int argc, char* argv[]) 
{

    arguments_check(argc, argv); //first we check if all arguments given are correct

    printf("Total theater seats: %d", N_SEAT * (N_ZONE_A + N_ZONE_B)); //seats must be 10*10 + 20*10 = 100 + 200 = 300

//-----------------------INITIALIZE ALL MUTEXES-------------------------------
    mutex_operation(&mutex_screen, INITIALIZATION);
    mutex_operation(&mutex_operators, INITIALIZATION);
    mutex_operation(&mutex_cashiers, INITIALIZATION);
    mutex_operation(&mutex_seats, INITIALIZATION);
    mutex_operation(&mutex_payment, INITIALIZATION);
//----------------------------------------------------------------------------

//----------------------INITIALIZE ALL CONDITION VARIABLES----------------------
    condition_operation(&condition_operators, INITIALIZATION);
    condition_operation(&condition_cashiers, INITIALIZATION);
//------------------------------------------------------------------------------
 
    int customer_id[number_of_customers]; //array of all customers' ids

    for (int i = 0; i < number_of_customers; i++) 
    {
        customer_id[i] = i + 1; //we want all customers have proper ids from 1 to number_of_customers
    }

    int rc;
    pthread_t running_threads[number_of_customers]; //all customer threads

    pthread_t* customers_threads;

    customers_threads = malloc(number_of_customers * sizeof(pthread_t)); //we have to be careful for the memory we use

    if (customers_threads == NULL)
    {
        printf("\nNo memory../\n");
        return -1;
    }

    for (int i = 0; i < number_of_customers; i++) 
    {
        if(i == 0) //we want to create the first customer with id number 1
        {
            rc = pthread_create(&running_threads[i], NULL, transaction, &customer_id[i]);

            if (rc != 0)
            {
                printf("\nFirst thread creation failed!\n");
                exit(-1);
            }
        }
        else //the rest will follow in order with random showing-up time
        {
            int next_customer_generation_time = rand_r(&seed) % (T_RESHIGH + 1 - T_RESLOW) + T_RESLOW;

            sleep(next_customer_generation_time);

            rc = pthread_create(&running_threads[i], NULL, transaction, &customer_id[i]);

            if (rc != 0)
            {
                printf("\nThread creation failed!\n");
                exit(-1);
            }

        }
         
    }

    void* stat; //stat used for joining method

    for (int i = 0; i < number_of_customers; i++) 
    {
        rc = pthread_join(running_threads[i], &stat);

        if (rc != 0) 
        {
            printf("\nThread join failed!\n");
            exit(-1);
        }
    }

    printf("\n\n--Plan of seats:");

    for (int i = 0; i < N_SEAT * (N_ZONE_A + N_ZONE_B); i++)
    {
        if (i < N_SEAT * N_ZONE_A) 
        {
            printf("\n--Zone A / ");

            if (theater_seats[i] != AVAILABLE)
            {

                printf("Row %d / Seat %d / Customer %d", (i / 10) + 1, i + 1, theater_seats[i]);

            }
            else
            {

                printf("Row %d / Seat %d / Empty", (i / 10) + 1, i + 1);

            }
        }

            
        else if (i < N_SEAT * (N_ZONE_A + N_ZONE_B)) 
        {
            printf("\n--Zone B / ");

            if (theater_seats[i] != AVAILABLE)
            {

                printf("Row %d / Seat %d / Customer %d", (i / 10) + 1 - 10, i + 1, theater_seats[i]);

            }
            else
            {

                printf("Row %d / Seat %d / Empty", (i / 10) + 1 - 10, i + 1);

            }
        }

    }

    mutex_operation(&mutex_screen, LOCK);
    printf("\n\nTotal revenue: %d$!", total_revenue);

    printf("\nTotal transactions: %d \n", number_of_transactions);
    printf("\nTotal successful transactions: %d", successful_transactions);
    printf("\nTotal failed transactions due to unavailable seats: %d", failed_due_to_unavailble_seats);
    printf("\nTotal failed transactions due to failed payment: %d", failed_due_to_payment);

    average_waiting_time = sum_waiting_time / (successful_transactions + failed_due_to_unavailble_seats + failed_due_to_payment);
    printf("\n\nAverage waiting time: %lf seconds", average_waiting_time);

    average_transaction_time = sum_transaction_time / (successful_transactions + failed_due_to_unavailble_seats + failed_due_to_payment);
    printf("\nAverage transactions time: %lf seconds", average_transaction_time);

    float percentage1 = (successful_transactions*100)/ (successful_transactions + failed_due_to_unavailble_seats + failed_due_to_payment);
    printf("\n\nPercentage of all successful transactions: %d%%", (int) percentage1);

    float percentage2 = (failed_due_to_unavailble_seats*100)/ (successful_transactions + failed_due_to_unavailble_seats + failed_due_to_payment);
    printf("\nPercentage of all failed transactions due to unavailable seats: %d%%", (int) percentage2);

    float percentage3 = (failed_due_to_payment*100)/ (successful_transactions + failed_due_to_unavailble_seats + failed_due_to_payment);
    printf("\nPercentage of all  failed transactions due to unsuccessful payment: %d%%", (int) percentage3);
    mutex_operation(&mutex_screen, UNLOCK);
     
    free(customers_threads);

    mutex_operation(&mutex_operators, DESTRUCTION);
    mutex_operation(&mutex_seats, DESTRUCTION);
    mutex_operation(&mutex_payment, DESTRUCTION);
    mutex_operation(&mutex_cashiers, DESTRUCTION);

    condition_operation(&condition_operators, DESTRUCTION);
    condition_operation(&condition_cashiers, DESTRUCTION);

    mutex_operation(&mutex_screen, DESTRUCTION);
     
    return 0;
}