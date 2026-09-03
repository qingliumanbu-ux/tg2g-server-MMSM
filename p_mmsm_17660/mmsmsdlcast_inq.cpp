/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2026-01-11
Description: 炉成本查询（SQL内嵌版，支持完整日期范围筛选）
**************************************************/
//框架头文件
#include "stdafx.h"	 

BM2F_ENTERACE(mmsmsdlcast_inq)

int f_mmsmsdlcast_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;

	CString cast_seq = "";
	CString grade_id = "";
	CString grade_type1 = "";
	CString f_route1 = "";
	CString c_flag = "0";
	CString ss_flag = "0";
	CString v_from = "";//开始时刻（格式：YYYYMMDD）
	CString v_to = "";//结束时刻（格式：YYYYMMDD）

	int pageNum = 1;
	int pageSize = 100;
	if (bcls_rec->Tables[0].Columns.Contains("PAGE_NUM"))
	{
		pageNum = atoi(bcls_rec->Tables[0].Rows[0]["PAGE_NUM"].ToString().Trim());
	}
	if (bcls_rec->Tables[0].Columns.Contains("PAGE_SIZE"))
	{
		pageSize = atoi(bcls_rec->Tables[0].Rows[0]["PAGE_SIZE"].ToString().Trim());
	}
	int startRow = (pageNum - 1) * pageSize + 1;
	int endRow = pageNum * pageSize;

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		// 1. 读取入参
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
		{
			v_from = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().SubstringNE(0, 8).Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
		{
			v_to = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8).Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("CAST_SEQ"))
		{
			cast_seq = bcls_rec->Tables[0].Rows[0]["CAST_SEQ"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("GRADE_ID"))
		{
			grade_id = bcls_rec->Tables[0].Rows[0]["GRADE_ID"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("GRADE_TYPE1"))
		{
			grade_type1 = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("F_ROUTE1"))
		{
			f_route1 = bcls_rec->Tables[0].Rows[0]["F_ROUTE1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("C_FLAG"))
		{
			c_flag = bcls_rec->Tables[0].Rows[0]["C_FLAG"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("SS_FLAG"))
		{
			ss_flag = bcls_rec->Tables[0].Rows[0]["SS_FLAG"].ToString().Trim();
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:
		case DB_KIND_DB2_ORACLE:
		case DB_KIND_MSSQL:
		case DB_KIND_ORACLE:
		default:
			sqlstr += R"(
SELECT
   MAX(AOD_BOF_E_DTIME) AS AOD_BOF_E_DTIME,
    CAST_SEQ, 
    COUNT(DISTINCT HEATNR) AS HEAT_COUNT,
    SUM(STEEL_WT) AS STEEL_WT,
    SUM(LTS_WEIGHT) AS LTS_WEIGHT,
    SUM(LF_WEIGHT) AS LF_WEIGHT,
    SUM(RECEIVE_WEIGHT) AS RECEIVE_WEIGHT,
    SUM(MAT_ACT_WT) AS MAT_ACT_WT,
    SUM(CUT_SCRAP_WT) AS CUT_SCRAP_WT,
    ROUND(SUM(IN_CR), 3) AS IN_CR,
    ROUND(SUM(IN_NI), 3) AS IN_NI,
    ROUND(SUM(IN_MO), 3) AS IN_MO,
    ROUND(SUM(AT_CR), 3) AS AT_CR,
    ROUND(SUM(AT_NI), 3) AS AT_NI,
    ROUND(SUM(AT_MO), 3) AS AT_MO,
    ROUND(AVG(CR_CP), 3) AS CR_CP,
    ROUND(AVG(NI_CP), 3) AS NI_CP,
    ROUND(AVG(MO_CP), 3) AS MO_CP,
    DECODE(SUM(IN_CR), 0, 0, ROUND(SUM(AT_CR)*100/SUM(IN_CR), 2)) AS CR_RATIO,
    DECODE(SUM(IN_NI), 0, 0, ROUND(SUM(AT_NI)*100/SUM(IN_NI), 2)) AS NI_RATIO,
    DECODE(SUM(IN_MO), 0, 0, ROUND(SUM(AT_MO)*100/SUM(IN_MO), 2)) AS MO_RATIO,
    DECODE(SUM(RECEIVE_WEIGHT + CUT_SCRAP_WT), 0, 0, ROUND(SUM(METAL_CONS * 1000) / SUM(RECEIVE_WEIGHT + CUT_SCRAP_WT), 0)) AS ALLAY_CON
 FROM
(
    SELECT
        t1.aod_bof_e_dtime,
        t1.HEATNR,
        t1.cast_seq,
        t1.td_no_1,
        t1.grade_id,
        t1.grade_type1,
        t1.f_route1,
        t1.heat_count,
        t1.steel_wt,
        (LTS.LADLE_DEPART_WT - LTS.EMPTY_LADLE_WEIGHT)/1000 AS LTS_WEIGHT,
        (lf.ladle_depart_wt - lf.empty_ladle_wt)/1000 AS LF_WEIGHT, 
        t1.RECEIVE_WEIGHT,
        t1.MAT_ACT_WT,
        t1.CUT_SCRAP_WT,
        ROUND(NVL(t2.IN_CR, 0)/100, 3) AS IN_CR, -- 投入纯铬
        ROUND(NVL(t2.IN_NI, 0)/100, 3) AS IN_NI, -- 投入纯镍
        ROUND(NVL(t2.IN_MO, 0)/100, 3) AS IN_MO, -- 投入纯钼
        ROUND(NVL(t3.AT_CR, 0)/100, 3) AS AT_CR, -- 产出铬
        ROUND(NVL(t3.AT_NI, 0)/100, 3) AS AT_NI, -- 产出镍
        ROUND(NVL(t3.AT_MO, 0)/100, 3) AS AT_MO, -- 产出钼
        NVL(CASE WHEN t3.ELM_006 = -1 THEN 0 ELSE t3.ELM_006 END, 0) AS CR_CP, -- 成品铬成分
        NVL(CASE WHEN t3.ELM_007 = -1 THEN 0 ELSE t3.ELM_007 END, 0) AS NI_CP, -- 成品镍成分
        NVL(CASE WHEN t3.ELM_008 = -1 THEN 0 ELSE t3.ELM_008 END, 0) AS MO_CP, -- 成品钼成分
        DECODE(NVL(t2.IN_CR, 0), 0, 0, ROUND(NVL(t3.AT_CR, 0)*100/NVL(t2.IN_CR, 0), 2)) AS CR_RATIO, -- 铬收得率
        DECODE(NVL(t2.IN_NI, 0), 0, 0, ROUND(NVL(t3.AT_NI, 0)*100/NVL(t2.IN_NI, 0), 2)) AS NI_RATIO, -- 镍收得率
        DECODE(NVL(t2.IN_MO, 0), 0, 0, ROUND(NVL(t3.AT_MO, 0)*100/NVL(t2.IN_MO, 0), 2)) AS MO_RATIO, -- 钼收得率
        DECODE(NVL(t1.RECEIVE_WEIGHT + t1.CUT_SCRAP_WT, 0), 0, 0, ROUND(NVL(t2.METAL_CONS, 0) * 1000 / (t1.RECEIVE_WEIGHT + t1.CUT_SCRAP_WT), 0)) AS ALLAY_CON, -- 金属料消耗
        t2.METAL_CONS 
    FROM (
        SELECT
            aod_bof_e_dtime,
            HEATNR,
            MAX(grade_id) AS grade_id,
            MAX(grade_type1) AS grade_type1,
            MAX(f_route1) AS f_route1,
            MAX(td_no_1) AS td_no_1,
            MAX(cast_seq) AS cast_seq,
            MAX(steel_wt) AS steel_wt,
            COUNT(1) AS heat_count,
            SUM(RECEIVE_WEIGHT) AS RECEIVE_WEIGHT,
            SUM(MAT_ACT_WT) AS MAT_ACT_WT,
            SUM(CUT_SCRAP_WT) AS CUT_SCRAP_WT
        FROM tqmtscb02_mx
        WHERE HEATNR IN (SELECT heat_no FROM TQMTSB0)
)";

			// 查询 - 开始时间+结束时间过滤
			if (!v_from.IsEmpty())
			{
				sqlstr += " AND aod_bof_e_dtime >= @v_from ";
			}
			if (!v_to.IsEmpty())
			{
				sqlstr += " AND aod_bof_e_dtime <= @v_to ";
			}
			sqlstr += R"(
        GROUP BY aod_bof_e_dtime, HEATNR
    ) t1
    LEFT JOIN (
        SELECT
            aod_bof_e_dtime,
            HEATNR,
            SUM(include_cr*WEIGHT) AS IN_CR,
            SUM(CASE WHEN GRADE_ID IN (SELECT st_no FROM tqmts0x t WHERE t.elm_std_idx_a IN (SELECT IDX_NO FROM tqmts02 WHERE SPE_MIN>0 AND elm_code='007')) THEN include_ni*WEIGHT ELSE 0 END) AS IN_NI,
            SUM(CASE WHEN GRADE_ID IN (SELECT STEEL_GRADE FROM tqmtscb09_dr WHERE GRADE_SERIES='含钼') THEN include_mo*WEIGHT ELSE 0 END) AS IN_MO,
            SUM(CASE WHEN MAT_CODE_DR IN (SELECT MAT_CODE_DR FROM tqmtscb08_dr WHERE MAT_TYPE_DESC IN('辅料', '步骤费')) THEN 0 ELSE WEIGHT END) AS METAL_CONS
        FROM tqmtscb01_mx
        WHERE HEATNR IN (SELECT heat_no FROM TQMTSB0)
          AND MAT_TYPE_DESC NOT IN('步骤费', '回收')
)";

			// 查询 - 开始时间+结束时间过滤
			if (!v_from.IsEmpty())
			{
				sqlstr += " AND aod_bof_e_dtime >= @v_from ";
			}
			if (!v_to.IsEmpty())
			{
				sqlstr += " AND aod_bof_e_dtime <= @v_to ";
			}
			sqlstr += R"(
        GROUP BY aod_bof_e_dtime, HEATNR
    ) t2 ON t1.aod_bof_e_dtime = t2.aod_bof_e_dtime AND t1.HEATNR = t2.HEATNR
    LEFT JOIN (
        SELECT
            t.aod_bof_e_dtime,
            HEATNR,
            SUM((RECEIVE_WEIGHT + CUT_SCRAP_WT)*NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END, 0)) AS AT_CR,
            SUM(CASE WHEN GRADE_ID IN (SELECT st_no FROM tqmts0x t WHERE t.elm_std_idx_a IN (SELECT IDX_NO FROM tqmts02 WHERE SPE_MIN>0 AND elm_code='007')) THEN (RECEIVE_WEIGHT + CUT_SCRAP_WT)*NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END, 0) ELSE 0 END) AS AT_NI,
            SUM(CASE WHEN t.GRADE_ID IN (SELECT STEEL_GRADE FROM tqmtscb09_dr WHERE GRADE_SERIES='含钼') THEN (RECEIVE_WEIGHT + CUT_SCRAP_WT)*NVL(CASE WHEN ELM_008 = -1 THEN 0 ELSE ELM_008 END, 0) ELSE 0 END) AS AT_MO,
            MAX(ELM_006) AS ELM_006,
            MAX(ELM_007) AS ELM_007,
            MAX(ELM_008) AS ELM_008
        FROM tqmtscb02_mx t
        LEFT JOIN TQMTSB0 t2 ON t.HEATNR = t2.heat_no
        WHERE HEATNR IN (SELECT heat_no FROM TQMTSB0)
)";

			//查询 - 开始时间+结束时间过滤
			if (!v_from.IsEmpty())
			{
				sqlstr += " AND t.aod_bof_e_dtime >= @v_from ";
			}
			if (!v_to.IsEmpty())
			{
				sqlstr += " AND t.aod_bof_e_dtime <= @v_to ";
			}
			sqlstr += R"(
        GROUP BY t.aod_bof_e_dtime, HEATNR
    ) t3 ON t1.aod_bof_e_dtime = t3.aod_bof_e_dtime AND t1.HEATNR = t3.HEATNR
    LEFT JOIN DA_LTS_PROD_SUMMARY LTS ON T1.HEATNR = LTS.HEAT_NUMBER
    LEFT JOIN da_lf_pro_summary lf ON t1.HEATNR = lf.heatnumber
    LEFT JOIN TQMTSB0 ana ON t1.HEATNR = ana.heat_no
)";

			//外层聚合层 - 完整日期范围过滤
			sqlstr += " WHERE 1=1 ";
			if (!v_from.IsEmpty())
			{
				sqlstr += " AND t1.aod_bof_e_dtime >= @v_from ";
			}
			if (!v_to.IsEmpty())
			{
				sqlstr += " AND t1.aod_bof_e_dtime <= @v_to ";
			}

			// 保留原有筛选条件
			if (!cast_seq.IsEmpty())
			{
				sqlstr += " AND cast_seq = @cast_seq ";
			}
			if (!grade_id.IsEmpty())
			{
				sqlstr += " AND grade_id = @grade_id ";
			}
			if (!grade_type1.IsEmpty())
			{
				sqlstr += " AND grade_type1 = @grade_type1 ";
			}
			if (!f_route1.IsEmpty())
			{
				sqlstr += " AND f_route1 = @f_route1 ";
			}
			if (c_flag == "1" && ss_flag == "0")
			{
				sqlstr += " AND (grade_id like '2%' or grade_id like '3%' or grade_id like '5%') ";
			}
			if (ss_flag == "1" && c_flag == "0")
			{
				sqlstr += " AND (grade_id like '1%' or grade_id like '4%') ";
			}

			// 完成SQL拼接（聚合+排序+分页）
			sqlstr += " ) GROUP BY CAST_SEQ ORDER BY CAST_SEQ";
			// 日志输出（调试用）
			Log::Trace("", __FUNCTION__, "最终执行SQL: {0}", sqlstr);

			// 绑定参数（包含v_from和v_to）
			cmd_inq.SetCommandText(sqlstr);
			if (!v_from.IsEmpty()) cmd_inq.Parameters.Set("v_from", v_from);
			if (!v_to.IsEmpty()) cmd_inq.Parameters.Set("v_to", v_to);
			if (!cast_seq.IsEmpty()) cmd_inq.Parameters.Set("cast_seq", cast_seq);
			if (!grade_id.IsEmpty()) cmd_inq.Parameters.Set("grade_id", grade_id);
			if (!grade_type1.IsEmpty()) cmd_inq.Parameters.Set("grade_type1", grade_type1);
			if (!f_route1.IsEmpty()) cmd_inq.Parameters.Set("f_route1", f_route1);

			// 执行查询并返回结果
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
		doFlag = -1;
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
	return doFlag;
}