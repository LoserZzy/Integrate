#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "get_additional_hwinfo.h"
#include "settings.h"

char* Int2String(int num,char *str)//10进制 
{
    int i = 0;
    if(num<0)
    {
        num = -num;
        str[i++] = '-';
    } 
    do
    {
        str[i++] = num%10+48;
        num /= 10;  
    }while(num);
    
    str[i] = '\0';
    int j = 0;
    if(str[0]=='-')
    {
        j = 1;
        ++i;
    }
    for(;j<i/2;j++)
    {
        str[j] = str[j] + str[i-1-j];
        str[i-1-j] = str[j] - str[i-1-j];
        str[j] = str[j] - str[i-1-j];
    } 
    return str;
}

void get_additional_hwinfo()
{
    system("arch > sysinfo");
    FILE *fp=fopen("sysinfo","r");
    char sys[10], lshw[10];
    fscanf(fp, "%s", sys);
    fclose(fp);
    if (sys[0] == 'x' && sys[1] == '8' && sys[2] == '6'){
        char lshw_x86[10] = "./lshw_x86";
        for (int a = 0; a < 10; a += 1)
            lshw[a] = lshw_x86[a];
    }
    system("rm sysinfo");


    char doit[100];
    sprintf(doit, "%s%s%s%s", lshw, " -class cpu -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_CPUINFO);
    system(doit);

    sprintf(doit, "%s%s%s%s", lshw, " -class network -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_NETINFO);
    system(doit);

    sprintf(doit, "%s%s%s%s", lshw, " -class memory -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_MEMINFO);
    system(doit);

    sprintf(doit, "%s%s%s%s", lshw, " -class disk -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_CDISKINFO);
    system(doit);

    sprintf(doit, "%s%s%s%s", lshw, " -class memory -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_BIOSINFO);
    system(doit);

    system("ls /etc/netplan > lsmsg");
    fp=fopen("lsmsg","r");
    char data[100];
    int num = 0;
    char number[10];
    while(!feof(fp))
    {
        fscanf(fp,"%s",data);
        if (strlen(data) != 0){
            num += 1;
            char doit[50];
            Int2String(num, number);
            sprintf(doit,"%s%s%s%s%s%s","cp /etc/netplan/", data, " ", SAVE_ADDITIONAL_INFO_DIR_PATH, "ipinfo", number);
            memset(data, 0, 100);
            system(doit);
        }
    }
    fclose(fp);
    
    char path[50];
    sprintf(path,"%s%s", SAVE_ADDITIONAL_INFO_DIR_PATH, "ipinfo_num");
    fp=fopen(path,"w");
    fputs(number, fp);
    fclose(fp);

    system("rm lsmsg");
}