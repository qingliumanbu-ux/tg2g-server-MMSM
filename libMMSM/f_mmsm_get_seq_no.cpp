/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:向萍
Date:2014-10-30
Version:1.0
Description: 返回出入库履历的跟踪序号
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件
//#include "tpssm14.h"

BM2_FUNCTION_EXPORT
int f_mmsm_get_seq_no(CString& resume_seq_no, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int ret = 0;
	CString sqlstr="";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	

	try
	{
		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		//获得输入参数
		//switch (conn->DatabaseKind)
		//{
		//				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		//				case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//				case DB_KIND_ORACLE:	    // Oracle 数据库
		//				default:
		//					sqlstr = "SELECT  PONO "
		//						" FROM    TPSSM11 "
		//						" WHERE   HEAT_NO = @heat_no";
		//				break;
		//}
		//cmd_sql.SetCommandText(sqlstr);
		//cmd_sql.Parameters.Clear();
		//cmd_sql.Parameters.Set("heat_no", heat_no);
		//cmd_sql.ExecuteReader();
		//if (cmd_sql.Read())
		//{
		//	pono = cmd_sql.GetString(1);
		//}
		//cmd_sql.Close();

		////Log::Info("", __FUNCTION__, "pono=[{0}]",pono);
		Log::Info("", __FUNCTION__, "1111=[{0}]", 111111);
		Log::Info("", __FUNCTION__, "conn->DatabaseKind=[{0}]", conn->DatabaseKind);
		/* 生成流水号  */
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			sqlstr = "SELECT LPAD(TO_CHAR(MMSM_MATNO_SEQ.NEXTVAL),6,'0') "
				"  FROM SYSIBM.SYSDUMMY1 ";	//6位前补零
			break;
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = "SELECT LPAD(TO_CHAR(MMSM_MATNO_SEQ.NEXTVAL),6,'0') "
				"  FROM DUAL ";	//6位前补零
			break;
		}
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Clear();
		cmd_sql.ExecuteReader();

		if (cmd_sql.Read())
		{
			resume_seq_no = cmd_sql.GetString(1);
		}
		cmd_sql.Close();


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			resume_seq_no = dateNow + resume_seq_no;
			break;
		}
			
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}

int f_mmsm_get_seq_no(CString& resume_seq_no, CString seq_name, CDecimal bit_num, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int ret = 0;
	CString sqlstr = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。


		/* 生成流水号  */
		/* 生成流水号  */
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			sqlstr = "SELECT LPAD(TO_CHAR(MMSM_MATNO_SEQ.NEXTVAL),6,'0') "
				"  FROM SYSIBM.SYSDUMMY1 ";	//6位前补零
			break;
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = "SELECT LPAD(TO_CHAR(MMSM_MATNO_SEQ.NEXTVAL),6,'0') "
				"  FROM DUAL ";	//6位前补零
			break;
		}


		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Set("bit_num", bit_num);
		cmd_sql.ExecuteReader();

		if (cmd_sql.Read())
		{
			resume_seq_no = cmd_sql.GetString(1);
		}
		cmd_sql.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			resume_seq_no = dateNow + resume_seq_no;
			break;
		}

		
		//Log::Info("", __FUNCTION__, "resume_seq_no=[{0}]", resume_seq_no);

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

	return doFlag;

}

