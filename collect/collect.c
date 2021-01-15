#include "HDMstruct.h"
// #include "FWCConnect.h"
#include "SysInfo.h"
#include "get_additional_hwinfo.h"
#include <stdio.h>

int main() {
   
    int sock;
    char report_msg[50];
    // 初始化连接，报告进度0
    // FWC_client_init(&sock);
    sprintf(report_msg, "start checking");
    // FWC_progress_report(sock, 5, 0, report_msg);

    // 开始获取CPU信息，进度报告5
    cpu_info(SAVE_DIR_PATH);
    sprintf(report_msg, "complete cpu_information checking");
    // FWC_progress_report(sock, 6, 5, report_msg);


    // 开始获取memory信息，进度报告10
    mem_info(SAVE_DIR_PATH);
    sprintf(report_msg, "complete memory_information checking");
    // FWC_progress_report(sock, 6, 10, report_msg);


    // 开始获取硬盘信息，进度报告15
    disk_info(SAVE_DIR_PATH);
    sprintf(report_msg, "complete disk_information checking");
    // FWC_progress_report(sock, 6, 15, report_msg);


    // 开始获取网卡信息，进度报告20
    net_info(SAVE_DIR_PATH);
    sprintf(report_msg, "complete netcard_infomation checking");
    // FWC_progress_report(sock, 6, 20, report_msg);


    // 开始获取BIOS信息，进度报告25
    bios_info(SAVE_DIR_PATH);
    sprintf(report_msg, "complete BIOS_information checking");
    // FWC_progress_report(sock, 6, 25, report_msg);

    // 开始获取硬件信息，进度报告40
    hardware(SAVE_DIR_PATH);
    sprintf(report_msg, "complete hardware checking");
    // FWC_progress_report(sock, 6, 40, report_msg);

    // 开始获取驱动信息，进度报告70
    driver(SAVE_DIR_PATH);
    sprintf(report_msg, "complete driver checking");
    // FWC_progress_report(sock, 6, 70, report_msg);

    // 开始获取模块信息，进度报告80
    module(SAVE_DIR_PATH);
    sprintf(report_msg, "complete module checking");
    // FWC_progress_report(sock, 6, 80, report_msg);

    get_additional_hwinfo();
    sprintf(report_msg, "complete additional info checking");
    // FWC_progress_report(sock, 6, 90, report_msg);

    sprintf(report_msg, "end checking");
    // FWC_progress_report(sock, 5, 100, report_msg);

    // 关闭连接
    // FWC_client_recovery(&sock);
    
    return 0;
}
