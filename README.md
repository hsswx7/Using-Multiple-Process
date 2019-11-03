Harprabh Sangha, hsswx7, M/W 9:30 am - 10:45 am
Project 2

Sites used to help with project:
http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html - used to learn getopt
http://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html - used to learn getopt_long
http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/shm/example-1.html - Shared Memory
http://stackoverflow.com/questions/1784136/simple-signals-c-programming-and-alarm-function - Alarm Handeling

_________________How to Use__________________________________
Please use the commands: 

exe = executable : master or ./master

-----commands-----
-h or -help  : shows steps on how to use the program 
-s x         : x is the maximum number of slave processes spawned (default 5) 
-l filename  : change the log file name (default test.out)
-i y         : The parameter y determines how many times each slave should increment and write to the file before terminating (default of 3)
-t z         : parameter z is the time in seconds when the master will terminate itself (default 20) 

-----examples------ 
master -h -help -lfilename.txt -s15 -t14 -i300
./master -h -help -l filename2.txt -s 15 -t 14 -i 300 
./master -h or -help shows commands and does not run the program.

----or ---
master  // this was done to simply test proj2 without the 

Wirting an invalid commands returns 0 and terminates the program -> program writes nothing to the logfile

---------Bugs with commands----------------------------------------------------------------------------------------------------------
Validate function checkes for (-s,-i,-t) if -sdfads or -s0 or -s-123. IF these are present each value is changed to default. 
If you do -lfilename-nx this producess and error
if you do -l-n it will name the file -n, I tried checking for -n but not still failed
what type of if statement would detect that its -n but it does not work.            
____________________________________________________________________________________________________________________________________

*******************Wiriting To File***********************************************************************************************
-Default name is test.out 
- When making a filename Please write it the name with .txt or .out
-If .txt or .out is not added the file will still be Written onto the new name.

-fopen I did use "a" , this allows multiple runs of the of proj1 and stored in the same file.
********************************************************************************************************************************


*********Time*******************************************************************************************************************
You asked for H:M but I also added S. I show time as H:M:S beacuse seconds actaully shows the time difference for the slaves . 
I have commented out ctime which i prefer.
********************************************************************************************************************************

----------------------------------Shared Memory---------------------------------------------------------------------------------
This works but, when calling Ctrl^C or Alarm(z) where z is the amount of seconds the master waits, the detachament of memeroy
using shmctl producess an error in the slave program. I commonted out the Perror beacuse It still detaches the memeroy.
(I believe the error is called beacuse Ctrl^C or alarm is called before even the child attaches shared memeory)
--------------------------------------------------------------------------------------------------------------------------------

