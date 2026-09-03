/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料主信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm2aft_insft)

int f_mmsm2aft_insft(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	int r_count = 0;
	CDecimal nd_seq_no = 0;
	CString datetime("");
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	//系统的分页类信息。
	//CPageInfo pageInfo;
	Log::Trace(" ", __FUNCTION__, "1111111111111");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm2a("TMMSM2A_YL");
	CModel tmmsm2a1("TMMSM2A_YL");

	CDbCommand cmd_inq(conn);

	try
	{
		
		r_count = bcls_rec->Tables["Table0"].Rows.get_Count();
		tmmsm2a1.MergeFrom(bcls_rec->Tables["Table1"].Rows[0]);

		if (r_count>0)
		{
			for (int i = 0; i <r_count; i++)
			{
				tmmsm2a.MergeFrom(bcls_rec->Tables["Table0"].Rows[i]);
				tmmsm2a["DEVO_WT"] = (tmmsm2a1["DEVO_WT"].ToDecimal() / r_count).Round(3);
				sqlstr = " SELECT NVL(MAX(PROC_COUNT),0) PROC_COUNT FROM TMMSM2A_YL WHERE MAT_CODE=@MAT_CODE AND "
					" DEV_CODE=@DEV_CODE AND HEAT_NO = @HEAT_NO AND REMARK_4 = @REMARK_4";
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("MAT_CODE", tmmsm2a["MAT_CODE"].ToString());
				cmd_inq.Parameters.Set("DEV_CODE", tmmsm2a["DEV_CODE"].ToString());
				cmd_inq.Parameters.Set("HEAT_NO", tmmsm2a["HEAT_NO"].ToString());
				cmd_inq.Parameters.Set("REMARK_4", tmmsm2a["REMARK_4"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					nd_seq_no = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();
				tmmsm2a.TrimOrBlank();
				tmmsm2a["PROC_COUNT"] = nd_seq_no + 1;
				tmmsm2a["DEVO_TIME"] = datetime;
				tmmsm2a["REC_CREATOR"] = s.userid;   //记录创建责任者
				tmmsm2a["REC_CREATE_TIME"] = datetime;   //记录创建时刻
				tmmsm2a.Insert();
			}
			tmmsm2a1["REMARK_4"] = "0";
			tmmsm2a1.Update("REMARK_4","MAT_CODE,PROC_COUNT");
		}
		
		//tmmsm2a.TrimOrBlank();
		//tmmsm2a["DEVO_TIME"] = datetime;
		//tmmsm2a["REC_CREATOR"] = s.userid;   //记录创建责任者
		//tmmsm2a["REC_CREATE_TIME"] = datetime;   //记录创建时刻
		//tmmsm2a["REMARK_4"] = "1";

		//tmmsm2a.Insert();

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
