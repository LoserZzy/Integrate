//
// Created by LQB on 2020/10/17.
//

#include"SysInfo.h"

int cpu_info_file(CpuInfo *cpu_list);

// int cpu_info_cmd(CpuInfo* cpu_list);
int net_info_struct(NetInfo *net_list);
int net_info_struct2(NetInfo *net_list);
int get_netunit_via_name (char *name, NetUnit *net_unit);

//int net_info_cmd(NetInfo* net_list);
int mem_info_file(MemInfo *mem_list);

//int mem_info_cmd(MemInfo* mem_list);
int disk_info_file(DiskInfo *disk_list);

//int disk_info_cmd(DiskInfo* disk_list);
int bios_info_file(BiosInfo *bios_list);

//int bios_info_cmd(BiosInfo* bios_list);
void show_cpu_info(CpuInfo *cpu_list);

void show_net_info(NetInfo *net_list);

void show_mem_info(MemInfo *mem_list);

void show_disk_info(DiskInfo *disk_list);

void show_bios_info(BiosInfo *bios_list);

int free_cpu_info(CpuInfo *cpu_list);

int free_net_info(NetInfo *net_list);

int free_mem_info(MemInfo *mem_list);

int free_disk_info(DiskInfo *disk_list);

int free_bios_info(BiosInfo *bios_list);

int free_mem_info_s(MemInfoS *mem_list_s);

int free_disk_info_s(DiskInfoS *disk_list_s);

int cpu_info(char *dir) {
    CpuInfo *cpu_via_file = (CpuInfo *) malloc(sizeof(CpuInfo));
    if (cpu_via_file == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error!\n");
        return -1;
    }
    memset(cpu_via_file, 0, sizeof(CpuInfo));
    cpu_via_file->core_num = 0;
    cpu_via_file->logic_num = 0;
    cpu_via_file->phy_num = 0;
    strcpy(cpu_via_file->architecture, platform());
    cpu_via_file->cpuuint = (CpuUnit *) malloc(sizeof(CpuUnit));
    if (cpu_via_file->cpuuint == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error!\n");
        return -1;
    }
    memset(cpu_via_file->cpuuint, 0, sizeof(CpuUnit));
    cpu_via_file->cpuuint->all_info = (char *) malloc(sizeof(char) * MAXSIZE);
    if (cpu_via_file->cpuuint->all_info == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error!\n");
        return -1;
    }
    memset(cpu_via_file->cpuuint->all_info, 0, sizeof(char) * MAXSIZE);
    //cpu_via_file->cpuuint->all_info = NULL;
    cpu_via_file->cpuuint->next = NULL;
    cpu_via_file->all_info = NULL;
    cpu_info_file(cpu_via_file);
    // show_cpu_info(cpu_via_file);
    char *save_path = cat_dirs_1(2, dir, SAVE_CPU_NAME);
    save_cpu_info(save_path, cpu_via_file);
    free_cpu_info(cpu_via_file);
    free(save_path);
    return 0;
}

int cpu_info_file(CpuInfo *cpu_list) {
    FILE *fp = fopen(CPUPATH, "r");
    if (fp == NULL) {
        perror("file open error");
        return -1;
    }
    // current line content
    char cur_line[MIDDLESIZE];
    // cpuinfo's attribute name
    char cur_name[SMALESIZE];
    memset(cur_line, 0, sizeof(char) * MIDDLESIZE);
    CpuUnit *cur_cpu = cpu_list->cpuuint;
    int malloc_flag = 0;
    while (fgets(cur_line, MIDDLESIZE - 1, fp)) {
        memset(cur_name, 0, sizeof(char) * SMALESIZE);
        if (strlen(cur_line) < 2) {
            // information of a cpu analysis complete
            cpu_list->logic_num++;
            //printf("logic cpu nums: %d,  physical cpu nums: %d\n", cpu_list->logic_num, cpu_list->phy_num);
            malloc_flag = 1;
            continue;
        } else if (malloc_flag == 1) {
            char first_item[10] = {0};
            if (strlen(cur_line) > 9){
                strncpy(first_item, cur_line, 9);
                // 判断前几个字符
                if (strcmp(first_item, "processor") == 0 || strcmp(first_item, "Processor") == 0){
                    malloc_flag = 0;
                    cur_cpu->next = (CpuUnit *) malloc(sizeof(CpuUnit));
                    if (cur_cpu->next == NULL) {
                        perror("memory malloc error!");
                        return -1;
                    }
                    cur_cpu = cur_cpu->next;
                    memset(cur_cpu, 0, sizeof(CpuUnit));
                    cur_cpu->all_info = (char *) malloc(sizeof(char) * MAXSIZE);
                    if (cur_cpu->all_info == NULL) {
                        perror("memory malloc error!");
                        return -1;
                    }
                    memset(cur_cpu->all_info, 0, sizeof(char) * MAXSIZE);
                    cur_cpu->next = NULL;
                }
                else {
                    // ---------------------------------------------------------------
                    // todo
                    continue;
                    // break;
                }
            }
        }
        strcat(cur_cpu->all_info, cur_line);    // save all line info
        int line_flag = 1;
        for (int i = 0; i < strlen(cur_line) && i < SMALESIZE; i++) {
            if (cur_line[i] == '\t') {
                line_flag = 0;
            } else if (line_flag == 1) {
                cur_name[i] = cur_line[i];
            }
            if (line_flag == 0 && cur_line[i] == ':') {
                i += 2;
                if (strcmp(cur_name, "processor") == 0) {
                    char processor[8] = {0};
                    strncpy(processor, cur_line + i, strlen(cur_line) - i - 1);
                    cur_cpu->processor = atoi(processor);
                    cur_cpu->disabled = 1;  // /proc/cpuinfo下的cpu都是启用状态
                    //printf("core id: %d\n", cur_cpu->core_id);
                } else if (strcmp(cur_name, "model name") == 0) {
                    strncpy(cur_cpu->name, cur_line + i, strlen(cur_line) - i - 1);
                    //printf("cpu name: %s\n", cur_cpu->name);
                } else if (strcmp(cur_name, "vendor_id") == 0) {
                    strncpy(cur_cpu->vendor, cur_line + i, strlen(cur_line) - i - 1);
                    //printf("vendor_id: %s\n", cur_cpu->vendor);
                } else if (strcmp(cur_name, "cpu cores") == 0) {
                    char cpu_cores[8] = {0};
                    strncpy(cpu_cores, cur_line + i, strlen(cur_line) - i - 1);
                    cpu_list->core_num = atoi(cpu_cores);
                    //printf("cpu cores: %d\n", cpu_list->core_num);
                }
                break;
            }
        }
    }
    fclose(fp);
    return 0;
}

