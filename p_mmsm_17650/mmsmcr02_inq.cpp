/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 铬镍收得率明细表
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmcr02_inq)

int f_mmsmcr02_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString st_no = " ";
	CString end_time_1 = " ";
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
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().TrimOrBlank().ToUpper();

		sqlstr = " SELECT BOF0_S,BOF0_E,BOF1_S,BOF1_E,BOF2_S,BOF2_E,BOF9_S,BOF9_E,AOD0_S,AOD0_E,AOD1_S,AOD1_E,AOD2_S,AOD2_E,AOD6_S,AOD6_E   FROM TMMSMZXHBB_LH  WHERE 1 = 1 ";
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

		}
		else
		{
			strcpy(s.msg, "未查询到炉号数据请先执行F3按钮生成数据");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		cmd_inq.Close();

		Log::Info("", __FUNCTION__, "st_no   =[{0}]", st_no);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no1, heat_no2);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no3, heat_no4);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no5, heat_no6);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no7, heat_no8);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no9, heat_no10);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no11, heat_no12);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no13, heat_no14);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no15, heat_no16);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
/*
			sqlstr = "SELECT MAX(DATE_1) JS_DATE,MAX(DAMIN) ADMIN,SUM(MAT_WT) SCL,SUM(HGCL) HGCL,SUM(AT_CR) CP_CR,SUM(AT_NI) CP_NI, "
			"	SUM(DAVO_CR) TR_CR,SUM(DAVO_NI) TR_NI, "
			"	ST_NO,MAX(ST_NO_MS),MAX(ST_NO_LB),MAX(ST_NO_DL), "
			"	ROUND(NVL(SUM(HY_CR), 0), 3) HY_CR , ROUND(NVL(SUM(HY_NI), 0), 3) HY_NI , "
			"	ROUND(nvl(CASE WHEN SUM(DAVO_CR) = 0 OR SUM(AT_CR) = 0 THEN 0 ELSE  (SUM(AT_CR))/(SUM(DAVO_CR)) END, 0), 5) * 100 CP_CR_SDL, "
			"	ROUND(nvl(CASE WHEN SUM(DAVO_NI) = 0 OR SUM(AT_NI) = 0 THEN 0 ELSE  (SUM(AT_NI))/(SUM(DAVO_NI)) END, 0), 5) * 100 CP_NI_SDL, "
			"	ROUND(nvl(CASE WHEN SUM(DAVO_CR) = 0 OR SUM(HY_CR) = 0 THEN 0 ELSE  (SUM(HY_CR))/(SUM(DAVO_CR)) END, 0), 5) * 100 HR_CR_SDL, "
			"	ROUND(nvl(CASE WHEN SUM(DAVO_NI) = 0 OR SUM(HY_NI) = 0 THEN 0 ELSE  (SUM(HY_NI))/(SUM(DAVO_NI)) END, 0), 5) * 100 HY_NI_SDL "
			"	FROM( "
			"	SELECT DATE_1,DAMIN,MAT_WT,HGCL,AT_CR,AT_NI,DAVO_CR,DAVO_NI,CR,NI,ST_NO,ST_NO_MS,ST_NO_LB,ST_NO_DL, "
			"	ROUND(NVL(HY_CR*HGCL, 0), 3) HY_CR, ROUND(NVL(HY_NI*HGCL, 0), 3) HY_NI  "
			"	FROM(  "
			"	SELECT ZXH.TIME_1 DATE_1,ZXH.USER_NAME DAMIN,ZXH.TAP_END_TIME,ZXH.HEAT_NO,ZXH.PROC_NO,ZXH.PROD_OUT_WT MAT_WT, "
			"	ZXH.ORIGIN_SYS_CODE DEV_CODE,NVL(MAT_ACT_WT, 0) HGCL, ZXH.ST_NO, ZXH.ST_NO_DESC ST_NO_MS, ZXH.ST_NO_SMALL_CLASS ST_NO_LB, ZXH.ST_NO_BIG_CLASS ST_NO_DL, "
			"	 ROUND(NVL(CR*MAT_ACT_WT, 0), 3) AT_CR, ROUND(NVL(Ni*MAT_ACT_WT, 0), 3) AT_NI, ROUND(ZXH.NI_VALUE, 3) DAVO_NI, "
			"	ROUND(ZXH.CR_VALUE, 3) DAVO_CR,HY_CR,HY_NI,"
			"	CASE WHEN ZXH.ST_NO = (SELECT ST_NO FROM TMMSMW8 WHERE ST_NO =ZXH.ST_NO  ) THEN 0 ELSE  "
			"	ROUND(nvl(CASE WHEN MAT_ACT_WT*Ni = 0 OR ZXH.NI_VALUE = 0 THEN 0 ELSE(ZXH.NI_VALUE) / (MAT_ACT_WT*Ni)  END, 0), 5) * 100 END  NI, "
			"	ROUND(nvl(CASE WHEN MAT_ACT_WT*CR = 0 OR ZXH.CR_VALUE = 0 THEN 0 ELSE(ZXH.CR_VALUE) / (MAT_ACT_WT*CR) END, 0), 5) * 100 CR "
			"	 FROM  (SELECT  ZXH.HEAT_NO, ZXH.ST_NO, MAX(TIME_STAMPS) TIME_1, MAX(USER_NAME) USER_NAME, MAX(TAP_END_TIME) TAP_END_TIME,  "
			"	MAX(PROC_NO)PROC_NO, MAX(PROD_OUT_WT) PROD_OUT_WT,MAX(ORIGIN_SYS_CODE) ORIGIN_SYS_CODE, MAX(ST_NO_DESC) ST_NO_DESC, "
			"	 MAX(ST_NO_SMALL_CLASS1)ST_NO_SMALL_CLASS, MAX(ST_NO_BIG_CLASS) ST_NO_BIG_CLASS, SUM(NI_VALUE*DEVO_WT) NI_VALUE, "
			"	SUM(CR_VALUE*DEVO_WT) CR_VALUE, SUM(DEVO_WT) DEVO_WT, NVL(MAX(B0.Ni), 0) Ni, NVL(MAX(B0.CR), 0) CR, "
			"	ROUND(NVL(MAX(TS24.TS_CR), 0), 3) HY_CR,ROUND(NVL(MAX(TS24.TS_NI), 0), 3) HY_NI,MAX(MAT_ACT_WT) MAT_ACT_WT FROM "
			"	(SELECT * FROM TMMSMZXHBB WHERE 1=1 and "
			"	((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
			"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
			"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
			"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16)) AND SUBSTR(ST_NO,1,1) IN('1','4') ) ZXH "
			"	LEFT JOIN (SELECT * FROM VMMSMCPCL_BB1 WHERE 1=1 and "
			"	((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
			"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
			"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
			"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16))) BB1 ON ZXH.HEAT_NO = BB1.HEAT_NO AND ZXH.ST_NO = BB1.ST_NO "
			"	 LEFT JOIN (SELECT NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END, 0) TS_CR, NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END, 0) TS_NI, HEAT_NO, ST_NO"
			"	FROM TQMTS24_INIT WHERE ST_SAMPLE_NO IN(SELECT ST_SAMPLE_NO FROM TQMTS24 WHERE REP_ELM_SEL_FLAG = '1') and "
			"	((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
			"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
			"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
			"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16))) TS24 ON ZXH.HEAT_NO = TS24.HEAT_NO  "
			"	LEFT JOIN  "
			"	(SELECT NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END,0) Cr, NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END,0) Ni, HEAT_NO, ST_NO FROM TQMTSB0 WHERE 1=1 and  "
			"	((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
			"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
			"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
			"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16))) B0 	ON ZXH.HEAT_NO = B0.HEAT_NO  WHERE 1 = 1 and "
			"	((ZXH.HEAT_NO >= @heat_no1  AND ZXH.HEAT_NO <= @heat_no2) OR(ZXH.HEAT_NO >= @heat_no3 AND ZXH.HEAT_NO <= @heat_no4) OR(ZXH.HEAT_NO >= @heat_no5  AND "
			"	ZXH.HEAT_NO <= @heat_no6) OR(ZXH.HEAT_NO >= @heat_no7 AND ZXH.HEAT_NO <= @heat_no8) OR(ZXH.HEAT_NO >= @heat_no9 "
			"	AND ZXH.HEAT_NO <= @heat_no10) OR(ZXH.HEAT_NO >= @heat_no11 AND ZXH.HEAT_NO <= @heat_no12) OR(ZXH.HEAT_NO >= @heat_no13 AND ZXH.HEAT_NO <= @heat_no14) "
			"	OR(ZXH.HEAT_NO >= @heat_no15 AND ZXH.HEAT_NO <= @heat_no16))  "
			"	GROUP BY ZXH.HEAT_NO,ZXH.ST_NO  order by ZXH.HEAT_NO ) ZXH )) ";
		
		
			sql_group = " GROUP BY ST_NO ORDER BY ST_NO  ";
			sqlstr = sqlstr  + sql_group;
*/

			sqlstr =
				//按钢种进行汇总
				" select st_no,sum(mat_act_wt) HGCL,round(sum(NI_VALUE),3) TR_NI,round(sum(CR_VALUE),3) TR_CR,round(sum(MO_VALUE),3) TR_MO,sum(DEVO_WT) DEVO_WT"
				" ,round(sum(mat_act_wt*HY_CR),3) HY_CR,round(sum(mat_act_wt*HY_NI),3) HY_NI,round(sum(mat_act_wt*HY_MO),3) HY_MO,round(sum(mat_act_wt*Cr),3) CP_Cr,round(sum(mat_act_wt*Ni),3) CP_Ni,round(sum(mat_act_wt*MO),3) CP_MO"
				" ,case when st_no in (select ST_NO FROM TMMSMW8)  then 0 when round(sum(mat_act_wt*HY_NI),3) = 0 then 0 else round(round(sum(NI_VALUE),3)*100/round(sum(mat_act_wt*HY_NI),3),3) end HY_NI_SDL"
				" ,case when st_no in (select ST_NO FROM TMMSMW8)  then 0 when round(sum(mat_act_wt*HY_CR),3) = 0 then 0 else round(round(sum(cr_VALUE),3)*100/round(sum(mat_act_wt*HY_CR),3),3) end HY_CR_SDL"
				" ,case when st_no in (select ST_NO FROM TMMSMW8)  then 0 when round(sum(mat_act_wt*HY_mo),3) = 0 then 0 else round(round(sum(mo_VALUE),3)*100/round(sum(mat_act_wt*HY_mo),3),3) end HY_mo_SDL"
				" ,case when st_no in (select ST_NO FROM TMMSMW8)  then 0 when round(sum(mat_act_wt*NI),3) = 0 then 0 else round(round(sum(NI_VALUE),3)*100/round(sum(mat_act_wt*NI),3),3) end CP_NI_SDL"
				" ,case when st_no in (select ST_NO FROM TMMSMW8)  then 0 when round(sum(mat_act_wt*CR),3) = 0 then 0 else round(round(sum(cr_VALUE),3)*100/round(sum(mat_act_wt*CR),3),3) end CP_CR_SDL"
				" ,case when st_no in (select ST_NO FROM TMMSMW8)  then 0 when round(sum(mat_act_wt*mo),3) = 0 then 0 else round(round(sum(mo_VALUE),3)*100/round(sum(mat_act_wt*mo),3),3) end CP_mo_SDL"
				" from ("
				//取消耗的投入量				
				" (select BB.heat_no,BB.st_no,PROD_OUT_WT,NVL(mat_act_wt,0) mat_act_wt,NI_VALUE,CR_VALUE,MO_VALUE,DEVO_WT,NVL(HY_CR,0) HY_CR,NVL(HY_NI,0) HY_NI,NVL(HY_MO,0) HY_MO,nvl(Cr,0) Cr,nvl(Ni,0) Ni,nvl(MO,0) MO"
				" FROM "
				"	(SELECT heat_no,st_no,MAX(PROD_OUT_WT) PROD_OUT_WT,SUM(NI_VALUE*DEVO_WT) NI_VALUE,SUM(CR_VALUE*DEVO_WT) CR_VALUE,SUM(MO_VALUE*DEVO_WT) MO_VALUE, SUM(DEVO_WT) DEVO_WT "
				" FROM TMMSMZXHBB WHERE 1=1 and "
				"	((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
				"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
				"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
				"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16)) AND SUBSTR(ST_NO,1,1) IN('1','4') "
				"   group by heat_no,st_no"
				"	)  BB "
				//产量
				" left join ("
				" SELECT heat_no,st_no,sum(mat_act_wt) mat_act_wt FROM VMMSMCPCL_BB1 WHERE 1 = 1 and "
				"	((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
				"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
				"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
				"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16))"
				" group by heat_no,st_no"
				" ) BB1 on BB.HEAT_NO=BB1.HEAT_NO and BB.ST_NO=BB1.ST_NO"
				//成分
				"	 LEFT JOIN ("
				"   SELECT HEAT_NO, ST_NO,NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END, 0) HY_CR, NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END, 0) HY_NI, NVL(CASE WHEN ELM_008 = -1 THEN 0 ELSE ELM_008 END, 0) HY_MO"
				"	FROM TQMTS24_INIT"
				"   WHERE ST_SAMPLE_NO IN(SELECT ST_SAMPLE_NO FROM TQMTS24 WHERE REP_ELM_SEL_FLAG = '1') and "
				"	((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
				"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
				"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
				"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16))"
				" ) TS24 ON BB.HEAT_NO = TS24.HEAT_NO  "
				//成分
				"	LEFT JOIN  ("
				"	SELECT  HEAT_NO, ST_NO,NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END,0) Cr, NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END,0) Ni, NVL(CASE WHEN ELM_008 = -1 THEN 0 ELSE ELM_008 END,0) MO "
				"   FROM TQMTSB0 WHERE 1=1 and  "
				"	((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
				"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
				"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
				"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16))"
				"  ) B0 	ON BB.HEAT_NO = B0.HEAT_NO  "
				" )"
				" ) group by st_no"
				;

			cmd_inq.Parameters.Set("st_no", st_no);
			cmd_inq.Parameters.Set("heat_no1", heat_no1);
			cmd_inq.Parameters.Set("heat_no2", heat_no2);
			cmd_inq.Parameters.Set("heat_no3", heat_no3);
			cmd_inq.Parameters.Set("heat_no4", heat_no4);
			cmd_inq.Parameters.Set("heat_no5", heat_no5);
			cmd_inq.Parameters.Set("heat_no6", heat_no6);
			cmd_inq.Parameters.Set("heat_no7", heat_no7);
			cmd_inq.Parameters.Set("heat_no8", heat_no8);
			cmd_inq.Parameters.Set("heat_no9", heat_no9);
			cmd_inq.Parameters.Set("heat_no10", heat_no10);
			cmd_inq.Parameters.Set("heat_no11", heat_no11);
			cmd_inq.Parameters.Set("heat_no12", heat_no12);
			cmd_inq.Parameters.Set("heat_no13", heat_no13);
			cmd_inq.Parameters.Set("heat_no14", heat_no14);
			cmd_inq.Parameters.Set("heat_no15", heat_no15);
			cmd_inq.Parameters.Set("heat_no16", heat_no16);
			cmd_inq.SetCommandText(sqlstr);
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
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
