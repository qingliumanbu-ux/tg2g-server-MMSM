/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     wcm
Version:    1.0
Date:       2023-11-14
Description: 工序投料生产实绩查询
**************************************************/
//框架头文件
#include "stdafx.h"




//业务头文件


//外部函数声明


BM2F_ENTERACE(mmsm2a_inq1)

int f_mmsm2a_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	CPageInfo pageInfo;

	/* 业务变量 */
	CModel tmmsm2a("TMMSM2A");
	
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString ch_start_time_f = "";
	CString ch_start_time_t = "";
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{

		tmmsm2a.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_F"))
			ch_start_time_f = bcls_rec->Tables[0].Rows[0]["START_TIME_F"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_T"))
			ch_start_time_t = bcls_rec->Tables[0].Rows[0]["START_TIME_T"].ToString();

		//Log::Trace("", __FUNCTION__, "HEAT_NO   =[{0}]", tmmsm2a["HEAT_NO"].ToString());
		//Log::Trace("", __FUNCTION__, "PROC_NO   =[{0}]", tmmsm2a["PROC_NO"].ToString());
		//Log::Trace("", __FUNCTION__, "MAT_CODE   =[{0}]", tmmsm2a["MAT_CODE"].ToString());
		//Log::Trace("", __FUNCTION__, "STATION_ID   =[{0}]", tmmsm2a["STATION_ID"].ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " select * from TMMSM2A where 1 = 1 "
				;

			//sqlstr = "  select  * from"
			//	" ((select * from TMMSM50 where  mat_station LIKE '%L%') a left join ( select * from TMMSM2A where HEAT_NO	= @heat_no and PROC_NO = @proc_no ) b  "
			//	"  on a.mat_code=b.mat_code )"
			//	"  where 1 = 1 "
			//	;

			if (tmmsm2a["HEAT_NO"].ToString().Trim() != "")
			{
				sqlstr += " AND HEAT_NO			= @heat_no";
			}
			if (tmmsm2a["PROC_NO"].ToString().Trim() != "")
			{
				sqlstr += " AND PROC_NO			= @proc_no";
			}
			
			if (ch_start_time_f.Trim() != "")
			{
				sqlstr += " AND DEVO_TIME			>= @ch_start_time_f";
			}
			if (ch_start_time_t.Trim() != "")
			{
				sqlstr += " AND DEVO_TIME		<= @ch_start_time_t";
			}
			//Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tmmsm2a["HEAT_NO"].ToString());
			cmd_inq.Parameters.Set("proc_no", tmmsm2a["PROC_NO"].ToString());
			cmd_inq.Parameters.Set("ch_start_time_f", ch_start_time_f);
			cmd_inq.Parameters.Set("ch_start_time_t", ch_start_time_t);
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


