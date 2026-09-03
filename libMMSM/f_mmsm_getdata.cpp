/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:			王佳倩
Version:		1.0
Date:			2022-10-11
Description:	查询物料数据 - 转炉，精炼，连铸数据（智慧质量调用）
***********************************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

BM2_FUNCTION_EXPORT
int f_mmsm_getdata(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int ret = 0;
	CString sqlstr="";

	CString heat_no = "";
	CString pono = "";
	CString item_1 = "";	//RH脱碳终氧
	CString item_2 = "";	//RH铝加入量
	CString item_3 = "";	//RH处理前氧
	CString item_4 = "";	//转炉停吹氧
	CString item_5 = "";	//纯脱气时间
	CString item_6 = "";	//OB量
	CString item_7 = "";	//过热度
	CString item_8 = "";	//高真空时间

	CDbCommand cmd_sql(conn);

	try
	{
		heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();

		Log::Info("", __FUNCTION__, "heat_no=[{0}],pono = [{1}]",heat_no,pono);

		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_CODE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_NAME");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ITEM_ACT_VALUE");
		/*****************1、转炉实绩 ****************/
		//转炉停吹氧
		//switch (conn->DatabaseKind)
		//{
		//	case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//	case DB_KIND_ORACLE:	    // Oracle 数据库
		//	default:
		//		sqlstr = "SELECT  PONO,FACTORY_DIV "
		//			" FROM    TMMSM21 "
		//			" WHERE   HEAT_NO = @heat_no"
		//			" AND PONO = @pono";
		//	break;
		//}
		//cmd_sql.SetCommandText(sqlstr);
		//cmd_sql.Parameters.Clear();
		//cmd_sql.Parameters.Set("heat_no", heat_no);
		//cmd_sql.ExecuteReader();
		//if (cmd_sql.Read())
		//{
		//	pono = cmd_sql.GetString(1);
		//	factory_div = cmd_sql.GetString(2);
		//}
		//cmd_sql.Close();

		/*****************2、RH精炼实绩 ****************/
		//RH脱碳终氧、RH铝加入量、过热度、高真空时间
		//RH处理前氧 - START_O2 、纯脱气时间 - PURE_DEGAS_DURATION
		//OB量 - O2_SUM_COMSUME
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "SELECT  START_O2 AS ITEM_3,PURE_DEGAS_DURATION AS ITEM_5,O2_SUM_COMSUME AS ITEM_6 "
					" FROM    TMMSM23 "
					" WHERE   HEAT_NO = @heat_no"
					" AND PONO = @pono";
			break;
		}
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Clear();
		cmd_sql.Parameters.Set("heat_no", heat_no);
		cmd_sql.Parameters.Set("pono", pono);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows.Add();

			item_3 = cmd_sql.GetString(1);
			bcls_ret->Tables[0].Rows[0]["ITEM_CODE"] = "ITEM_3";
			bcls_ret->Tables[0].Rows[0]["ITEM_CODE"] = "RH处理前氧";
			bcls_ret->Tables[0].Rows[0]["ITEM_ACT_VALUE"] = item_3;

			item_5 = cmd_sql.GetString(2);
			bcls_ret->Tables[0].Rows[1]["ITEM_CODE"] = "ITEM_5";
			bcls_ret->Tables[0].Rows[1]["ITEM_CODE"] = "纯脱气时间";
			bcls_ret->Tables[0].Rows[1]["ITEM_ACT_VALUE"] = item_5;

			item_6 = cmd_sql.GetString(3);
			bcls_ret->Tables[0].Rows[2]["ITEM_CODE"] = "ITEM_6";
			bcls_ret->Tables[0].Rows[2]["ITEM_CODE"] = "OB量";
			bcls_ret->Tables[0].Rows[2]["ITEM_ACT_VALUE"] = item_6;
		}
		cmd_sql.Close();

		/*****************3、连铸实绩 ****************/
		//switch (conn->DatabaseKind)
		//{
		//	case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//	case DB_KIND_ORACLE:	    // Oracle 数据库
		//	default:
		//		sqlstr = "SELECT  PONO,FACTORY_DIV "
		//			" FROM    TMMSM31 "
		//			" WHERE   HEAT_NO = @heat_no"
		//			" AND PONO = @pono";
		//	break;
		//}
		//cmd_sql.SetCommandText(sqlstr);
		//cmd_sql.Parameters.Clear();
		//cmd_sql.Parameters.Set("heat_no", heat_no);
		//cmd_sql.ExecuteReader();
		//if (cmd_sql.Read())
		//{
		//	pono = cmd_sql.GetString(1);
		//	factory_div = cmd_sql.GetString(2);
		//}
		//cmd_sql.Close();

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
