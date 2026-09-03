/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:   
Version:    1.0
Date:       2016-07-21
Description: 铸坯组批成分信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

int f_mmsm38cf_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
/// <summary>
/// 铸坯组批信息查询
/// <para>
/// 铸坯组批信息查询
/// </para>
/// </summary>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm38cf_inq)


int f_mmsm38cf_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString matNo = "";
	int blkNum = 0;
	int fetchRowCount = 0;
	CString pono = "";
	CString heatNo = "";
	CString sqlstr("");

	CModel tqmts29("TQMTQQ0");
	CModel tep0002("TEP0002");

	CDbCommand cmd_inq(conn);

	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	try
	{
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_ret->Tables[0].Columns["HEAT_NO"].set_Caption("熔炼号");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "PONO");
		bcls_ret->Tables[0].Columns["PONO"].set_Caption("制造命令号");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_NO");
		bcls_ret->Tables[0].Columns["ST_NO"].set_Caption("出钢记号");

		/* 根据代码配置表压入化学成分列名 */
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = "SELECT * "
				"  FROM TEP0002 "
				" WHERE CODE_CLASS = 'QMYS' "
				"	AND TRIM(CODE_DESC_2_CONTENT) IS NOT NULL "
				" ORDER BY CODE_DESC_2_CONTENT ASC  ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tep0002);
			tep0002["CODE_DESC_1_CONTENT"] = tep0002["CODE_DESC_1_CONTENT"].ToString().ToLower();

			//增加返回块的列名(化学成分)
			bcls_ret->Tables[0].Columns.Add(DT_STRING, tep0002["CODE_DESC_1_CONTENT"].ToString());
		}
		cmd_inq.Close();


		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			pono = bcls_rec->Tables[0].Rows[i]["PONO"].ToString().Trim();
			EDLog(1, 1, "pono = [%s]", (const char*)pono);

			/* 根据PONO,取得各个化学元素的值 */
			if (pono.Trim() != "")
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = "SELECT * "
						"  FROM TQMTQQ0 "
						" WHERE PONO = @tmmsm01.PONO "
						" ORDER BY ELM_CODE ASC";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmmsm01.PONO", pono);
				int count = 0;
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					count++;
					if (count == 1)
					{
						bcls_ret->Tables[0].Rows.Add();
						bcls_ret->Tables[0].Rows[i]["PONO"] = bcls_rec->Tables[0].Rows[i]["PONO"];
						bcls_ret->Tables[0].Rows[i]["HEAT_NO"] = bcls_rec->Tables[0].Rows[i]["HEAT_NO"];
						bcls_ret->Tables[0].Rows[i]["ST_NO"] = bcls_rec->Tables[0].Rows[i]["ST_NO"];
					}
					cmd_inq.Fetch(tqmts29);
					tqmts29.TrimOrBlank();
					tqmts29["ELM_NAME"] = tqmts29["ELM_NAME"].ToString().ToLower();
					//Log::Trace("", __FUNCTION__, "tqmtqq0.ELM_NAME	= [{0}]", (const char*)tqmts29["ELM_NAME"].ToString());
					//Log::Trace("", __FUNCTION__, "tqmtqq0.ELM_ACT		= [{0}]", tqmts29["ELM_ACT"].ToDecimal().ToDouble());
					//增加返回块的(化学成分)值
					if (bcls_ret->Tables[0].Columns.Contains(tqmts29["ELM_NAME"].ToString()) == true)
					{
						bcls_ret->Tables[0].Rows[i][tqmts29["ELM_NAME"].ToString()] = tqmts29["ELM_ACT"];
					}
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
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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
