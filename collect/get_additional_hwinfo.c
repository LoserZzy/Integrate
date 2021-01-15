#include <stdio.h>
#include <stdlib.h>
#include "get_additional_hwinfo.h"
#include "settings.h"

void get_additional_hwinfo()
{
    char doit[100];
    sprintf(doit,"%s%s%s","./lshw -class cpu -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_CPUINFO);
    system(doit);

    sprintf(doit,"%s%s%s","./lshw -class cpu -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_NETINFO);
    system(doit);

    sprintf(doit,"%s%s%s","./lshw -class cpu -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_MEMINFO);
    system(doit);

    sprintf(doit,"%s%s%s","./lshw -class cpu -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_CDISKINFO);
    system(doit);

    sprintf(doit,"%s%s%s","./lshw -class cpu -json > ", SAVE_ADDITIONAL_INFO_DIR_PATH, SAVE_ADDITIONAL_BIOSINFO);
    system(doit);
}