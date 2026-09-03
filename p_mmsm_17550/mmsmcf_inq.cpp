/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     dyl
Version:    1.0
Date:       2023-09-21
Description: 工序实绩成分查询
explain: 修改实绩查询成分信息
**************************************************/
//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(mmsmcf_inq)

int f_mmsmcf_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	
	CPageInfo pageInfo;

	/* 业务变量 */
	CString heat_no = "";//熔炼号
	CString st_sample_no = "";
	CString whole_backlog_code = "";

	EIClass temp;
	
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr1;
	CString sqlstr_where;//查询条件

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();

		if (!bcls_ret->Tables[0].Columns.Contains("ST_SAMPLE_NO")) {
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
		}
		if (!bcls_ret->Tables[0].Columns.Contains("WHOLE_BACKLOG_CODE")) {
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "WHOLE_BACKLOG_CODE");
		}

		if (heat_no != "")
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr = " SELECT ST_SAMPLE_NO, WHOLE_BACKLOG_CODE FROM TQMTS24 WHERE  1 =1 ";

				sqlstr_where = " AND HEAT_NO = '" + heat_no + "'";

				sqlstr += sqlstr_where;

				Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					st_sample_no = cmd_inq.GetString(1);
					whole_backlog_code = cmd_inq.GetString(2);

					temp.Tables[0].Rows.Clear();

					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:

						sqlstr1 = "select (SELECT nvl(ELM_ACT,0) FROM TQMTS25 WHERE heat_no = '"+ heat_no +"'  and elm_code = '012' and st_sample_no = '"+ st_sample_no +"' ) C," +
							" (SELECT nvl(ELM_ACT,0) FROM TQMTS25 WHERE heat_no = '"+ heat_no +"'  and elm_code = '028' and st_sample_no = '"+ st_sample_no +"' ) SI," +
							" (SELECT nvl(ELM_ACT,0) FROM TQMTS25 WHERE heat_no = '"+ heat_no +"'  and elm_code = '055' and st_sample_no = '"+ st_sample_no +"' ) MN," +
							" (SELECT nvl(ELM_ACT,0) FROM TQMTS25 WHERE heat_no = '"+ heat_no +"'  and elm_code = '030' and st_sample_no = '"+ st_sample_no +"' ) P," +
							" (SELECT nvl(ELM_ACT,0) FROM TQMTS25 WHERE heat_no = '"+ heat_no +"'  and elm_code = '032' and st_sample_no = '"+ st_sample_no +"' ) S," +
							" (SELECT nvl(ELM_ACT,0) FROM TQMTS25 WHERE heat_no = '"+ heat_no +"'  and elm_code = '051' and st_sample_no = '"+ st_sample_no +"' ) V," +
							" (SELECT nvl(ELM_ACT,0) FROM TQMTS25 WHERE heat_no = '"+ heat_no +"'  and elm_code = '093' and st_sample_no = '"+ st_sample_no +"' ) NB," +
							" (SELECT nvl(ELM_ACT,0) FROM TQMTS25 WHERE heat_no = '"+ heat_no +"'  and elm_code = '211' and st_sample_no = '"+ st_sample_no +"' ) ALS," +
							" (SELECT nvl(ELM_ACT,0) FROM TQMTS25 WHERE heat_no = '"+ heat_no +"'  and elm_code = '052' and st_sample_no = '"+ st_sample_no +"' ) CR," +
							" (SELECT nvl(ELM_ACT,0) FROM TQMTS25 WHERE heat_no = '"+ heat_no +"'  and elm_code = '058' and st_sample_no = '"+ st_sample_no +"' ) NI from SYSIBM.SYSDUMMY1 ";

						cmd_inq1.SetCommandText(sqlstr1);
						cmd_inq1.ExecuteQuery(temp.Tables[0]);
						if (!temp.Tables[0].Columns.Contains("ST_SAMPLE_NO")) {
							temp.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
						}
						if (!temp.Tables[0].Columns.Contains("WHOLE_BACKLOG_CODE")) {
							temp.Tables[0].Columns.Add(DT_STRING, "WHOLE_BACKLOG_CODE");
						}
	
						if (temp.Tables[0].Rows.get_Count() > 0) {
							temp.Tables[0].Rows[0]["ST_SAMPLE_NO"] = st_sample_no;
							temp.Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"] = whole_backlog_code;
							if (!bcls_ret->Tables[0].Columns.Contains("C")) {
								bcls_ret->Tables[0].Clone(temp.Tables[0]);
							}
							CDataRow& dt = bcls_ret->Tables[0].Rows.Add();
							dt.Merge(temp.Tables[0].Rows[0]);
						}
					}
					cmd_inq1.Close();
				}
				cmd_inq.Close();
			}
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


