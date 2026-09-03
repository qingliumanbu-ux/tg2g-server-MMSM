/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 获取消耗工序基表
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmxhhd_inq)

int f_mmsmxhhd_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_temp = "";
	CString sqlstr_group = "";
	int	 union_flag = 0;
	int	 empty_flag = 0;
	CString v_table_type = "";//表名称。
	CString v_mat_code = "";
	CString v_mat_id = "";
	CString v_dev_code = "";
	CString v_mat_name = "";
	CString v_hadle_div = "";
	CString v_start_time = "";
	CString v_end_time = "";
	CDbCommand cmd_inq(conn);
	CModel tmmsm56a2("TMMSM56A2");
	try
	{
		tmmsm56a2.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (bcls_rec->Tables[0].Columns.Contains("TABFLAG"))
			v_table_type = bcls_rec->Tables[0].Rows[0]["TABFLAG"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_ID"))
			v_mat_code = bcls_rec->Tables[0].Rows[0]["MAT_ID"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME"))
			v_mat_name = bcls_rec->Tables[0].Rows[0]["MAT_NAME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HANDLE_DIV"))
			v_hadle_div = bcls_rec->Tables[0].Rows[0]["HANDLE_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();

		if (v_start_time.Trim() == "")
		{
			sprintf(s.msg, "开始时间不能为空。"); 			
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm56a2["WEEK_DAY"].ToString().Trim() == "")
		{
			sprintf(s.msg, "炉号日期不能为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsm56a2.TrimOrBlank();
		//判断必须同时为空，或是同时有值
		if ((tmmsm56a2["AOD0_S"].ToString().Trim() == "" && tmmsm56a2["AOD0_E"].ToString().Trim() != "") || (tmmsm56a2["AOD0_S"].ToString().Trim() != "" && tmmsm56a2["AOD0_E"].ToString().Trim() == ""))
		{
			sprintf(s.msg, "开始炉号结束炉号必须同时有值或是同时为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ((tmmsm56a2["AOD1_S"].ToString().Trim() == "" && tmmsm56a2["AOD1_E"].ToString().Trim() != "") || (tmmsm56a2["AOD1_S"].ToString().Trim() != "" && tmmsm56a2["AOD1_E"].ToString().Trim() == ""))
		{
			sprintf(s.msg, "开始炉号结束炉号必须同时有值或是同时为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ((tmmsm56a2["AOD2_S"].ToString().Trim() == "" && tmmsm56a2["AOD2_E"].ToString().Trim() != "") || (tmmsm56a2["AOD2_S"].ToString().Trim() != "" && tmmsm56a2["AOD2_E"].ToString().Trim() == ""))
		{
			sprintf(s.msg, "开始炉号结束炉号必须同时有值或是同时为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ((tmmsm56a2["AOD6_S"].ToString().Trim() == "" && tmmsm56a2["AOD6_E"].ToString().Trim() != "") || (tmmsm56a2["AOD6_S"].ToString().Trim() != "" && tmmsm56a2["AOD6_E"].ToString().Trim() == ""))
		{
			sprintf(s.msg, "开始炉号结束炉号必须同时有值或是同时为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ((tmmsm56a2["BOF0_S"].ToString().Trim() == "" && tmmsm56a2["BOF0_E"].ToString().Trim() != "") || (tmmsm56a2["BOF0_S"].ToString().Trim() != "" && tmmsm56a2["BOF0_E"].ToString().Trim() == ""))
		{
			sprintf(s.msg, "开始炉号结束炉号必须同时有值或是同时为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ((tmmsm56a2["BOF1_S"].ToString().Trim() == "" && tmmsm56a2["BOF1_E"].ToString().Trim() != "") || (tmmsm56a2["BOF1_S"].ToString().Trim() != "" && tmmsm56a2["BOF1_E"].ToString().Trim() == ""))
		{
			sprintf(s.msg, "开始炉号结束炉号必须同时有值或是同时为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if ((tmmsm56a2["BOF2_S"].ToString().Trim() == "" && tmmsm56a2["BOF2_E"].ToString().Trim() != "") || (tmmsm56a2["BOF2_S"].ToString().Trim() != "" && tmmsm56a2["BOF2_E"].ToString().Trim() == ""))
		{
			sprintf(s.msg, "开始炉号结束炉号必须同时有值或是同时为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ((tmmsm56a2["BOF9_S"].ToString().Trim() == "" && tmmsm56a2["BOF9_E"].ToString().Trim() != "") || (tmmsm56a2["BOF9_S"].ToString().Trim() != "" && tmmsm56a2["BOF9_E"].ToString().Trim() == ""))
		{
			sprintf(s.msg, "开始炉号结束炉号必须同时有值或是同时为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}



		sqlstr_temp += "";
		if (tmmsm56a2["AOD0_S"].ToString().Trim() != "" && tmmsm56a2["AOD0_E"].ToString().Trim() != "")
		{
			sqlstr_temp += "  AND ( (HEAT_NO >= '" + tmmsm56a2["AOD0_S"].ToString() + "' AND HEAT_NO <= '" + tmmsm56a2["AOD0_E"].ToString() + "')";
		}
		if (tmmsm56a2["AOD1_S"].ToString().Trim() != "" && tmmsm56a2["AOD1_E"].ToString().Trim() != "")
		{
			if (sqlstr_temp != "")
			{
				sqlstr_temp += " OR (HEAT_NO>='" + tmmsm56a2["AOD1_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD1_E"].ToString() + "')";	
			}
			else
			{
				sqlstr_temp += "  AND ( (HEAT_NO>='" + tmmsm56a2["AOD1_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD1_E"].ToString() + "')";	
			}
		}
		if (tmmsm56a2["AOD2_S"].ToString().Trim() != "" && tmmsm56a2["AOD2_E"].ToString().Trim() != "")
		{
			if (sqlstr_temp != "")
			{
			sqlstr_temp += " OR (HEAT_NO>='" + tmmsm56a2["AOD2_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD2_E"].ToString() + "')";
			}
			else
			{
				sqlstr_temp += " AND ( (HEAT_NO>='" + tmmsm56a2["AOD2_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD2_E"].ToString() + "')";

			}
		}
		if (tmmsm56a2["AOD6_S"].ToString().Trim() != "" && tmmsm56a2["AOD6_E"].ToString().Trim() != "")
		{
			if (sqlstr_temp != "")
			{
			sqlstr_temp += " OR (HEAT_NO>='" + tmmsm56a2["AOD6_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD6_E"].ToString() + "')";
			}
			else
			{
				sqlstr_temp += " AND ( (HEAT_NO>='" + tmmsm56a2["AOD6_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD6_E"].ToString() + "')";

			}
		}
		if (tmmsm56a2["BOF0_S"].ToString().Trim() != "" && tmmsm56a2["BOF0_E"].ToString().Trim() != "")
		{
			if (sqlstr_temp != "")
			{
			sqlstr_temp += " OR (HEAT_NO>='" + tmmsm56a2["BOF0_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF0_E"].ToString() + "')";
			}
			else
			{
				sqlstr_temp += " AND ((HEAT_NO>='" + tmmsm56a2["BOF0_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF0_E"].ToString() + "')";

			}
		}
		if (tmmsm56a2["BOF1_S"].ToString().Trim() != "" && tmmsm56a2["BOF1_E"].ToString().Trim() != "")
		{
			if (sqlstr_temp != "")
			{
			sqlstr_temp += " OR (HEAT_NO>='" + tmmsm56a2["BOF1_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF1_E"].ToString() + "')";
			}
			else
			{
				sqlstr_temp += " and ((HEAT_NO>='" + tmmsm56a2["BOF1_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF1_E"].ToString() + "')";

			}
		}

		if (tmmsm56a2["BOF2_S"].ToString().Trim() != "" && tmmsm56a2["BOF2_E"].ToString().Trim() != "")
		{
			if (sqlstr_temp != "")
			{
			sqlstr_temp += " OR (HEAT_NO>='" + tmmsm56a2["BOF2_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF2_E"].ToString() + "')";
			}
			else
			{
				sqlstr_temp += " and ((HEAT_NO>='" + tmmsm56a2["BOF2_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF2_E"].ToString() + "')";

			}
		}
		if (tmmsm56a2["BOF9_S"].ToString().Trim() != "" && tmmsm56a2["BOF9_E"].ToString().Trim() != "")
		{
			if (sqlstr_temp != "")
			{
			sqlstr_temp += " OR (HEAT_NO>='" + tmmsm56a2["BOF9_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF9_E"].ToString() + "')";
			}
			else
			{
				sqlstr_temp += " and ((HEAT_NO>='" + tmmsm56a2["BOF9_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF9_E"].ToString() + "')";

			}
		}
		if (sqlstr_temp == "")
		{
			sqlstr_temp = " and ((stat_date=@stat_date)";			
		}		

		if (bcls_rec->Tables[0].Rows[0]["HEAT_IN"].ToString().Trim() != "")
		{
			bcls_rec->Tables[0].Rows[0]["HEAT_IN"] = bcls_rec->Tables[0].Rows[0]["HEAT_IN"].ToString().Replace(",", "','");
			sqlstr_temp += " OR (heat_no in ('" + bcls_rec->Tables[0].Rows[0]["HEAT_IN"].ToString() + "') AND HANDLE_DIV != 'F')";
		}
		if (bcls_rec->Tables[0].Rows[0]["HEAT_OUT"].ToString().Trim() != "")
		{
			//Log::Info("", __FUNCTION__, "HEAT_OUT =[{0}],HEAT_OUT后=[{1}]", bcls_rec->Tables[0].Rows[0]["HEAT_OUT"].ToString(), bcls_rec->Tables[0].Rows[0]["HEAT_OUT"].ToString().Replace(",", "', '"));

			bcls_rec->Tables[0].Rows[0]["HEAT_OUT"] = bcls_rec->Tables[0].Rows[0]["HEAT_OUT"].ToString().Replace(",", "','");
			sqlstr_temp += " and heat_no not in ('" + bcls_rec->Tables[0].Rows[0]["HEAT_OUT"].ToString() + "')";
		}

		sqlstr_temp = sqlstr_temp + ")";
		
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			if (v_table_type == "1")
			{
				sqlstr = " SELECT STAT_DATE,LOT_NO,SM_PLAN_NOL2,HEAT_NO,PROD_DATE,DEVO_TIME,MAT_CODE,MAT_NAME,SYSTEM_ID_MAT,DEV_CODE,L2_PROC_NO,ST_NO,WEIGH_NO,QUALITY_BATCH_NO,SEND_FLAG,SEND_TIME,HANDLE_DIV"
					", t.DEVO_WT/1000 DEVO_WT_S"
					" FROM  TMMSM2A_SEND t"
					" WHERE 1=1 "
					" AND RTN_FLAG<>'1' "
					" AND SEND_FLAG = '1'"
					" AND HANDLE_DIV != 'F'"
					+ sqlstr_temp +
					" union all"
					" SELECT t.STAT_DATE,t.LOT_NO,t.SM_PLAN_NOL2,t.HEAT_NO,substr(t.RECV_MAT_TIME,1,8) PROD_DATE,t.OUT_STOCK_TIME as DEVO_TIME,t.MAT_CODE,t2.MAT_NAME"
					",t2.SYSTEM_ID_MAT"
					",t.DEV_CODE,t.L2_PROC_NO,t.ST_NO,t.WEIGH_NO,t.QUALITY_BATCH_NO,' ' SEND_FLAG,' ' SEND_TIME,' ' HANDLE_DIV"
					", t.OUT_STOCK_WT/1000 DEVO_WT_S"
					" FROM  tmmsm56 t left join tmmsm50 t2 on t.mat_code = t2.mat_code"
					" WHERE 1=1 "
					"  and t.mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
					" AND NOT EXISTS(SELECT 1 FROM tmmsm2a_send t2 where   t2.RTN_FLAG<>'1'  AND t2.SEND_FLAG = '1' AND t.heat_no=t2.heat_no)"
					+ sqlstr_temp +	 					
					" union all "
					" SELECT STAT_DATE,LOT_NO,SM_PLAN_NOL2,HEAT_NO,PROD_DATE,DEVO_TIME,MAT_CODE,MAT_NAME,SYSTEM_ID_MAT,DEV_CODE,L2_PROC_NO,ST_NO,WEIGH_NO,QUALITY_BATCH_NO,SEND_FLAG,SEND_TIME,HANDLE_DIV"
					", t.DEVO_WT/1000 DEVO_WT_S"
					" FROM  TMMSM2A_SEND t"
					" WHERE 1=1 "
					" AND RTN_FLAG<>'1' "
					" AND SEND_FLAG = '1'"
					" AND HANDLE_DIV = 'F'"
					;
				if (v_end_time.Trim() != "")
				{
					sqlstr = sqlstr + " AND SEND_TIME <=@v_end_time" ;
				}
					sqlstr = sqlstr +
					" AND SEND_TIME >=@v_start_time"
					" AND STAT_DATE =@stat_date"
					" order by PROD_DATE,heat_no,mat_code"
					;
			}
			else if (v_table_type == "2")
			{
				sqlstr =
					" SELECT MAT_CODE ,MAX(MAT_NAME) MAT_NAME,"
					" SUM(DEVO_WT)/1000 DEVO_WT,"
					" SUM(CASE WHEN HANDLE_DIV='F' THEN DEVO_WT END)/1000 DEVO_WT_F,"
					" SUM(CASE WHEN HANDLE_DIV<>'F' THEN DEVO_WT END)/1000 DEVO_WT_T,"
					" 'Ton'  UNIT"
					" FROM "
					" ( SELECT MAT_CODE, MAT_NAME, HANDLE_DIV, SUM(DEVO_WT) DEVO_WT"
					" FROM  TMMSM2A_SEND t"
					" WHERE 1=1 "
					" AND RTN_FLAG<>'1' "
					" AND SEND_FLAG = '1'"
					" AND HANDLE_DIV != 'F'" 					
					+ sqlstr_temp +
					" group by MAT_CODE,MAT_NAME,HANDLE_DIV"
					" union all"
					" select MAT_CODE, MAT_NAME, ' ' as HANDLE_DIV, SUM(OUT_STOCK_WT) DEVO_WT FROM  tmmsm56 t"
					" WHERE 1=1 "
					"  and t.mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
					" AND NOT EXISTS(SELECT 1 FROM tmmsm2a_send t2 where  t2.RTN_FLAG<>'1'  AND t2.SEND_FLAG = '1' AND t.heat_no=t2.heat_no)"
					+ sqlstr_temp +
					" group by MAT_CODE,MAT_NAME"
					" union all "
					" SELECT MAT_CODE, MAT_NAME, HANDLE_DIV, SUM(DEVO_WT) DEVO_WT"
					" FROM  TMMSM2A_SEND t"
					" WHERE 1=1 "
					" AND RTN_FLAG<>'1' "
					" AND SEND_FLAG = '1'"
					" AND HANDLE_DIV = 'F'"
					;
				if (v_end_time.Trim() != "")
				{
					sqlstr = sqlstr + " AND SEND_TIME <=@v_end_time" ;
				}
				sqlstr = sqlstr +
					" AND SEND_TIME >=@v_start_time"
					" AND STAT_DATE =@stat_date"
					" group by MAT_CODE,MAT_NAME,HANDLE_DIV"
					" )"
					" where 1=1 " 					
					;
				if (v_mat_code.Trim() != "")
				{
					sqlstr += " AND MAT_CODE LIKE  '%'|| @MAT_CODE||'%'";
				}
				if (v_mat_name.Trim() != "")
				{
					sqlstr += " AND MAT_NAME LIKE  '%'|| @MAT_NAME||'%'";
				}
				sqlstr += " group by mat_code order by mat_code";
			} 

			else if (v_table_type == "3")
			{
				sqlstr =
					" SELECT t.MAT_CODE ,t2.MAT_NAME,case when QUALITY_FLAS='1' then t.LOT_NO else ' ' end LOT_NO,"
					" SUM(DEVO_WT)/1000 DEVO_WT,"
					" SUM(CASE WHEN HANDLE_DIV='F' THEN DEVO_WT END)/1000 DEVO_WT_F,"
					" SUM(CASE WHEN HANDLE_DIV<>'F' THEN DEVO_WT END)/1000 DEVO_WT_T,"
					" 'Ton'  UNIT"
					" FROM "
					" ( select MAT_CODE,MAT_NAME,HANDLE_DIV,LOT_NO,SUM(DEVO_WT) DEVO_WT FROM  TMMSM2A_SEND t"
					" WHERE 1=1 "
					" AND RTN_FLAG<>'1' "
					" AND SEND_FLAG = '1'"
					" AND HANDLE_DIV != 'F'"
					+ sqlstr_temp +
					" group by MAT_CODE,MAT_NAME,HANDLE_DIV,LOT_NO"
					" union all"
					" select MAT_CODE, MAT_NAME, ' ' as HANDLE_DIV, LOT_NO, SUM(OUT_STOCK_WT) DEVO_WT FROM  tmmsm56 t"
					" WHERE 1=1 "
					"  and t.mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
					" AND NOT EXISTS(SELECT 1 FROM tmmsm2a_send t2 where  t2.RTN_FLAG<>'1'  AND t2.SEND_FLAG = '1' AND t.heat_no=t2.heat_no)"
					+ sqlstr_temp +
					" group by MAT_CODE,MAT_NAME,LOT_NO"
					" union all "
					" SELECT MAT_CODE,MAT_NAME,HANDLE_DIV,LOT_NO,SUM(DEVO_WT) DEVO_WT"
					" FROM  TMMSM2A_SEND t"
					" WHERE 1=1 "
					" AND RTN_FLAG<>'1' "
					" AND SEND_FLAG = '1'"
					" AND HANDLE_DIV = 'F'"
					;
				if (v_end_time.Trim() != "")
				{
					sqlstr = sqlstr + " AND SEND_TIME <=@v_end_time";
				}
				sqlstr = sqlstr +
					" AND SEND_TIME >=@v_start_time"
					" AND STAT_DATE =@stat_date"
					" group by MAT_CODE, MAT_NAME, HANDLE_DIV, LOT_NO  "
					" ) t left join tmmsm50 t2 on t.mat_code = t2.mat_code"
					" where 1=1 "
					;
				if (v_mat_code.Trim() != "")
				{
					sqlstr += " AND t.MAT_CODE LIKE  '%'|| @MAT_CODE||'%'";
				}
				if (v_mat_name.Trim() != "")
				{
					sqlstr += " AND t2.MAT_NAME LIKE  '%'|| @MAT_NAME||'%'";
				}
				sqlstr += " group by t.mat_code,t2.mat_name,case when QUALITY_FLAS='1' then t.LOT_NO else ' ' end order by mat_code,LOT_NO";
			}
			else if (v_table_type == "4")
			{
				sqlstr =
					" SELECT MAT_CODE ,MAX(MAT_NAME) MAT_NAME,"
					" SUM(CASE WHEN HANDLE_DIV='F' THEN DEVO_WT END)/1000 DEVO_WT_F,"
					" - SUM(DEVO_WT)/1000 DEVO_WT,"
					" '261' MVT,"
					" '6241' STOCK_PLACE_NO,"
					" 'Ton'  BUN"
					" FROM "
					" ( SELECT MAT_CODE, MAT_NAME, HANDLE_DIV, SUM(DEVO_WT) DEVO_WT"
					" FROM  TMMSM2A_SEND t"
					" WHERE 1=1 "
					" AND RTN_FLAG<>'1' "
					" AND SEND_FLAG = '1'"
					" AND HANDLE_DIV != 'F'"
					+ sqlstr_temp +
					" group by MAT_CODE,MAT_NAME,HANDLE_DIV"
					" union all"
					" select MAT_CODE, MAT_NAME, ' ' as HANDLE_DIV, SUM(OUT_STOCK_WT) DEVO_WT FROM  tmmsm56 t"
					" WHERE 1=1 "
					"  and t.mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
					" AND NOT EXISTS(SELECT 1 FROM tmmsm2a_send t2 where  t2.RTN_FLAG<>'1'  AND t2.SEND_FLAG = '1' AND t.heat_no=t2.heat_no)"
					+ sqlstr_temp +
					" group by MAT_CODE,MAT_NAME"
					" union all "
					" SELECT MAT_CODE, MAT_NAME, HANDLE_DIV, SUM(DEVO_WT) DEVO_WT"
					" FROM  TMMSM2A_SEND t"
					" WHERE 1=1 "
					" AND RTN_FLAG<>'1' "
					" AND SEND_FLAG = '1'"
					" AND HANDLE_DIV = 'F'"
					;
				if (v_end_time.Trim() != "")
				{
					sqlstr = sqlstr + " AND SEND_TIME <=@v_end_time";
				}
				sqlstr = sqlstr +
					" AND SEND_TIME >=@v_start_time"
					" AND STAT_DATE =@stat_date"
					" group by MAT_CODE,MAT_NAME,HANDLE_DIV"
					" )"
					" where 1=1 "
					;
				if (v_mat_code.Trim() != "")
				{
					sqlstr += " AND MAT_CODE LIKE  '%'|| @MAT_CODE||'%'";
				}
				if (v_mat_name.Trim() != "")
				{
					sqlstr += " AND MAT_NAME LIKE  '%'|| @MAT_NAME||'%'";
				}
				sqlstr += " group by mat_code order by mat_code";
			}

			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			break;
		}

		cmd_inq.Parameters.Set("MAT_CODE", v_mat_code);
		cmd_inq.Parameters.Set("MAT_NAME", v_mat_name);
		cmd_inq.Parameters.Set("HANDLE_DIV", v_hadle_div);
		cmd_inq.Parameters.Set("v_start_time", v_start_time);
		cmd_inq.Parameters.Set("stat_date", v_start_time.SubstringNE(0,6));
		cmd_inq.Parameters.Set("v_end_time", v_end_time); 
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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
