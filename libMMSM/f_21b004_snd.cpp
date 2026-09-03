/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2023-11-10 11:17:56
Description: 铁水质量信息发送
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

//电文发送头文件
#include "epex.h"

//接口头文件
#include "x21b00421B004.h"
#include "x21b00421B004_1.h"

int f_21b004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn){

	CTracer log(__FUNCTION__);
	int doFlag = 0;

	CString tc_no = "21B004";
	EPEX epex;

	C21B00421B004 X21b004(conn);
	C21B00421B004_1 X21b004_1(conn);

	try
	{
		if (epex.Initialize(tc_no) < 0)
		{
			strcpy(s.msg, "电文初始化失败。");
			Log::Trace("", __FUNCTION__, "电文初始化失败[{0}]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		X21b004.MergeFrom(bcls_ret->Tables[0].Rows[0]);
		
		if (epex.SetValue("21B004", 0, X21b004) < 0){
			strcpy(s.msg, "电文拼接失败。");
			Log::Trace("", __FUNCTION__, "电文拼接失败[{0}]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++){
			X21b004_1.Reset();

			X21b004_1.MergeFrom(bcls_ret->Tables[0].Rows[i]);

			X21b004_1.Print();

			if (epex.SetValue("21B004_1", i, X21b004_1) < 0){
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