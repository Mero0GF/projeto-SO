
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h> 

sem_t sem_ContaA;
sem_t sem_ContaB;
bool clienteA = false,clienteB = false;


int GetMoneyA(){
  FILE* fptrA;
  fptrA = fopen("saldoA.txt","r");
  char fileA[100];
  fgets(fileA,100,fptrA);
  int moneyA = atoi(fileA);
  fclose(fptrA);
  return moneyA;
}

int GetMoneyB(){
  FILE* fptrB;
  fptrB = fopen("saldoB.txt","r");
  char fileB[100];
  fgets(fileB,100,fptrB);
  int moneyB = atoi(fileB);
  fclose(fptrB);
  return moneyB;
}

void WriteToFileB(int money){
  FILE* fptrB;
  fptrB = fopen("saldoB.txt","w");
  char *new_balance;
  fprintf(fptrB,"%d",money);
  fclose(fptrB);
}

void WriteToFileA(int money){
  FILE* fptrA;
  fptrA = fopen("saldoA.txt","w");
  char *new_balance;
  fprintf(fptrA,"%d",money);
  fclose(fptrA);
}


void *transactionA(){
  srand(time(NULL));
  for(int i = 0;i < 3;i++){
    int tempo_espera = rand() % 5;
    clienteA = true;
    if (clienteB == false){
      sleep(tempo_espera);
      sem_wait(&sem_ContaA);
      int moneyA = GetMoneyA();
      sem_wait(&sem_ContaB);
      int moneyB = GetMoneyB();
      int transaction = rand() % 1000;
      moneyB = moneyB + transaction;
      printf("\n/////////////////////////////////////////////////////////////\n");
      printf("Uma transacao foi feita de A para B em um valor de %d\nSaldo atual do Cliente A: %d\nSaldo atual do Cliente B: %d\n",transaction, moneyA-transaction, moneyB);
      printf("/////////////////////////////////////////////////////////////\n");
      WriteToFileB(moneyB);
      WriteToFileA(moneyA-transaction);
      sem_post(&sem_ContaA);
      sem_post(&sem_ContaB);
    }
    else{
      i--;
    }
    clienteA = false;
  }
}

void *transactionB(){
  srand(time(NULL));
  for(int i = 0;i < 3;i++){
    int tempo_espera = rand() % 5;
    clienteB = true;
    if (clienteA == false){
      sleep(tempo_espera);
      sem_wait(&sem_ContaA);
      int moneyA = GetMoneyA();
      sem_wait(&sem_ContaB);
      int moneyB = GetMoneyB();
      int transaction = rand() % 1000;
      moneyA = moneyA + transaction;
      printf("\n/////////////////////////////////////////////////////////////\n");
      printf("Uma transacao foi feita de B para A em um valor de %d\nSaldo atual do Cliente A: %d\nSaldo atual do Cliente B: %d\n",transaction, moneyA, moneyB-transaction);
      printf("/////////////////////////////////////////////////////////////\n");
      WriteToFileA(moneyA);
      WriteToFileB(moneyB-transaction);
      sem_post(&sem_ContaA);
      sem_post(&sem_ContaB);
    }
    else{
      i--;
    }
    clienteB = false;
  }
}

int main(void) {
  pthread_t pid1, pid2;
  pthread_create(&pid1,NULL,transactionA,NULL);
  pthread_create(&pid2,NULL,transactionB,NULL);

  sem_init(&sem_ContaA, 0, 1); 
  sem_init(&sem_ContaB, 0, 1); 
  pthread_join(pid1, NULL);
  pthread_join(pid2, NULL);
  
  pthread_exit(NULL);
}