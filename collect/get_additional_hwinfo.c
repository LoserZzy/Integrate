#include <stdio.h>
#include <stdlib.h>

int main()
{
    system("./lshw -class cpu > cpuinfo");
    system("./lshw -class network > networkinfo");
    system("./lshw -class memory > memoryinfo");
    system("./lshw -class disk > diskinfo");
    system("./lshw -class memory > biosinfo");
}