int cpu_info_file_x86(CpuInfo *cpu_list) {
    FILE *fp = fopen(CPUPATH, "r");
    if (fp == NULL) {
        perror("file open error");
        return -1;
    }
    // current line content
    char cur_line[MIDDLESIZE];
    // cpuinfo's attribute name
    char cur_name[SMALESIZE];
    memset(cur_line, 0, sizeof(char) * MIDDLESIZE);
    CpuUnit *cur_cpu = cpu_list->cpuuint;
    int malloc_flag = 0;
    SetInt *phy_set = NULL;
    while (fgets(cur_line, MIDDLESIZE - 1, fp)) {
        memset(cur_name, 0, sizeof(char) * SMALESIZE);
        if (strlen(cur_line) < 2) {
            // information of a cpu analysis complete
            cpu_list->logic_num++;
            cpu_list->phy_num = getSetSize(phy_set);
            //printf("logic cpu nums: %d,  physical cpu nums: %d\n", cpu_list->logic_num, cpu_list->phy_num);
            malloc_flag = 1;
            continue;
        } else if (malloc_flag == 1) {
            malloc_flag = 0;
            cur_cpu->next = (CpuUnit *) malloc(sizeof(CpuUnit));
            if (cur_cpu->next == NULL) {
                perror("memory malloc error!");
                // fprintf(stderr, "Memory malloc error!\n");
                return -1;
            }
            cur_cpu = cur_cpu->next;
            memset(cur_cpu, 0, sizeof(CpuUnit));
            cur_cpu->all_info = (char *) malloc(sizeof(char) * MAXSIZE);
            if (cur_cpu->all_info == NULL) {
                perror("memory malloc error!");
                return -1;
            }
            memset(cur_cpu->all_info, 0, sizeof(char) * MAXSIZE);
            cur_cpu->next = NULL;
        }
        strcat(cur_cpu->all_info, cur_line);    // save all line info
        int line_flag = 1;
        for (int i = 0; i < strlen(cur_line) && i < SMALESIZE; i++) {
            if (cur_line[i] == '\t') {
                line_flag = 0;
            } else if (line_flag == 1) {
                cur_name[i] = cur_line[i];
            }
            if (line_flag == 0 && cur_line[i] == ':') {
                i += 2;
                if (strcmp(cur_name, "model name") == 0) {
                    strncpy(cur_cpu->name, cur_line + i, strlen(cur_line) - i - 1);
                    //printf("cpu name: %s\n", cur_cpu->name);
                } else if (strcmp(cur_name, "vendor_id") == 0) {
                    strncpy(cur_cpu->vendor, cur_line + i, strlen(cur_line) - i - 1);
                    //printf("vendor_id: %s\n", cur_cpu->vendor);
                } else if (strcmp(cur_name, "physical id") == 0) {
                    char physical_id[8] = {0};
                    strncpy(physical_id, cur_line + i, strlen(cur_line) - i - 1);
                    phy_set = addInt2Set(phy_set, atoi(physical_id));
                    //printf("physical id: %d\n", cur_cpu->phy_id);
                } else if (strcmp(cur_name, "cpu cores") == 0) {
                    char cpu_cores[8] = {0};
                    strncpy(cpu_cores, cur_line + i, strlen(cur_line) - i - 1);
                    cpu_list->core_num = atoi(cpu_cores);
                    //printf("cpu cores: %d\n", cpu_list->core_num);
                }
                break;
            }
        }
    }
    fclose(fp);
    freeSetInt(phy_set);
    //free(cur_name);
    //cur_name=NULL;
    return 0;
}

