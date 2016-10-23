//Harprabh Sangha
//hsswx7

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

void comOptions (int argc, char **argv, int c, int *x, int *y, int *z, char **filename, int *hflag, int *hepflag, int *nflag, int *lflag, int *nempty);
void displayHelpMesg();
void validate(int *x,int temp,char y);
void test(int x, int y, int z,char*file);  // this is just to print o
void INThandler(int sig);  // handeling Ctrl^C signal
void on_alarm(int signal);  // handeling the alarm(z) signal 

#define SHMSZ sizeof(int) //size 
pid_t parent_pid; 
int alarm_stop = 0;  // I don't know why I really need this but I have it 
int z = 20; // the -t is global so on_alarm can access the variable without me having to pass it in 
int x =5; // Also global
pid_t pidArr[20]; // Matt taught me how to be lazy and just 20 size array to store slave ID 
int shareID;  //I made this global so INThandler
int *sharedNum;

int main(int argc, char **argv)
{
  int c; //This is for the Switch statement for using getopt
  char *filename = "test.out";
  int y = 3;  // Times a slave should write to logfile
  int hflag =0;
  int hepflag =  0; // flags for each command for customized messages
  int nflag = 0;  
  int lflag = 0;
  int nempty = 0; 

  comOptions(argc,argv,c,&x,&y,&z,&filename, &hflag, &hepflag, &nflag, &lflag,&nempty); // H
  test(x,y,z,filename);
  key_t key = 1994; // making key for access 
  typedef struct PeterStruct
  {
     int sum;
	 int turn;
	 enum state { idle, want_in, in_cs } flag[x];
  }peter;
  
  // Fork Proccesses 
  
  if((shareID = shmget(key, SHMSZ, IPC_CREAT | 0600)) < 0)  // creating the shared memory 
  {
	  perror("shmget");
	  exit(1);
  }
  peter *shareData;
  shareData = (peter*)shmat(shareID, NULL,0);
  if (shareData->sum ==(int)-1) // Now we attach the segment to our data space.
  {
	  perror("Shmat error in Main");
	  exit(1);
  }
  shareData->sum = 0;
  
  
  char max_writes [32];
  sprintf(max_writes, "%d",y);
  char xx[32];
  //sprintf(xx, "%d",x);
  char keyPass[32];
  sprintf(keyPass, "%d",key);
  char shareIDpass[32];
  sprintf(shareIDpass, "%d",shareID);
  char numOfSlaves[32];
  sprintf(numOfSlaves, "%d",x);
  //char *args[] = {"./slave",filename,NULL};
  
  
  signal (SIGINT,INThandler);
  signal(SIGALRM,on_alarm);
  alarm(z);
  int i = 0;
  for (i = 0; i < x ; i++)
  { 
    sprintf(xx,"%d",i); // processes number
    pid_t pid = fork();

    if (pid < 0)
    {
      perror("Fork() Failed."); 
      exit(-1); 
    }
    if (pid == 0)
    {
      pidArr[x]=getpid();
      //printf("Child pid: %d\n",pidArr[x]);
      // printf("Starting Exec.\n");
      execl("./slave",filename,max_writes,xx,keyPass,shareIDpass,numOfSlaves,NULL);
      signal (SIGINT,INThandler);
      perror("Child failed to execl");
    }
		
  }
  	  


    wait(NULL);
  if((shmdt(shareData)) == -1) //detach from shared memory
  {
    perror("Error in shmdt in Parent:");
  }
  if((shmctl(shareID, IPC_RMID, NULL)) == -1)//mark shared memory for deletion
  { 
    perror ("Error in shmclt"); 
  }	
  return 0;
}

//Functions----------------

