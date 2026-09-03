/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XX
Version:    1.0
Date:
Description: 能源请求消耗
**************************************************/
//框架头文件
#include "stdafx.h" 

//程序用头文件


int f_t8f0s1_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//能源请求消耗
BM2F_ENTERACE(mmsmt8f0s1_inq)
int f_mmsmt8f0s1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	int i_c = 0;
	CString sqlstr = " ";
	CString sql_insert = " ";
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_2a(conn);
	CString station_id = " ";
	CString v_proc_div = " ";
	CString	datetime("");
	CString id = " ";
	CString sm_plan_nol2 = " ";
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	blkNum = bcls_rec->Tables.IndexOf("T8F0S1");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("T8F0S1");
	}

	if (!bcls_rec->Tables["T8F0S1"].Columns.Contains("NO"))
	{
		bcls_rec->Tables["T8F0S1"].Columns.Add(DT_STRING, "NO");
	}
	if (!bcls_rec->Tables["T8F0S1"].Columns.Contains("ITEMID"))
	{
		bcls_rec->Tables["T8F0S1"].Columns.Add(DT_STRING, "ITEMID");
	}
	if (!bcls_rec->Tables["T8F0S1"].Columns.Contains("CLOCK"))
	{
		bcls_rec->Tables["T8F0S1"].Columns.Add(DT_STRING, "CLOCK");
	}
	//DATI_MSG_SENT
	if (!bcls_rec->Tables["T8F0S1"].Columns.Contains("DATI_MSG_SENT"))
	{
		bcls_rec->Tables["T8F0S1"].Columns.Add(DT_STRING, "DATI_MSG_SENT");
	}
	bcls_rec->Tables["T8F0S1"].Rows.Clear();

	try
	{
		Log::Trace("", "", "", "", "[{0}]", __LINE__);
		sqlstr = ("  select ROW_NUMBER() OVER (ORDER BY METE_CODE_2) AS no,METE_CODE_2,TO_CHAR(sysdate-1, 'yyyy-MM-dd') as CLOCK,TO_CHAR(sysdate-1, 'yyyyMMdd') as CLOCK_1 from TMMSMT8METE  ");
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{
			bcls_rec->Tables["T8F0S1"].Rows.Add();
			bcls_rec->Tables["T8F0S1"].Rows[i_c]["NO"] = cmd_sql.GetString(4) + cmd_sql.GetString(1);
			bcls_rec->Tables["T8F0S1"].Rows[i_c]["ITEMID"] = cmd_sql.GetString(2);
			bcls_rec->Tables["T8F0S1"].Rows[i_c]["CLOCK"] = cmd_sql.GetString(3);
			bcls_rec->Tables["T8F0S1"].Rows[i_c]["DATI_MSG_SENT"] = cmd_sql.GetString(4);
			i_c++;
			Log::Trace("", "Rows", "Rows = {0}", bcls_rec->Tables["T8F0S1"].Rows.get_Count(), i_c);


		}
		cmd_sql.Close();

		doFlag = f_t8f0s1_snd(bcls_rec, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", "Rows", "Rows = {0}", 1);



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
		Log::Trace("", __FUNCTION__, "s.flag[{0}]", s.flag);
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

