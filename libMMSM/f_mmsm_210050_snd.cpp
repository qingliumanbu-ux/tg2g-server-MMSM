/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2024-05-08 11:17:56
Description: 铁水数据发送
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

//电文发送头文件
#include "epex.h"

//接口头文件
#include "x210050bapiheader.h"
#include "x210050tqmtsts.h"

int f_mmsm_210050_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn){

	CTracer log(__FUNCTION__);
	int doFlag = 0;

	CString tc_no = "210050";
	EPEX epex;

	C210050BAPIHEADER X210050BAPIHEADER(conn);
	C210050TQMTSTS X210050TQMTSTS(conn);

	try
	{
		if (epex.Initialize(tc_no) < 0)
		{
			strcpy(s.msg, "电文初始化失败。");
			Log::Trace("", __FUNCTION__, "电文初始化失败[{0}]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		for (int i = 0; i < bcls_rec->Tables["TQMTSTS"].Rows.get_Count(); i++){
			X210050BAPIHEADER.Reset();
			X210050BAPIHEADER.MergeFrom(bcls_rec->Tables["BAPIHEADER"].Rows[i]);
			if (epex.SetValue("BAPIHEADER", i, X210050BAPIHEADER) < 0){
				strcpy(s.msg, "电文拼接失败。");
				Log::Trace("", __FUNCTION__, "电文拼接失败[{0}]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			X210050TQMTSTS.Reset();
			X210050TQMTSTS.MergeFrom(bcls_rec->Tables["TQMTSTS"].Rows[i]);

			X210050TQMTSTS.Print();

			if (epex.SetValue("TQMTSTS", i, X210050TQMTSTS) < 0){
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