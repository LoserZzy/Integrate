#include"HDMstruct.h"

ModuleList* check_module();
NameList* get_proc_modules(NameList* moduleList);
int is_excute(NameList* moduleList, char* mod);
char* get_modinfo(char* modName);
void show_module_list(ModuleList* mod);
void save_module(ModuleList* mod, char* savePath);
int free_module_list(ModuleList* mod);

int module(char* dir){
    ModuleList* mListHead = check_module();
    // printf("check module ok\n");
    //show_module_list(mListHead->next);
    if (mListHead == NULL) {
        return -1;
    }
    // printf("mListHead is not NULL\n");
    char* savePath = cat_dirs(2, dir, SAVE_MODULE_PARH);
    // printf("befor save module.result\n");
    save_module(mListHead->next, savePath);
    free_module_list(mListHead);
    return 0;
}

/*-------------------------------------------------------------------------------
check module via /sys/module/++ and /proc/modules
return "ModuleList*": head point of ModuleList
--------------------------------------------------------------------------------*/
ModuleList* check_module() {
    DIR *dir;
    // int errno;
    struct stat dirstat;
    struct dirent *entry;
    ModuleList* mRes = (ModuleList*)malloc(sizeof(ModuleList));
    if (mRes == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error.\n");
        return NULL;
    }
    memset(mRes, 0, sizeof(ModuleList));
    ModuleList* temp = mRes;
    /*----------Obtain info from /sys/module----------*/
    if ((dir = opendir("/sys/module/")) == NULL) {
        perror("/sys/module cannot access");
        // fprintf(stderr, "%s Error:/sys/module/ cannot access  %s\n", __FUNCTION__, strerror(errno));
        return mRes;
    }
    NameList* pModuleList = (NameList*)malloc(sizeof(NameList));
    if (pModuleList == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error.\n");
        return NULL;
    }
    memset(pModuleList, 0, sizeof(NameList));
    pModuleList->next = NULL;
    pModuleList = get_proc_modules(pModuleList);        // module list of modules in /proc/modules
    // debug
    // printf("--- showNameList(pModuleList) ---\n");
    // showNameList(pModuleList->next);
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        if (entry->d_type == 4) {       // current file is a directory
            temp->next = (ModuleList*)malloc(sizeof(ModuleList));
            if (temp->next == NULL) {
                perror("memory malloc error!");
                // fprintf(stderr, "Memory malloc error.\n");
                return NULL;
            }
            memset(temp->next, 0, sizeof(ModuleList));
            temp = temp->next;
            temp->next = NULL;
            strcpy(temp->Name, entry->d_name);      // Name
            strcpy(temp->Path, (char*)"/sys/module/");
            strcat(temp->Path, entry->d_name);      // Path
            //// debug
            // printf("module name: %s\n", temp->Name);
            // printf("module path: %s\n", temp->Path);
            temp->Excute = is_excute(pModuleList, entry->d_name);   // Excute
            temp->Modinfo = NULL;
            // printf("temp->Modinfo:%s Excute:%d\n", entry->d_name, temp->Excute);
            
            if (temp->Excute) {
                // temp->Modinfo = (char*)malloc(sizeof(char)*MODINFO_BUF);
                // if (temp->Modinfo == NULL) {
                //     perror("memory malloc error!");
                //     // fprintf(stderr, "Memory malloc error.\n");
                //     return NULL;
                // }
                // memset(temp->Modinfo, 0, MODINFO_BUF);
                // get_modinfo(entry->d_name, temp->Modinfo);      // Modinfo

                temp->Modinfo = get_modinfo(entry->d_name);      // Modinfo
                //// Debug
                // printf("%s", temp->Modinfo);
            }
            else {
                temp->Modinfo = NULL;
            }

        }
    }
    freeNameList(pModuleList);  // there exists some problem
    // free pModuleList
    closedir(dir);
    return mRes;
}


/*-------------------------------------------------------------------------------
get modulelist in /proc/modules
"moduleList": head point of modulelist
return "NameList*": head point of modulelist
--------------------------------------------------------------------------------*/
NameList* get_proc_modules(NameList* moduleList) {
    unsigned long filesize = -1;
    FILE *fp;
    if ((fp = fopen("/proc/modules", "r")) == NULL) {
        perror("/proc/modules cannot open.");
        // fprintf(stderr, "/proc/modules cannot open.\n");
        return NULL;
    }

    // fseek(fp, 0L, SEEK_END);
    // int size = ftell(fp);
    // fseek(fp, 0L, SEEK_SET);
    // printf("/proc/modules size: %d\n", size);
    // if (size == 0){
    //     return moduleList;
    // }

    char* line = (char*)malloc(sizeof(char)*FILE_BUF);
    if (line == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error.\n");
        return NULL;
    }
    memset(line, 0, sizeof(char)*FILE_BUF);
    NameList* temp = moduleList;
    fgets(line, FILE_BUF, fp);
    while (!feof(fp)) {
        temp->next = (NameList*)malloc(sizeof(NameList));
        if (temp->next == NULL) {
            perror("memory malloc error!");
            // fprintf(stderr, "Memory malloc error.\n");
            return NULL;
        }
        memset(temp->next, 0, sizeof(NameList));
        temp = temp->next;
        temp->next = NULL;
        int i;
        for (i = 0; line[i] != ' '; i++) {  // 检测第一个空格
            temp->Name[i] = line[i];
        }
        temp->Name[i] = '\0';
        fgets(line, FILE_BUF, fp);
        /*int i = 0;
        while (line[i] != ' ') {
            i++;
        }
        line[i] = '\0';
        strcpy(temp->Name, line);
        */
    }
    free(line);
    fclose(fp);
    return moduleList;
}

