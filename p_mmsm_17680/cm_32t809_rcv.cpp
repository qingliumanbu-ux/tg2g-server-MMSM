/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:sw
Date:2023-11-13
Version:1.0
Description: 铁区库存信息接收
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE_TELE(cm_32t809_rcv)

int f_cm_32t809_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString next_date = "";
	CString sqlstr = "";
	CDbCommand cmd_inq(conn);
	CModel tmmsm57d("TMMSM57D");
	CModel tmmsm57d_m("TMMSM57D");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm57d.Reset();
			tmmsm57d.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm57d["STAT_DATE"] = bcls_rec->Tables[0].Rows[i]["START_DATE"].ToString();
			
			if ("" == tmmsm57d["MAT_CODE"].ToString().Trim())
			{
				strcpy(s.msg, "物料代码不能为空!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if ("" == tmmsm57d["STAT_DATE"].ToString().Trim())
			{
				strcpy(s.msg, "统计日期不能为空!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm57d.Delete("MAT_CODE,STAT_DATE");
			tmmsm57d["REC_CREATE_TIME"] = datetime;
			tmmsm57d["REC_CREATOR"] = s.userid;
			tmmsm57d.TrimOrBlank();
			tmmsm57d.Insert();

			//取最新的更新为月库存
			tmmsm57d_m.Reset();
			tmmsm57d_m.CopyFrom(tmmsm57d);
			tmmsm57d_m["STAT_DATE"] = tmmsm57d["STAT_DATE"].ToString().SubstringNE(0, 6);
			tmmsm57d_m.Delete("MAT_CODE,STAT_DATE");
			tmmsm57d_m.TrimOrBlank();
			tmmsm57d_m.Insert();

		}
		

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
