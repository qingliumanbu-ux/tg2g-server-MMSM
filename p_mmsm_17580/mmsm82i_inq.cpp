/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 检测异常信息
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm82i_inq)
int f_mmsm82i_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_where = "";
	CString heat_no = "";
	CString type_flag = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDbCommand cmd_inq(conn);

	CModel tpssm35("TPSSM35");

	try
	{ 	

		begin_time = bcls_rec->Tables[0].Rows[0]["START_TIME_S"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["START_TIME_E"].ToString();
		type_flag = bcls_rec->Tables[0].Rows[0]["TYPE_FLAG"].ToString();

		if (type_flag.Trim() == "")
		{
			strcpy(s.msg, "错误类型不能为空！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (begin_time.Trim() == "" || end_time.Trim() == "")
		{
			strcpy(s.msg, "开始时间结束时间不能为空！");
			throw CApplicationException(-1, s.msg, log.Location);
		} 		

		if (type_flag == "1") //已收货未发送的
		{
			sqlstr = " select heat_no, '未发送炉号信息' as remark from tmmsmgy05"
				" where heat_no in (select heat_no from   "
				" (select heat_no, min(RECV_MAT_TIME) RECV_MAT_TIME from(	 "
				" select heat_no, RECV_MAT_TIME from tmmsm01  where RECV_MAT_TIME != ' '  "
				" union select heat_no, RECV_MAT_TIME from hmmsm01  where RECV_MAT_TIME != ' ' "
				" ) group by heat_no  "
				" ) where RECV_MAT_TIME <= @end_time and RECV_MAT_TIME >= @begin_time "
				" ) and VALID_FLAG_1 != '1'	 "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}

		if (type_flag == "2") //已收货未发送的
		{
			sqlstr = " select heat_no, '未收货完成' as remark "
				" from   "
				" (select heat_no, min(RECV_MAT_TIME) RECV_MAT_TIME from(	 "
				" select heat_no, RECV_MAT_TIME from tmmsm01  where RECV_MAT_TIME != ' '  "
				" union select heat_no, RECV_MAT_TIME from hmmsm01  where RECV_MAT_TIME != ' ' "
				" ) group by heat_no  "
				" ) t1 where RECV_MAT_TIME <= @end_time and RECV_MAT_TIME >= @begin_time "
				" ) and exists (select 1 from tmmsm01 t2 where t2.RECV_MAT_TIME = ' ' and t1.heat_no=t2.heat_no"
				" union select 1 from hmmsm01 t2 where t2.RECV_MAT_TIME = ' ' and t1.heat_no=t2.heat_no)	 "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
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