int net_info(char *dir) {
    NetInfo *net_via_struct = (NetInfo *) malloc(sizeof(NetInfo));
    if (net_via_struct == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error!\n");
        return -1;
    }
    memset(net_via_struct, 0, sizeof(NetInfo));
    net_via_struct->all_info = NULL;
    // ---------------------------------------------------------
    // net_via_struct->netunit = (NetUnit *) malloc(sizeof(NetUnit));
    // if (net_via_struct->netunit == NULL) {
    //     perror("memory malloc error!");
    //     // fprintf(stderr, "Memory malloc error!\n");
    //     return -1;
    // }
    // memset(net_via_struct->netunit, 0, sizeof(NetUnit));
    // net_via_struct->netunit->next = NULL;
    // net_info_struct(net_via_struct);
    // ---------------------------------------------------------
    net_info_struct2(net_via_struct);
    //show_net_info(net_via_struct);
    char *save_path = cat_dirs_1(2, dir, SAVE_NET_NAME);
    save_net_info(save_path, net_via_struct);
    free(save_path);
    free_net_info(net_via_struct);
    return 0;
}

int net_info_struct(NetInfo *net_list) {
    gethostname(net_list->host_name, sizeof(net_list->host_name));
    int fd;
    int interface_num = 0;
    struct ifreq buf[16];
    struct ifconf ifc;
    struct ifreq ifrcopy;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        close(fd);
        return -1;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t) buf;
    if (!ioctl(fd, SIOCGIFCONF, (char *) &ifc)) {
        interface_num = ifc.ifc_len / sizeof(struct ifreq);
        // printf("interface num = %d\n", interface_num);
        NetUnit *cur_net = net_list->netunit;
        while (interface_num-- > 0) {
            sprintf(cur_net->net_name, "%s", buf[interface_num].ifr_name);
            //printf("device name: %s\n", buf[interface_num].ifr_name);
            //ignore the interface that not up or not runing
            ifrcopy = buf[interface_num];
            if (ioctl(fd, SIOCGIFFLAGS, &ifrcopy)) {
                perror("iotcl error in netcard info");
                // fprintf(stderr,"ioctl: [%s:%d]\n", __FILE__, __LINE__);
                close(fd);
                return -1;
            }
            //get the mac of this interface
            if (!ioctl(fd, SIOCGIFHWADDR, (char *) (&buf[interface_num]))) {
                sprintf(cur_net->mac, "%02x:%02x:%02x:%02x:%02x:%02x",
                        (unsigned char) buf[interface_num].ifr_hwaddr.sa_data[0],
                        (unsigned char) buf[interface_num].ifr_hwaddr.sa_data[1],
                        (unsigned char) buf[interface_num].ifr_hwaddr.sa_data[2],
                        (unsigned char) buf[interface_num].ifr_hwaddr.sa_data[3],
                        (unsigned char) buf[interface_num].ifr_hwaddr.sa_data[4],
                        (unsigned char) buf[interface_num].ifr_hwaddr.sa_data[5]);
                //printf("device mac: %s\n", cur_net->mac);
            } else {
                perror("iotcl error in netcard info");
                // fprintf(stderr, "ioctl: [%s:%d]\n", __FILE__, __LINE__);
                close(fd);
                return -1;
            }

            //get the IP of this interface
            if (!ioctl(fd, SIOCGIFADDR, (char *) &buf[interface_num])) {
                sprintf(cur_net->ip_addr, "%s",
                        (char *) inet_ntoa(((struct sockaddr_in *) &(buf[interface_num].ifr_addr))->sin_addr));
            } else {
                perror("iotcl error in netcard info");
                // fprintf(stderr, "ioctl: [%s:%d]\n", __FILE__, __LINE__);
                close(fd);
                return -1;
            }

            //get the broad address of this interface
            if (!ioctl(fd, SIOCGIFBRDADDR, &buf[interface_num])) {
                sprintf(cur_net->broad_addr, "%s",
                        (char *) inet_ntoa(((struct sockaddr_in *) &(buf[interface_num].ifr_broadaddr))->sin_addr));
            } else {
                perror("iotcl error in netcard info");
                // fprintf(stderr, "ioctl: [%s:%d]\n", __FILE__, __LINE__);
                close(fd);
                return -1;
            }

            //get the subnet mask of this interface
            if (!ioctl(fd, SIOCGIFNETMASK, &buf[interface_num])) {
                sprintf(cur_net->subnet_mask, "%s",
                        (char *) inet_ntoa(((struct sockaddr_in *) &(buf[interface_num].ifr_netmask))->sin_addr));
            } else {
                perror("iotcl error in netcard info");
                // fprintf(stderr, "ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return -1;
            }
            if (interface_num > 0) {
                cur_net->next = (NetUnit *) malloc(sizeof(NetUnit));
                if (cur_net->next == NULL) {
                    perror("memory malloc error!");
                    // fprintf(stderr, "Memory malloc error!\n");
                    return -1;
                }
                cur_net = cur_net->next;
                memset(cur_net, 0, sizeof(NetUnit));
                cur_net->next = NULL;
            }
        }
    } else {
        perror("iotcl error in netcard info");
        // fprintf(stderr,"ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int net_info_struct2(NetInfo *net_list) {
    gethostname(net_list->host_name, sizeof(net_list->host_name));
    struct sockaddr_in *sin = NULL;
    struct ifaddrs *ifa = NULL, *ifList;
    if (getifaddrs(&ifList) < 0) {
        return -1;
    }
    NetUnit* cur_unit = net_list->netunit;
    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family == AF_PACKET) {
            if (net_list->netunit == NULL) {
                net_list->netunit = (NetUnit*)malloc(sizeof(NetUnit));
                if (net_list->netunit == NULL) {
                    perror("memory malloc error!");
                    return -1;
                }
                memset(net_list->netunit, 0, sizeof(NetUnit));
                cur_unit = net_list->netunit;
                cur_unit->next = NULL;
            }
            else {
                cur_unit->next = (NetUnit*)malloc(sizeof(NetUnit));
                if (cur_unit->next == NULL) {
                    perror("memory malloc error!");
                    return -1;
                }
                memset(cur_unit->next, 0, sizeof(NetUnit));
                cur_unit = cur_unit->next;
                cur_unit->next = NULL;
            }
            get_netunit_via_name(ifa->ifa_name, cur_unit);
        }
    }

}

