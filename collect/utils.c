//
// Created by LQB on 2020/10/17.
//

#include"SysInfo.h"


SetInt* addInt2Set(SetInt *set_head, int src_int) {
    if (set_head == NULL) {
        set_head = (SetInt *) malloc(sizeof(SetInt));
        if (set_head == NULL) {
            perror("memory malloc error!");
            // fprintf(stderr, "Memory malloc error!\n");
            return NULL;
        }
        set_head->item = src_int;
        set_head->idx = 0;
        set_head->next = NULL;
    } else {
        int last_id = 0;
        SetInt *temp = set_head;
        while (temp->next != NULL) {
            if (temp->item == src_int) {
                return set_head;
            }
            temp = temp->next;
        }
        if (temp->item == src_int) {
            return set_head;
        }
        last_id = temp->idx;
        temp->next = (SetInt *) malloc(sizeof(SetInt));
        if (temp->next == NULL) {
            perror("memory malloc error!");
            // fprintf(stderr, "Memory malloc error!\n");
            return NULL;
        }
        temp = temp->next;
        temp->item = src_int;
        temp->idx = last_id + 1;
        temp->next = NULL;
    }
    return set_head;
}

int getSetSize(SetInt *set_head) {
    int last_id = 0;
    if (set_head != NULL) {
        SetInt *temp = set_head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        last_id = temp->idx + 1;
    }
    return last_id;
}

int freeSetInt(SetInt* set_head){
    SetInt* temp = set_head;
    SetInt* cur;
    while(temp!=NULL){
        cur = temp->next;
        free(temp);
        temp = cur;
    }
    set_head = NULL;
    return 0;
}

/*-------------------------------------------------------------------------------
concatenate path
"num": number of subpath
"fmt": subpath
return "char*": concatenated path
--------------------------------------------------------------------------------*/
char* cat_dirs_1(int num, char* fmt, ...) {
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

char* platform() {
    struct utsname u;
    char *res = (char*)malloc(sizeof(char)*SMALESIZE);
    memset(res, 0, sizeof(char)*SMALESIZE);
    if(uname(&u) != 0) {
        strcpy(res, "i386");
    }
    else{
        strcpy(res, u.machine);
    }
    return res;
}