/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    
Version:    1.0
Date:     2024
Description: 按日期铬镍收得率明细表
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmzxh01_inq)
int f_mmsm_t80r91_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送炉成本
int f_mmsmzxh01_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString end_time = " ";
	CString begin_time = " ";
	CDecimal SUM_CR = 0;
	CDecimal SUM_NI = 0;
	CString heat_no1 = " ";
	CString heat_no2 = " ";
	CString heat_no3 = " ";
	CString heat_no4 = " ";
	CString heat_no5 = " ";
	CString heat_no6 = " ";
	CString heat_no7 = " ";
	CString heat_no8 = " ";
	CString heat_no9 = " ";
	CString heat_no10 = " ";
	CString heat_no11 = " ";
	CString heat_no12 = " ";
	CString heat_no13 = " ";
	CString heat_no14 = " ";
	CString heat_no15 = " ";
	CString heat_no16 = " ";
	CDbCommand cmd_inq(conn);

	try
	{
		
		
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0,8);
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString().SubstringNE(0, 8);

		/*sqlstr = " SELECT HEAT_NO1,HEAT_NO2,HEAT_NO3,HEAT_NO4,HEAT_NO5,HEAT_NO6,HEAT_NO7,HEAT_NO8,HEAT_NO9,HEAT_NO10,HEAT_NO11,HEAT_NO12,HEAT_NO13,HEAT_NO14,HEAT_NO15,HEAT_NO16  FROM TMMSMZXHBB_LH  WHERE 1=1 ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			heat_no1 = cmd_inq.GetString(1);
			heat_no2 = cmd_inq.GetString(2);
			heat_no3 = cmd_inq.GetString(3);
			heat_no4 = cmd_inq.GetString(4);
			heat_no5 = cmd_inq.GetString(5);
			heat_no6 = cmd_inq.GetString(6);
			heat_no7 = cmd_inq.GetString(7);
			heat_no8 = cmd_inq.GetString(8);
			heat_no9 = cmd_inq.GetString(9);
			heat_no10 = cmd_inq.GetString(10);
			heat_no11 = cmd_inq.GetString(11);
			heat_no12 = cmd_inq.GetString(12);
			heat_no13 = cmd_inq.GetString(13);
			heat_no14 = cmd_inq.GetString(14);
			heat_no15 = cmd_inq.GetString(15);
			heat_no16 = cmd_inq.GetString(16);

		}*/
		
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = " select AOD_BOF_E_DTIME,CAST_SEQ,st_no, count(1) HEAT_NUM, sum(MAT_ACT_WT) MAT_ACT_WT "
				", sum(AT_CR) AT_CR, sum(AT_NI) AT_NI, sum(AT_MO) AT_MO"
				", sum(DAVO_NI) DAVO_NI, sum(DAVO_CR) DAVO_CR, sum(DAVO_MO) DAVO_MO	"
				", decode(sum(DAVO_CR), 0, 0, round(sum(AT_CR) * 100 / sum(DAVO_CR), 3)) CR	"
				", decode(sum(DAVO_NI), 0, 0, round(sum(AT_NI) * 100 / sum(DAVO_NI), 3)) NI	"
				", decode(sum(DAVO_MO), 0, 0, round(sum(AT_MO) * 100 / sum(DAVO_MO), 3)) MO	"
				" from(	"
				" SELECT AOD_BOF_E_DTIME,t1.HEATNR as heat_no, t1.GRADE_ID as st_no,t1.CAST_SEQ,t1.CAST_DIV_NO_1, t1.MAT_ACT_WT "
				", t1.MAT_ACT_WT*NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END, 0) AT_CR "
				", t1.MAT_ACT_WT*NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END, 0) AT_NI "
				", t1.MAT_ACT_WT*NVL(CASE WHEN ELM_008 = -1 THEN 0 ELSE ELM_007 END, 0) AT_MO  "
				", DAVO_NI, DAVO_CR, DAVO_MO  "
				" FROM TQMTSCB02_MX t1 "
				" left join TQMTSB0 t2 on t1.HEATNR = t2.heat_no	 "
				" left join(	 "
				" select HEATNR, SUM(WEIGHT*INCLUDE_NI) as DAVO_NI, SUM(WEIGHT*INCLUDE_CR) as DAVO_CR, SUM(WEIGHT*INCLUDE_MO) as DAVO_MO	 "
				" from TQMTSCB01_MX"
				" WHERE 1 = 1 "
				" and AOD_BOF_E_DTIME <= @end_time "
				" and AOD_BOF_E_DTIME >= @begin_time "
				" group by HEATNR"
				" ) t3 on t1.HEATNR = t3.HEATNR	"
				" WHERE 1 = 1 "
				" and AOD_BOF_E_DTIME <= @end_time "
				" and AOD_BOF_E_DTIME >= @begin_time "
				" )"
				" group by AOD_BOF_E_DTIME,CAST_SEQ,st_no	"
				" ORDER BY AOD_BOF_E_DTIME,CAST_SEQ,st_no	"
				;
			cmd_inq.Parameters.Set("end_time", end_time+"235959");
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close(); 

		
			EIClass bcls_ret3;
			EIClass bcls_rec3;
			bcls_rec3.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			bcls_rec3.Tables[0].Rows.Add();
			bcls_rec3.Tables[0].Rows[0]["HEAT_NO"]="A1403917"; 			
			//doFlag = f_mmsm_t80r91_snd(&bcls_rec3, &bcls_ret3, conn);
			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t80r91_snd失败-------");
				//throw CApplicationException(-1, s.msg, log.Location);
			}
			
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
