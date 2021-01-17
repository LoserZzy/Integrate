# -*- coding: gbk -*-
from mysql_interface import Get_Raw_Data, Upload_Raw_Data
from config import username, password, database


def ana_cpu(cpu_raw_info, checker_id):  
    cpuinfo = {'checker_id': checker_id}
    mid = cpu_raw_info.split('description:')
    if len(mid) > 1:
        cpuinfo['description'] = mid[1].split(',\n')[0]

    mid = cpu_raw_info.split('physid:')
    if len(mid) > 1:
        cpuinfo['physid'] = mid[1].split(',\n')[0]

    mid = cpu_raw_info.split('product:')
    if len(mid) > 1:
        cpuinfo['product'] = mid[1].split(',\n')[0]

    mid = cpu_raw_info.split('vendor:')
    if len(mid) > 1:
        cpuinfo['vendor_name'] = mid[1].split(',\n')[0]

    mid = cpu_raw_info.split('businfo:')
    if len(mid) > 1:
        cpuinfo['businfo'] = mid[1].split(',\n')[0]

    mid = cpu_raw_info.split('version:')
    if len(mid) > 1:
        cpuinfo['version'] = mid[1].split(',\n')[0]

    mid = cpu_raw_info.split('serial:')
    if len(mid) > 1:
        cpuinfo['serial'] = mid[1].split(',\n')[0]

    return cpuinfo


def ana_net(net_raw_info, checker_id):  
    netinfo = {'checker_id': checker_id}

    mid = net_raw_info.split('description:')
    if len(mid) > 1:
        netinfo['description'] = mid[1].split(',\n')[0]

    mid = net_raw_info.split('physid:')
    if len(mid) > 1:
        netinfo['physid'] = mid[1].split(',\n')[0]

    mid = net_raw_info.split('product:')
    if len(mid) > 1:
        netinfo['product'] = mid[1].split(',\n')[0]

    mid = net_raw_info.split('vendor:')
    if len(mid) > 1:
        netinfo['vendor'] = mid[1].split(',\n')[0]

    mid = net_raw_info.split('version:')
    if len(mid) > 1:
        netinfo['version'] = mid[1].split(',\n')[0]

    mid = net_raw_info.split('serial:')
    if len(mid) > 1:
        netinfo['serial'] = mid[1].split(',\n')[0]

    return netinfo


def ana_disk(disk_raw_info, checker_id):  
    diskinfo = {'checker_id': checker_id}
    mid = disk_raw_info.split('description:')
    if len(mid) > 1:
        diskinfo['description'] = mid[1].split(',\n')[0]

    mid = disk_raw_info.split('physid:')
    if len(mid) > 1:
        diskinfo['physid'] = mid[1].split(',\n')[0]

    mid = disk_raw_info.split('product:')
    if len(mid) > 1:
        diskinfo['product'] = mid[1].split(',\n')[0]

    mid = disk_raw_info.split('vendor:')
    if len(mid) > 1:
        diskinfo['vendor'] = mid[1].split(',\n')[0]

    mid = disk_raw_info.split('version:')
    if len(mid) > 1:
        diskinfo['version'] = mid[1].split(',\n')[0]

    mid = disk_raw_info.split('size:')
    if len(mid) > 1:
        diskinfo['total_size'] = mid[1].split(',\n')[0]

    mid = disk_raw_info.split('businfo:')
    if len(mid) > 1:
        diskinfo['businfo'] = mid[1].split(',\n')[0]

    mid = disk_raw_info.split('serial:')
    if len(mid) > 1:
        diskinfo['serial'] = mid[1].split(',\n')[0]

    return diskinfo


