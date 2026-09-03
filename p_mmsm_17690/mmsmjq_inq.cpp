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
BM2F_ENTERACE(mmsmjq_inq)

int f_mmsmjq_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int blkNum = 0;
	int doFlag = 0;
	CString s_formname = "";
	CString v_proc_div = "";
	CString sqlstr = "";
	CString sqlstr_temp = "  ";
	CString sqlstr_where = "  ";
	// 解析后的条件
	CString p_condition = "0=1";
	CString cu_condition = "0=1";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel tmmsm19("TMMSM19");
	CModel tmmsm20("TMMSM20");

	try
	{
		CString v_res_type = "2";
		CString s_res_type = "3";
		if (bcls_rec->Tables[1].Rows[0]["FN_NO"].ToString() == "F5")
		{
			v_res_type = "4";
			s_res_type = "5";
			sqlstr_where = " and com_flag='1' ";
		}
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			sqlstr_temp += "'" + bcls_rec->Tables[0].Rows[i]["LOT_NO"].ToString() + "',";
		}
		if (sqlstr_temp.Trim().GetLength() > 0)
		{
			sqlstr_temp = sqlstr_temp.SubstringNE(0, sqlstr_temp.GetLength() - 1);
		}
		// ===================== 读取磷配置：支持 > >= < <= = =====================
		sqlstr = "SELECT CODE_DESC_1_CONTENT FROM TWMSMZD02 WHERE REC_CREATE_TIME = (SELECT MAX(REC_CREATE_TIME) FROM TWMSMZD02 t WHERE CODE_CLASS ='MMSMRQJQ' AND code='01') AND code='01'";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			CString cfg = cmd_inq.GetString(1);
			Log::Trace("", __FUNCTION__, "[{0}]", cfg);
			cfg.TrimLeft();
			cfg.TrimRight();
			Log::Trace("", __FUNCTION__, "[{0}]", cfg);
			int nSplit = 1;
			if (cfg.GetLength() >= 2)
			{
				CString c2 = cfg.SubstringNE(1, 1);
				if (c2 == "=") // 两个字符操作符：>= <= ==
				{
					nSplit = 2;
				}
			}

			if (cfg.GetLength() > nSplit)
			{
				CString op = cfg.SubstringNE(0, nSplit);          // 取操作符：> >= < <=
				CString val = cfg.SubstringNE(nSplit, cfg.GetLength() - nSplit); // 取数值
				p_condition = "max(TO_NUMBER(P_VALUE)) " + op + " " + val;
			}
		}
		cmd_inq.Close();
		// ===================== 读取铜配置：支持 > >= < <= = =====================
		sqlstr = "SELECT CODE_DESC_1_CONTENT FROM TWMSMZD02 WHERE REC_CREATE_TIME = (SELECT MAX(REC_CREATE_TIME) FROM TWMSMZD02 t WHERE CODE_CLASS ='MMSMRQJQ' AND code='02') AND code='02'";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			CString cfg = cmd_inq.GetString(1);
			cfg.TrimLeft();
			cfg.TrimRight();

			int nSplit = 1;
			if (cfg.GetLength() >= 2)
			{
				CString c2 = cfg.SubstringNE(1, 1);
				if (c2 == "=")
				{
					nSplit = 2;
				}
			}

			if (cfg.GetLength() > nSplit)
			{
				CString op = cfg.SubstringNE(0, nSplit);
				CString val = cfg.SubstringNE(nSplit, cfg.GetLength() - nSplit);
				cu_condition = "max(TO_NUMBER(CU_VALUE)) " + op + " " + val;
			}
		}
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "[{0}]", p_condition);
		Log::Trace("", __FUNCTION__, "[{0}]", cu_condition);
		sqlstr = " SELECT * FROM TMMSMWQ where LOT_NO in (" + sqlstr_temp + ") AND RES_TYPE='" + v_res_type + "' AND C_STATE='1'  order by LOT_NO, REC_CREATE_TIME ";
		Log::Trace("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		 
		bcls_ret->Tables.Add();
		//round(sum(MAT_AMOUNT1 * ACTRESULT) / sum(ACTRESULT) , 4)
	    sqlstr = " SELECT  max(PRODUCE_DATE) PRODUCE_DATE,                                                        \
			max(LOT_NO)                                                                                            LOT_NO,\
			max(QUALITY_BATCH_NO)                                                                                  QUALITY_BATCH_NO,\
			max(MAT_CODE)                                                                                          MAT_CODE,\
			max(MAT_NAME)                                                                                          MAT_NAME,\
			sum(ACTRESULT)																						   ACTRESULT,\
			round(sum(METAL_YIELD_RATE * ACTRESULT) / sum(ACTRESULT), 1)										   METAL_YIELD_RATE,\
			sum(MAT_AMOUNT1)																					   MAT_AMOUNT1,\
			CASE WHEN MAX(C_VALUE) = -1 AND MIN(C_VALUE) = -1 THEN -1 ELSE round(sum(C_VALUE * decode(C_VALUE, -1, null, ACTRESULT)) / sum(decode(C_VALUE, -1, null, ACTRESULT)), 4)  END C_VALUE,\
			CASE WHEN MAX(SI_VALUE) = -1 AND MIN(SI_VALUE) = -1 THEN -1 ELSE round(sum(SI_VALUE * decode(SI_VALUE, -1, null, ACTRESULT)) / sum(decode(SI_VALUE, -1, null, ACTRESULT)), 4) END  SI_VALUE,\
			CASE WHEN MAX(MN_VALUE) = -1 AND MIN(MN_VALUE) = -1 THEN -1 ELSE round(sum(MN_VALUE * decode(MN_VALUE, -1, null, ACTRESULT)) / sum(decode(MN_VALUE, -1, null, ACTRESULT)), 4) END  MN_VALUE,\
			CASE WHEN MAX(P_VALUE) = -1 AND MIN(P_VALUE) = -1 THEN -1 ELSE CASE WHEN " + p_condition + " THEN -1 ELSE ROUND(SUM(P_VALUE * DECODE(P_VALUE, -1, NULL, ACTRESULT)) / SUM(DECODE(P_VALUE, -1, NULL, ACTRESULT)), 4) END END  P_VALUE,\
			CASE WHEN MAX(S_VALUE) = -1 AND MIN(S_VALUE) = -1 THEN -1 ELSE round(sum(S_VALUE * decode(S_VALUE, -1, null, ACTRESULT)) / sum(decode(S_VALUE, -1, null, ACTRESULT)), 4) END   S_VALUE,\
			CASE WHEN MAX(CR_VALUE) = -1 AND MIN(CR_VALUE) = -1 THEN -1 ELSE round(sum(CR_VALUE * decode(CR_VALUE, -1, null, ACTRESULT)) / sum(decode(CR_VALUE, -1, null, ACTRESULT)), 4) END  CR_VALUE,\
			CASE WHEN MAX(NI_VALUE) = -1 AND MIN(NI_VALUE) = -1 THEN -1 ELSE round(sum(NI_VALUE * decode(NI_VALUE, -1, null, ACTRESULT)) / sum(decode(NI_VALUE, -1, null, ACTRESULT)), 4) END  NI_VALUE,\
			CASE WHEN MAX(MO_VALUE) = -1 AND MIN(MO_VALUE) = -1 THEN -1 ELSE round(sum(MO_VALUE * decode(MO_VALUE, -1, null, ACTRESULT)) / sum(decode(MO_VALUE, -1, null, ACTRESULT)), 4) END  MO_VALUE,\
			CASE WHEN MAX(CU_VALUE) = -1 AND MIN(CU_VALUE) = -1 THEN -1 ELSE CASE WHEN " + cu_condition + " THEN -1 ELSE ROUND(SUM(CU_VALUE * DECODE(CU_VALUE, -1, NULL, ACTRESULT)) / SUM(DECODE(CU_VALUE, -1, NULL, ACTRESULT)), 4) END END  CU_VALUE,\
			CASE WHEN MAX(PB_VALUE) = -1 AND MIN(PB_VALUE) = -1 THEN -1 ELSE round(sum(PB_VALUE * decode(PB_VALUE, -1, null, ACTRESULT)) / sum(decode(PB_VALUE, -1, null, ACTRESULT)), 4) END  PB_VALUE,\
			CASE WHEN MAX(SN_VALUE) = -1 AND MIN(SN_VALUE) = -1 THEN -1 ELSE round(sum(SN_VALUE * decode(SN_VALUE, -1, null, ACTRESULT)) / sum(decode(SN_VALUE, -1, null, ACTRESULT)), 4) END  SN_VALUE,\
			CASE WHEN MAX(SB_VALUE) = -1 AND MIN(SB_VALUE) = -1 THEN -1 ELSE round(sum(SB_VALUE * decode(SB_VALUE, -1, null, ACTRESULT)) / sum(decode(SB_VALUE, -1, null, ACTRESULT)), 4) END SB_VALUE,\
			CASE WHEN MAX(AS_VALUE) = -1 AND MIN(AS_VALUE) = -1 THEN -1 ELSE round(sum(AS_VALUE * decode(AS_VALUE, -1, null, ACTRESULT)) / sum(decode(AS_VALUE, -1, null, ACTRESULT)), 4) END AS_VALUE,\
			CASE WHEN MAX(BI_VALUE) = -1 AND MIN(BI_VALUE) = -1 THEN -1 ELSE round(sum(BI_VALUE * decode(BI_VALUE, -1, null, ACTRESULT)) / sum(decode(BI_VALUE, -1, null, ACTRESULT)), 4) END BI_VALUE,\
			'"+s_res_type+"' RES_TYPE\
			FROM TMMSMWQ\
			WHERE LOT_NO in (" + sqlstr_temp + ") " + sqlstr_where + "\
			AND RES_TYPE = '" + v_res_type + "' AND C_STATE='1' group by LOT_NO ";
		Log::Trace("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
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
