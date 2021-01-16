import sys
import os
sys.path.append("../")
from analysis import update
import argparse
import config
from additional_analyze import additional_ana


def parse_command():
    parser = argparse.ArgumentParser(description='Process some argument.')
    parser.add_argument("--checker_id", "-CI", type=int, default=1, help="current checker_id")
    parser.add_argument("--hardware_entity_table", "-HE", type=str, default="zb1_hardwaredriver_hardware", \
                        help="hardwaredriver_hardware entity table name")
    parser.add_argument("--driver_entity_table", "-DE", type=str, default="zb1_hardwaredriver_driver",
                        help="hardwaredriver_driver entity table name")
    parser.add_argument("--module_entity_table", "-ME", type=str, default="zb1_hardwaredriver_module",
                        help="hardwaredriver_module entity table name")
    parser.add_argument("--hardware_check_table", "-HC", type=str, default="zb1_hardwaredriver_check2hardware",
                        help="hardwaredriver_hardware check table name")
    parser.add_argument("--driver_check_table", "-DC", type=str, default="zb1_hardwaredriver_check2driver",
                        help="hardwaredriver_driver check table name")
    parser.add_argument("--module_check_table", "-MC", type=str, default="zb1_hardwaredriver_check2module",
                        help="hardwaredriver_module check table name")
    parser.add_argument("--username", "-u", type=str, default="lqb", help="username of database")
    parser.add_argument("--password", "-p", type=str, default="lqb", help="password of database")
    parser.add_argument("--database", "-d", type=str, default="hardware0527", help="name of database")
    args = parser.parse_args()
    return args


def conf_update(args):
    config.module_entity_table_name = args.module_entity_table
    config.module_check_table_name = args.module_check_table
    config.driver_entity_table_name = args.driver_entity_table
    config.driver_check_table_name = args.driver_check_table
    config.hardware_entity_table_name = args.hardware_entity_table
    config.hardware_check_table_name = args.hardware_check_table
    config.username = args.username
    config.password = args.password
    config.database = args.database

    return args.checker_id


def main(checker_id):
    update(checker_id)
    additional_ana(checker_id)
    return (0, "分析成功结束。")


# if __name__ == "__main__":
#     main()