int get_netunit_via_name (char *name, NetUnit *net_unit) {
    strcpy(net_unit->net_name, name);
    int socketfd;
    struct sockaddr_in sin;
    struct sockaddr_in netmask;
    struct sockaddr_in broad;
    struct ifreq ifr;
    unsigned char arp[6];
    if(!name || !*name || !net_unit) {
        return -1;
    }
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd == -1) {
        perror("socket error");
        close(socketfd);
        return -1;
    }
    strcpy(ifr.ifr_name, name);
    // strncpy(ifr.ifr_name, name, IF_NAMESIZE);  // IF_NAMESIZE = 16 define in <net/if.h>
    if (ioctl(socketfd, SIOCGIFADDR, &ifr) == 0) {
        memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
        sprintf(net_unit->ip_addr, "%s", inet_ntoa(sin.sin_addr));
        // sprintf(net_unit->ip_addr, "%s", (char*)inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
        // printf("### Debug: %s ip_addr: %s\n", name, net_unit->ip_addr);
    }
    // else {
    //     printf("### Error: %s doesn't have ip4 addr!\n", name);
    // }
    if (ioctl(socketfd, SIOCGIFHWADDR, &ifr) == 0) {
        memcpy(arp, ifr.ifr_hwaddr.sa_data, 6);
        sprintf(net_unit->mac, "%02x:%02x:%02x:%02x:%02x:%02x",arp[0],arp[1],arp[2],arp[3],arp[4],arp[5]);
        // sprintf(net_unit->mac, "%02x:%02x:%02x:%02x:%02x:%02x",
        // (unsigned char)ifr.ifr_hwaddr.sa_data[0],
        // (unsigned char)ifr.ifr_hwaddr.sa_data[1],
        // (unsigned char)ifr.ifr_hwaddr.sa_data[2],
        // (unsigned char)ifr.ifr_hwaddr.sa_data[3],
        // (unsigned char)ifr.ifr_hwaddr.sa_data[4],
        // (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
        // printf("### Debug: %s mac: %s\n", name, net_unit->mac);
    }
    // else {
    //     printf("### Error: %s doesn't have mac addr!\n", name);
    // }
    if (ioctl(socketfd, SIOCGIFNETMASK, &ifr) == 0) {
        memcpy(&netmask, &ifr.ifr_netmask, sizeof(netmask));
        sprintf(net_unit->subnet_mask, "%s", inet_ntoa(netmask.sin_addr));
        // sprintf(net_unit->subnet_mask, "%s", (char*)inet_ntoa(((struct sockaddr_in*)&ifr.ifr_netmask)->sin_addr));
        // printf("### Debug: %s subnet_mask: %s\n", name, net_unit->subnet_mask);
    }
    // else {
    //     printf("### Error: %s doesn't have netmask addr!\n", name);
    // }
    if (ioctl(socketfd, SIOCGIFBRDADDR, &ifr) == 0) {
        memcpy(&broad, &ifr.ifr_broadaddr, sizeof(broad));
        sprintf(net_unit->broad_addr, "%s", inet_ntoa(broad.sin_addr));
        // sprintf(net_unit->broad_addr, "%s", (char*)inet_ntoa(((struct sockaddr_in*)&ifr.ifr_broadaddr)->sin_addr));
        // printf("### Debug: %s broad_addr: %s\n", name, net_unit->broad_addr);
    }
    // else {
    //     printf("### Error: %s doesn't have broad addr!\n", name);
    // }
    return 0;
}

int disk_info(char *dir) {
    DiskInfo *disk_via_file = (DiskInfo *) malloc(sizeof(DiskInfo));
    if (disk_via_file == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error!\n");
        return -1;
    }
    memset(disk_via_file, 0, sizeof(DiskInfo));
    disk_via_file->next = NULL;
    disk_info_file(disk_via_file);
    //show_disk_info(disk_via_file);
    char *save_path = cat_dirs_1(2, dir, SAVE_DISK_NAME);
    save_disk_info(save_path, disk_via_file->next);
    free_disk_info(disk_via_file);
    DiskInfoS *disk_via_cmd = NULL;
    memset(save_path, 0, sizeof(char) * PATH_LEN);
    strcpy(save_path, cat_dirs_1(2, dir, SAVE_DISK_NAME_S));
    save_disk_info_s(save_path, disk_via_cmd);
    free_disk_info_s(disk_via_cmd);
    free(save_path);
    return 0;
}

