import MySQLdb


def Upload_Raw_Data(Username, Password, Database, TableName, Data, AutoFlag=0):
    """
    该函数的作用是上传由采集程序采集的数据，典型的调用时机应该是在数据从嵌入式设备传出来后
    调用后返回的值与AutoFlag有关，返回类型为列表，调用成功数据会上传到数据库
    :param Username: 数据库用户名
    :param Password: 数据库用户密码
    :param Database: 使用的数据库
    :param TableName: 想要获取列信息的表名称
    :param Data: 需要上传的数据，以字典列表的形式保存；不要添加自增列的字段！
    :param AutoFlag: 标记，默认为0，取值应为0或1
    :return: 当AutoFlag值为0时，返回一个空列表；当AutoFlag值为1时，返回插入数据后自增列新增的值
    """
    # connect database
    conn = MySQLdb.connect(
        host="0.0.0.0",
        user=Username,
        password=Password,
        database=Database,
        charset="utf8")
    cursor = conn.cursor()
    # get column info
    columnName = ', '.join('{}'.format(k) for k in Data[0].keys())
    columnValue = ', '.join('%({})s'.format(k) for k in Data[0].keys())
    # upload data to entitytable
    sql = "insert into {0}({1}) values({2})" \
        .format(TableName, columnName, columnValue)
    #print("\nmysql>" + sql)
    cursor.executemany(sql, Data)
    conn.commit()
    # update checktable
    autoIncrementID = []
    if (AutoFlag != 0):
        sql = "select LAST_INSERT_ID()"
        cursor.execute(sql)
        beginID = cursor.fetchone()[0]
        endID = beginID + len(Data)
        autoIncrementID = list(range(beginID, endID))
    # close connect
    cursor.close()
    conn.close()
    return autoIncrementID


def Get_Raw_Data(Username, Password, Database, CheckID, TableName, ColumnNeed, CheckTable=None, KeyColunm=None):
    """
    该函数的作用是从数据库获取所需要的原始数据，典型的调用时机应该是分析程序在进行分析之前
    调用后返回与checkID相关的数据，返回类型为字典列表
    :param Username: 数据库用户名
    :param Password: 数据库用户密码
    :param Database: 使用的数据库
    :param CheckID: 该次检测的ID值
    :param TableName: 想要获取列信息的表名称
    :param ColumnNeed: 想要获取的列名称，可以使用Get_Table_Column获取列名后根据需要进行删除，或者自己构建列名列表
    :param CheckTable: 当需要获取的表没有CheckID时，需要与检测表一起进行多表查询
    :param KeyColunm: 目标表的主键列表
    :return:当CheckTable值为None时，表示不需要进行多表查询；当CheckTable值不为None时，需要提供主键列表KeyColunm
    """
    # connect database
    conn = MySQLdb.connect(
        host="0.0.0.0",
        user=Username,
        password=Password,
        database=Database,
        charset="utf8")
    cursor = conn.cursor()
    # get column info
    columnNeed = ', '.join('{}'.format(k) for k in ColumnNeed)
    # obtain data from table
    if CheckTable is None:
        sql = "select {0} from {1} where checker_id = {2}" \
            .format(columnNeed, TableName, CheckID)
    else:
        # sql = "select {0} from {1} where {2} in (select {2} from {3} where CheckID = {4})" \
        #     .format(columnNeed, TableName, KeyColunm, CheckTable, CheckID)
        # -------------------------------------------------------------------
        sql = "select {0} from {1} where id in (select {2} from {3} where checker_id = {4})" \
            .format(columnNeed, TableName, KeyColunm, CheckTable, CheckID)
        # -------------------------------------------------------------------
    cursor.execute(sql)
    result = cursor.fetchall()
    data = []
    for value in result:
        value = list(value)
        newDict = dict(zip(ColumnNeed, value))
        data.append(newDict)
    # close connect
    cursor.close()
    conn.close()
    return data


def Update_Raw_Data(Username, Password, Database, CheckID, TableName, Data, KeyColunm):
    """
    该函数的作用是将分析后需要更新的数据上传到数据库，典型的调用时机应该是分析程序在进行分析之后
    调用后不返回数据（可能会添加错误信息)，调用成功后数据库对应的数据会更新
    :param Username: 数据库用户名
    :param Password: 数据库用户密码
    :param Database: 使用的数据库
    :param CheckID: 该次检测的ID值
    :param TableName: 想要获取列信息的表名称
    :param Data: 想要更新的数据，以字典列表的形式保存；
    :param KeyColunm: 目标表的主键列表
    :return:
    """
    # connect database
    conn = MySQLdb.connect(
        host="0.0.0.0",
        user=Username,
        password=Password,
        database=Database,
        charset="utf8")
    cursor = conn.cursor()
    # get column that need update
    columnUpdate = ', '.join('{0}=%({0})s'.format(k) for k in Data[0].keys())
    keyColumn = ' and '.join('{0}=%({0})s'.format(k) for k in KeyColunm)
    # update data of table
    sql = "update {0} set {1} where {2}" \
        .format(TableName, columnUpdate, keyColumn)
    #print("\nmysql>" + sql)
    cursor.executemany(sql, Data)
    conn.commit()
    # close connect
    cursor.close()
    conn.close()


def Get_Table_Column(Username, Password, Database, TableName, Flag=0):
    """
    该函数的作用是获取指定数据表的列信息
    调用后会返回指定表的所有列信息，并以列表形式返回
    :param Username: 数据库用户名
    :param Password: 数据库用户密码
    :param Database: 使用的数据库
    :param TableName: 想要获取列信息的表名称
    :param Flag: 标记，默认为0，取值应为0或1
    :return: 当Flag为0时，只会返回列名信息；当Flag为1时，会返回列名、数据类型、是否自增、主键等信息
    """
    # connect database
    conn = MySQLdb.connect(
        host="0.0.0.0",
        user=Username,
        password=Password,
        database=Database,
        charset="utf8")
    cursor = conn.cursor()
    # obtain column info
    # -------------------------------------------------------------------
    sql = "select COLUMN_NAME,DATA_TYPE,EXTRA\
        from information_schema.columns \
        where table_schema=%s and table_name = %s;"
    # -------------------------------------------------------------------
    ret = cursor.execute(sql, [Database, TableName])
    # generate string for sql
    columnName = []
    if Flag == 0:
        for _ in range(ret):
            curColumn = cursor.fetchone()
            columnName.append(curColumn[0])
            # print('curColumn:', curColumn)
            # print('columnName:', columnName)
    elif Flag == 1:
        for _ in range(ret):
            curColumn = cursor.fetchone()
            columnName.append(curColumn)
    # close connect
    cursor.close()
    conn.close()
    # print(columnName)
    return columnName
