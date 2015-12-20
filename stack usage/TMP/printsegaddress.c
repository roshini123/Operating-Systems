#include<stdio.h>
extern int etext,edata,end;
void printsegaddress()
{
printf("\n\n2. printsegaddress");
kprintf("\npreceeding etext[0x%08x] :0x%08x,  current etext[0x%08x] :0x%08x, following etext[0x%08x] :0x%08x", (&etext-1),*(&etext-1),&etext,*(&etext),(&etext+1),*(&etext+1));
kprintf("\npreceeding edata[0x%08x] :0x%08x,  current edata[0x%08x] :0x%08x, following edata[0x%08x] :0x%08x", (&edata-1),*(&edata-1),&edata,*(&edata),(&edata+1),*(&edata+1));
kprintf("\npreceeding ebss[0x%08x] :0x%08x,  current ebss[0x%08x] :0x%08x, following ebss[0x%08x] :0x%08x\n", (&end-1),*(&end-1),&end,*(&end),(&end+1),*(&end+1));
}
