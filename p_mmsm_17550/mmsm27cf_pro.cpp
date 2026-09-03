/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     wsl
Version:    1.0
Date:       2024-01-25
Description: 维护保存
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件


BM2F_ENTERACE(mmsm27cf_pro)

int f_mmsm27cf_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString CODE_now = "";
	CModel tmmsm27("TMMSM27");
	try
	{
		tmmsm27.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm27["REC_REVISOR"] = s.userid;
		tmmsm27["REC_REVISE_TIME"] = dateNow;
		sqlstr = "UPDATE TMMSM27 SET REC_REVISE_TIME='" + tmmsm27["REC_REVISE_TIME"].ToString() + "',REC_REVISOR ='" + tmmsm27["REC_REVISOR"].ToString() + "',HEATNO_PREMELT1 = '" + tmmsm27["HEATNO_PREMELT1"].ToString() + "',HEATNO_PREMELT2 = '" + tmmsm27["HEATNO_PREMELT2"].ToString() + "',HEATNO_PREMELT3 = '" + tmmsm27["HEATNO_PREMELT3"].ToString() + "',HEATNO_PREMELT4= '"+tmmsm27["HEATNO_PREMELT4"].ToString()+"'"
			",HEATNO_PREMELT5= '" + tmmsm27["HEATNO_PREMELT5"].ToString() + "',HEATNO_PREMELT6 = '" + tmmsm27["HEATNO_PREMELT6"].ToString() + "' "
			" WHERE HEAT_NO = '" + tmmsm27["HEAT_NO"].ToString() + "' AND PROC_NO = '"+ tmmsm27["PROC_NO"].ToString() + "'";
		Log::Trace("", "", "sqlstr = {0}", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


