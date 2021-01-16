from mysql_interface import Get_Raw_Data, Update_Raw_Data, Get_Table_Column
from config import *


class Redundant:
    def __init__(self, check_id):
        self.check_id = check_id
        self.module_data = []
        self.driver_data = []
        self.hardware_data = []
        
    def module_data(self):
        """
        get module data [module_entity_data, module_check_data]
        :return: module_data [module_entity_data, module_check_data]
        """
        self.module_data = [self._module_entity_data, self._module_check_data]
        return self.module_data

    def driver_data(self):
        """
        get driver data [driver_entity_data, driver_check_data]
        :return: driver_data [driver_entity_data, driver_check_data]
        """
        self.driver_data = [self._driver_entity_data, self._driver_check_data]
        return self.driver_data

    def hardware_data(self):
        """
        get hardware data [hardware_entity_data, hardware_check_data]
        :return: hardware_data [hardware_entity_data, hardware_check_data]
        """
        self.hardware_data = [self._hardware_entity_data, self._hardware_check_data]
        return self.hardware_data

    def _get_data(self):
        # get module data
        module_entity_column = Get_Table_Column(username, password, database, module_entity_table_name)
        module_check_column = Get_Table_Column(username, password, database, module_check_table_name)
        self._module_entity_data = Get_Raw_Data(username, password, database, self.check_id, module_entity_table_name,
                                                module_entity_column, module_check_table_name, KeyColunm="module_id")
        self._module_check_data = Get_Raw_Data(username, password, database, self.check_id,
                                               module_check_table_name, module_check_column)
        # get driver data
        driver_entity_column = Get_Table_Column(username, password, database, driver_entity_table_name)
        driver_check_column = Get_Table_Column(username, password, database, driver_check_table_name)
        self._driver_entity_data = Get_Raw_Data(username, password, database, self.check_id, driver_entity_table_name,
                                                driver_entity_column, driver_check_table_name, KeyColunm="driver_id")
        self._driver_check_data = Get_Raw_Data(username, password, database, self.check_id,
                                               driver_check_table_name, driver_check_column)
        # get hardware data
        hardware_entity_column = Get_Table_Column(username, password, database, hardware_entity_table_name)
        hardware_check_column = Get_Table_Column(username, password, database, hardware_check_table_name)
        self._hardware_entity_data = Get_Raw_Data(username, password, database, self.check_id,
                                                  hardware_entity_table_name, hardware_entity_column,
                                                  hardware_check_table_name, KeyColunm="hardware_id")
        self._hardware_check_data = Get_Raw_Data(username, password, database, self.check_id,
                                                 hardware_check_table_name, hardware_check_column)

    @staticmethod
    def _get_depends(mod_info: str):
        """
        get module's depends via parsing mod_info
        :param mod_info: modinfo
        :return: depends list
        """
        res = []
        if len(mod_info) > 0:
            pattern = 'depends:'
            depends = ''
            idx = mod_info.find(pattern) + len(pattern)
            while mod_info[idx] != '\n' and mod_info[idx] != '\r':
                if mod_info[idx] != ' ':
                    if mod_info[idx] == '-':
                        depends += '_'
                    else:
                        depends += mod_info[idx]

                idx += 1
            if len(depends) > 0:
                res = depends.split(',')
        return res

    @staticmethod
    def _set_redundant(redundant, foreign_key, foreign_id, check_data):
        """
        set redundance number
        :param redundant: redundance number to be set
        :param foreign_key: foreign key column name
        :param foreign_id: foreign id to be set
        :param check_data: check data
        :return: 0: abnormal  1: normal
        """
        flag = 0
        for item in check_data:
            if item[foreign_key] == foreign_id:
                item["redundance"] = redundant
                flag = 1
                break
        return flag

    @staticmethod
    def _get_redundant(foreign_key, foreign_id, check_data):
        res = -1
        for item in check_data:
            if item[foreign_key] == foreign_id:
                if item["redundance"] is not None:
                    res = item["redundance"]
                break
        return res

    @staticmethod
    def _get_excute(foreign_id, check_data):
        """
        get excute number, aiming at module_check_table
        :param foreign_id: check table's foreign key's value
        :param check_data: check table's content
        :return: excute number
        """
        res = -1
        for item in check_data:
            if item["ModuleID"] == foreign_id:
                res = item["excute"]
                break
        return res

    @staticmethod
    def _get_item(data, src_key, src_value, res_key):
        """
        get item[res_key] when item[src_key] is src_value, in data
        :param data: src data to be searched
        :param src_key: src_key in data
        :param src_value: vaule to be matched
        :param res_key: return key in data
        :return: item[res_key]
        """
        res = []
        for item in data:
            if item[src_key] == src_value:
                res.append(item[res_key])
        return res

    @staticmethod
    def del_key(data, src_key):
        """
        delete item[src_key] in dict-list data
        :param data: src data to be searched
        :param src_key: src_key in data to be deleted
        :return: dict-list data
        """
        for item in data:
            try:
                del(item[src_key])
            except:
                pass
        return data

    def _driver_redundant(self):
        """
        analyse driver data, if driver not in hardware's driver, set as redundance
        :return: None
        """
        hd_driver = []
        for item in self._hardware_entity_data:
            if item["driver_id"] is not None:   # driver被硬件使用，视为不冗余
                hd_driver.append(item["driver_id"])
        no_re_num = 0   # 不冗余数量
        re_num = 0      # 冗余数量
        print(hd_driver)
        for item in self._driver_entity_data:   # driver中的id在不冗余驱动列表中
            if item["id"] in hd_driver:  # 在hd_driver里面，存在，不冗余，Redundance置0
                self._set_redundant(0, "driver_id", item["id"], self._driver_check_data)
                no_re_num += 1
            else:
                self._set_redundant(1, "driver_id", item["id"], self._driver_check_data)
                re_num += 1
        print("no_re_num:", no_re_num, "re_num:", re_num)

    def _module_redundant(self):
        """
        analysis module data, if module not in driver's not redundant module,
        :return: None
        """
        dr_module = []
        re_module = set()
        no_re_module = set()
        depends_set = set()
        for item in self._driver_entity_data:   # module被驱动使用且活跃，视为不冗余
            if item["module_id"] is not None\
                    and self._get_redundant("driver_id", item["id"], self._driver_check_data) == 0:
                dr_module.append(item["module_id"])
        for item in self._module_entity_data:   # module中的id在不冗余模块列表中
            if item["id"] in dr_module:   # 在dr_module，存在，不冗余，Redundance置0
                self._set_redundant(0, "module_id", item["id"], self._module_check_data)
                no_re_module.add(item["module_name"])
                depends_list = self._get_depends(item["module_info"])
                for i in depends_list:
                    depends_set.add(i)
            else:
                self._set_redundant(1, "module_id", item["id"], self._module_check_data)
                re_module.add(item["module_name"])
        print("0 re_module number:", len(re_module), "not_re_module number:", len(no_re_module))
        # print(re_module)
        # print("depends set:", depends_set)
        while len(depends_set) > 0:     # 递归将不冗余模块的依赖模块设为不冗余
            module_name = depends_set.pop()     # 此处module_name应该为不冗余模块的依赖模块，应该被设为不冗余模块
            if module_name in no_re_module:     # 若已经在不冗余列表中，继续下一个
                # print("module in no_re_module:", module_name)
                continue
            for item in self._module_entity_data:   # 依次遍历模块实体表中的数据
                if module_name == item["module_name"] and module_name in re_module:     # module_name在冗余列表中，
                    self._set_redundant(0, "module_id", item["id"], self._module_check_data)    # 设为不冗余模块
                    no_re_module.add(module_name)   # 不冗余模块集合添加
                    re_module.remove(module_name)   # 冗余模块集合移除
                    depends_list = self._get_depends(item["module_info"])   # 继续获得当前module_name的依赖模块
                    # print('0', depends_set)
                    for dep in depends_list:    # 将这些依赖模块添加到依赖模块集合中
                        if dep in re_module:
                            depends_set.add(dep)
                    # print('1', depends_set)
                    break
            print("1 re_module number:", len(re_module), "not_re_module number:", len(no_re_module))

    def analysis(self):
        """
        redundance analysis
        """
        self._get_data()
        self._driver_redundant()
        self._module_redundant()

    def update_data(self):
        """
        update database data after analysis
        """
        self.del_key(self._driver_check_data, 'id')     # 由于id为自增项，不能对自增项进行设置，因此移除该项
        Update_Raw_Data(username, password, database, self.check_id, driver_check_table_name,
                        self._driver_check_data, ["checker_id", "driver_id"])
        self.del_key(self._module_check_data, 'id')     # 由于id为自增项，不能对自增项进行设置，因此移除该项
        Update_Raw_Data(username, password, database, self.check_id, module_check_table_name,
                        self._module_check_data, ["checker_id", "module_id"])
        self._get_data()


# def Update_Raw_Data(Username, Password, Database, CheckID, TableName, Data, KeyColunm):
# def Get_Table_Column(Username, Password, Database, TableName, Flag=0):

def update(check_id):
    """
    update data via check_id
    :param check_id: check id
    :return: None
    """
    redd = Redundant(check_id)
    redd.analysis()
    redd.update_data()
