/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		yanlei
Version:    1.0
Date:		2015-1-29
Description:一炼钢高炉铁水信息发送电文
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm191c13_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	 CTracer log(__FUNCTION__);
	 /* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;

	/* 业务变量 */
	CString tc_no = " "; //电文号
	CString iron_no = " "; //计划号
	CString tpc_no = " "; //前炉制造命令号

	
	EPEX epex;

	/* 实体类定义 */

	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	
	try
	{
		//获取输入参数
		tmmsm11.IRON_NO = bcls_rec->Tables[0].Rows[0]["IRON_NO"].ToString().Trim();
		tmmsm11.TPC_YL_NO = bcls_rec->Tables[0].Rows[0]["TPC_YL_NO"].ToString().Trim();
	
		
		tc_no = "191C13"; 

		Log::Trace("", __FUNCTION__, "===tc_no= [{0}]", tc_no);

		tmmsm11.Query("IRON_NO,TPC_YL_NO");
		tmmsm11.TrimOrBlank();
		
		race("",__FUNCTION__,"tmmsm21 BEGIN 发送电文开始 tmmsm21.HEAT_NO	= [{0}]",tmmsm21.HEAT_NO);	
				
		if(epex.Initialize(tc_no) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location );
		}
		
		if (epex.SetValue(0, tmmsm11) < 0)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if(epex.SendTele() < 0)
		{
			sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location );
		}
		
		// 释放
		epex.Uninitialize();	
		
		/* ********* 程序处理结束 ********** */
		strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);	
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
	
}
