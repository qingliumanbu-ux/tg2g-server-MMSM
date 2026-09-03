/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   WANGSHULING
Version:    1.0
Date:     2023-12-26 11:17:56
Description: 【二炼北】缓冷发送实绩
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

//电文发送头文件
#include "epex.h"

int f_mmsm_210038_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn){

	CTracer log(__FUNCTION__);
	int doFlag = 0;

	CString tc_no = "210038";
	EPEX epex;

	CModel tmmsm36("TMMSM36");

	try{
		if (epex.Initialize(tc_no) < 0){
			strcpy(s.msg, "电文初始化失败。");
			Log::Trace("", __FUNCTION__, "电文初始化失败[{0}]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (epex.SetValue("bapiheader.msgtype", 0, tc_no) < 0)
		{
			sprintf(s.msg, "电文压电文号失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//bapiheader.freeuse1-5  自由使用      37表中没有字段


		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++){
			tmmsm36.Reset();
			tmmsm36.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (epex.SetValue(i, tmmsm36) < 0)
			{
				sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (epex.SetValue("tmmsm37.prod_group_no", i, tmmsm36["PROD_SHIFT_GROUP"].ToString()) < 0)
			{
				sprintf(s.msg, "电文压生产班组号值失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//group_leader 班长 
			if (epex.SetValue("tmmsm37.operator", i, tmmsm36["REC_CREATOR"].ToString()) < 0)
			{
				sprintf(s.msg, "电文压值1失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("tmmsm37.treat_id", i, tmmsm36["PROC_NO"].ToString()) < 0)
			{
				sprintf(s.msg, "电文压值2失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//tmmsm37.load_time 装入时刻
			//tmmsm37.load_temp 装入温度
			//tmmsm37.cooldown_rate 缓冷速度
			//tmmsm37.cooldown_duration 缓冷时间(实绩)
			//tmmsm37.unload_time 出坑时刻
			//tmmsm37.unload_temp 出坑温度
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