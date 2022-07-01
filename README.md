# Booking-Theater-Seats-POSIX-Threads
Project for Operating Systems Course for Athens University Of Economics and Business: C language algorithm, using POSIX threads (pthreads) to book theater seats for multiple customers.

In this project, each customer first closes the seats he/she wants, then pays for them with a credit card and, finally, the money are transferred to the company's account. There is a large number of clients served by a limited number of service points, so our program must implement mutual blocking (with mutexes) and synchronization (with condition variables).

The seat purchase system has a bank account, a call center with Ntel operators who serve the customers and Ncash cashiers who make the payments. The theater seats have a rectangular layout, with each row having Nseat seats and the rows being divided into two zones: front NzoneA rows cost CzoneA per seat and rear NzoneB rows cost CzoneB per seat. The first customer calls at time 0, and each subsequent customer calls after a random integer number of seconds in the interval [treslow, treshigh]. 
When all the attendants are busy, the customer waits for the next available attendant. When a customer connects to an operator, it randomly selects a zone with probability PzoneA for the front rows, (1-PzoneA) for the back rows, and a random integer number of tickets in the interval [Nseatlow, Nseathigh]. The operator takes a random integer number of seconds in the interval [tseatlow,tseathigh] to test whether there are enough consecutive seats in any order in that zone. If there are no contiguous positions, the client receives an error message and completes the call. However, if there are consecutive seats, they are committed to the theater plan, their total cost is calculated, and the client proceeds with the payment by credit card. When all cashiers are busy, the customer waits for the next available cashier.When the customer connects to the cashier, the cashier takes a random integer number of seconds in the interval [tcashlow,tcashhigh] to try to make the payment. With Pcardsuccess probability the payment is accepted, the customer is charged the appropriate cost, the money is transferred to the company account and the customer is informed of the total cost and their seats in the selected zone. If the payment fails, the seats are returned to the theater plan by the cashier and the customer receives an error message and ends the call. When Ncust processes clients, the system prints its results.

System's printing results:
-
For each customer that is printed on the screen one of the following messages, depending on how their booking ended up, which will start with the customer number:

• Booking completed successfully. Positions are in zone _, series _, number <_, _, …> and the cost of the transaction is <_>.

• The reservation failed because there are no available seats.

• The booking failed because the credit card transaction was not accepted. 

The order of the lines will be random, but the lines must not be intertwined. At the end of the run, the system will print the following:

• The seating plan, e.g. Zone _ / Row _ / Position _ / Customer _, Zone _ / Row _ / Position _ / Customer _, ...

• Total revenue.
  
• The percentage of transactions that are completed in each of the above three ways.

• The average customer wait time (from the time the customer shows up until they speak to the operator and, if the transaction proceeds to payment, from the time the operator finishes until the cashier takes over).

• The average customer service time (from the moment the customer shows up, until the booking is completed or failed).