int disk_info_file(DiskInfo *disk_list) {
    FILE *fp = fopen(MOUNTPATH, "r");
    if (fp == NULL) {
        perror("failed to open file.");
        // fprintf(stderr, "failed to open %s.\n", MOUNTPATH);
        return -1;
    }
    char file_sys[40];
    char cur_line[200];
    char *cur_c, *e = cur_line;
    DiskInfo *temp_disk = disk_list;
    while (fgets(cur_line, 200, fp)) {
        memset(file_sys, 0, sizeof(file_sys));
        int i = 0;
        for (cur_c = cur_line; *cur_c; cur_c++) {
            if (*cur_c == ' ') {
                e = cur_c + 1;
                break;
            } else {
                file_sys[i] = *cur_c;
                i++;
            }
        }
        for (cur_c = e; *cur_c; cur_c++) {
            if (*cur_c == ' ') {
                *cur_c = '\0';
                break;
            }
        }
        // df part start -----------------------------------
        struct statfs st;
        if (statfs(e, &st) < 0) {
            perror("statfs get error.");
            // fprintf(stderr, "%s: %s\n", e, strerror(errno));
            continue;
        }
        if (st.f_blocks != 0) {
            temp_disk->next = (DiskInfo *) malloc(sizeof(DiskInfo));
            if (temp_disk->next == NULL) {
                perror("memory malloc error!");
                return -1;
            }
            memset(temp_disk->next, 0, sizeof(DiskInfo));
            temp_disk = temp_disk->next;
            temp_disk->next = NULL;
            strcpy(temp_disk->fs_type, file_sys);
            strcpy(temp_disk->mount_path, e);
            temp_disk->block_size = st.f_bsize;
            temp_disk->total = ((long long) st.f_blocks * st.f_bsize) / 1024;
            temp_disk->available = ((long long) st.f_bfree * st.f_bsize) / 1024;
            temp_disk->used = ((long long) (st.f_blocks - st.f_bfree) * st.f_bsize) / 1024;
        }
        // df part end -------------------------------------
    }
    fclose(fp);
    return 0;
}

int mem_info(char *dir){
    MemInfo *mem_via_file = (MemInfo *) malloc(sizeof(MemInfo));
    if (mem_via_file == NULL) {
        perror("memory malloc error!");
        // fprintf(stderr, "Memory malloc error!\n");
        return -1;
    }
    memset(mem_via_file, 0, sizeof(MemInfo));
    mem_via_file->all_info = NULL;
    mem_info_file(mem_via_file);
    //show_mem_info(mem_via_file);
    char *save_path = cat_dirs_1(2, dir, SAVE_MEM_NAME);
    save_mem_info(save_path, mem_via_file);
    free_mem_info(mem_via_file);
    MemInfoS *mem_via_cmd = NULL;
    memset(save_path, 0, sizeof(char) * PATH_LEN);
    strcpy(save_path, cat_dirs_1(2, dir, SAVE_MEM_NAME_S));
    save_mem_info_s(save_path, mem_via_cmd);
    free_mem_info_s(mem_via_cmd);
    free(save_path);
    return 0;
}

int mem_info_file(MemInfo *mem_list) {
    struct sysinfo s_info;
    // On success, zero is returned. On error, -1 is returned, and errno is set appropriately.
    if (sysinfo(&s_info) != 0) {
        perror("sysinfo get error");
        // fprintf(stderr, "sysinfo() get error %s\n", strerror(errno));
        return -1;
    }
    mem_list->total_ram = s_info.totalram;
    mem_list->free_ram = s_info.freeram;
    mem_list->shared_ram = s_info.sharedram;
    mem_list->buffer_ram = s_info.bufferram;
    mem_list->total_swap = s_info.totalswap;
    mem_list->free_swap = s_info.freeswap;
    return 0;
}

int bios_info(char *dir) {
    BiosInfo *bios_via_file = (BiosInfo *) malloc(sizeof(BiosInfo));
    if (bios_via_file == NULL) {
        perror("memory malloc error!");
        return -1;
    }
    memset(bios_via_file, 0, sizeof(BiosInfo));
    bios_via_file->all_info = NULL;
    int ret_b = bios_info_file(bios_via_file);
    //show_bios_info(bios_via_file);
    char *save_path = cat_dirs_1(2, dir, SAVE_BIOS_NAME);
    save_bios_info(save_path, bios_via_file, ret_b);
    free_bios_info(bios_via_file);
    free(save_path);
    return 0;
}

int bios_info_file(BiosInfo *bios_list) {
    FILE *fp = NULL;
    if ((fp = fopen(BIOSDATEPATH, "r")) == NULL) {
        perror("file open error");
        return -1;
    }
    fgets(bios_list->release_date, sizeof(bios_list->release_date), fp);
    fclose(fp);
    if ((fp = fopen(BIOSVENDORPATH, "r")) == NULL) {
        perror("file open error");
        return -1;
    }
    fgets(bios_list->vendor, sizeof(bios_list->vendor), fp);
    fclose(fp);
    if ((fp = fopen(BIOSVERSIONPATH, "r")) == NULL) {
        perror("file open error");
        return -1;
    }
    fgets(bios_list->version, sizeof(bios_list->version), fp);
    fclose(fp);
    return 0;
}

