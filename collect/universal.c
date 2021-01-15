#include"HDMstruct.h"

/*-------------------------------------------------------------------------------
get file's softlink
"filePath": source path;  "destBuf": destination path
"sign"=1: devices -> /sys/devices    output path
"sign"=0: drivers -> driver_name    output name
"sign"=2: modules -> /sys/module    output name
return "int": 0 (normal exit)    -1 (abnormal exit)
--------------------------------------------------------------------------------*/
int get_softlink(char* filePath, char* destBuf, int sign) {
    char buf[PATH_LEN];
    struct stat dirstat;
    if (stat(filePath, &dirstat) == -1) {
        strcpy(destBuf, "");
        return -1;
    }
    int res = readlink(filePath, buf, PATH_LEN);
    if (res < 0 || res >= PATH_LEN) {
        perror("Error:Link obtain fail.");
        // fprintf(stderr, "%s Error:Link obtain fail.\n", __FUNCTION__);
        return -1;
    }
    buf[res] = '\0';
    if (sign == 0) {
        strcpy(destBuf, "/sys");
        strcat(destBuf, strstr(buf, "/devices/"));      // return /sys/devices/+++
    }
    else if (sign == 1) {
        strcpy(destBuf, strstr(buf, "/drivers/") + 9);  // return driver name
    }
    else if (sign == 2) {
        strcpy(destBuf, strstr(buf, "/module/") + 8);   // return module name
    }
    return 0;
}

/* -----------------------------------------------------------------------------
get absolute path
"fileDir": dir of target file
"fileName": file name
"absPath": file's absolute path, need pre-alloc space.
return "int": 0(normal), 1(abnormal)
------------------------------------------------------------------------------- */
int get_abs_path(char* fileDir, char* fileName, char* absPath){
    char* filePath = cat_dirs(2, fileDir, fileName); // get file path
    // get softlink of filePath
    char relativePath[PATH_LEN] = {0};
    struct stat dirstat;
    if (stat(filePath, &dirstat) == -1) {
        free(filePath); 
        return -1;
    }
    int res = readlink(filePath, relativePath, PATH_LEN);
    if (res < 0 || res >= PATH_LEN) {
        perror("Error:Link obtain fail.");
        // fprintf(stderr, "%s Error:Link obtain fail.\n", __FUNCTION__);
        free(filePath); 
        return -1;
    }

    char* fullPath = cat_dirs(2, fileDir, relativePath);
    realpath(fullPath, absPath);
    // printf("%s -> %s\n", filePath, relativePath);
    // printf("%s -> %s\n", fullPath, absPath);
    free(fullPath); 
    free(filePath);
    return 0;
}

/*-------------------------------------------------------------------------------
read file via filePath
"filePath": file path
"destBuf": file content
return "char*": file content
--------------------------------------------------------------------------------*/
char* read_file(char* filePath, char* destBuf) {
    FILE* fp = NULL;
    if ((fp = fopen(filePath, "r")) == NULL) {
        perror("File cannot open.");
        return NULL;
        // exit(1);
    }
    else {
        fseek(fp, 0L, SEEK_END);
        int flen;
        flen = ftell(fp);
        destBuf = (char*)malloc(flen + 1);
        memset(destBuf, 0, flen + 1);
        if (destBuf == NULL) {
            perror("memory malloc error!");
            return NULL;
        }
        fseek(fp, 0L, SEEK_SET);
        fread(destBuf, flen, 1, fp);
        destBuf[flen] = '\0';
        fclose(fp);
        return destBuf;
    }
}

/*-------------------------------------------------------------------------------
concatenate path
"num": number of subpath
"fmt": subpath
return "char*": concatenated path
--------------------------------------------------------------------------------*/
char* cat_dirs(int num, char* fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    char* res = (char*)malloc(sizeof(char) * PATH_LEN);
    char* temp = (char*)malloc(sizeof(char) * PATH_LEN);
    if (res == NULL || temp == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error!\n");
        return NULL;
    }
    memset(res, 0, PATH_LEN);
    memset(temp, 0, PATH_LEN);
    strcpy(res, fmt);   // the first path
    if ((strlen(res) > 0) && (res[strlen(res) - 1] == '/')) {
        res[strlen(res) - 1] = '\0';    // remove the end char '/'
    }
    int i;
    for (i = 1; i < num; i++) {
        strcpy(temp, va_arg(argp, char*));
        if(strlen(temp) == 0)
            continue;
        if (temp[0] != '/') {
            strcat(res, "/");
        }
        if(strlen(temp) == 0)
            continue;
        if (temp[strlen(temp) - 1] == '/') {
            temp[strlen(temp) - 1] = '\0';  // remove the end char '/'
        }
        strcat(res, temp);
    }
    free(temp);
    // printf("cat_dirs: %s\n", res);
    return res;
}