void comOptions (int argc, char **argv , int c, int *x, int *y, int *z, char **filename, int *hflag, int *hepflag, int *nflag, int *lflag, int *nempty)
{ 
  int temp = 0;
  static struct option long_options[] = 
  { 
    {"help", no_argument, 0, 'p'},
    { 0,     0          , 0,  0 } 
  };
  int long_index = 0;

  while((c = getopt_long_only(argc, argv, "hs:i:t:l:", long_options, &long_index)) != -1)
  {
    switch (c)
    {
      case 'h':  // -h
        *hflag = 1;
        *nempty = 1; 
        if (*hepflag ==0) {displayHelpMesg();}
      break;
      
      case 'p':  // -help
       //printf("help used \n");
       *hepflag = 1;
       *nempty = 1;
       if (*hflag ==0) {displayHelpMesg();}
      break;
      
      case 's':  // -s x
        *nflag = 1;
        *nempty = 1;
	temp = *x;
	*x = atoi(optarg);
        if (*x > 20)
	{
          printf("Inputed: %d is to big. (Limit 20). Reverting back to default 5.\n", *x);
          *x = temp;
        }
	validate(x,temp,'x');
      break;

      case 'i':
        *nempty = 1;
	temp = *y;
	*y = atoi(optarg);
	validate(y,temp,'y');
      break;

      case 't':
        *nempty = 1;
	 temp = *z;
	 *z = atoi(optarg);
	 validate(z,temp,'z');
      break;

      case 'l':
        if (optopt == 'n')
        {
          printf("Please enter a valid filename.");
          return;
        }
        //printf("Log file name changed to: %s\n", optarg);
        *lflag = 1;
        *nempty = 1;
        *filename = optarg;
      break;
      
      case '?':
        if (optopt == 'l')
        {
          printf("Command -l requires filename. Ex: -lfilename.txt | -l filename.txt.\n");
		  exit(0);
        }
        else if (optopt == 's')
        {
          printf("Commands -s requires int value. Ex: -s213| -s 2132\n");
		  exit(0);
        }
	else if (optopt == 'i')
	{
		printf("Command -y requires int value. Ex: -i213| -i 2132\n");
		exit(0);
	}
	else if (optopt == 't')
	{
		printf("Command -z requires int value. Ex: -t13| -t 2132\n");
		exit(0);	
	}
        else
        {
          printf("You have used an invalid command, please use -h or -help for command options, closing program.\n"); 
		  exit(0);
        }
      return;
	  
      default :
        if (optopt == 'l')
        {
          *lflag = 1;
          *nempty = 1; 
          printf ("Please enter filename after -l \n");
          exit(0);
        }
	else if (optopt == 'n')
        { 
          printf ("Please enter integer x after -n \n");
	  *nflag = 1;
          *nempty = 1;
        }
        printf("Running Program without Commands.");
      break;
    }
  }
  if ((nempty == 0) &&argv[optind] == NULL || argv[optind + 1] == NULL)
  {
    printf("Running program without commands. Please use -h or -help for commands options next time.\n");
  } 
}
void validate(int *x,int temp,char y)
{
  char *print;
  char *print2;
  if (y == 'y')
  {
    print = "y";
    print2 = "-i";
  } 
  else if (y == 'z')
  {
    print = "z";
    print2 = "-t";	  
  }
  else if (y == 'x')
  {
    print = "x";
    print2 = "-s";	  
  }
  
  
  if (*x == 0)
  {
    printf("Intput invalid for %s changing %s back or default.\n",print2,print);
    *x = temp;
  }
  else if (*x < 0)
  {
    printf("Intput invalid for %s changing %s back or default.\n",print2,print);
    *x = temp; 
  }
}
void displayHelpMesg()
{
  printf (" -h or -help  : shows steps on how to use the program \n");
  printf (" -s x         : x is the maximum number of slave processes spawned (default 5) \n");
  printf (" -l filename  : change the log file name \n");
  printf (" -i y         : The parameter y determines how many times each slave should increment and write to the file before terminating (default of 3). \n");
  printf (" -t z         : parameter z is the time in seconds when the master will terminate itself (default 20) \n"); 
  printf ("\nClosing Program Becuase it's a good idea, Good Luck Running the Program.\n");
  exit(0);
}
void test (int x, int y, int z, char *file)
{
  printf ("Number of Slaves (x): %d\n", x);
  printf ("Slaves will print(y): %d\n", y);
  printf ("Time limit       (z): %d\n", z);
  printf ("Filename            : %s\n\n", file);
}
void INThandler(int sig)
{ 
  signal(sig, SIG_IGN);
  printf("\nCtrl^C Called. Closing All Process.\n");
  fflush(stdout);

  if((shmctl(shareID, IPC_RMID, NULL)) == -1)//mark shared memory for deletion
  {
     perror ("Error in shmclt"); 
  }
  int i =0;
  for (i=0; i<x;i++)
  {
	  kill(pidArr[x], SIGQUIT);
  }
   	
  
  exit(0);
}
void on_alarm(int signal)
{
  if(alarm_stop)
  {
    printf("Alarm\n");
  }
  else
  {
    printf("Timer of %d seconds is over killing all slave processes.\n", z);
    int i = 0;
    for (i=0; i<x;i++)
    {
      kill(pidArr[x], SIGTERM); // killing em child by children
    }
    if((shmctl(shareID, IPC_RMID, NULL)) == -1)//mark shared memory for deletion
    {
      perror ("Error in shmclt"); 
    }	
    exit(0);
  }
}
