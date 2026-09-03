/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   herui
Version:    1.0
Date:     2024-01-23
Description: 一钢计量实绩接收
**************************************************/

/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"
//#include "x_psi_tel.h"			//用于PO电文收发
//#include "tmmsm81.h"

/**** 引入函数声明 ****/
BM2_FUNCTION_IMPORT
int f_mmsm81_d021_handle_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_ophpmm_24ph05_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
/**** Service入口 ****/
//BM2F_ENTERACE_TELE(d02410_rcv)改为rest服务调用
BM2F_ENTERACE(cm_ewt801_rcv)
int f_cm_ewt801_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;

	//电文号
	CString tc_no = "ZCHO_JLBD_RFC"; //计量系统计量磅单电文

	//系统时间
	//CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	//数据库命令执行变量
	CDbCommand cmd(conn);
	CString sqlstr("");

	////添加并设置块名
	//int index = 0;
	//index = bcls_rec->Tables.IndexOf("ZCHO_JLBD_RFC");
	//if (index < 0)
	//{
	//	bcls_rec->Tables.Add("ZCHO_JLBD_RFC");
	//}

	try
	{
		Log::Trace("", __FUNCTION__, "------------ZCHO_JLBD_RFC----------", "");

		//tmmsm81_rcv["WEIGH_NO"] = 
		//	bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["WEIGH_NO"].ToString();//磅单号
		//tmmsm81_rcv["MAT_CODE"] =
		//	bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["MAT_CODE"].ToString();//物料代码

		//Log::Trace("", __FUNCTION__, "WEIGH_NO=[{0}]", tmmsm81_rcv["WEIGH_NO"].ToString());
		//Log::Trace("", __FUNCTION__, "MAT_CODE=[{0}]", tmmsm81_rcv["MAT_CODE"].ToString());
		for (int i = 0; i < bcls_rec->Tables.get_Count(); i++)
		{
			Log::Trace("", "", "--表名：[{0}]---", bcls_rec->Tables[i].get_TableName());
			bcls_rec->Tables[0].set_TableName("ZCHO_JLBD_RFC");
		}
		doFlag = f_mmsm81_d021_handle_rcv(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		Log::Trace("", __FUNCTION__, "------------RECEIVE_SUCCESS----------", "");


		/*设置系统返回参数*/
		sprintf(s.msg, _RES("RECEIVE_SUCCESS")/*电文接收成功。*/);
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("RECEIVE_FAIL")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.msg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd.Close();
	return doFlag;
}