int save_cpu_info(char *save_path, CpuInfo *cpu_list) {
    FILE *fp;
    if ((fp = fopen(save_path, "w")) == NULL) {
        perror("file open error");
        return -1;
    }
    CpuUnit *cpu_unit = cpu_list->cpuuint;
    fprintf(fp, "[\n");
    while (cpu_unit != NULL) {
        fprintf(fp, "{\"logic_nums\":%d,", cpu_list->logic_num);
        fprintf(fp, "\"core_nums\":%d,", cpu_list->core_num);
        fprintf(fp, "\"physical_nums\":%d,", cpu_list->phy_num);
        fprintf(fp, "\"architecture\":\"%s\",", cpu_list->architecture);
        fprintf(fp, "\"cpu_name\":\"%s\",", cpu_unit->name);
        fprintf(fp, "\"vendor\":\"%s\",", cpu_unit->vendor);
        fprintf(fp, "\"disabled\":%d,", cpu_unit->disabled);
        fprintf(fp, "\"processor\":%d,", cpu_unit->processor);
        // ----------------------------------------------------------
        fprintf(fp, "\"physid\":\"%s\",", cpu_unit->physid);
        fprintf(fp, "\"description\":\"%s\",", cpu_unit->description);
        fprintf(fp, "\"product\":\"%s\",", cpu_unit->product);
        fprintf(fp, "\"vendor_name\":\"%s\",", cpu_unit->vendor_name);
        fprintf(fp, "\"businfo\":\"%s\",", cpu_unit->businfo);
        fprintf(fp, "\"version\":\"%s\",", cpu_unit->version);
        fprintf(fp, "\"serial\":\"%s\",", cpu_unit->serial);
        // ----------------------------------------------------------
        fprintf(fp, "\"other_info\":\"%s\"}", cpu_unit->all_info);
        cpu_unit = cpu_unit->next;
        if (cpu_unit != NULL) {
            fprintf(fp, ",\n");
        }
    }
    fprintf(fp, "\n]");
    fclose(fp);
    return 0;
}

int save_net_info(char *save_path, NetInfo *net_list) {
    FILE *fp;
    if ((fp = fopen(save_path, "w")) == NULL) {
        perror("file open error");
        // fprintf(stderr, "%s File open error %d - %s\n", save_path, errno, strerror(errno));
        return -1;
    }
    NetUnit *net_unit = net_list->netunit;
    fprintf(fp, "[\n");
    while (net_unit != NULL) {
        fprintf(fp, "{\"host_name\":\"%s\",", net_list->host_name);
        fprintf(fp, "\"net_name\":\"%s\",", net_unit->net_name);
        fprintf(fp, "\"mac_addr\":\"%s\",", net_unit->mac);
        fprintf(fp, "\"ip_addr\":\"%s\",", net_unit->ip_addr);
        fprintf(fp, "\"broad_addr\":\"%s\",", net_unit->broad_addr);
        fprintf(fp, "\"subnet_mask\":\"%s\",", net_unit->subnet_mask);
        // ----------------------------------------------------------
        fprintf(fp, "\"physid\":\"%s\",", net_unit->physid);
        fprintf(fp, "\"description\":\"%s\",", net_unit->description);
        fprintf(fp, "\"product\":\"%s\",", net_unit->product);
        fprintf(fp, "\"vendor\":\"%s\",", net_unit->vendor);
        fprintf(fp, "\"serial\":\"%s\",", net_unit->serial);
        // ----------------------------------------------------------
        if(net_list->all_info!=NULL){
            fprintf(fp, "\"other_info\":\"%s\"}", net_list->all_info);
        } else{
            fprintf(fp, "\"other_info\":\"\"}");
        }
        net_unit = net_unit->next;
        if (net_unit != NULL) {
            fprintf(fp, ",\n");
        }
    }
    fprintf(fp, "\n]");
    fclose(fp);
    return 0;
}

int save_disk_info(char *save_path, DiskInfo *disk_list) {
    FILE *fp;
    if ((fp = fopen(save_path, "w")) == NULL) {
        perror("file open error");
        return -1;
    }
    DiskInfo *temp_disk = disk_list;
    fprintf(fp, "[\n");
    while (temp_disk != NULL) {
        fprintf(fp, "{\"fs_type\":\"%s\",", temp_disk->fs_type);
        fprintf(fp, "\"mount_path\":\"%s\",", temp_disk->mount_path);
        fprintf(fp, "\"available\":%lu,", temp_disk->available);
        fprintf(fp, "\"total_size\":%lu,", temp_disk->total);
        fprintf(fp, "\"used_size\":%lu,", temp_disk->used);
        fprintf(fp, "\"block_size\":%u}", temp_disk->block_size);
        // ----------------------------------------------------------
        
        // ----------------------------------------------------------
        temp_disk = temp_disk->next;
        if (temp_disk != NULL) {
            fprintf(fp, ",\n");
        }
    }
    fprintf(fp, "\n]");
    fclose(fp);
    return 0;
}

int save_disk_info_s(char *save_path, DiskInfoS *disk_list_s) {
    FILE *fp;
    if ((fp = fopen(save_path, "w")) == NULL) {
        perror("file open error");
        return -1;
    }
    DiskInfoS *temp_disk_s = disk_list_s;
    fprintf(fp, "[\n");
    while (temp_disk_s != NULL) {
        fprintf(fp, "{\"physid\":\"%s\",", temp_disk_s->physid);
        fprintf(fp, "\"description\":\"%s\",", temp_disk_s->description);
        fprintf(fp, "\"product\":\"%s\",", temp_disk_s->product);
        fprintf(fp, "\"vendor\":\"%s\",", temp_disk_s->vendor);
        fprintf(fp, "\"version\":\"%s\",", temp_disk_s->version);
        fprintf(fp, "\"businfo\":\"%s\",", temp_disk_s->businfo);
        fprintf(fp, "\"serial\":\"%s\",", temp_disk_s->serial);
        fprintf(fp, "\"total_size\":\"%s\"}", temp_disk_s->total_size);
        temp_disk_s = temp_disk_s->next;
        if (temp_disk_s != NULL) {
            fprintf(fp, ",\n");
        }
    }
    fprintf(fp, "\n]");
    fclose(fp);
    return 0;
}

