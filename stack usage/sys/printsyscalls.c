#include <stdio.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

int flag = 0;
int call_count[NPROC][42];
int call_flag[NPROC][42];
const char syscalls_name[42][15]={"chprio","getpid","getprio","gettime","kill","naminit","nammap","namopen","namrepl","pcount","pcreate","pdelete","pinit","preceive","preset","psend","premove","pquery","receive","recvclr","recvtim","remove","rename","resume","scount","sdelete","send","sendf","setdev","setnok","setcreate","signal","signaln","sleep","sleep10","sleep100","sleep1000","sreset","stacktrace","suspend","unsleep","wait"};

void syscallsummary_start()
{
    int i, j;
     flag=1;
     for(i=0;i<NPROC;i++)
        {
                for(j=0;j<42;j++)
                {
                         call_count[i][j]=0;
                        call_flag[i][j]=0;
                }
        }

}

void syscallsummary_stop()
{
    flag = 0;
}

void printsyscalls()
{
	int i,j;
 	kprintf("\n\n5.printsyscalls\n");
	for(i=0;i<=NPROC;i++)
        for(j=0;j<42;j++)
        {
                if(call_flag[i][j]==0)
                continue;
                else
                kprintf("\nprocess id:%d, system call: %s, call count:%d",i,syscalls_name[j],call_count[i][j]);
        }

}