def ana_mem(mem_raw_info, checker_id):  
    meminfo = {'checker_id': checker_id}

    mid = mem_raw_info.split('description:')
    if len(mid) > 1:
        meminfo['description'] = mid[1].split(',\n')[0]

    mid = mem_raw_info.split('physid:')
    if len(mid) > 1:
        meminfo['physid'] = mid[1].split(',\n')[0]

    mid = mem_raw_info.split('businfo:')
    if len(mid) > 1:
        meminfo['businfo'] = mid[1].split(',\n')[0]

    mid = mem_raw_info.split('serial:')
    if len(mid) > 1:
        meminfo['serial'] = mid[1].split(',\n')[0]

    mid = mem_raw_info.split('vendor:')
    if len(mid) > 1:
        meminfo['vendor'] = mid[1].split(',\n')[0]

    mid = mem_raw_info.split('version:')
    if len(mid) > 1:
        meminfo['version'] = mid[1].split(',\n')[0]

    return meminfo


def ana_bios(bios_raw_info, checker_id):  
    biosinfo = {'checker_id': checker_id}

    mid = bios_raw_info.split('physid:')
    if len(mid) > 1:
        biosinfo['physid'] = mid[1].split(',\n')[0]

    return biosinfo

def additional_ana(checker_id):
    # 数据获取，需要根据具体情况调整
    raw_data = Get_Raw_Data(username, password, database, checker_id, 'Rawinfo',['Cpuinfo', 'Networkinfo', 'Memoryinfo', 'Diskinfo', 'Businfo'])
    ipdata = Get_Raw_Data(username, password, database, checker_id, 'Ipinfo',['Ipinfo'])

    for datas in raw_data:
        cpu_raw = datas['Cpuinfo'].replace(' ', '').replace('\"', '').split('{\nid:') # 数据获取，需要根据具体情况调整
        if len(cpu_raw) > 1:
            cpu_raw = cpu_raw[1:]
        for data in cpu_raw:
            cpuinfo = ana_cpu(data, checker_id)
            # 数据上传,需要根据具体情况调整
            Upload_Raw_Data(username, password, database, 'hardwaredriver_syscpuinfo', [cpuinfo])

    for datas in raw_data:
        net_raw = datas['Networkinfo'].replace(' ', '').replace('\"', '').split('{\nid:')
        if len(net_raw) > 1:
            net_raw = net_raw[1:]
        for data in net_raw:
            netinfo = ana_net(data, checker_id)

            id = data.split('logicalname:')
            if len(id)>1:
                id = id[1].split('\n')[0]
                for rawip in ipdata:
                    if id in rawip['Ipinfo']:
                        if "dhcp4: true" in rawip['Ipinfo']:
                            netinfo['ipalloc'] = 'Dynamic'
                        elif "dhcp4: no" in rawip['Ipinfo'] or "dhcp4: false" in rawip['Ipinfo']:
                            netinfo['ipalloc'] = 'Static'

            # 数据上传,需要根据具体情况调整
            Upload_Raw_Data(username, password, database, 'hardwaredriver_sysnetinfo', [netinfo])

    for datas in raw_data:
        disk_raw = datas['Diskinfo'].replace(' ', '').replace('\"', '').split('{\nid:')
        if len(disk_raw) > 1:
            disk_raw = disk_raw[1:]
        for data in disk_raw:
            diskinfo = ana_disk(data, checker_id)
            # 数据上传,需要根据具体情况调整
            Upload_Raw_Data(username, password, database, 'hardwaredriver_sysdiskinfo_s', [diskinfo])
    
    for datas in raw_data:
        mem_raw = datas['Memoryinfo'].replace(' ', '').replace('\"', '').split('{\nid:')
        if len(mem_raw) > 1:
            mem_raw = mem_raw[1:]
        for data in mem_raw:
            if 'description:BIOS' not in data:
                meminfo = ana_mem(data, checker_id)
                # 数据上传,需要根据具体情况调整
                Upload_Raw_Data(username, password, database, 'hardwaredriver_sysmeminfo_s', [meminfo])
    
    for datas in raw_data:
        bios_raw = datas['Cpuinfo'].replace(' ', '').replace('\"', '').split('{\nid:')
        if len(bios_raw) > 1:
            bios_raw = bios_raw[1:]
        for data in bios_raw:
            if 'description:BIOS' in data:
                biosinfo = ana_bios(data, checker_id)
                # 数据上传,需要根据具体情况调整
                Upload_Raw_Data(username, password, database, 'hardwaredriver_sysbiosinfo', [biosinfo])
    