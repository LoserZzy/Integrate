#include"HDMstruct.h"


HardwareList* check_hardware();
HardwareList* check_hardware_bus(HardwareList* hw, char *bus);
int check_hardware_class(HardwareList* hw, char* className);
void show_hardware_list(HardwareList* hw);
void save_hardware(HardwareList* hw, char* savePath);
int free_hardware_list(HardwareList* hw);

int hardware(char* dir){
    HardwareList* hwListHead = check_hardware();
    if (hwListHead == NULL) {
        return -1;
    }
    char* savePath = cat_dirs(2, dir, SAVE_HARDWARE_PARH);
    save_hardware(hwListHead->next, savePath);
    // show_hardware_list(hwListHead->next);
    free_hardware_list(hwListHead);
    return 0;
}

/*-------------------------------------------------------------------------------
check hardware via /sys/bus/++/devices/++ and /sys/class/++
return "HardwareList*": head point of HardwareList
--------------------------------------------------------------------------------*/
HardwareList* check_hardware() {
    DIR *dir;
    // int errno;
    struct stat dirstat;
    struct dirent *entry;
    HardwareList* hwRes = (HardwareList*)malloc(sizeof(HardwareList));
    if (hwRes == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error.\n");
        return NULL;
    }
    memset(hwRes, 0, sizeof(HardwareList));
    HardwareList* temp = hwRes;
    /*----------Obtain info from /sys/bus----------*/
    if ((dir = opendir("/sys/bus/")) == NULL) {
        perror("Error:/sys/bus/ cannot access");
        // fprintf(stderr, "%s Error:/sys/bus/ cannot access  %s\n", __FUNCTION__, strerror(errno));
        return hwRes;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        if (entry->d_type == 4) {
            temp = check_hardware_bus(temp, entry->d_name);
        }
    }
    closedir(dir);
    /*----------Obtain info from /sys/class---------*/
    if ((dir = opendir("/sys/class/")) == NULL) {
        perror("Error:/sys/class/ cannot access");
        // fprintf(stderr, "%s Error:/sys/class/ cannot access   %s\n", __FUNCTION__, strerror(errno));
        return NULL;
    }
    //printf("------------------- start check class ------------------\n");
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        if (entry->d_type == 4) {
            temp = hwRes->next;     // hwRes is the head point of hardwareList
            check_hardware_class(temp, entry->d_name);
        }
    }
    closedir(dir);
    return hwRes;
}

/*-------------------------------------------------------------------------------
check hardware via /sys/bus/++/devices/++
"hw": the point of current hardware
"bus": bus name of devices
return "HardwareLis*": the point of final devices in /sys/bus/++/devices/
--------------------------------------------------------------------------------*/
HardwareList* check_hardware_bus(HardwareList* hw, char *bus) {
    DIR *dir;
    // int errno;
    struct dirent *entry;
    HardwareList* hwRes = hw;
    // connect path, path="/sys/bus/++/devices/"
    char* path = cat_dirs(3, (char*)"/sys/bus/", bus, (char*)"/devices/");
    if ((dir = opendir(path)) == NULL) {
        perror("Error failed to open");
        // fprintf(stderr, "Error:%s cannot access\n", __FUNCTION__);
        // fprintf(stderr, "Error failed to open %s -%s\n", path, strerror(errno));
        return hwRes;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        if (entry->d_type == DT_LNK) {
            if (hwRes == NULL) {
                hwRes = (HardwareList*)malloc(sizeof(HardwareList));                
                if (hwRes == NULL) {
                    perror("memory malloc error!");
                    // fprintf(stderr, "Memory malloc error.\n");
                    return NULL;
                }
                memset(hwRes, 0, sizeof(HardwareList));
                // printf("hwRes is NULL %s%s\n", path, entry->d_name);
            }
            else {
                hwRes->next = (HardwareList*)malloc(sizeof(HardwareList));
                if (hwRes->next == NULL) {
                    perror("memory malloc error!");
                    // fprintf(stderr, "Memory malloc error.\n");
                    return NULL;
                }
                memset(hwRes->next, 0, sizeof(HardwareList));
                hwRes = hwRes->next;
                // printf("%s%s\n", path, entry->d_name);
            }
            hwRes->next = NULL;
            hwRes->Class = NULL;
            strcpy(hwRes->Name, entry->d_name);     //Name
            strcpy(hwRes->Bus, bus);                //Bus
            // -----------------------------------
            get_abs_path(path, entry->d_name, hwRes->Path);     // Path
            char* hwPath = cat_dirs(2, path, entry->d_name);    // hardware path
            get_abs_path(hwPath, (char*)"driver", hwRes->Driver);  // Driver
            // char* hwPath = cat_dirs(2, path, entry->d_name);  // hardware path
            // get_softlink(hwPath, hwRes->Path, 0);   //Path
            // char* dPath = cat_dirs(2, hwPath, (char*)"/driver");  //driver Path
            // get_softlink(dPath, hwRes->Driver, 1);  //Driver
            char* uPath = cat_dirs(2, hwPath, (char*)"/uevent");  //uevent Path
            hwRes->Uevent = read_file(uPath, hwRes->Uevent);    // uevent
            free(uPath); 
            free(hwPath);
        }
    }
    closedir(dir);
    free(path);
    return hwRes;
}