/*-------------------------------------------------------------------------------
judge module is excutive or not (compare modulelist in /proc/modules)
"moduleList": moduleList in /proc/modules
"mod": name of current module
return "int":  0(not excutive)    1(excutive)
--------------------------------------------------------------------------------*/
int is_excute(NameList* moduleList, char* mod) {
    NameList* temp = moduleList->next;
    if (temp == NULL){
        return 0;
    }
    while (temp->next != NULL) {    // 将module与moduleList(/proc/modules)逐个比较
        if (strcmp(temp->Name, mod) == 0) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

/*-------------------------------------------------------------------------------
get modinfo via "modinfo name" command
"modName": name of module
"result": output of "modinfo name" command
return "char*": NULL(abnormal)  result(normal)
--------------------------------------------------------------------------------*/
char* get_modinfo(char* modName) {
    char buffer[FILE_BUF];
    char* cmd = cat_strs(NAME_LEN, 2, (char*)"modinfo ", modName);
    FILE* pipe = popen(cmd, "r");   //获取命令行输出 modinfo module
    if (!pipe) {
        perror("popen error!");
        // fprintf(stderr, "popen error!\n");
        return NULL;
    }
    int info_buff = 0;
    while (!feof(pipe)) {
        char ch = fgetc(pipe);
        info_buff++;
    }

    pipe = popen(cmd, "r");   //获取命令行输出 modinfo module
    if (!pipe) {
        perror("popen error.");
        // fprintf(stderr, "popen error!\n");
        return NULL;
    }

    // fseek(pipe, 0L, SEEK_END);
    // int info_buff = ftell(pipe);
    // printf("modinfo size: %d\n", info_buff);

    char* result = (char*)malloc(sizeof(char)*info_buff + 2);
    if (result == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error.\n");
        return NULL;
    }
    fseek(pipe, 0L, SEEK_SET);
    memset(result, 0, info_buff + 2);
    while (!feof(pipe)) {
        if (fgets(buffer, FILE_BUF, pipe)) {
            strcat(result, buffer);
        }
        // printf("?");
    }
    // printf("result: %s", result);
    free(cmd);
    pclose(pipe);
    return result;
}

/*-------------------------------------------------------------------------------
show ModuleList
"mod": the first module point of ModuleList
return "void"
--------------------------------------------------------------------------------*/
void show_module_list(ModuleList* mod) {
    ModuleList* temp = mod;
    int i = 0;
    while (temp != NULL) {
        printf("==== %d ====\n", i++);
        printf("Name: %s\n", temp->Name);
        printf("Path: %s\n", temp->Path);
        printf("Excute: %d\n", temp->Excute);
        printf("Modinfo: %s\n", temp->Modinfo);
        temp = temp->next;
    }
}


/*-------------------------------------------------------------------------------
save ModuleList
"mod": the first module point of ModuleList
"savePath": save_path of ModuleList
return "void"
--------------------------------------------------------------------------------*/
void save_module(ModuleList* mod, char* savePath) {
    FILE *fp;
    if ((fp = fopen(savePath, "w")) == NULL) {
        perror("File open error.");
        // fprintf(stderr, "%s File open error\n", savePath);
        return ;
    }
    ModuleList* tempM = mod;
    int i = 0;
    fprintf(fp, "[\n");
    while (tempM != NULL) {
        // fprintf(fp, "Name: %s\n", tempM->Name);
        // fprintf(fp, "Path: %s\n", tempM->Path);
        // fprintf(fp, "Excute: %d\n", tempM->Excute);
        // fprintf(fp, "Modinfo: %s\n", tempM->Modinfo);
        // fprintf(fp, "== %d over==\n\n", i++);
        fprintf(fp, "{\"ModuleName\":\"%s\",", tempM->Name);
        fprintf(fp, "\"ModuleDir\":\"%s\",", tempM->Path);
        fprintf(fp, "\"Excute\":%d,", tempM->Excute);
        if (tempM->Modinfo != NULL){
            fprintf(fp, "\"Modinfo\":\"%s\"}", tempM->Modinfo);
        }
        else {
            fprintf(fp, "\"Modinfo\":\"\"}");
        }
        tempM = tempM->next;
        if (tempM != NULL) {
            fprintf(fp, ",\n");
        }
    }
    fprintf(fp, "\n]");
    fclose(fp);
}


int free_module_list(ModuleList* mod){
    ModuleList *temp1 = mod, *temp2;
    while(temp1!=NULL){
        temp2 = temp1->next;
        if(temp1->Modinfo != NULL) {
            free(temp1->Modinfo);
            // printf("free modinfo\t");
        }
        // printf("free %s module\n", temp1->Name);
        free(temp1);
        temp1 = temp2;
    }
    temp1 = NULL;
    temp2 = NULL;
    mod = NULL;
    return 0;
}