int save_mem_info(char *save_path, MemInfo *mem_list) {
    FILE *fp;
    if ((fp = fopen(save_path, "w")) == NULL) {
        perror("file open error");
        // fprintf(stderr, "%s File open error %d - %s\n", save_path, errno, strerror(errno));
        return -1;
    }
    fprintf(fp, "[\n");
    fprintf(fp, "{\"total_ram\":%lu,", mem_list->total_ram);
    fprintf(fp, "\"free_ram\":%lu,", mem_list->free_ram);
    fprintf(fp, "\"shared_ram\":%lu,", mem_list->shared_ram);
    fprintf(fp, "\"buffer_ram\":%lu,", mem_list->buffer_ram);
    fprintf(fp, "\"total_swap\":%lu,", mem_list->total_swap);
    fprintf(fp, "\"free_swap\":%lu,", mem_list->free_swap);
    if (mem_list->all_info!=NULL) {
        fprintf(fp, "\"other_info\":\"%s\"", mem_list->all_info);
    } else {
        fprintf(fp, "\"other_info\":\"\"");
    }
    fprintf(fp, "}\n]");
    fclose(fp);
    return 0;
}

int save_mem_info_s(char *save_path, MemInfoS *mem_list_s) {
    FILE *fp;
    if ((fp = fopen(save_path, "w")) == NULL) {
        perror("file open error");
        return -1;
    }
    MemInfoS *temp_mem_s = mem_list_s;
    fprintf(fp, "[\n");
    while (temp_mem_s != NULL) {
        // ----------------------------------------------------------
        fprintf(fp, "{\"physid\":\"%s\",", temp_mem_s->physid);
        fprintf(fp, "\"description\":\"%s\",", temp_mem_s->description);
        fprintf(fp, "\"product\":\"%s\",", temp_mem_s->product);
        fprintf(fp, "\"vendor\":\"%s\",", temp_mem_s->vendor);
        fprintf(fp, "\"businfo\":\"%s\",", temp_mem_s->businfo);
        fprintf(fp, "\"version\":\"%s\",", temp_mem_s->version);
        fprintf(fp, "\"serial\":\"%s\"}", temp_mem_s->serial);
        // ----------------------------------------------------------
        temp_mem_s = temp_mem_s->next;
        if (temp_mem_s != NULL) {
            fprintf(fp, ",\n");
        }
    }
    fprintf(fp, "\n]");
    fclose(fp);
    return 0;
}

int save_bios_info(char *save_path, BiosInfo *bios_list, int flag) {
    FILE *fp;
    if ((fp = fopen(save_path, "w")) == NULL) {
        perror("file open error");
        // fprintf(stderr, "%s File open error %d - %s\n", save_path, errno, strerror(errno));
        return -1;
    }
    fprintf(fp, "[\n");
    if (flag == 0){
        fprintf(fp, "{\"release_date\":\"%s\",", bios_list->release_date);
        fprintf(fp, "\"vendor_name\":\"%s\",", bios_list->vendor);
        fprintf(fp, "\"version\":\"%s\",", bios_list->version);
        // ----------------------------------------------------------
        fprintf(fp, "\"physid\":\"%s\",", bios_list->physid);
        // ----------------------------------------------------------
        if (bios_list->all_info!=NULL) {
            fprintf(fp, "\"other_info\":\"%s\"", bios_list->all_info);
        } else {
            fprintf(fp, "\"other_info\":\"\"");
        }
        fprintf(fp, "}");
    }
    fprintf(fp, "\n]");
    fclose(fp);
    return 0;
}

void show_cpu_info(CpuInfo *cpu_list) {
    CpuUnit *cpu_unit = cpu_list->cpuuint;
    while (cpu_unit != NULL) {
        printf("{\"logic_nums\":%d,", cpu_list->logic_num);
        printf("\"core_nums\":%d,", cpu_list->core_num);
        printf("\"physical_nums\":%d,", cpu_list->phy_num);
        printf("\"architecture\":\"%s\",", cpu_list->architecture);
        printf("\"cpu_name\":\"%s\",", cpu_unit->name);
        printf("\"cpu_vendor\":\"%s\",", cpu_unit->vendor);
        printf("\"disabled\":%d,", cpu_unit->disabled);
        printf("\"other_info\":\"%s\"}", cpu_unit->all_info);
        cpu_unit = cpu_unit->next;
        if (cpu_unit != NULL) {
            printf(",\n");
        }
    }
}

void show_net_info(NetInfo *net_list){
    NetUnit *net_unit = net_list->netunit;
    while (net_unit != NULL) {
        printf("{\"host_name\":\"%s\",", net_list->host_name);
        printf("\"net_name\":\"%s\",", net_unit->net_name);
        printf("\"mac\":\"%s\",", net_unit->mac);
        printf("\"ip_addr\":\"%s\",", net_unit->ip_addr);
        printf("\"broad_addr\":\"%s\",", net_unit->broad_addr);
        printf("\"subnet_mask\":\"%s\",", net_unit->subnet_mask);
        if(net_list->all_info!=NULL){
            printf("\"other_info\":\"%s\"}", net_list->all_info);
        }
        else{
            printf("\"other_info\":\"\"}");
        }
        net_unit = net_unit->next;
        if (net_unit != NULL) {
            printf(",\n");
        }
    }
}

