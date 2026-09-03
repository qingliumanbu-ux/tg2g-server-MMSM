/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     mfj
Version:    1.0
Date:       2023-09-04
Description: 工序实绩成分查询
explain:将成分信息查询并显示在画面右侧
**************************************************/
//框架头文件
#include "stdafx.h"




//业务头文件


//外部函数声明


BM2F_ENTERACE(mmsmele_inq)

int f_mmsmele_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	


	CPageInfo pageInfo;

	/* 业务变量 */
	CString heat_no = "";//熔炼号
	CString proc_no = "";//处理号
	CString table_type = "";//表名
	CString pono = "";//制造命令号
	
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_where;//查询条件

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
			table_type = bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"].ToString();
		Log::Trace("", __FUNCTION__, "table_type[{0}]  ", table_type);
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("L2_PROC_NO"))
			proc_no = bcls_rec->Tables[0].Rows[0]["L2_PROC_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString();


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr = " select B.CODE_DESC_1_CONTENT ELM_NAME,A.ELM_ACT,A.SAMPLE_TAKEN_TIME REC_CREATE_TIME,A.ST_SAMPLE_NO from (SELECT HEAT_NO,WHOLE_BACKLOG_CODE,SUBSTR(ELM_NAME,5,7) AS ELM_NAME,ELM_ACT,ST_SAMPLE_NO,SAMPLE_TAKEN_TIME "
				" FROM TQMTS24_INIT "
				" unpivot(ELM_ACT for ELM_NAME in(elm_001, elm_002, elm_003, elm_004, elm_005, elm_006, elm_007, elm_008, elm_009, elm_010, elm_011, elm_012, elm_013, elm_014, elm_015, elm_016, elm_017, elm_018, elm_019, elm_020, elm_021, elm_022, elm_023, elm_024, elm_025, elm_026, elm_027, elm_028, elm_029, elm_030, elm_031, elm_032, elm_033, elm_034, elm_035, elm_036, elm_037, elm_038, elm_039, elm_040, elm_041, elm_042, elm_043, elm_044, elm_045, elm_046, elm_047, elm_048, elm_049, elm_050, elm_051, elm_052, elm_053, elm_054, elm_055, elm_056, elm_057, elm_058, elm_059, elm_060, elm_061, elm_062, elm_063, elm_064, elm_065, elm_066, elm_067, elm_068, elm_069, elm_070, elm_071, elm_072, elm_073, elm_074, elm_075, elm_076, elm_077, elm_078, elm_079, elm_080, elm_081, elm_082, elm_083, elm_084, elm_085, elm_086, elm_087, elm_088, elm_089, elm_090, elm_091, elm_092, elm_093, elm_094, elm_095, elm_096, elm_097, elm_098, elm_099, elm_100, elm_101, elm_102, elm_103, elm_104, elm_105, elm_106, elm_107, elm_108, elm_109, elm_110, elm_111, elm_112, elm_113, elm_114, elm_115, elm_116, elm_117, elm_118, elm_119, elm_120, elm_121, elm_122, elm_123, elm_124, elm_125, elm_126, elm_127, elm_128, elm_129, elm_130, elm_131, elm_132, elm_133, elm_134, elm_135, elm_136, elm_137, elm_138, elm_139, elm_140, elm_141, elm_142, elm_143, elm_144, elm_145, elm_146)) "
				" ) A LEFT JOIN TEP0002 B ON A.ELM_NAME = B.CODE where B.CODE_CLASS = 'QMYS2N' AND A.ELM_ACT!='-1'  ";
			if (table_type == "TMMSM14")
			{
				sqlstr_where = " AND A.HEAT_NO = '" + heat_no + "' AND  WHOLE_BACKLOG_CODE ='D'";
			}
			else if (table_type == "TMMSM19")
			{
				sqlstr_where = " AND A.HEAT_NO = '" + heat_no + "' AND  WHOLE_BACKLOG_CODE ='Z' ";
			}
			else if (table_type == "TMMSM20")
			{
				sqlstr_where = " AND A.HEAT_NO = '" + proc_no + "' AND  A.WHOLE_BACKLOG_CODE ='E' ";
			}
			else if (table_type == "TMMSM21")
			{
				sqlstr_where = " AND A.HEAT_NO = '" + heat_no + "' AND  A.WHOLE_BACKLOG_CODE ='B' ";
			}
			else if (table_type == "TMMSM23")
			{
				sqlstr_where = " AND A.HEAT_NO = '" + heat_no + "' AND  A.WHOLE_BACKLOG_CODE ='R' ";
			}
			else if (table_type == "TMMSM24")
			{
				sqlstr_where = " AND A.HEAT_NO = '" + heat_no + "' AND  A.WHOLE_BACKLOG_CODE ='F' ";
			}
			else if (table_type == "TMMSM25")
			{
				sqlstr_where = " AND A.HEAT_NO = '" + heat_no + "' AND  A.WHOLE_BACKLOG_CODE ='V' ";
			}
			else if (table_type == "TMMSM26")
			{
				sqlstr_where = " AND A.HEAT_NO = '" + heat_no + "' AND  A.WHOLE_BACKLOG_CODE ='T' ";
			}
			else if (table_type == "TMMSM27")
			{
				sqlstr_where = " AND A.HEAT_NO = '" + heat_no + "' AND  A.WHOLE_BACKLOG_CODE ='A' ";
			}
			else if (table_type == "TMMSM31")
			{
				sqlstr_where = " AND A.HEAT_NO = '" + heat_no + "' AND  A.WHOLE_BACKLOG_CODE ='C' ";
			}
			else if (table_type == "TMMSMKR")
			{
				sqlstr_where = " AND A.HEAT_NO = '" + heat_no + "' AND  A.WHOLE_BACKLOG_CODE ='D' and A.HEAT_NO!=' '  ";
			}


			sqlstr += sqlstr_where;

			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{

		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