/*-------------------------------------------------------------------------------
check hardware via /sys/class/++, compare className with each hardware devices in /sys/bus/++/devices/++
"hardware": head point of HardwareList (after /sys/bus/++/devices/++ checking)
"className": name of /sys/class/++
return "int": 1(normal)  0(abnormal)
--------------------------------------------------------------------------------*/
int check_hardware_class(HardwareList* hw, char* className) {
    DIR *dir;
    // int errno;
    struct dirent *entry;
    HardwareList *tempHW = NULL, *tempMax = NULL;
    int maxLen = 0;
    // connect path, path="/sys/class/++/"
    char* path = cat_dirs(2, (char*)"/sys/class/", className);
    if ((dir = opendir(path)) == NULL) {
        perror("Error failed to open");
        // fprintf(stderr, "Error:%s cannot access\n", __FUNCTION__);
        // fprintf(stderr, "Error failed to open %s -%s\n", path, strerror(errno));
        return -1;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        if (entry->d_type == DT_LNK) {
            tempHW = hw;
            tempMax = NULL;
            maxLen = 0;
            char* dPath = (char*)malloc(sizeof(char)*PATH_LEN);     // device path            
            if (dPath == NULL) {
                perror("memory malloc error!");
                // fprintf(stderr, "Memory malloc error.\n");
                return -1;
            }
            memset(dPath, 0, sizeof(char)*PATH_LEN);
            get_abs_path(path, entry->d_name, dPath);
            // -------------------------------------------
            // char* cPath = cat_dirs(2, path, entry->d_name);         // class path
            // get_softlink(cPath, dPath, 0);
            while (tempHW != NULL) {
                if ((strstr(dPath, tempHW->Path) != NULL) && (strlen(tempHW->Path) > maxLen)) {    // 判断temp->Path是否是dPath的字串
                    maxLen = strlen(tempHW->Path);
                    tempMax = tempHW;
                }
                tempHW = tempHW->next;
            }
            // if (tempHW == NULL) {
            //     printf("Error. No Hardware match.\n");
            // }
            if (tempMax != NULL) {
                tempMax->Class = add_to_namelist(tempMax->Class, className);  //找到了
            }
            // free(cPath); 
            free(dPath); 
        }
    }
    free(path);
    closedir(dir);
    return 0;
}

/*-------------------------------------------------------------------------------
show HardwareList
"hardware": the first hardware point of HardwareList
return "void"
--------------------------------------------------------------------------------*/
void show_hardware_list(HardwareList* hw) {
    HardwareList* temp = hw;
    int i = 0;
    while (temp != NULL) {
        printf("Name:%s\n", temp->Name);
        printf("Path:%s\n", temp->Path);
        printf("Bus:%s\n", temp->Bus);
        printf("Class:");
        showNameList(temp->Class);
        printf("Driver:%s\n", temp->Driver);
        printf("Uevent:%s\n", temp->Uevent);
        printf("== %d over==\n\n", i++);
        temp = temp->next;
    }
}

/*-------------------------------------------------------------------------------
save HardwareList
"hardware": the first hardware point of HardwareList
"savePath": save_path of HardwareList
return "void"
--------------------------------------------------------------------------------*/
void save_hardware(HardwareList* hw, char* savePath) {
    FILE *fp;
    if ((fp = fopen(savePath, "w")) == NULL) {
        perror("File open error.");
        // fprintf(stderr, "%s File open error\n", savePath);
        return ;
    }
    HardwareList* tempHW = hw;
    int i = 0;
    fprintf(fp, "[\n");
    while (tempHW != NULL) {

        fprintf(fp, "{\"HardwareName\":\"%s\",", tempHW->Name);
        fprintf(fp, "\"DriverID\": %d,", 0);
        fprintf(fp, "\"HardwareDir\":\"%s\",", tempHW->Path);
        fprintf(fp, "\"Bus\":\"%s\",", tempHW->Bus);
        fprintf(fp, "\"Class\": [");
        NameList* tempC = tempHW->Class;
        while (tempC) {
            fprintf(fp, "\"%s\"", tempC->Name);
            tempC = tempC->next;
            if (tempC != NULL) {
                fprintf(fp, ", ");
            }
        }
        fprintf(fp, "],");
        fprintf(fp, "\"Driver\":\"%s\",", tempHW->Driver);
        if (tempHW->Uevent != NULL)
            fprintf(fp, "\"Modalias\":\"%s\"}", tempHW->Uevent);
        else
            fprintf(fp, "\"Modalias\":\"\"}");
        tempHW = tempHW->next;
        if (tempHW != NULL) {
            fprintf(fp, ",\n");
        }
    }
    fprintf(fp, "\n]");
    fclose(fp);
}

int free_hardware_list(HardwareList* hw){
    HardwareList *temp1 = hw, *temp2;
    while(temp1!=NULL){
        temp2 = temp1->next;
        // printf("free %s namelist\t\t\t", temp1->Name);
        freeNameList(temp1->Class);
        free(temp1->Uevent);
        // printf("free %s.\n", temp1->Name);
        free(temp1);
        temp1 = temp2;
    }
    temp1 = NULL;
    temp2=NULL;
    hw=NULL;
    return 0;
}

/* 分析uevent信息，将分析结果存放在OtherInfo中，
主要针对于pci和usb分析，根据modalias中的id信息，获取对应的Name

return "int": 0(normal) 1(abnormal)
*/
// int parse_uevent(char* uevent, char* otherInfo){

//     return 0;
// }
