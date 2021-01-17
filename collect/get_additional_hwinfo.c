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

/*
* 读取文件内容
* path:文件路径
* length:文件大小(out)
* return:文件内容
*/
char * ReadFile(char * path, int *length)
{
	FILE * pfile;
	char * data;
 
	pfile = fopen(path, "rb");
	if (pfile == NULL)
	{
		return NULL;
	}
	fseek(pfile, 0, SEEK_END);
	*length = ftell(pfile);
	data = (char *)malloc((*length + 1) * sizeof(char));
	rewind(pfile);
	*length = fread(data, 1, *length, pfile);
	data[*length] = '\0';
	fclose(pfile);
	return data;
}



void get_additional_hwinfo()
{
    system("arch > sysinfo");
    FILE *fp=fopen("sysinfo","r");
    char sys[10], lshw[10];
    fscanf(fp, "%s", sys);
    fclose(fp);
    if (sys[0] == 'x' && sys[1] == '8' && sys[2] == '6'){ // 判断为x86
        char lshw_x86[10] = "./lshw_x86";
        for (int a = 0; a < 10; a += 1)
            lshw[a] = lshw_x86[a];
    }

    // 针对多种架构留下的空
    // else if (*****){ 
    //     char lshw_*****[10] = "./lshw_*****";
    //     for (int a = 0; a < 10; a += 1)
    //         lshw[a] = lshw_*****[a];
    // }
    
    system("rm sysinfo");

    char path[50];
    char * buf;
    int length;
    char doit[100];
    sprintf(path, "%s%s", SAVE_ADDITIONAL_INFO_DIR_PATH,SAVE_ADDITIONAL_INFO);
    FILE *rfp=fopen(path,"w");
    fclose(rfp);
    rfp=fopen(path,"a+");
    fputs("{\n", rfp);


    // 采集cpu信息暂存于cpuinfo并最终存入allinfo中
    sprintf(doit, "%s%s%s%s", lshw, " -class cpu -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_CPUINFO);
    system(doit);
    sprintf(path, "%s%s", SAVE_ADDITIONAL_INFO_DIR_PATH,SAVE_ADDITIONAL_CPUINFO);
    buf = ReadFile(path, &length);
    fputs("\"cpuinfo\":\"",rfp);
    fwrite(buf, length, 1, rfp);
    fputs("\",\n\n",rfp);
    sprintf(doit, "%s%s%s", "rm ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_CPUINFO);
    system(doit);

    
    // network
    sprintf(doit, "%s%s%s%s", lshw, " -class network -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_NETINFO);
    system(doit);
    sprintf(path, "%s%s", SAVE_ADDITIONAL_INFO_DIR_PATH,SAVE_ADDITIONAL_NETINFO);
    buf = ReadFile(path, &length);
    fputs("\"netinfo\":\"",rfp);
    fwrite(buf, length, 1, rfp);
    fputs("\",\n\n",rfp);
    sprintf(doit, "%s%s%s", "rm ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_NETINFO);
    system(doit);


    // memory
    sprintf(doit, "%s%s%s%s", lshw, " -class memory -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_MEMINFO);
    system(doit);
    sprintf(path, "%s%s", SAVE_ADDITIONAL_INFO_DIR_PATH,SAVE_ADDITIONAL_MEMINFO);
    buf = ReadFile(path, &length);
    fputs("\"meminfo\":\"",rfp);
    fwrite(buf, length, 1, rfp);
    fputs("\",\n\n",rfp);
    sprintf(doit, "%s%s%s", "rm ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_MEMINFO);
    system(doit);


    // disk
    sprintf(doit, "%s%s%s%s", lshw, " -class disk -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_CDISKINFO);
    system(doit);
    sprintf(path, "%s%s", SAVE_ADDITIONAL_INFO_DIR_PATH,SAVE_ADDITIONAL_CDISKINFO);
    buf = ReadFile(path, &length);
    fputs("\"diskinfo\":\"",rfp);
    fwrite(buf, length, 1, rfp);
    fputs("\",\n\n",rfp);
    sprintf(doit, "%s%s%s", "rm ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_CDISKINFO);
    system(doit);

    // bios
    sprintf(doit, "%s%s%s%s", lshw, " -class memory -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_BIOSINFO);
    system(doit);
    sprintf(path, "%s%s", SAVE_ADDITIONAL_INFO_DIR_PATH,SAVE_ADDITIONAL_BIOSINFO);
    buf = ReadFile(path, &length);
    fputs("\"biosinfo\":\"",rfp);
    fwrite(buf, length, 1, rfp);
    fputs("\",\n\n",rfp);
    sprintf(doit, "%s%s%s", "rm ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_BIOSINFO);
    system(doit);


    // ip分配（动态\静态）
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
            system(doit);
            memset(data, 0, 100);

            sprintf(path, "%s%s%s", SAVE_ADDITIONAL_INFO_DIR_PATH, "ipinfo", number);
            buf = ReadFile(path, &length);
            char phrase[20];
            sprintf(phrase, "%s%s%s", "\"ipinfo", number, "\":\"[\n");
            fputs(phrase,rfp);
            fwrite(buf, length, 1, rfp);
            fputs("]\",\n\n",rfp);
            sprintf(doit, "%s%s", "rm ", path);
            system(doit);
        }
    }
    fclose(fp);
    
    sprintf(path,"%s%s", SAVE_ADDITIONAL_INFO_DIR_PATH, "ipinfo_num");
    fp=fopen(path,"w");
    fputs(number, fp);
    fclose(fp);

    buf = ReadFile(path, &length);
    fputs("\"ipinfo_num\":\"[",rfp);
    fwrite(buf, length, 1, rfp);
    fputs("]\"\n\n}",rfp);
    sprintf(doit, "%s%s", "rm ", path);
    system(doit);
    
    fclose(rfp);

    system("rm lsmsg");
}