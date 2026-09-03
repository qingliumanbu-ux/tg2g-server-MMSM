/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:		李晓明
Version:	1.0
Date:		2023-11-13 17:13:56
Description: 鱼雷罐实绩查询(测试)
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsmxm01_inq)

int f_mmsmxm01_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString recvIronEndTimeStart = "";
	CString recvIronEndTimeEnd = "";

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsmts01("TMMSMTS01");

	CDbCommand cmd_inq(conn);

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
		tmmsmts01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsmts01.Print();

		sqlstr_count = "SELECT COUNT(1) FROM TMMSMTS01 WHERE 1=1 ";
		sqlstr = "SELECT * FROM TMMSMTS01 WHERE 1=1 ";

		//高炉号
		if (tmmsmts01["BF_ID"].ToString().Trim() != ""){
			sqlstr_temp += "AND BF_ID LIKE '%' || @tmmsmts01.BF_ID || '%'";
		}

		//铁次号
		if (tmmsmts01["IRON_NO"].ToString().Trim() != ""){
			sqlstr_temp += "AND IRON_NO LIKE '%' || @tmmsmts01.IRON_NO || '%'";
		}

		//罐号
		if (tmmsmts01["TPC_YL_NO"].ToString().Trim() != ""){
			sqlstr_temp += "AND TPC_YL_NO LIKE '%' || @tmmsmts01.TPC_YL_NO || '%'";
		}

		//罐次号
		if (tmmsmts01["TRE_TPC_NO"].ToString().Trim() != ""){
			sqlstr_temp += "AND TRE_TPC_NO LIKE '%' || @tmmsmts01.TRE_TPC_NO || '%'";
		}

		//受铁结束时间起
		if (bcls_rec->Tables[0].Rows[0]["RECV_IRON_END_TIME_START"].ToString().Trim() != ""){
			recvIronEndTimeStart = bcls_rec->Tables[0].Rows[0]["RECV_IRON_END_TIME_START"].ToString().Trim();
			sqlstr_temp += "AND RECV_IRON_END_TIME >= @recvIronEndTimeStart ";
		}
		//受铁结束时间止
		if (bcls_rec->Tables[0].Rows[0]["RECV_IRON_END_TIME_END"].ToString().Trim() != ""){
			recvIronEndTimeEnd = bcls_rec->Tables[0].Rows[0]["RECV_IRON_END_TIME_END"].ToString().Trim();
			sqlstr_temp += "AND RECV_IRON_END_TIME <= @recvIronEndTimeEnd ";
		}

		sqlstr_count = sqlstr_count + sqlstr_temp;
		sqlstr_temp += "ORDER BY TRE_TPC_NO";
		sqlstr = sqlstr + sqlstr_temp;

		cmd_inq.Parameters.Set("tmmsmts01.BF_ID", tmmsmts01["BF_ID"].ToString());
		cmd_inq.Parameters.Set("tmmsmts01.IRON_NO", tmmsmts01["IRON_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsmts01.TPC_YL_NO", tmmsmts01["TPC_YL_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsmts01.TRE_TPC_NO", tmmsmts01["TRE_TPC_NO"].ToString());
		cmd_inq.Parameters.Set("recvIronEndTimeStart", recvIronEndTimeStart);
		cmd_inq.Parameters.Set("recvIronEndTimeEnd", recvIronEndTimeEnd);
		Log::Trace("", __FUNCTION__, "dateStart = {0}, dateEnd = {1}", recvIronEndTimeStart, recvIronEndTimeEnd);

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();

		Log::Trace("", __FUNCTION__, "sqlstr = {0}", sqlstr);
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