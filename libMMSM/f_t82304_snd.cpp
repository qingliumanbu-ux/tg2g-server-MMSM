/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2023-11-10 11:17:56
Description: 发送实际工艺路径
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

//电文发送头文件
#include "epex.h"



int f_t82304_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn){

	CTracer log(__FUNCTION__);
	int doFlag = 0;

	CString tc_no = "T82304";
	CString heat_no = "";
	CString whole_backlog_act = "";
	CString sqlstr = "";

	EPEX epex; 

	CDbCommand cmd_inq(conn);
	
	 
	try
	{
		if(epex.Initialize(tc_no) < 0)
		{
			strcpy(s.msg, "电文初始化失败。");
			Log::Trace("", __FUNCTION__, "电文初始化失败[{0}]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		//如工艺路径拼接
		sqlstr = " select dev_code from tmmsmgy06"
			" where 1=1"
			" and HANDLE_DIV in ( 'I',' ','U')"
			" and heat_no=@heat_no"
			" order by start_time"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			whole_backlog_act = whole_backlog_act + cmd_inq.GetString(1);
		}
		cmd_inq.Close();  		

		if (epex.SetValue("HEAT_NO", 0, heat_no) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (epex.SetValue("WHOLE_BACKLOG_ACT", 0, whole_backlog_act) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (epex.SendTele() < 0)
		{
			strcpy(s.msg, "电文发送失败。");
			Log::Trace("", __FUNCTION__, "电文发送失败[{0}]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		epex.Uninitialize();
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}