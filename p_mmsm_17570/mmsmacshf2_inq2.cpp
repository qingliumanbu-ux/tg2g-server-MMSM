/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   mfj
Version:    1.0
Date:     2024-01-03
Description: 收货履历查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/


/* ***** 静态函数申明 ***** */


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///
/// <para>
///自动收货管理查询
///
///
///
///
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsmacshf2_inq2)

int f_mmsmacshf2_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;


	int		TotalRecordCount = 0;
	CString v_archive_flag = "";//记录类型   T  在线数据    H 历史数据
	CString v_mat_destion = "";//材料去向
	CString v_mat_status = "";//材料状态
	CString v_pono = "";//制造命令号
	CString v_prod_time_from = "";//开始时间
	CString v_prod_time_to = "";//结束时间
	CString v_heat_no = "";//熔炼号
	CString v_order_no = "";//合同号
	CString v_stock_no = "";//库区号
	CString v_in_flag = "";//入库标记
	CString v_st_no = "";//出钢记号
	CString v_mat_no = "";//材料号



	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm33zdsh("TMMSM33ZDSH");
	CModel tmmsm3e("TMMSM3E");

	CDbCommand cmd_inq(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		//获取传入参数
		tmmsm3e.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "v_mat_no[{0}] ", v_mat_no);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) WHERE MAT_NO = '" + v_mat_no + "'";
			sqlstr = " SELECT * FROM TMMSM3E  WHERE MAT_NO = '" + v_mat_no + "'";

			if (tmmsm3e["MAT_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_NO	= @v_mat_no";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY  RESUME_SEQ_NO DESC";
			//sqlstr_temp += " ORDER BY  PROD_TIME DESC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		cmd_inq.Parameters.Set("v_mat_no", v_mat_no);


		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();


		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

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
