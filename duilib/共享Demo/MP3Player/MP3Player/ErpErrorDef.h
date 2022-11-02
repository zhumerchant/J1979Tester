#ifndef _ERPERRORDEF_H
#define _ERPERRORDEF_H

#define SYSTEM_ERROR	  1
#define ACCPERIOD_DEFERR  2
#define DBCON_NOTSET 	  3
#define SYSPARAM_ERROR	  4
#define DATABASE_ERROR	  5
#define VOU_NOT_FOUND     6
#define VOU_ALREADY_AUTH  7
#define VOU_ALREADY_ACC   8
#define VOU_NOT_ACC       11
#define VOU_NOT_AUTH      9
#define JOURNAL_NOT_BALANCE 10
#define STOCKNOSUCHITEM   11
#define NOSUCH_BALTYPE    12
#define CUST_SETTLE_INVALID 13
#define INPUT_DATA_INVALID  14
#define COA_NOT_EXIST       15
#define COA_OUTOF_BAL       16
#define COA_NOT_LEAF        17
#define PARAM_SETUP_ERROR   18
#define NO_SUCH_TRANSACTION 19
#define ITEM_NOT_FOUND      20

#define MSG_PACK_ERR    21
#define MSG_UNPACK_ERR  22
#define RAWJOURNAL_ALREADY_CORRECT  23

#define UNKNOW_OPTION 24 
#define NOT_CURRENT_PERIOD  25
#define SQL_PREPARE_ERR 26
#define SQL_NOT_FOUND 27

#define UPNODE_NOT_FOUND 28
#define UNCORRECT_UPNODE 29

#define REC_IN_USE 30

#define TPL_NOT_FOUND      31

#define WSHEET_IS_OPEN 32
#define WSHEET_IS_CLOSE 33

#define PROD_IS_EXIST 34

#define AMOUNT_TOO_LARGE 35

#define ORDER_IS_CLOSE 36
#define WSHEET_IS_NOT_OPEN 37

#define DATE_ILLEGAL    38
#define UNKNOWOPTION    39

#define BRH_NOT_FOUND   40
#define RECORD_NOT_FOUND   41
#define BOM_TPL_UNCORRECT 42

#define IS_NOT_PRODUCT 43
#define AMOUNT_TOO_SMALL 44

#define OPR_NOT_FOUND	 45
#define PASSWD_NOT_MATCH 46
#define PASSWD_INVALID   47

#define ORDER_NOT_FOUND  48
#define ORDER_ALERADY_OPEN 49
#define ORDER_ALREADY_CLOSE 50
#define ORDER_EXECUTING 51
#define NO_SUCH_CUST    52

#define INVAL_PARA_TYPE 53
#define DATA_NOT_EQUAL 54
#define GLAC_ALREADY_INIT 55
#define GLAC_NOT_INIT  56

#define VALUE_EQUAL 57
#define PROD_IS_CHG 58

#define RECORD_AlREADY_EXIST  59

#define JUST_WARNING  60
#define DATASETNOTPOST 61
#define SEQFIELDINCONSISTENT 62
#define SEQFIELDVALUENOTSET 63
#define DATAISNOTINTEGER  64
#define FIELDNOTINSEQ 65
#define DATAISNOTFLOAT 66
#define SEQNOMOREDATA 67
#define FIELDNAMEINVALID 68

#define PARAM_NOT_FOUND_IN_PARAMS 69
#define ERP_RECV_SOCKET_TIMEOUT 70
#define ERP_SEND_SOCKET_TIMEOUT 71
#define SERVER_CANNOT_FOUND_COMPANY 72
#define ERP_TRANS_PACKAGE_INVALID 73

#define ERP_MYSQL_ERROR  74

#endif


