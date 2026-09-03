/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-25
Description: 制造命令号对应的板坯查询
**************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/ 
//#include "tmmsm15.h"


// service入口
BM2F_ENTERACE(mmsm50af8_snd)
//-EP_SYSTEM_HEAD_END                                                  
int f_mmsm50af8_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection* conn)
{

	EPEX epex;
CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	int ret = 0;
	/* 业务变量 */
	CString lpsz_tc_no = "GEG901";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");;
	CString v_table_type  = "";
	CString work_date = "";
	
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	CModel tmmsm15("TMMSM15");

	try
	{	
		////初始化
		//ret = epex.Initialize(lpsz_tc_no);
		//if (ret < 0)
		//{
		//	//EDLog(1, 1, "epex.Initialize [%s] code= [%d]",lpsz_tc_no,ret);
		//	CFormattable arguments[] = { lpsz_tc_no }; // 定义参数列表的数组
		//	CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

		////传入参数检核
		//if (bcls_rec->Tables[0].Rows.get_Count() == 0)
		//{
		//	strncpy(s.msg, "传入参数出错...", sizeof(s.msg) - 1);
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		//work_date = datetime.Substring(0, 8);
		//for (int i = 1; i <= bcls_rec->Tables[0].Rows.get_Count(); i++)
		//{
		//	tmmsm15["IRON_NO"] = bcls_rec->Tables[0].Rows[i - 1]["IRON_NO"].ToString().Trim();
		//	tmmsm15["CFID"] = bcls_rec->Tables[0].Rows[i - 1]["CFID"].ToString().Trim();
		//	tmmsm15["IRON_DEST"] = bcls_rec->Tables[0].Rows[i - 1]["FACTORY_DIV"].ToString().Trim();
		//	tmmsm15["ARRIVE_REAL_TIME"] = datetime;
		///*	Log::Info("", __FUNCTION__, "tmmsm15.IRON_NO=[{0}]", tmmsm15.IRON_NO);
		//	Log::Info("", __FUNCTION__, "tmmsm15.CFID=[{0}]", tmmsm15.CFID);
		//	Log::Info("", __FUNCTION__, "tmmsm15.ARRIVE_REAL_TIME=[{0}]", tmmsm15.ARRIVE_REAL_TIME*/
		//	
	
		//	tmmsm15.Update("ARRIVE_REAL_TIME","IRON_NO,CFID");
		//	if (epex.SetValue("OP_FLAG", 0, "I"))
		//	{
		//		strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//	if (epex.SetValue("WORK_DATE", 0, work_date))
		//	{
		//		strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//	if (epex.SetValue("IR_TAP_NO", 0, tmmsm15["IRON_NO"]))
		//	{
		//		strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//	//if (tmmsm15["CFID"].GetLength() >2 && tmmsm15["CFID"].SubstringNE(0, 2) == "TG")
		//	//{

		//	//	tmmsm15["CFID"] = tmmsm15["CFID"].SubstringNE(2, tmmsm15["CFID"].GetLength() - 2);    //20181220GZG修改
		//	//}
		//	if (epex.SetValue("TPC_NO", 0, tmmsm15["CFID"]))
		//	{
		//		strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//	if (epex.SetValue("ARRIVE_TIME", 0, datetime))
		//	{
		//		strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//	if (epex.SendTele() < 0)
		//	{
		//		strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
		//		strcpy(s.sysmsg, "电文发送失败");
		//		Log::Trace("", __FUNCTION__, "电文发送失败 = [{0}]", epex.GetMsg());
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
	

		//}
		//epex.Uninitialize();


	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
} 




