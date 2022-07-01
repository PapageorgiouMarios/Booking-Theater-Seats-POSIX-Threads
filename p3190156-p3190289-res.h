#pragma once
#include <pthread.h>

#define N_SEAT 10 // number of seats on each row

#define N_ZONE_A 10 // number of rows of zone A
#define N_ZONE_B 20 // number of rows of zone B

 
#define C_ZONE_A 30 // cost of one zone A seat
#define C_ZONE_B 20 // cost of one zone B seat

#define P_ZONE_A 0.3 //chance of choosing zone A
#define P_ZONE_B 0.7 //chance of choosing zone B

#define N_TEL 3 //number of operators(telephones)
#define N_CASH 2 //number of cashiers

#define N_SEATLOW 1 //lowest number of chosen seats
#define N_SEATHIGH 5 //maximum number of chosen seats
#define T_SEATLOW 5 //lowest number of seconds to search seats
#define T_SEATHIGH 13 //maximum number of seaconds to search seats
#define T_CASHLOW 4 // lowest number of seconds to realize payment
#define T_CASHHIGH 8 // maximum number of seconds to realize payment

#define T_RESLOW 1 //lowest number of seconds to create new customer
#define T_RESHIGH 5 //maximum number of seconds to create new customer

#define P_CARDSUCCESS 0.9 //percentage for successful payment 

//---------------MUTEX OPERATIONS---------------
#define INITIALIZATION 0
#define DESTRUCTION 1
#define LOCK 2
#define UNLOCK 3
//----------------------------------------------

#define ZONE_A 0 //variable to represent zone A
#define ZONE_B 1 //variable to represent zone B

#define AVAILABLE 0 //variable to represent the seat is available
#define TAKEN 3 //variable to represent the seat is booked

#define SUCCESS 0 //variable to represent success
#define FAIL 1 //variable to represent failure

//----------------------CUSTOMER STRUCTURE---------------------
typedef struct customer_request 
{
    int transaction_number; //Number of transaction
    int seats[N_SEATHIGH]; // Array of (maximum) 5 seat the customer picks
    int requested_seats; // How many seats the customer asked
    int requested_zone; // What zone does the customer picks
    int cost; // What price the customer pays
    int state; // State to represent the case the customer's request is accepted, rejected due to seats or rejected due to unsuccessful payment
    float waited; //How many seconds did the customer wait in total 

}CUSTOMER_REQUEST;
//-------------------------------------------------------------

void mutex_operation(pthread_mutex_t* mutex, int operation); //method to call whatever mutex operation we like

void condition_operation(pthread_cond_t* cond, int operation); //method to call whatever condition variable operation we like

int generate_probabilty(float percent); //generator for random probabilities

void operators(CUSTOMER_REQUEST* info, int operation); //method to call mutex operations for all telephones

void cashiers(int operation); //method to call mutex operations for all cashiers

int ask_seats(int clientID); //method to generate random number of desired seats

int ask_zone(int clientID); //method to generate customer's desired zone

int search_seats_for_customer(int clientID, CUSTOMER_REQUEST* info); //method to search available seats for customers

int payment_probability_generator(int amount); //method to generate payment probability and realize or reject payment

void change_to_available(int new_state, CUSTOMER_REQUEST* info); //method to change a seats availability

int find_seats(int clientID, CUSTOMER_REQUEST* info); //method to find available seats for customers

char print_zone(CUSTOMER_REQUEST* info); //method to print the chosen zone from the struct

void payment(int clientID, CUSTOMER_REQUEST* info); //method for payment session

void* transaction(void* clientID); //method for the total transaction(used by all customer threads)

void arguments_check(int argc, char* argv[]); //method to check all arguments for the a.out and the sh file
