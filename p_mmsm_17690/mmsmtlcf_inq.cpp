/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 原料模板画面维护
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmtlcf_inq)

int f_mmsmtlcf_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int blkNum = 0;
	int doFlag = 0;
	CString s_formname = "";
	CString v_proc_div = "";
	CString sqlstr = "";
	CString sqlstr_temp = "";
	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel tmmsm19("TMMSM19");
	CModel tmmsm20("TMMSM20");

	try
	{

		tmmsm19["PROC_NO"] = bcls_rec->Tables[0].Rows[0]["EAF_HEAT_NO"].ToString();
		tmmsm20["PROC_NO"] = bcls_rec->Tables[0].Rows[0]["EAF_HEAT_NO"].ToString();
		sqlstr = " SELECT ST_SAMPLE_NO,\
			ELM_001 C_VALUE,\
			ELM_002 SI_VALUE,\
			ELM_003 MN_VALUE,\
			ELM_004 P_VALUE,\
			ELM_005 S_VALUE,\
			ELM_006 CR_VALUE,\
			ELM_007 NI_VALUE,\
			ELM_008 MO_VALUE,\
			ELM_009 CU_VALUE,\
			ELM_021 AS_VALUE,\
			ELM_022 PB_VALUE,\
			ELM_023 SN_VALUE,\
			ELM_024 SB_VALUE,\
			ELM_025 BI_VALUE\
			 FROM TQMTS24\
			WHERE 1 = 1\
			AND HEAT_NO = '" + tmmsm19["PROC_NO"].ToString() + "'\
			ORDER BY REC_CREATE_TIME DESC ";
		Log::Trace("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		bcls_ret->Tables.Add();
		sqlstr = "SELECT LOT_NO, QUALITY_BATCH_NO, SUM(DEVO_WT)/1000 MAT_AMOUNT1\
			FROM TMMSM2A_YL\
			WHERE MAT_CODE = '" + bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString() + "'\
			AND PROC_NO = '" + tmmsm19["PROC_NO"].ToString() + "'\
			GROUP BY LOT_NO, QUALITY_BATCH_NO ";
		Log::Trace("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();

		bcls_ret->Tables.Add();
		if (bcls_rec->Tables[0].Rows[0]["EAF_HEAT_NO"].ToString().SubstringNE(0, 1) == "E")
		{
			sqlstr = " select round(nvl(ACTRESULT * 100 / DEVO_WT,0), 1) METAL_YIELD_RATE\
				from tmmsm20 A\
				left join(SELECT PROC_NO, SUM(DEVO_WT) / 1000 DEVO_WT\
					FROM TMMSM2A_YL A\
					left join tmmsm50 b on a.MAT_CODE = b.MAT_CODE\
					WHERE PROC_NO = '"+ tmmsm20["PROC_NO"].ToString()+"'\
					and b.mat_type = '2'\
					group by PROC_NO) B on a.PROC_NO = b.PROC_NO\
			where a.PROC_NO = '" + tmmsm20["PROC_NO"].ToString() + "' ";
		}
		if (bcls_rec->Tables[0].Rows[0]["EAF_HEAT_NO"].ToString().SubstringNE(0, 1) == "F")
		{
			sqlstr =  " select round(nvl(ACTRESULT * 100 / DEVO_WT,0), 1) METAL_YIELD_RATE\
				from tmmsm19 A\
				left join(SELECT PROC_NO, SUM(DEVO_WT) / 1000 DEVO_WT\
					FROM TMMSM2A_YL A\
					left join tmmsm50 b on a.MAT_CODE = b.MAT_CODE\
					WHERE PROC_NO = '" + tmmsm19["PROC_NO"].ToString() + "'\
					and b.mat_type = '2'\
					group by PROC_NO) B on a.PROC_NO = b.PROC_NO\
			where a.PROC_NO = '" + tmmsm19["PROC_NO"].ToString() + "' ";
		}
		if (sqlstr.Trim() != "")
		{
			Log::Trace("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
			cmd_inq.Close();
		}
		else
		{
			bcls_ret->Tables[2].Columns.Add(DT_DECIMAL, "METAL_YIELD_RATE");
			bcls_ret->Tables[2].Rows.Add();
			bcls_ret->Tables[2].Rows[0]["METAL_YIELD_RATE"] = 0;
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