void show_mem_info(MemInfo *mem_list){
    printf("[\n");
    printf("{\"total_ram\":%lu,", mem_list->total_ram);
    printf("\"free_ram\":%lu,", mem_list->free_ram);
    printf("\"shared_ram\":%lu,", mem_list->shared_ram);
    printf("\"buffer_ram\":%lu,", mem_list->buffer_ram);
    printf("\"total_swap\":%lu,", mem_list->total_swap);
    printf("\"free_swap\":%lu,", mem_list->free_swap);
    if (mem_list->all_info != NULL) {
        printf("\"other_info\":\"%s\"", mem_list->all_info);
    } else {
        printf("\"other_info\":\"\"");
    }
    printf("}\n]");
}

void show_disk_info(DiskInfo *disk_list){
    DiskInfo *temp_disk = disk_list;
    printf("[\n");
    while (temp_disk != NULL) {
        printf("{\"fs_type\":\"%s\",", temp_disk->fs_type);
        printf("\"mount_path\":\"%s\",", temp_disk->mount_path);
        printf("\"available\":%lu,", temp_disk->available);
        printf("\"total\":%lu,", temp_disk->total);
        printf("\"used\":%lu,", temp_disk->used);
        printf("\"block_size\":%u}", temp_disk->block_size);
        temp_disk = temp_disk->next;
        if (temp_disk != NULL) {
            printf(",\n");
        }
    }
}

void show_bios_info(BiosInfo *bios_list){
    printf("[\n");
    printf("{\"release_date\":\"%s\",", bios_list->release_date);
    printf("\"vendor\":\"%s\",", bios_list->vendor);
    printf("\"version\":\"%s\",", bios_list->version);
    if (bios_list->all_info!=NULL) {
        printf("\"other_info\":\"%s\"", bios_list->all_info);
    } else {
        printf("\"other_info\":\"\"");
    }
    printf("}\n]");
}

int free_cpu_info(CpuInfo *cpu_list) {
    CpuInfo *temp_cpu = cpu_list;
    if (temp_cpu != NULL) {
        CpuUnit *temp_unit = temp_cpu->cpuuint;
        CpuUnit *cur_unit = NULL;
        while (temp_unit != NULL) {
            cur_unit = temp_unit->next;
            if (temp_unit->all_info != NULL) {
                free(temp_unit->all_info);
                temp_unit->all_info = NULL;
            }
            free(temp_unit);
            temp_unit = cur_unit;
        }
        temp_unit = NULL;
        cur_unit = NULL;
        temp_cpu->cpuuint = NULL;
        if (temp_cpu->all_info != NULL) {
            free(temp_cpu->all_info);
            temp_cpu->all_info = NULL;
        }
        free(temp_cpu);
        temp_cpu = NULL;
        cpu_list = NULL;
    }
    return 0;
}

int free_net_info(NetInfo *net_list) {
    NetInfo *temp_net = net_list;
    if(temp_net!=NULL){
        NetUnit *temp_unit = temp_net->netunit;
        NetUnit *cur_unit = NULL;
        while(temp_unit!=NULL){
            cur_unit=temp_unit->next;
            free(temp_unit);
            temp_unit=cur_unit;
        }
        temp_unit=NULL;
        cur_unit=NULL;
        temp_net->netunit = NULL;
        if(temp_net->all_info!=NULL){
            free(temp_net->all_info);
            temp_net->all_info=NULL;
        }
        free(temp_net);
        temp_net=NULL;
        net_list=NULL;
    }
    return 0;
}

int free_mem_info(MemInfo *mem_list) {
    if(mem_list!=NULL){
        if(mem_list->all_info!=NULL){
            free(mem_list->all_info);
            mem_list->all_info=NULL;
        }
        free(mem_list);
        mem_list=NULL;
    }
    return 0;
}

int free_disk_info(DiskInfo *disk_list) {
    DiskInfo *temp1 = disk_list, *temp2;
    while(temp1!=NULL){
        temp2= temp1->next;
        free(temp1);
        temp1=temp2;
    }
    temp1=NULL;
    temp2=NULL;
    disk_list=NULL;
    return 0;
}

int free_bios_info(BiosInfo *bios_list) {
    if(bios_list->all_info!=NULL){
        free(bios_list->all_info);
        bios_list->all_info=NULL;
    }
    free(bios_list);
    return 0;
}

int free_mem_info_s(MemInfoS *mem_list_s) {
    MemInfoS *temp1 = mem_list_s, *temp2;
    while(temp1!=NULL){
        temp2= temp1->next;
        free(temp1);
        temp1=temp2;
    }
    temp1=NULL;
    temp2=NULL;
    mem_list_s=NULL;
    return 0;
}

int free_disk_info_s(DiskInfoS *disk_list_s) {
    DiskInfoS *temp1 = disk_list_s, *temp2;
    while(temp1!=NULL){
        temp2= temp1->next;
        free(temp1);
        temp1=temp2;
    }
    temp1=NULL;
    temp2=NULL;
    disk_list_s=NULL;
    return 0;
}