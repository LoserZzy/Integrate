//
// Created by LQB on 2020/10/17.
//

#ifndef SYS_INFO_SYSINFO_H
#define SYS_INFO_SYSINFO_H

// #define SAVE_DIR_PATH "./result/"
#define SAVE_SYSINFO_PATH "Sys_info.txt"
#define SAVE_CPU_NAME "cpu_info.txt"
#define SAVE_NET_NAME "net_info.txt"
#define SAVE_MEM_NAME "mem_info.txt"
#define SAVE_DISK_NAME "disk_info.txt"
#define SAVE_MEM_NAME_S "mem_info_s.txt"
#define SAVE_DISK_NAME_S "disk_info_s.txt"
#define SAVE_BIOS_NAME "bios_info.txt"
#define MOUNTPATH "/proc/mounts"
#define CPUPATH "/proc/cpuinfo"
#define MEMPATH "/proc/meminfo"
#define BIOSDATEPATH "/sys/class/dmi/id/bios_date"
#define BIOSVERSIONPATH "/sys/class/dmi/id/bios_version"
#define BIOSVENDORPATH "/sys/class/dmi/id/bios_vendor"

#define SMALESIZE 32
#define MIDDLESIZE 128
#define LARGESIZE 512
#define MAXSIZE 4096
#define PATH_LEN 255


// all
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdarg.h>
// net
#include<netdb.h>
#include<arpa/inet.h>
#include<ifaddrs.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<linux/if.h>
#include <fcntl.h>
//#include <net/if.h>
#include <errno.h>
typedef struct NetUnit {
    char net_name[50];
    char mac[18];
    char ip_addr[16];
    char broad_addr[16];
    char subnet_mask[16];
    struct NetUnit* next;
    // // 后期获取
    char physid[20];
    char description[100];  
    char product[100];
    char vendor[100];
    char serial[30];
}NetUnit;

typedef struct NetInfo {
    char host_name[100];
    NetUnit* netunit;
    char* all_info;
}NetInfo;


// disk  flash
#include <sys/vfs.h>
typedef struct DiskInfo {
    char fs_type[40];		// 文件系统类型
    char mount_path[PATH_LEN];	// 挂载路径
    unsigned long available;// 可用大小KB
    unsigned long total;	// 总大小KB
    unsigned long used;		// 已用大小KB
    unsigned block_size;	// 块大小KB
    struct DiskInfo* next;
}DiskInfo;

typedef struct DiskInfoS {
    // 后期获取
    char physid[20];
    char description[100];  
    char product[100];
    char vendor[100];
    char version[50];
    char businfo[50];
    char serial[30];
    char total_size[20];
    struct DiskInfoS *next;
}DiskInfoS;

//typedef struct DiskInfo {
//    DiskUnit* diskunit;
//    char* all_info;
//}DiskInfo;

// memory
#include<sys/sysinfo.h>
typedef struct MemInfo {
    unsigned long total_ram;	// 总可用内存B
    unsigned long free_ram;		// 未使用内存B
    unsigned long shared_ram;	// 共享存储器B
    unsigned long buffer_ram;	// 缓冲区大小B
    unsigned long total_swap;	// 交换区大小B
    unsigned long free_swap;	// 可用交换区大小B
    char* all_info;
}MemInfo;

typedef struct MemInfoS {
    // // 后期获取
    char physid[20];
    char description[100];  
    char product[100];
    char vendor[100];
    char businfo[50];
    char version[50];
    char serial[30];
    struct MemInfoS *next;
}MemInfoS;

// cpu
typedef struct CpuUnit {
    int processor;
    char name[100];			// 名称
    char vendor[100];	    // 厂商id
    int disabled;           // 是否启用, 启用1，不启用0
    char* all_info;
    struct CpuUnit* next;
    // // 后期获取
    char physid[20];
    char description[100];  
    char product[100];
    char vendor_name[100];
    char businfo[50];
    char version[50];
    char serial[30];
}CpuUnit;
typedef struct CpuInfo {
    CpuUnit* cpuuint;
    int phy_num;			// 物理cpu个数
    int core_num;			// cpu核数
    int logic_num;			// 逻辑cpu个数
    char architecture[20];    // cpu架构
    char* all_info;			// 全部信息
}CpuInfo;

// bios
typedef struct BiosInfo {
    char vendor[100];
    char version[100];
    char release_date[40];
    char* all_info;
    // // 后期获取
    char physid[20];    
}BiosInfo;

typedef struct SetInt{
    int item;
    int idx;
    struct SetInt *next;
}SetInt;


// function
int cpu_info(char* dir);
int net_info(char* dir);
int disk_info(char* dir);
int mem_info(char* dir);
int bios_info(char* dir);

int save_cpu_info(char* save_path, CpuInfo* cpu_list);
int save_net_info(char* save_path, NetInfo * net_list);
int save_disk_info(char* save_path, DiskInfo* disk_list);
int save_mem_info(char* save_path, MemInfo* mem_list);
int save_bios_info(char* save_path, BiosInfo* bios_list, int flag);

int save_disk_info_s(char *save_path, DiskInfoS *disk_list_s);
int save_mem_info_s(char *save_path, MemInfoS *mem_list_s);
SetInt* addInt2Set(SetInt* set_head, int src_int);
int getSetSize(SetInt* set_head);
int freeSetInt(SetInt* set_head);
char* cat_dirs_1(int num, char* fmt, ...);

#include <sys/utsname.h>
char* platform();

#endif //SYS_INFO_SYSINFO_H
