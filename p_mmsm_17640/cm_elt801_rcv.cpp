/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      XXX
Version:     1.0
Date:        2023-10-23
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// DES生产KR实绩表电文
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

void f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn); //班次班别函数
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//L4发送
int f_t8z_23m_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送专家系统数据
BM2F_ENTERACE_TELE(cm_elt801_rcv)

int f_cm_elt801_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	/* 业务变量 */

	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString v_proc_div = " ";
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_insert(conn);
	CString sql_insert="";
	CModel tmmsmkr14("TMMSMKR14");
	CModel da_kr_summary("DA_KR_SUMMARY");
	EIClass tmmsm14_back;
	CString dev_code = "";
	CString sm_plan_no2 = "";//炼钢计划号

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString strShiftNo("");
	CString strShiftGroup("");
	CString cal_starttime("");
	EIClass in_23m;
	in_23m.Tables[0].Columns.Add(DT_STRING, "TC_NO");
	in_23m.Tables[0].Rows.Add();
	in_23m.Tables[0].Rows[0]["TC_NO"] = "T82319";
	in_23m.Tables.Add();
	in_23m.Tables[1].Columns.Add(tmmsmkr14);

	try
	{
		tmmsmkr14["ID_SJ"] = bcls_rec->Tables["DES_SUMMARY"].Rows[0]["ID"].ToString();
		tmmsmkr14.MergeFrom(bcls_rec->Tables["DES_SUMMARY"].Rows[0]);
		Log::Trace("", "LADLE_ARRIVE1", "LADLE_ARRIVE1[{0}]", tmmsmkr14["LADLE_ARRIVE"].ToString());
		if (tmmsmkr14["LADLE_ARRIVE"].ToString() != " "){
			cmd_inq.SetCommandText(" SELECT TO_CHAR(TO_DATE('" + tmmsmkr14["LADLE_ARRIVE"].ToString() + "', 'YYYY-MM-DD HH24:MI:SS'), 'yyyyMMddHH24miss') from dual ");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsmkr14["LADLE_ARRIVE"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}
		Log::Trace("", "LADLE_ARRIVE", "LADLE_ARRIVE[{0}]", tmmsmkr14["LADLE_ARRIVE"].ToString());
		//DES_SUMMARY.LADLE_LEAVE
		if (tmmsmkr14["LADLE_LEAVE"].ToString() != " "){
			cmd_inq.SetCommandText(" SELECT TO_CHAR(TO_DATE('" + tmmsmkr14["LADLE_LEAVE"].ToString() + "', 'YYYY-MM-DD HH24:MI:SS'), 'yyyyMMddHH24miss') from dual ");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsmkr14["LADLE_LEAVE"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}
		//DES_SUMMARY.DES_START
		if (tmmsmkr14["DES_START"].ToString() != " "){
			cmd_inq.SetCommandText(" SELECT TO_CHAR(TO_DATE('" + tmmsmkr14["DES_START"].ToString() + "', 'YYYY-MM-DD HH24:MI:SS'), 'yyyyMMddHH24miss') from dual ");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsmkr14["DES_START"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}
		//DES_SUMMARY.DES_END
		if (tmmsmkr14["DES_END"].ToString() != " "){
			cmd_inq.SetCommandText(" SELECT TO_CHAR(TO_DATE('" + tmmsmkr14["DES_END"].ToString() + "', 'YYYY-MM-DD HH24:MI:SS'), 'yyyyMMddHH24miss') from dual ");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsmkr14["DES_END"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}
		//DES_SUMMARY.RESIDUE_FIRST_S
		if (tmmsmkr14["RESIDUE_FIRST_S"].ToString() != " "){
			cmd_inq.SetCommandText(" SELECT TO_CHAR(TO_DATE('" + tmmsmkr14["RESIDUE_FIRST_S"].ToString() + "', 'YYYY-MM-DD HH24:MI:SS'), 'yyyyMMddHH24miss') from dual ");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsmkr14["RESIDUE_FIRST_S"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}
		//DES_SUMMARY.RESIDUE_FIRST_E
		if (tmmsmkr14["RESIDUE_FIRST_E"].ToString() != " "){
			cmd_inq.SetCommandText(" SELECT TO_CHAR(TO_DATE('" + tmmsmkr14["RESIDUE_FIRST_E"].ToString() + "', 'YYYY-MM-DD HH24:MI:SS'), 'yyyyMMddHH24miss') from dual ");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsmkr14["RESIDUE_FIRST_E"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}
		//DES_SUMMARY.RESIDUE_LAST_S
		if (tmmsmkr14["RESIDUE_LAST_S"].ToString() != " "){
			cmd_inq.SetCommandText(" SELECT TO_CHAR(TO_DATE('" + tmmsmkr14["RESIDUE_LAST_S"].ToString() + "', 'YYYY-MM-DD HH24:MI:SS'), 'yyyyMMddHH24miss') from dual ");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsmkr14["RESIDUE_LAST_S"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}
		//DES_SUMMARY.RESIDUE_LAST_E
		if (tmmsmkr14["RESIDUE_LAST_E"].ToString() != " "){
			cmd_inq.SetCommandText(" SELECT TO_CHAR(TO_DATE('" + tmmsmkr14["RESIDUE_LAST_E"].ToString() + "', 'YYYY-MM-DD HH24:MI:SS'), 'yyyyMMddHH24miss') from dual ");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsmkr14["RESIDUE_LAST_E"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}
		if (tmmsmkr14["LADLE_ARRIVE"].ToString().Trim() != "")
		{
			cal_starttime = tmmsmkr14["LADLE_ARRIVE"].ToString();
		}
		else
		{
			cal_starttime = datetime;
		}
		Log::Info("", __FUNCTION__, "cal_starttime =[{0}]", cal_starttime);
		f_epep_get_shift_group("SMDD", cal_starttime, strShiftNo, strShiftGroup, conn);//取得班次班组
		
		Log::Info("", __FUNCTION__, "strShiftGroup =[{0}]", strShiftGroup);
		Log::Info("", __FUNCTION__, "strShiftNo =[{0}]", strShiftNo);
		tmmsmkr14["SHIFT_CLASS"] = strShiftGroup;		//生产班组
		
		tmmsmkr14["PROC_NO"] = tmmsmkr14["DES_ID"].ToString();
		tmmsmkr14["L2_PROC_NO"] = tmmsmkr14["DES_ID"].ToString();
		tmmsmkr14["REC_CREATE_TIME"] = datetime;
		tmmsmkr14["REC_CREATOR"] = s.userid;
		tmmsmkr14["REMARK"] = "KR3";
		if (tmmsmkr14.Query("DES_ID")){
			tmmsmkr14.Delete();
		}
		tmmsmkr14.Insert();
		tmmsmkr14.MergeTo(in_23m.Tables[1]);
		da_kr_summary.MergeFrom(bcls_rec->Tables["DES_SUMMARY"].Rows[0]);
		//DES_SUMMARY.DES_ID
		if (da_kr_summary.Query("DES_ID")){
			da_kr_summary.Delete();
		}
		da_kr_summary.Insert();
		v_proc_div = "I";
		if (da_kr_summary.Query("DES_ID")){
			v_proc_div = "U";
			da_kr_summary.Delete();
		}
		da_kr_summary.Insert();
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
		//DES_SUMMARY.DES_ID
		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("DES_ID"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "DES_ID");
		}

		bcls_rec->Tables["MMSMSND"].Rows.Add();

		//
		bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM14B";//210015

		bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["MMSMSND"].Rows[0]["DES_ID"] = tmmsmkr14["DES_ID"];

		doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);

		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (in_23m.Tables[1].Rows.get_Count() > 0)
		{
			doFlag = f_t8z_23m_snd(&in_23m, bcls_ret, conn);
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


