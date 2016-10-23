#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <error.h>
#include <assert.h>

FILE* fileWrite(char *fileName);
int random_number(int min_num, int max_num);
void INThandler(int sig); // for the ctrl^C
void TimeHandler(int sig); // for alarm (z)

int slaveNum = 0; // the x number of slaves is global so the INThandler and TimeHandler can access it
int shareID;
#define SHMSZ sizeof(int)


int main(int argc, char *argv[])
{
  signal(SIGTERM, TimeHandler); // getting is alarm is activated
  signal(SIGQUIT,INThandler); //getting if ctrl^c called 
  signal(SIGINT, INThandler); // for funs 
  
  pid_t mypid;
  char *filename = argv[0];  // filename to write too
  int max_writes = atoi(argv[1]); //the Y value in the master
  slaveNum = atoi(argv[2]); // the x value in master (amount of slaves)
  key_t key = atoi(argv[3]); // making key for access
  shareID = atoi(argv[4]);  
  int n = atoi(argv[5]);
  
  typedef struct PeterStruct // shared memory 
  { 
     int sum;
     int turn;
     enum state { idle, want_in, in_cs } flag[n];
  }peter;
  
 /*if((shareID = shmget(key, SHMSZ, IPC_CREAT | 0600)) < 0)  // creating the shared memory 
  {
	  perror("shmget");
	  exit(1);
  }*/
  peter *shareData;
  shareData = (peter*)shmat(shareID, NULL,0);
  if (shareData->sum ==(int)-1) // Now we attach the segment to our data space.
  {
     perror("Shmat error in Main");
	 exit(1);
  }
  
  char buff[100];  // for the time
  
  mypid = getpid();  

  
  int j;
  int counter = 0;
  for (counter = 0; counter < max_writes; counter++)
  {	
    do 
   {
     shareData->flag[slaveNum] = want_in; // Raise my flag
     j = shareData->turn; // Set local variable
     // wait until its my turn
     while ( j != slaveNum)
	 {
       j = (shareData->flag[j] != idle ) ? shareData->turn : ( j + 1 ) % n;
	 }
     // Declare intention to enter critical section
     shareData->flag[slaveNum] = in_cs;
     // Check that no one else is in critical section
     for ( j = 0; j < n; j++ )
	 {
       if ( ( j != slaveNum) && (shareData->flag[j] == in_cs ) )
         break;
	 }
    } while ( ( j < n ) || ( shareData->turn != slaveNum && shareData->flag[shareData->turn] != idle ) );
    // Assign turn to self and enter critical section
    shareData->turn = slaveNum;
    //Critical secion
    printf("Process %d Entering Critical Section.\n",slaveNum+1);
    sleep(random_number(0,3)); // this was only don't 0 or 1 so 0,3 leads to 0-2 seconds for sleep
    FILE *file = fileWrite(filename); // opening file to write into	
    shareData->sum+=1;
    time_t now = time(0); // getting time 
    strftime(buff, 100, "%H:%M:%S",localtime(&now)); //I included Seconds because I like it that way
    printf("File Modified by process number %d at the time %s with sharedNum = %d\n",slaveNum+1,buff,shareData->sum);
    fprintf(file,"File Modified by process number %d at the time %s with sharedNum = %d\n",slaveNum+1,buff,shareData->sum);
	fclose(file);
    sleep(random_number(0,3));
    printf("Process %d Exiting Critical Section.\n", slaveNum+1);
    //Exit critical section
    j = (shareData->turn + 1) % n;
    while (shareData->flag[j] == idle)
      j = (j + 1) % n;
    // Assign turn to next waiting process; change own flag to idle
    shareData->turn = j; shareData->flag[slaveNum] = idle;
  } 
  return 0;
}
FILE* fileWrite(char *fileName) // Opening file for writing.
{
  FILE *file = fopen (fileName, "a");
    if (file == 0)
    {
      perror("Could not open file ");
      exit(0);
    }
    else 
    {
      return file;
    }
}
void INThandler(int sig)
{ 
  signal(sig, SIG_IGN); // ignoring any signal passed to the INThandler
  fprintf(stderr, "\nCtrl^C Called, Process %d Exiting\n", slaveNum+1);
 if((shmctl(shareID, IPC_RMID, NULL)) == -1)//mark shared memory for deletion
  {
    perror("Error In shmdt Child INThandler");
  }
  kill(getpid(), SIGKILL);
}
void TimeHandler(int sig)
{
  if((shmctl(shareID, IPC_RMID, NULL)) == -1)//mark shared memory for deletion
  {
    perror("Error In shmdt Child TimeHandler");
  }
  //shmctl(shmid, IPC_RMID, NULL); //mark shared memory for deletion
  fprintf(stderr, "\nOut of Time, Process %d Exiting\n", slaveNum+1);
  kill(getpid(), SIGKILL);
  //exit(0);
}
int random_number(int min_num, int max_num)
{
  int result =0,low_num=0,hi_num=0;
  if(min_num < max_num)
  {
    low_num=min_num;
    hi_num=max_num+1;
  }
  else
  {
    low_num=max_num+1;
    hi_num=min_num;
  }
  srand(time(NULL));
  result = (rand()%(hi_num-low_num))+low_num;
  return result;
}

