# Booking-Theater-Seats-POSIX-Threads

## 🔭 Overview  
This is a project for an Operating Systems course (Athens University of Economics and Business, AUEB).  
It implements a multi-threaded seat booking system (in C) using POSIX threads (pthreads), to simulate booking theater seats by multiple concurrent customers.  
The system handles seat reservation, mutual exclusion, synchronization, and payment processing via simulated call-center operators and cashiers.

---

## 🎯 Main Goal  

- Theater simulation:  
  - Each row has `Nseat` seats.  
  - The rows are divided into two zones:  
    - Front zone: `NzoneA` rows — each seat costs `CzoneA`.  
    - Rear zone: `NzoneB` rows — each seat costs `CzoneB`.  
- Theater personnel:  
  - `Ntel` operators for seat selection.  
  - `Ncash` cashiers for payment processing.  
- Customers arrive at random times:  
  - First customer at time 0;  
  - Every other customer calls after a random number of seconds in `[treslow, treshigh]`.  
- When a customer is served by an operator:  
  - They randomly choose a zone: front with probability `PzoneA`.  
  - They request a random number of seats `[Nseatlow, Nseathigh]`.  
  - The operator takes a random number of seconds in `[tseatlow, tseathigh]` to check if there are enough **consecutive seats** in that zone.  
    - If no such block exists → the customer receives an error, call ends.  
    - If seats are available → seats are **committed** (reserved), total cost calculated, and the customer proceeds to payment.  
- Payment:  
  - If all cashiers busy, the customer waits.  
  - Cashier tries to process payment — takes random integer seconds in `[tcashlow, tcashhigh]`.  
  - Payment succeeds with probability `Pcardsuccess`.  
    - If success → seats are confirmed, money transferred to company account, customer notified (with seats and cost).  
    - If failure → committed seats are released back, customer informed of failure.  
- After processing `Ncust` customers, the system prints final results.
---

## 📤 Ouput Display:
For each customer that is printed on the screen one of the following messages, depending on how their booking ended up, which will start with the customer number:

• ✔️ Booking completed successfully. Positions are in zone _, series _, number <_, _, …> and the cost of the transaction is <_>.

• ❌ The reservation failed because there are no available seats.

• ❌ The booking failed because the credit card transaction was not accepted. 

The order of the lines will be random, but the lines must not be intertwined. At the end of the run, the system will print the following:

• 📋 The seating plan, e.g. Zone _ / Row _ / Position _ / Customer _, Zone _ / Row _ / Position _ / Customer _, ...

• 💰 Total revenue.
  
• 📊 The percentage of transactions that are completed in each of the above three ways.

• ⏱️ The average customer wait time (from the time the customer shows up until they speak to the operator and, if the transaction proceeds to payment, from the time the operator finishes until the cashier takes over).

• ⏱️ The average customer service time (from the moment the customer shows up, until the booking is completed or failed).

---
## 🐳 Docker Deployment

### 🛠️ Build the image
```
docker compose build
```

### 🧪 Run tests
```
docker compose run --rm tests
```

### ▶️ Run the main program
```
docker-compose run --rm main 
```

