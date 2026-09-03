/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-04-18
Description: 成品按收货时间导出
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(mmsmdbjl_inq)

int f_mmsmdbjl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_start_time = "";//开始时刻
	CString v_end_time = ""; //结束时刻
	CString v_heat_no = "";
	CString v_batch = "";
	CString v_st_no = "";

	CPageInfo pageInfo;

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr_union = "";
	CString sqlstr_temp = "";
	CString sqlstr_count = "";
	CString sqlstr_order = "";


	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		try{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_FROM"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["PROD_TIME_FROM"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_TO"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["PROD_TIME_TO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("BATCH"))
			v_batch = bcls_rec->Tables[0].Rows[0]["BATCH"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();

		

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//查询条件放在前面
			if (v_start_time != ""){
				sqlstr_temp += " AND TRAN_END_TIME = '" + v_start_time.Substring(0,6) + "01000000"+"'";

			}
			if (v_end_time != "")
			{
				sqlstr_temp += " AND TRAN_END_TIME <= '" + v_end_time + "'";
			}
			if (v_heat_no != "")
			{
				sqlstr_temp += " AND HEAT_NO = '" + v_heat_no + "'";
			}
			if (v_batch != "")
			{
				sqlstr_temp += " AND BATCH = '" + v_batch + "'";
			}
			if (v_st_no != "")
			{
				sqlstr_temp += " AND ST_NO = '" + v_st_no + "'";
			}

			//只查已经调拨的数据
			if (true)
			{
				sqlstr_temp += " and C_STATESIGN ='3'  AND  USAGE_DECISION <> '3005'";
			}

			sqlstr = " SELECT T.SLAB_CUT_TIME,T.PROD_SHIFT_GROUP,T.HEAT_NO,T.BATCH,' ' WL_CODE,' ' WL_DESCRIPTION,T.ST_NO,T.MAT_ACT_LEN,T.MAT_ACT_WIDTH,T.MAT_ACT_THICK,T.RECEIVE_WEIGHT,T.GUIDE_DEST,T.CASTING_PRE_JUDGMENT,																																																																										"
				"	T.CASTING_PURPOSE, T.SURF_QUALITY, DECODE(T.RECEIVE_WEIGHT, 0, 'N', 'Y') RECEIVE_STATUS, T.RECV_MAT_TIME, DECODE(T.RECEIVE_WEIGHT, 0, 'N', 'S') SPECIFIC_SEND, DECODE(T.C_STATESIGN, '3', 'S', '1', 'W', 'N') C_STATESIGN,																																																																							  "
				"	T.USAGE_DECISION, tm34.MEND_SHIFT as PROD_GROUP_TMMSM34,																																																																																																														  "
				"	CASE  WHEN T.C_DELIVERY_STOCK = '6235' THEN 'C0'																																																																																																																	  "
				"	WHEN(SUBSTR(T.MAT_NO, 0, 2) = 'A0' OR  SUBSTR(T.MAT_NO, 0, 2) = 'A1' OR  SUBSTR(T.MAT_NO, 0, 2) = 'A2' OR  SUBSTR(T.MAT_NO, 0, 2) = 'A3' OR SUBSTR(T.MAT_NO, 0, 2) = 'A5' OR SUBSTR(T.MAT_NO, 0, 2) = 'A5' OR SUBSTR(T.MAT_NO, 0, 2) = 'A6' OR SUBSTR(T.MAT_NO, 0, 2) = 'B0' OR																																																										  "
				"	SUBSTR(T.MAT_NO, 0, 2) = 'B1' OR SUBSTR(T.MAT_NO, 0, 2) = 'B2' OR SUBSTR(T.MAT_NO, 0, 2) = 'B9') AND tm34.mat_no is null AND TM34_1.MAT_NO <>' ' AND TM34_1.MEND_AFTER_WEIGHT <> tm34_1.MEND_BEFORE_WEIGHT THEN 'B0' WHEN T.ARCHIVE_TIME <>' ' AND  T.MEND_FLAG = '0' THEN 'A0'																																																										  "
				"	else tm34.MEND_SET end MEND_SET,																																																																																																																					  "
				"	tm34.MEND_AFTER_WEIGHT MEND_AFTER_WEIGHT,																																																																																																																			  "
				"	tm34.START_TIME XM_TIME, tm34.MEND_INNER_MODE  MEND_INNER_MODE, tm34.MEND_OUTER_MODE  MEND_OUTER_MODE, tm34.MEND_CALCULATE_RATE, tm34.MEND_TOTAL_TIME MEND_TOTAL_TIME,																																																																																				  "
				"	T.TRAN_END_TIME, T.HAND_OVER_GROUP JK_SHIFT_GROUP, t.DST_STOCK_CODE, t.mat_act_wt,																																																																																																									  "
				"	TW62.LOAD_CODE_FACTORY RETURNPLANT, TW62.SHIFT_GROUP SHIFTRETURN,																																																																																																													  "
				"	TW62.LOAD_END_TIME RETURNDATE, TW62.RETURNREASON RETURNREASON,																																																																																																														  "
				"	tm39.RECUT_DATE GQ_RECUT_DATE, tm39.PROD_SHIFT_GROUP GQ_PROD_GROUP, tm39.CUTTING_TYPE GQ_CUTTING_TYPE,																																																																																																				  "
				"	to_number(decode(substr(T.slab_no, length(T.slab_no) - 2, 3),' ',0,substr(T.slab_no, length(T.slab_no) - 2, 3))) CAST_DIV_NO, t.DEV_CODE, (SELECT PROD_SHIFT_GROUP FROM TMMSM36 WHERE MAT_NO = T.MAT_NO) PROD_GROUP_SB, t.HOT_SEND_FLAG, t.ZL_REASON_DESC,																																																																													  "
				"	t.JUDGE_RESULT_1, t.REMARK, twm12.UNLOAD_CODE_AREA, TWM12.TRUCK_NO, t.STOCK_L2, t.ORDER_NO, t.mat_no, (SELECT DELIVY_DATE from tqmom01 where ORDER_NO = T.ORDER_NO) DELIVY_DATE, (SELECT ORDER_THICK from tqmom01 where  ORDER_NO = T.ORDER_NO) ORDER_THICK,																																																														  "
				"	t.SG_GRADE_1, (SELECT GRADE_TYPE FROM DA_GRADE_TYPE WHERE GRADE_ID = T.ST_NO) GRADE_TYPE, (SELECT SCRAP_TYPE FROM DA_GRADE_TYPE WHERE GRADE_ID = T.ST_NO) SCRAP_TYPE, t.slab_no,																																																																																	  "
				"	tm34.GRINDING_START_TIME, tm34.GRINDING_OUTER_END_TIME, decode(tm34.MEND_BEFORE_UPPER_TEMP, 0, tm34.MEND_AFTER_UPPER_TEMP, tm34.MEND_BEFORE_UPPER_TEMP) MEND_BEFORE_TEMP, decode(tm34.MEND_BEFORE_BOTTOM_TEMP, 0, tm34.MEND_AFTER_BOTTOM_TEMP, MEND_BEFORE_BOTTOM_TEMP)  MEND_AFTER_TEMP,																																																							  "
				"	t.TRUCK_NO as CHEHAO, T.STOCK_PLACE_NO, T.STOCK_L2 BP_TOCK_L2, TM34.REC_REVISE_TIME REC_REVISE_TIME, ' ' ORDER_NO_DD, DECODE(SUBSTR(T.ST_NO, 0, 1), 1, '不锈钢', '碳钢') ST_NO_TYPE_1,																																																																																 "
				"	CASE WHEN SUBSTR(T.ST_NO, 0, 2) = '1A' OR SUBSTR(T.ST_NO, 0, 2) = '1D' THEN '镍钢' WHEN SUBSTR(T.ST_NO, 0, 2) = '1M' OR SUBSTR(T.ST_NO, 0, 2) = '1F' THEN '铬钢' WHEN SUBSTR(T.ST_NO, 0, 1) = '1' THEN '不锈钢' else '碳钢' end  as  ST_NO_TYPE_2,																																																																	 "
				"	TWM12.SHIFT_GROUP ZC_PROD_GROUP, TWM12.OUT_STOCK_TIME ZC_TIME,																																																																																																														  "
				"	T.C_DELIVERY_STOCK, DECODE(T.LOGISTICS_STATUS, '2', 'W', '3', 'Y', 'N')  LOGISTICS_STATUS, DECODE(T.LGORT, '6242', '成品库', '6246', '修磨库') VALUE_TYPE, T.PRODUCT_FLAG																																																																																				"
				"	FROM(SELECT MAT_NO, SLAB_CUT_TIME, PROD_SHIFT_GROUP, HEAT_NO, BATCH, ST_NO, MAT_ACT_LEN, MAT_ACT_WIDTH, MAT_ACT_THICK, RECEIVE_WEIGHT, GUIDE_DEST, CASTING_PRE_JUDGMENT, CASTING_PURPOSE, SURF_QUALITY, RECV_MAT_TIME, C_STATESIGN, USAGE_DECISION,																																																																	  "
				"	ARCHIVE_TIME, MEND_FLAG, C_DELIVERY_STOCK, TRAN_END_TIME, PRACTICE_NO, DST_STOCK_CODE, mat_act_wt, slab_no, DEV_CODE, HOT_SEND_FLAG, ZL_REASON_DESC, JUDGE_RESULT_1, REMARK, UNLOAD_CODE, ORDER_NO, SG_GRADE_1, TRUCK_NO, STOCK_PLACE_NO, STOCK_L2, LOAD_SCHEME_NO, LOGISTICS_STATUS, LGORT, PRODUCT_FLAG, HAND_OVER_GROUP FROM TMMSM01  where 1 = 1									" + sqlstr_temp + "																																			  "
				"	AND MAT_NO NOT IN(select IN_MAT_NO FROM TMMSM35 GROUP BY  IN_MAT_NO)																																																																																																							  "
				"	UNION ALL SELECT MAT_NO, SLAB_CUT_TIME, PROD_SHIFT_GROUP, HEAT_NO, BATCH, ST_NO, MAT_ACT_LEN, MAT_ACT_WIDTH, MAT_ACT_THICK, RECEIVE_WEIGHT, GUIDE_DEST, CASTING_PRE_JUDGMENT, CASTING_PURPOSE, SURF_QUALITY, RECV_MAT_TIME,																																																																							  "
				"	C_STATESIGN, USAGE_DECISION, ARCHIVE_TIME, MEND_FLAG, C_DELIVERY_STOCK, TRAN_END_TIME, PRACTICE_NO, DST_STOCK_CODE, mat_act_wt, slab_no, DEV_CODE, HOT_SEND_FLAG, ZL_REASON_DESC, JUDGE_RESULT_1, REMARK, UNLOAD_CODE, ORDER_NO, SG_GRADE_1, TRUCK_NO, STOCK_PLACE_NO, STOCK_L2, LOAD_SCHEME_NO, LOGISTICS_STATUS, LGORT, PRODUCT_FLAG, HAND_OVER_GROUP FROM HMMSM01	 where  1 = 1				" + sqlstr_temp + "																																  "
				"	AND MAT_NO NOT IN(select IN_MAT_NO FROM TMMSM35 GROUP BY  IN_MAT_NO)																																																																																																							  "
				"	) T   LEFT JOIN(SELECT PROD_SHIFT_GROUP, MEND_SHIFT,MEND_SET, MEND_AFTER_WEIGHT, START_TIME, MEND_INNER_MODE, MEND_OUTER_MODE, MEND_CALCULATE_RATE, MEND_TOTAL_TIME, GRINDING_START_TIME, GRINDING_OUTER_END_TIME, MEND_BEFORE_UPPER_TEMP, MEND_AFTER_UPPER_TEMP, MEND_BEFORE_BOTTOM_TEMP, MEND_AFTER_BOTTOM_TEMP, REC_REVISE_TIME, mat_no	FROM(																																												  "
				"	SELECT ROW_NUMBER() over(PARTITION BY MAT_NO ORDER BY REC_CREATE_TIME DESC) TM34_MAX, PROD_SHIFT_GROUP,MEND_SHIFT, MEND_SET, MEND_AFTER_WEIGHT, START_TIME, MEND_INNER_MODE, MEND_OUTER_MODE, MEND_CALCULATE_RATE, MEND_TOTAL_TIME, GRINDING_START_TIME, GRINDING_OUTER_END_TIME, MEND_BEFORE_UPPER_TEMP, MEND_AFTER_UPPER_TEMP, MEND_BEFORE_BOTTOM_TEMP, MEND_AFTER_BOTTOM_TEMP, REC_REVISE_TIME, mat_no FROM TMMSM34)	WHERE TM34_MAX = 1) tm34 on t.mat_no = tm34.MAT_NO													  "
				"	LEFT JOIN(select RECUT_DATE, PROD_SHIFT_GROUP, CUTTING_TYPE, MAT_no from(SELECT ROW_NUMBER() over(PARTITION BY MAT_NO ORDER BY RESUME_SEQ_NO DESC) TM39max, RECUT_DATE, PROD_SHIFT_GROUP, CUTTING_TYPE, MAT_NO from tmmsm39) where TM39max = '1') TM39 ON  t.mat_no = tm39.mat_no																																																									  "
				"	LEFT JOIN(select LOAD_CODE_FACTORY, SHIFT_GROUP, MAT_NO, LOAD_END_TIME, RETURNREASON from(SELECT ROW_NUMBER() over(PARTITION BY MAT_NO ORDER BY REC_CREATE_TIME DESC) tw62_max, LOAD_CODE_FACTORY, mat_no, SHIFT_GROUP, LOAD_END_TIME, RETURNREASON from twmsm62)  where tw62_max = '1') TW62 ON T.MAT_NO = TW62.MAT_NO																																																  "
				"	LEFT JOIN(SELECT MAT_NO, MEND_BEFORE_WEIGHT, MEND_AFTER_WEIGHT FROM(SELECT ROW_NUMBER() over(PARTITION BY MAT_NO ORDER BY REC_CREATE_TIME DESC) TM34_1MAX, MAT_NO, MEND_BEFORE_WEIGHT, MEND_AFTER_WEIGHT FROM TMMSM34_1)																																																																							  "
				"	WHERE TM34_1MAX = '1') tm34_1 on t.mat_no = tm34_1.MAT_NO																																																																																																															  "
				"	LEFT JOIN vWMSM12 TWM12 ON  T.MAT_NO = TWM12.MAT_NO AND TWM12.LOAD_SCHEME_NO = T.LOAD_SCHEME_NO";

			sqlstr_order += " ORDER BY  SLAB_CUT_TIME ASC";

			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);


			sqlstr = sqlstr + sqlstr_order;


			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			break;

		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


