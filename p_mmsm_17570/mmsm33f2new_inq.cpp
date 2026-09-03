/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:  WXF
Version:    1.0
Date:     2023-07-05
Description: 板坯切断炉次查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/
/* ***** 静态函数申明 ***** */


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// CC实绩查询
/// <para>
/// 1.根据时间范围,炉号等条件进行CC实绩查询。
///
/// </para>
/// <para>数据库表：TMMSM31(CC炉次实绩表)          </para>
/// <para>主调用函数：前台MMSM31画面F2(查询)按钮         </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns> CC实绩 </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm33f2new_inq)

int f_mmsm33f2new_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_tpssm03 = "";
	CString sqlstr_tmmsm33 = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	int   fetchRowCount = 0;
	int   fetchRowCount1 = 0;

	CString v_pono = "";
	CString v_castno = "";
	CString v_heat_no = "";
	CString v_cc_mach_no = "";
	CString v_billet_type = "";
	CString v_ingot_code = "";
	CString v_history_flag = "";
	CString v_current_flag = "";
	CString v_cut_fin_flag = "";
	CString v_slab_dest = "";
	CString v_dev_code = "";
	CString v_station_id = "";
	CString v_station_no = "";
	CString v_heat_confm_time_f = "";
	CString v_heat_confm_time_t = "";

	CDecimal v_plan_slab_num = 0;
	CDecimal v_cut_slab_num = 0;

	CDecimal v_slab_num = 0;
	CDecimal v_slab_cut_num = 0;

	CString v_lslab_no = "";
	CString v_lslab_no_pre = "";

	CString psTableName = "";
	CString ps2TableName = "";


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm31("TMMSM31");
	CModel tpssm11("TPSSM11");
	CModel tpssm12("TPSSM12");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_tpssm03(conn);
	CDbCommand cmd_inq_tmmsm33(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		//--------------------------------
		//获取传入参数
		/*tpssm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);*/

		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			v_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
			v_station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_NO"))
			v_station_no = bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HISTORY_FLAG"))
			v_history_flag = bcls_rec->Tables[0].Rows[0]["HISTORY_FLAG"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("CURRENT_FLAG"))
			v_current_flag = bcls_rec->Tables[0].Rows[0]["CURRENT_FLAG"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("CAST_NO"))
			v_castno = bcls_rec->Tables[0].Rows[0]["CAST_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_CONFM_TIME_F"))
			v_heat_confm_time_f = bcls_rec->Tables[0].Rows[0]["HEAT_CONFM_TIME_F"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_CONFM_TIME_T"))
			v_heat_confm_time_t = bcls_rec->Tables[0].Rows[0]["HEAT_CONFM_TIME_T"].ToString();

		Log::Info("", __FUNCTION__, "v_history_flag      =[{0}]", v_history_flag);
		Log::Info("", __FUNCTION__, "v_current_flag      =[{0}]", v_current_flag);
		Log::Info("", __FUNCTION__, "v_station_id      =[{0}]", v_station_id);
		Log::Info("", __FUNCTION__, "v_station_no      =[{0}]", v_station_no);
		Log::Info("", __FUNCTION__, "v_castno      =[{0}]", v_castno);

		if (v_station_no.Trim() != "")
		{
			v_dev_code = v_station_id + v_station_no;
			Log::Info("", __FUNCTION__, "v_dev_code11      =[{0}]", v_dev_code);
		}
		Log::Info("", __FUNCTION__, "v_dev_code22      =[{0}]", v_dev_code);

		if (v_history_flag == "1" || v_current_flag == "1")
		{
			v_cut_fin_flag = v_history_flag;
			v_cut_fin_flag = v_current_flag;
		}
		else if (v_history_flag == "2" || v_current_flag == "2")
		{
			v_cut_fin_flag = v_history_flag;
			v_cut_fin_flag = v_current_flag;
		}
		Log::Info("", __FUNCTION__, "v_cut_fin_flag      =[{0}]", v_cut_fin_flag);


		if (!bcls_ret->Tables[0].Columns.Contains("PROC_NO"))
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "PROC_NO");
		if (!bcls_ret->Tables[0].Columns.Contains("BILLET_TYPE"))
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "BILLET_TYPE");
		if (!bcls_ret->Tables[0].Columns.Contains("INGOT_CODE"))
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "INGOT_CODE");
		if (!bcls_ret->Tables[0].Columns.Contains("SLAB_PLAN_DEST"))
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "SLAB_PLAN_DEST");
		if (!bcls_ret->Tables[0].Columns.Contains("PLAN_SLAB_NUM"))
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "PLAN_SLAB_NUM");
		if (!bcls_ret->Tables[0].Columns.Contains("CUT_SLAB_NUM"))
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "CUT_SLAB_NUM");

		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "PONO      =[{0}]", v_pono);
		Log::Info("", __FUNCTION__, "HEAT_NO   =[{0}]", v_heat_no);
		Log::Info("", __FUNCTION__, "CC_MACH_NO=[{0}]", v_station_no);
		Log::Info("", __FUNCTION__, "HISTORY_FLAG=[{0}]", v_history_flag);

		if (bcls_rec->Tables[0].Rows[0]["HISTORY_FLAG"].ToString() == "1")
		{
			psTableName = "TPSSM41";
			ps2TableName = "TPSSM42";
		}
		else if (bcls_rec->Tables[0].Rows[0]["CURRENT_FLAG"].ToString() != "")
		{
			psTableName = "TPSSM11";
			ps2TableName = "TPSSM12";
		}
		else
		{
			psTableName = "TPSSM11";
			ps2TableName = "TPSSM12";
		}

		sqlstr =
			"	SELECT a.HEAT_NO, a.PONO, a.ST_NO, a.CUT_FIN_FLAG, a.cast_no, a.cast_div_no, a.proc_no, a.factory_div,a.sm_plan_no,a.START_TIME_REAL,  "
			"	a.HEAT_CONFM_TIME, substr(a.dev_code, 2, 1) as CC_MACH_NO, b.SLAB_DEST AS SLAB_PLAN_DEST    "
			"	, TPSSM01.Cast_Lot_No,tqmts0x.LABEL1, "
			"	b.BILLET_TYPE, b.INGOT_CODE, b.SLAB_NUM AS PLAN_SLAB_NUM, nvl(c.mat_tube, 0) AS CUT_SLAB_NUM, c.SLAB_WT "
			"	, nvl(MAT_TUBE1, 0) as MAT_TUBE1, nvl(MAT_TUBE2, 0) as MAT_TUBE2, nvl(MAT_TUBE3, 0) as MAT_TUBE3, nvl(MAT_TUBE4, 0) as MAT_TUBE4 "
			"	, nvl(MAT_TUBE5, 0) as MAT_TUBE5, nvl(MAT_TUBE6, 0) as MAT_TUBE6, nvl(MAT_TUBE7, 0) as MAT_TUBE7, nvl(MAT_TUBE8, 0) as MAT_TUBE8 "
			"   , nvl(MAT_LD_NUM, 0) as MAT_LD_NUM, nvl(MAT_RS_NUM, 0) as MAT_RS_NUM,nvl(MAT_GY_NUM, 0) as MAT_GY_NUM  " 
			"	from( "
			"		SELECT a.HEAT_NO, A.PONO, A.ST_NO, A.CUT_FIN_FLAG, a.cast_no, a.cast_div_no,A.FACTORY_DIV,A.SM_PLAN_NO,b.START_TIME_REAL "
			"		, A.HEAT_CONFM_TIME, b.proc_no, b.dev_code FROM "
			"		" + psTableName + " a, " + ps2TableName + "  b "
			"		WHERE a.heat_no = b.heat_no AND  b.area_id = 5 AND  a.run_status >= 52) a "
			"	left join "
			"	( "
			"		SELECT PONO, SLAB_DEST, BILLET_TYPE, INGOT_CODE, sum(SLAB_NUM)SLAB_NUM FROM TPSSM03 "
			"		GROUP BY PONO, SLAB_DEST, BILLET_TYPE, INGOT_CODE) b on a.pono = b.pono "
			"	LEFT JOIN "
			"	(SELECT pono, SLAB_PLAN_DEST "
			"		, sum(mat_tube) mat_tube, sum(slab_wt) SLAB_WT    "
			"		, sum(decode(STRAND_NO, '1', 1, 0)) as MAT_TUBE1  "
			"		, sum(decode(STRAND_NO, '2', 1, 0)) as MAT_TUBE2  "
			"		, sum(decode(STRAND_NO, '3', 1, 0)) as MAT_TUBE3  "
			"		, sum(decode(STRAND_NO, '4', 1, 0)) as MAT_TUBE4  "
			"		, sum(decode(STRAND_NO, '5', 1, 0)) as MAT_TUBE5  "
			"		, sum(decode(STRAND_NO, '6', 1, 0)) as MAT_TUBE6  "
			"		, sum(decode(STRAND_NO, '7', 1, 0)) as MAT_TUBE7  "
			"		, sum(decode(STRAND_NO, '8', 1, 0)) as MAT_TUBE8  "
			"		, sum(decode(REASON_LD, '1', 1, 0)) as MAT_LD_NUM "
			"		, sum(decode(REASON_LD, '2', 1,' ',1, 0)) as MAT_RS_NUM "
			"		, sum(decode(METAL_ABNY_CODE,' ',0, 1)) as MAT_GY_NUM "
			"		FROM TMMSM33									  "
			"		GROUP BY pono, SLAB_PLAN_DEST) C ON C.pono = b.pono AND C.SLAB_PLAN_DEST = b.SLAB_DEST "
			"	LEFT JOIN TPSSM01 ON TPSSM01.PONO = a.PONO "
			"  LEFT JOIN TQMTS0X ON TQMTS0X.ST_NO=a.ST_NO"
			"	WHERE 1 = 1 ";
		//" AND substr(b.dev_code,1,1) =@station_id "
		if (bcls_rec->Tables[0].Rows[0]["HISTORY_FLAG"].ToString() == "1")
		{
			if (v_heat_confm_time_f.Trim() != "")
			{
				sqlstr_temp += " AND a.HEAT_CONFM_TIME	>= @v_heat_confm_time_f";
			}
			if (v_heat_confm_time_t.Trim() != "")
			{
				sqlstr_temp += " AND a.HEAT_CONFM_TIME	<= @v_heat_confm_time_t";
			}
		}
		if (v_heat_no.Trim() != "")
			sqlstr_temp += " AND a.heat_no	= @heat_no";
		if (v_station_id.Trim() != "")
			sqlstr_temp += " AND substr(a.dev_code,1,1) ='C' ";
		if (v_pono.Trim() != "")
			sqlstr_temp += " AND a.pono	= @pono";
		if (v_dev_code.Trim() != "")
			sqlstr_temp += " AND a.dev_code	= @dev_code";
		if (v_castno.Trim() != "")
			sqlstr_temp += " AND a.cast_no	= @v_castno";
		if (v_cut_fin_flag.Trim() == "1")
			sqlstr_temp += " AND a.cut_fin_flag  = @v_cut_fin_flag";
		if (v_cut_fin_flag.Trim() == "2")
			sqlstr_temp += " AND a.cut_fin_flag  != '1'";
		sqlstr_temp += " ORDER BY CC_MACH_NO ASC , a.START_TIME_REAL DESC";

		sqlstr_count = "select count(0) from (" + sqlstr + sqlstr_temp + ")";
		sqlstr = sqlstr + sqlstr_temp;

		Log::Info("", __FUNCTION__, "sqlstr_count  =[{0}]", sqlstr_count);
		Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);

		cmd_inq.Parameters.Set("heat_no", v_heat_no);
		cmd_inq.Parameters.Set("pono", v_pono);
		cmd_inq.Parameters.Set("dev_code", v_dev_code);
		cmd_inq.Parameters.Set("station_id", v_station_id);
		cmd_inq.Parameters.Set("v_castno", v_castno);
		cmd_inq.Parameters.Set("v_heat_confm_time_f", v_heat_confm_time_f);
		cmd_inq.Parameters.Set("v_heat_confm_time_t", v_heat_confm_time_t);
		cmd_inq.Parameters.Set("v_cut_fin_flag", v_cut_fin_flag);

		cmd_inq.SetCommandText(sqlstr_count);// 设置执行的SQL语句
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();

		cmd_inq.SetCommandText(sqlstr);// 设置执行的SQL语句
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		//逐行读取
		//cmd_inq.ExecuteReader();
		//while (cmd_inq.Read())
		//{
		//	fetchRowCount1 = 0;
		//	v_plan_slab_num = 0;
		//	v_slab_num = 0;
		//
		//	cmd_inq.Fetch(tpssm11);
		//	cmd_inq.Fetch(tpssm12);
		//	
		//	//Log::Info("", __FUNCTION__, "fetchRowCount  =[{0}]", fetchRowCount);
		//	//Log::Info("", __FUNCTION__, "tpssm11.PONO  =[{0}]", tpssm11["PONO"].ToString());

		//	tpssm11.MergeTo(bcls_ret->Tables[0], false);

		//	sqlstr_tpssm03 = " SELECT SLAB_DEST,sum(SLAB_NUM) FROM TPSSM03 WHERE PONO = @pono GROUP BY SLAB_DEST ";
		//	cmd_inq_tpssm03.SetCommandText(sqlstr_tpssm03);
		//	cmd_inq_tpssm03.Parameters.Clear();
		//	cmd_inq_tpssm03.Parameters.Set("pono", tpssm11["PONO"].ToString());
		//	cmd_inq_tpssm03.ExecuteReader();
		//	if (cmd_inq_tpssm03.Read())
		//	{
		//		v_slab_dest = cmd_inq_tpssm03.GetString(1);
		//		v_plan_slab_num = cmd_inq_tpssm03.GetDecimal(2);
		//	} 
		//	cmd_inq_tpssm03.Close();

		//	sqlstr_tmmsm33 = " SELECT sum(mat_tube) FROM tmmsm33 WHERE PONO = @pono";
		//	cmd_inq_tmmsm33.SetCommandText(sqlstr_tmmsm33);
		//	cmd_inq_tmmsm33.Parameters.Clear();
		//	cmd_inq_tmmsm33.Parameters.Set("pono", tpssm11["PONO"].ToString());
		//	cmd_inq_tmmsm33.ExecuteReader();
		//	if (cmd_inq_tmmsm33.Read())
		//		v_cut_slab_num = cmd_inq_tmmsm33.GetDecimal(1);
		//	cmd_inq_tmmsm33.Close();

		//	if (bcls_ret->Tables[0].Columns.Contains("PROC_NO"))
		//		bcls_ret->Tables[0].Rows[fetchRowCount]["PROC_NO"] = tpssm12["PROC_NO"];


		//	if (bcls_ret->Tables[0].Columns.Contains("SLAB_PLAN_DEST"))
		//		bcls_ret->Tables[0].Rows[fetchRowCount]["SLAB_PLAN_DEST"] = v_slab_dest;
		//	if (bcls_ret->Tables[0].Columns.Contains("PLAN_SLAB_NUM"))
		//		bcls_ret->Tables[0].Rows[fetchRowCount]["PLAN_SLAB_NUM"] = v_plan_slab_num;
		//	if (bcls_ret->Tables[0].Columns.Contains("CUT_SLAB_NUM"))
		//		bcls_ret->Tables[0].Rows[fetchRowCount]["CUT_SLAB_NUM"] = v_cut_slab_num;

		//	fetchRowCount++;

		//}
		cmd_inq.Close(); //关闭游标

		//返回分页总数量信息 
	/*	bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = fetchRowCount;*/

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
