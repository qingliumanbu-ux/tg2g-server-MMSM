/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2024-01-29 11:17:56
Description: 来铁信息发送倒罐站
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

//电文发送头文件
#include "epex.h"

//业务头文件
#include "xt8ed01.h"

int f_t8ed01_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn){

	CTracer log(__FUNCTION__);
	int doFlag = 0;

	CString tc_no = "T8ED01";
	EPEX epex;

	CT8ED01 xt8ed01(conn);

	try{
		if (epex.Initialize(tc_no) < 0){
			strcpy(s.msg, "电文初始化失败。");
			Log::Trace("", __FUNCTION__, "电文初始化失败[{0}]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		Log::Trace("", __FUNCTION__, "COUNT = {0}", bcls_rec->Tables[0].Rows.get_Count());
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++){
			xt8ed01.Reset();

			xt8ed01.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			xt8ed01.Print();

			if (epex.SetValue(i, xt8ed01)){
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
