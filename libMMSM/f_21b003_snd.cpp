/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2023-11-10 11:17:56
Description: 铁水收料确认实绩发送
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

//电文发送头文件
#include "epex.h"

//接口头文件
#include "x21b003TSDK.h"

int f_21b003_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn){

	CTracer log(__FUNCTION__);
	int doFlag = 0;

	CString tc_no = "21B003";
	EPEX epex;

	C21B003TSDK X21b003(conn);
	 
	try
	{
		if(epex.Initialize(tc_no) < 0)
		{
			strcpy(s.msg, "电文初始化失败。");
			Log::Trace("", __FUNCTION__, "电文初始化失败[{0}]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++){
			X21b003.Reset();

			X21b003.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			X21b003.Print();

			if (epex.SetValue("TSDK", i, X21b003) < 0){
				strcpy(s.msg, "电文拼接失败。");
				Log::Trace("", __FUNCTION__, "电文拼接失败[{0}]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
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