/*-------------------------------------------------------------------------------
concatenate several string
"size": preallocated memory size
"num": number of string
"fmt": string
return "char*": concatenated string
--------------------------------------------------------------------------------*/
char* cat_strs(int size, int num, char* fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    char* res = (char*)malloc(sizeof(char) * size);
    if (res == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error!\n");
        return NULL;
    }
    memset(res, 0, sizeof(char) * size);
    strcpy(res, fmt);
    int i;
    for (i = 1; i < num; i++)
        strcat(res, va_arg(argp, char*));       // argp find next char* value
    va_end(argp);
    return res;
}

/*-------------------------------------------------------------------------------
add name to NameList
"nameList": destination
"name": name to be added
return "NameList*": pointer of nameList
--------------------------------------------------------------------------------*/
NameList* add_to_namelist(NameList* nameList, char* name) {
    if (nameList == NULL) {     
        nameList = (NameList*)malloc(sizeof(NameList));
        if (nameList == NULL) {
            perror("memory malloc error!");
            // fprintf(stderr, "Memory malloc error!\n");
            return NULL;
        }
        memset(nameList, 0, sizeof(NameList));
        strcpy(nameList->Name, name);
        nameList->next = NULL;
    }
    else {
        NameList* temp = nameList;
        while (temp->next != NULL) {
            if (strcmp(temp->Name, name) == 0) {    // compare name with namelist
                return nameList;
            }
            temp = temp->next;
        }
        if (strcmp(temp->Name, name) == 0) {
            return nameList;
        }
        temp->next = (NameList*)malloc(sizeof(NameList));
        if (temp->next == NULL) {
            perror("memory malloc error!");
            // fprintf(stderr, "Memory malloc error!\n");
            return NULL;
        }
        memset(temp->next, 0, sizeof(NameList));
        temp = temp->next;
        strcpy(temp->Name, name);
        temp->next = NULL;
    }
    return nameList;
}

/*-------------------------------------------------------------------------------
add path to PathList
"pathList": destination
"path": path to be added
return "PathList*": pointer of pathList
--------------------------------------------------------------------------------*/
PathList* add_to_pathlist(PathList* pathList, char* path) {
    if (pathList == NULL) {
        pathList = (PathList*)malloc(sizeof(PathList));        
        if (pathList == NULL) {
            perror("memory malloc error!");
            // fprintf(stderr, "Memory malloc error!\n");
            return NULL;
        }
        memset(pathList, 0, sizeof(pathList));
        strcpy(pathList->Path, path);
        pathList->next = NULL;
    }
    else {
        PathList* temp = pathList;
        while (temp->next != NULL) {
            if (strcmp(temp->Path, path) == 0){
                return pathList;
            }
            temp = temp->next;
        }
        if (strcmp(temp->Path, path) == 0){
            return pathList;
        }
        temp->next = (PathList*)malloc(sizeof(PathList));
        if (temp->next == NULL) {
            perror("memory malloc error!");
            // fprintf(stderr, "Memory malloc error!\n");
            return NULL;
        }
        memset(temp->next, 0, sizeof(pathList));
        temp = temp->next;
        strcpy(temp->Path, path);
        temp->next = NULL;
    }
    return pathList;
}

/*-------------------------------------------------------------------------------

--------------------------------------------------------------------------------*/
void showNameList(NameList* nameList) {
    NameList* temp = nameList;
    while (temp != NULL) {
        printf("%s, ", temp->Name);
        temp = temp->next;
    }
    printf("\n");
}

/*-------------------------------------------------------------------------------

--------------------------------------------------------------------------------*/
void showPathList(PathList* pathList) {
    PathList* temp = pathList;
    while (temp != NULL) {
        printf("%s\n", temp->Path);
        temp = temp->next;
    }
    printf("\n");
}

/*-------------------------------------------------------------------------------

--------------------------------------------------------------------------------*/
int freeNameList(NameList* nList) {
    NameList *temp1 = nList, *temp2;
    nList = NULL;
    if (temp1 == NULL) {
        return 0;
    }
    else {
        while (temp1->next) {
            temp2 = temp1->next;
            // printf("%s free\n", temp1->Name);
            free(temp1->Name);
            temp1 = temp2;
        }
        // printf("%s free\n", temp1->Name);
        free(temp1->Name);
        temp1 = NULL;
        temp2 = NULL;
        return 0;
    }
    return -1;
}

/*-------------------------------------------------------------------------------

--------------------------------------------------------------------------------*/
int freePathList(PathList* pList) {
    PathList *temp1 = pList, *temp2;
    pList = NULL;
    if (temp1 == NULL) {
        return 0;
    }
    else {
        while (temp1->next) {
            temp2 = temp1->next;
            // printf("%s free\n", temp1->Path);
            free(temp1->Path);
            temp1 = temp2;
        }
        // printf("%s free\n", temp1->Path);
        free(temp1->Path);
        temp1 = NULL;
        temp2 = NULL;
        return 0;
    }
    return -1;
}
