/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 铁水信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsm12qys2nf8_upd)

int f_mmsm12qys2nf8_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlstr2 = "";
	CString sqlstr3 = "";
	CString sqlstr4 = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;

	CString v_tpc_yl_no = "";
	CString v_iron_no = "";

	CString ch_start_time_f = "";
	CString ch_start_time_t = "";


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm12("TMMSM12");

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


		//--------------------------------
		//获取传入参数
		tmmsm12.MergeFrom(bcls_rec->Tables[0].Rows[0]);


		if (bcls_rec->Tables[0].Columns.Contains("TPC_YL_NO"))
			v_tpc_yl_no = bcls_rec->Tables[0].Rows[0]["TPC_YL_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("IRON_NO"))
			v_iron_no = bcls_rec->Tables[0].Rows[0]["IRON_NO"].ToString();
		

		/* ***** 打印输入参数 ***** */
		////Log::Info("", __FUNCTION__, "FACTORY_DIV =[{0}]", tmmsm14["FACTORY_DIV"].ToString());
		////Log::Info("", __FUNCTION__, "start_time_f  =[{0}]", ch_start_time_f);
		////Log::Info("", __FUNCTION__, "start_time_t  =[{0}]", ch_start_time_t);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM12 "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * "
				"   FROM TMMSM12 "
				"  WHERE 1=1 "
				;

			sqlstr1 = "UPDATE TMMSM12"
				"	SET POUR_FLAG1 = 'N' "
				"  WHERE TPC_YL_NO1	= @v_tpc_yl_no AND IRON_NO1	= @v_iron_no  "
				;

			sqlstr2 = "UPDATE TMMSM12"
				"	SET POUR_FLAG2 = 'N' "
				"  WHERE TPC_YL_NO2	= @v_tpc_yl_no AND IRON_NO2	= @v_iron_no  "
				;
			sqlstr3 = "UPDATE TMMSM12"
				"	SET POUR_FLAG3 = 'N' "
				"  WHERE TPC_YL_NO3	= @v_tpc_yl_no AND IRON_NO3	= @v_iron_no  "
				;
			sqlstr4 = "UPDATE TMMSM12"
				"	SET POUR_FLAG4 = 'N' "
				"  WHERE TPC_YL_NO4	= @v_tpc_yl_no AND IRON_NO4	= @v_iron_no  "
				;


			sqlstr_count = sqlstr_count + sqlstr_temp;
			
			break;
		}

		
		cmd_inq.Parameters.Set("v_tpc_yl_no", v_tpc_yl_no);
		cmd_inq.Parameters.Set("v_iron_no", v_iron_no);
		

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr1);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.SetCommandText(sqlstr2);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.SetCommandText(sqlstr3);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.SetCommandText(sqlstr4);
		cmd_inq.ExecuteNonQuery();
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
