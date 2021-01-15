#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<dirent.h>
#include<unistd.h>
#include<sys/stat.h>
#include <sys/types.h>
#include<errno.h>
#include<stdarg.h>

/*---------------------------------------------------------------------------------------
This file defines the struct that used by Hardware-Driver-Module
---------------------------------------------------------------------------------------*/
#define NAME_LEN 50
#define PATH_LEN 255
#define FILE_BUF 255    // buf size of each line
#define MODINFO_BUF 4096    // buf size of modinfo
#define SAVE_HARDWARE_PARH "Hardware_info.txt"
#define SAVE_DRIVER_PARH "Driver_info.txt"
#define SAVE_MODULE_PARH "Module_info.txt"


typedef struct PathList {
    char Path[PATH_LEN];
    struct PathList* next;
}PathList;


typedef struct NameList {
    char Name[NAME_LEN];
    struct NameList* next;
}NameList;
/*------------------------Hardware info------------------------------------------------*/
typedef struct Hardware {
    struct Hardware *next;
    char Name[NAME_LEN];        // /sys/bus/--/device/name(name)
    char Path[PATH_LEN];        // /sys/bus/--/device/name(path softlink)
    NameList *Class;
    //char Class[NAME_LEN];     // /sys/class/--/name(num binary_sign)
    char Bus[NAME_LEN];         // /sys/bus/--/device/name(name bus)
    char Driver[PATH_LEN];      // /sys/bus/--/device/name/driver(name softlink)
    char *Uevent;               // /sys/bus/--/device/name/uevent(file)
} HardwareList;

/*------------------------Driver info--------------------------------------------------*/
typedef struct Driver {
    struct Driver *next;
    char Name[NAME_LEN];        // /sys/bus/--/driver/name(name name)
    char Path[PATH_LEN];        // /sys/bus/--/driver/name(path now)
    char Bus[NAME_LEN];         // /sys/bus/--/driver/name(name bus)
    char Module[PATH_LEN];      // /sys/bus/--/driver/name/module(name softlink)
} DriverList;

/*------------------------Module info--------------------------------------------------*/
typedef struct ModuleList {
    struct ModuleList *next;
    char Name[NAME_LEN];        // /sys/module/name(name name)
    char Path[PATH_LEN];        // /sys/module/name/driver(name *)
    int Excute;                 // /proc/modules(whetherin)
    char *Modinfo;              // modinfo
} ModuleList;

// help.c
int get_softlink(char* filePath, char* destBuf, int sign);
int get_abs_path(char* fileDir, char* fileName, char* absPath);
char* read_file(char* filePath, char* destBuf);
char* cat_dirs(int num, char* fmt, ...);
char* cat_strs(int size, int num, char* fmt, ...);
NameList* add_to_namelist(NameList* nameList, char* name);
PathList* add_to_pathlist(PathList* pathList, char* path);
void showNameList(NameList* nameList);
void showPathList(PathList* pathList);
int freeNameList(NameList* nList);
int freePathList(PathList* pList);

//hwutils.c
int hardware(char* dir);

//drutils.c
int driver(char* dir);

//modutils.c
int module(char* dir);
