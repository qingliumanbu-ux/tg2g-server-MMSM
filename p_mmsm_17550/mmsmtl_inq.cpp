/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XX
Version:    1.0
Date:
Description: 连铸退钢水查询
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件

int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//L4
BM2F_ENTERACE(mmsmtl_inq)

int f_mmsmtl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = " ";
	CString sql_insert = " ";
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_2a(conn);
	CString station_id = " ";
	CString v_proc_div = " ";
	CString	datetime("");
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsmgy05("TMMSMGY05");
	CModel tmmsm31("TMMSM31");
	CString id = " ";
	CString heat_no = " ";
	CString sm_plan_nol2 = " ";
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	blkNum = bcls_rec->Tables.IndexOf("T823");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("T823");
	}
	bcls_rec->Tables["T823"].Columns.Add(tmmsm2a);
	if (!bcls_rec->Tables["T823"].Columns.Contains("DEAL_FLAG"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "DEAL_FLAG");
	}
	try
	{
		sqlstr = (" select * from TMMSM31 WHERE REC_CREATE_TIME>='20240630606060' and REC_CREATE_TIME<='20240730606060' AND REC_CREATOR!='QC' ");
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{
			cmd_sql.Fetch(tmmsm31);
			//调用发送电文
			blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("MMSMSND");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_DIV");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("HEAT_NO"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "HEAT_NO");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_NO"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_NO");
			}
			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("L2_PROC_NO"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "L2_PROC_NO");
			}

			bcls_rec->Tables["MMSMSND"].Rows.Add();
			bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM31";
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "U";
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tmmsm31["PROC_NO"];
			bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tmmsm31["HEAT_NO"];
			bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tmmsm31["L2_PROC_NO"];
			doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		cmd_sql.Close();
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


