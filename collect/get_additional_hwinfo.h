#ifndef GET_ADDITIONAL_HWINFO
#define GET_ADDITIONAL_HWINFO
// #define SAVE_ADDITIONAL_INFO_DIR_PATH "./result/additional_info/" //其它未收集齐的补充信息（朱正禹的部分）
#define SAVE_ADDITIONAL_CPUINFO "cpuinfo"
#define SAVE_ADDITIONAL_NETINFO "networkinfo"
#define SAVE_ADDITIONAL_MEMINFO "memoryinfo"
#define SAVE_ADDITIONAL_CDISKINFO "diskinfo"
#define SAVE_ADDITIONAL_BIOSINFO "biosinfo"

void get_additional_hwinfo(void);

#endif 