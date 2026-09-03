/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 总消耗查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmzxhf4_inq)

int f_mmsmzxhf4_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CModel tmmsmzxhbb_lh("TMMSMZXHBB_LH");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		tmmsmzxhbb_lh.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		tmmsmzxhbb_lh["REC_REVISOR"] = s.userid;
		tmmsmzxhbb_lh["REC_REVISE_TIME"] = datetime;
		tmmsmzxhbb_lh["CHECK_FLAG"] = "1";
		tmmsmzxhbb_lh.Update("REC_REVISOR,REC_REVISE_TIME,CHECK_FLAG", "USER_ID,TIME_STAMPS");

		//将核算结果插入历史表
		sqlstr = " insert into hmmsmzxhbb"
			" select * from tmmsmzxhbb"
			" where USER_ID = @user_id"
			" and TIME_STAMPS=@time_stamps"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
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
