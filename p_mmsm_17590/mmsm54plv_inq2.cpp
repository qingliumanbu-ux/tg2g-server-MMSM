/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:   1.0
Date:     2016-01-29 17:13:56
Description: 原辅料计划管理
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsm54plv_inq2)

int f_mmsm54plv_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString v_mat_kind = "";
	int		TotalRecordCount = 0;
	CString v_cast_lot_no = "";//浇次批号
	CString v_st_no = "";//出钢记号
	CString v_table_type = "";//画面名
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm54("TMMSM54");

	CDbCommand cmd_inq(conn);

	try
	{
		try{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
			v_table_type = bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"].ToString();


		if (v_table_type.Trim() == "MMSM54F1PLV")
		{

			//获取浇次信息以作为查询条件
			if (bcls_rec->Tables[0].Columns.Contains("CAST_LOT_NO"))
				v_cast_lot_no = bcls_rec->Tables[0].Rows[0]["CAST_LOT_NO"].ToString();
			/*if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
				v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();*/

			Log::Info("", __FUNCTION__, "v_cast_lot_no =[{0}]  v_st_no =[{1}]", v_cast_lot_no.Trim(), v_st_no.Trim());

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				//sql条件
				if (v_cast_lot_no.Trim() != "")
				{
					sqlstr_temp += " AND A.CAST_LOT_NO = '" + v_cast_lot_no + "'";
				}
				/*if (v_st_no.Trim() != "")
				{
					sqlstr_temp += " AND ST_NO = '" + v_st_no + "'";
				}*/

				sqlstr = "SELECT a.CAST_LOT_NO,a.CC_MACH_NO,a.st_no,a.PLAN_TAP_WT,a.PLAN_DATE,a.sg_sign,b.mat_code,b.mat_name,decode(b.Unit, 'KG', round((b.use_Unit * a.PLAN_TAP_WT), 0), 'TON', round((b.use_Unit * a.PLAN_TAP_WT), 3)) use_Unit,b.Unit  "
					" FROM(SELECT * "
					" FROM(SELECT t.CAST_LOT_NO,MAX(t.CC_MACH_NO)CC_MACH_NO,MAX(t.st_no)st_no,MAX(t.PLAN_TAP_WT)PLAN_TAP_WT,MAX(t.PLAN_DATE)PLAN_DATE,MAX(t.sg_sign) sg_sign   "
					" FROM TPSSM01 t  WHERE t.CAST_LOT_NO <> ' '  group by CAST_LOT_NO  ORDER BY t.CAST_LOT_NO  "
					" --计划状态  "
					" --AND t.CAST_LOT_NO IN(SELECT DISTINCT CAST_LOT_NO FROM tpssm01 WHERE PONO_STATUS >= '13' AND PONO_STATUS < '91')  \n "
					" )) a, "
					" (SELECT mat_code,mat_name,use_Unit,Unit,st_no  "
					" FROM tmmsm54A) b  WHERE(a.st_no(+) = b.st_no  " + sqlstr_temp + ") ";

				break;
			}

			Log::Info("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
			cmd_inq.Close();

		}
		else if (v_table_type.Trim() == "MMSM54F2PLV")
		{


			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				//sql条件


				sqlstr = "select MAT_CODE,MAT_NAME,0 AS USE_UNIT,PRIMARY_MAT_UNIT UNIT FROM TMMSM60";

				break;
			}

			Log::Info("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
			cmd_inq.Close();
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
