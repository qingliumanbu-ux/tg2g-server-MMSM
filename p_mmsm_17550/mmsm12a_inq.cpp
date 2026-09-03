/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:     李晓明
Version:    1.0
Date:     2023-11-14 11:28:56
Description: 鱼雷罐倒铁实绩查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsm12a_inq)

int f_mmsm12a_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm12a("TMMSM12A");

	CDbCommand cmd(conn);

	try{
		try
		{
			//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		//获取传入参数
		tmmsm12a.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		sqlstr_count = "SELECT COUNT(1) FROM TMMSM12A WHERE 1 = 1 ";
		sqlstr = "SELECT * FROM TMMSM12A T WHERE 1 = 1 ";

		//处理号
		if (tmmsm12a["TPD_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += "AND TPD_NO LIKE '%' || @tmmsm12a.TPD_NO || '%' ";
		}

		//铁水包号
		if (tmmsm12a["IRON_LADLE_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += "AND IRON_LADLE_NO LIKE '%' || @tmmsm12a.IRON_LADLE_NO || '%' ";
		}

		//铁次号
		if (tmmsm12a["IRON_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += "AND IRON_NO LIKE '%' || @tmmsm12a.IRON_NO || '%' ";
		}

		//鱼雷罐号
		if (tmmsm12a["TPC_YL_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += "AND TPC_YL_NO LIKE '%' || @tmmsm12a.TPC_YL_NO || '%' ";
		}

		cmd.Parameters.Set("tmmsm12a.TPD_NO", tmmsm12a["TPD_NO"].ToString().Trim());
		cmd.Parameters.Set("tmmsm12a.IRON_LADLE_NO", tmmsm12a["IRON_LADLE_NO"].ToString().Trim());
		cmd.Parameters.Set("tmmsm12.IRON_NO", tmmsm12a["IRON_NO"].ToString().Trim());
		cmd.Parameters.Set("tmmsm12a.TPC_YL_NO", tmmsm12a["TPC_YL_NO"].ToString().Trim());

		sqlstr_count = sqlstr_count;
		sqlstr_temp += "ORDER BY TPD_NO DESC, PROC_COUNT ASC";
		sqlstr = sqlstr + sqlstr_temp;
		
		cmd.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd.ExecuteScalar().ToInt32();
		Log::Trace("", __FUNCTION__, "count = {0}", TotalRecordCount);
		//分页获取
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd.Close();

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