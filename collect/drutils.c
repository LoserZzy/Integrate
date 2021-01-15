#include"HDMstruct.h"

DriverList* check_driver();
DriverList* check_driver_bus(DriverList* dr, char *bus);
void show_driver_list(DriverList* dr);
void save_driver(DriverList* dr, char* savePath);
int free_driver_list(DriverList* dr);


int driver(char* dir){
    DriverList* drListHead = check_driver();
    if (drListHead == NULL) {
        return -1;
    }
    char* savePath = cat_dirs(2, dir, SAVE_DRIVER_PARH);
    save_driver(drListHead->next, savePath);
    // show_driver_list(drListHead->next);
    free_driver_list(drListHead);
    return 0;
}


/*-------------------------------------------------------------------------------
check driver via /sys/bus/++/drivers/++
return "DriverList*": head point of DriverList
--------------------------------------------------------------------------------*/
DriverList* check_driver() {
    DIR *dir;
    // int errno = 111;
    struct stat dirstat;
    struct dirent *entry;
    DriverList* drRes = (DriverList*)malloc(sizeof(DriverList));
    if (drRes == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error.\n");
        return NULL;
    }
    memset(drRes, 0, sizeof(DriverList));
    DriverList* temp = drRes;
    /*----------Obtain info from /sys/bus----------*/
    if ((dir = opendir("/sys/bus/")) == NULL) {
        perror("Error failed to open input directory");
        // fprintf(stderr, "Error failed to open input directory -%s\n",strerror(errno));
        // fprintf(stderr, "%s Error:/sys/bus/ cannot access  %d\n", __FUNCTION__, errno);
        return drRes;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        if (entry->d_type == 4) {       // current file is a directory
            //// debug
            // printf("bus name: %s\n", entry->d_name);
            temp = check_driver_bus(temp, entry->d_name);
        }
    }
    closedir(dir);
    return drRes;
}

/*-------------------------------------------------------------------------------
check driver via /sys/bus/++/drivers/++
"dr": the point of current driver
"bus": bus name
return "DriverList*": the point of final drivers in /sys/bus/++/drivers/
--------------------------------------------------------------------------------*/
DriverList* check_driver_bus(DriverList* dr, char *bus) {
    DIR *dir;
    // int errno = 44;
    struct dirent *entry;
    DriverList* drRes = dr;
    // connect path, path="/sys/bus/++/drivers/"
    char* path = cat_dirs(3, (char*)"/sys/bus/", bus, (char*)"/drivers/");
    if ((dir = opendir(path)) == NULL) {
        perror("Error failed to open input directory");
        // fprintf(stderr, "Error failed to open input directory -%s\n", strerror(errno));
        // fprintf(stderr, "Error:%s cannot access\n", __FUNCTION__);
        return drRes;
    }
    //// debug
    // printf("check_driver_bus path: %s\n", path);
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        if (entry->d_type == 4) {   // directory
            // printf("driver name: %s\n", entry->d_name);
            if (drRes == NULL) {
                drRes = (DriverList*)malloc(sizeof(DriverList));
                if (drRes == NULL) {
                    perror("memory malloc error!");
                    // fprintf(stderr, "Memory malloc error.\n");
                    return NULL;
                }
                memset(drRes, 0, sizeof(DriverList));
                // printf("driver is NULL%s%s\n", path, entry->d_name);
            }
            else {
                drRes->next = (DriverList*)malloc(sizeof(DriverList));
                if (drRes->next == NULL) {
                    perror("memory malloc error!");
                    // fprintf(stderr, "Memory malloc error.\n");
                    return NULL;
                }
                memset(drRes->next, 0, sizeof(DriverList));
                drRes = drRes->next;
                // printf("%s%s\n", path, entry->d_name);
            }
            drRes->next = NULL;
            strcpy(drRes->Name, entry->d_name);     // Name
            char* dPath = cat_dirs(2, path, entry->d_name);
            strcpy(drRes->Path, dPath);  // Path
            strcpy(drRes->Bus, bus);                // Bus
            //-------------------------------------------------------
            if (get_abs_path(drRes->Path, (char*)"/module", drRes->Module) < 0) {
                strcpy(drRes->Module, (char*)"");
            }

            // char* mName = cat_dirs(3, path, entry->d_name, "/module");    //module path
            // get_softlink(mName, drRes->Module, 2);  // Module
            // free(mName);
            //// debug
            // if (get_softlink(mName, drRes->Module, 2) < 0) {
            //     perror("memory malloc error!");
            //     // fprintf(stderr, "%s has no module.\n", entry->d_name);
            // }
            // else {
            //     perror("memory malloc error!");
            //     // fprintf(stderr, "%s module path: %s\n", entry->d_name, drRes->Module);
            // }
            // printf("free dPath\n");
            free(dPath); 
        }
    }
    free(path);
    closedir(dir);
    return drRes;
}

/*-------------------------------------------------------------------------------
show DriverList
"dr": the first driver point of DriverList
return "void"
--------------------------------------------------------------------------------*/
void show_driver_list(DriverList* dr) {
    DriverList* temp = dr;
    int i = 0;
    while (temp != NULL) {
        printf("==== %d ====\n", i++);
        printf("Name: %s\n", temp->Name);
        printf("Path: %s\n", temp->Path);
        printf("Bus: %s\n", temp->Bus);
        printf("Module: %s\n", temp->Module);
        temp = temp->next;
    }
}

/*-------------------------------------------------------------------------------
save DriverList
"dr": the first driver point of DriverList
"savePath": save_path of DriverList
return "void"
--------------------------------------------------------------------------------*/
void save_driver(DriverList* dr, char* savePath) {
    FILE *fp;
    // int errno  = 152;
    if ((fp = fopen(savePath, "w")) == NULL) {
        perror("File open error.");
        // fprintf(stderr, "%s File open error %d - %s\n", savePath, errno, strerror(errno));
        return ;
    }
    DriverList* tempD = dr;
    int i = 0;
    fprintf(fp, "[\n");
    while (tempD != NULL) {
        // fprintf(fp, "Name: %s\n", tempD->Name);
        // fprintf(fp, "Path: %s\n", tempD->Path);
        // fprintf(fp, "Bus: %s\n", tempD->Bus);
        // fprintf(fp, "Module: %s\n", tempD->Module);
        // fprintf(fp, "== %d over==\n\n", i++);

        fprintf(fp, "{\"DriverName\": \"%s\",", tempD->Name);
        fprintf(fp, "\"ModuleId\": %d,", 0);
        fprintf(fp, "\"DriverDir\": \"%s\",", tempD->Path);
        fprintf(fp, "\"Bus\": \"%s\",", tempD->Bus);
        fprintf(fp, "\"Module\": \"%s\"}", tempD->Module);
        tempD = tempD->next;
        if (tempD != NULL) {
            fprintf(fp, ",\n");
        }
    }
    fprintf(fp, "\n]");
    fclose(fp);
}


int free_driver_list(DriverList* dr){
    DriverList *temp1 = dr, *temp2;
    while(temp1!=NULL){
        temp2 = temp1->next;
        // printf("free %s driver\n", temp1->Name);
        free(temp1);
        temp1 = temp2;
    }
    temp1 = NULL;
    temp2 = NULL;
    dr = NULL;
    return 0;
}