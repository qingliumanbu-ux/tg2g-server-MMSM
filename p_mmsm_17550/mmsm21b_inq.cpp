/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56  
Description: 炼钢转炉作业测厚实绩查询
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/ 

// service入口
BM2F_ENTERACE(mmsm21b_inq)

int f_mmsm21b_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int TotalRecordCount = 0  ;

	CString calip_thick_time_f = "";
	CString calip_thick_time_t = "";
	CString station_no = "";
	CString heat_no = "";
	CString prod_shift_no = "";
	CString prod_shift_group = "";
	//系统的分页类信息。
	CPageInfo pageInfo; 
	CDbCommand cmd_inq(conn);

	try
	{
		try
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch(CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize   = 1000;
		}
		//获取传入参数
		if(bcls_rec->Tables[0].Columns.Contains("CALIP_THICK_TIME_F"))
			calip_thick_time_f = bcls_rec->Tables[0].Rows[0]["CALIP_THICK_TIME_F"].ToString().Trim();
		if(bcls_rec->Tables[0].Columns.Contains("CALIP_THICK_TIME_T"))
			calip_thick_time_t = bcls_rec->Tables[0].Rows[0]["CALIP_THICK_TIME_T"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_NO"))
			station_no = bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_NO"))
			prod_shift_no = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_GROUP"))
			prod_shift_group = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_GROUP"].ToString().Trim();
		/* ***** 打印输入参数 ***** */
		//Log::Info("", __FUNCTION__, "calip_thick_time_f  =[{0}]", calip_thick_time_f);
		//Log::Info("", __FUNCTION__, "calip_thick_time_t  =[{0}]", calip_thick_time_t);
	
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TMMSM21B T1"
					"  WHERE 1=1 "
					;
				sqlstr	 = "SELECT "
						" T1.REC_CREATOR,"
						" T1.REC_CREATE_TIME,"
						" T1.REC_REVISOR,"
						" T1.REC_REVISE_TIME,"
						" T1.REC_ID,"
						" T1.HEAT_NO,"
						" T1.PRE_HEAT_NO,"
						" T1.STATION_NO,"
						" T1.PROD_SHIFT_GROUP,"
						" T1.PROD_SHIFT_NO,"
						" T1.FURNACE_AGE,"
						" T1.CALIP_THICK_TIME,"
						" T1.FURNACE_COUNT,"
						" T1.BOTTOM_BLOW_RESULT,"
						" T1.BOTTOM_BLOW_HOLE_NUM,"
						" T1.REMARK,"
						" T1.THICK_AREA_HEAD,"
						" T1.THICK_AREA_BACK,"
						" T1.THICK_POOL_HEAD,"
						" T1.THICK_POOL_BACK,"
						" T1.THICK_POOL_EAST,"
						" T1.THICK_POOL_WEST,"
						" T1.SLAGLINE_HEAD_WEST,"
						" T1.SLAGLINE_HEAD_EAST,"
						" T1.SLAGLINE_BACK_EAST,"
						" T1.SLAGLINE_BACK_WEST,"
						" T1.THICK_BOTTOM_MIN,"
						" T1.THICK_BOTTOM_MAX,"
						" T1.THICK_TRUNNION_EAST,"
						" T1.THICK_TRUNNION_WEST,"
						" T1.THICK_CAP_EAST_MAX,"
						" T1.THICK_CAP_WEST_MAX,"
						" T1.CORRODE_AREA_HEAD,"
						" T1.CORRODE_AREA_BACK,"
						" T1.CORRODE_POOL_HEAD,"
						" T1.CORRODE_POOL_BACK,"
						" T1.CORRODE_POOL_EAST,"
						" T1.CORRODE_POOL_WEST,"
						" T1.CORRODE_BOTTOM_MIN,"
						" T1.CORRODE_BOTTOM_MAX,"
						" T1.CORRODE_SLAGLINE_HEAD_WEST,"
						" T1.CORRODE_SLAGLINE_HEAD_EAST,"
						" T1.CORRODE_SLAGLINE_BACK_EAST,"
						" T1.CORRODE_SLAGLINE_BACK_WEST,"
						" T1.CORRODE_TRUNNION_EAST,"
						" T1.CORRODE_TRUNNION_WEST,"
						" T1.CORRODE_CAP_EAST,"
						" T1.CORRODE_CAP_WEST,"
						" NVL(T2.FURNACE_AGE, 0) FURNACE_AGE_MIN,"
						" NVL(T2.FURNACE_AGE_MAX, 0) FURNACE_AGE_MAX,"
						" NVL(T2.THICK_AREA_HEAD, 0) STD_THICK_AREA_HEAD,"
						" NVL(T2.THICK_AREA_BACK, 0) STD_THICK_AREA_BACK,"
						" NVL(T2.THICK_POOL_HEAD, 0) STD_THICK_POOL_HEAD,"
						" NVL(T2.THICK_POOL_BACK, 0) STD_THICK_POOL_BACK,"
						" NVL(T2.THICK_POOL_EAST, 0) STD_THICK_POOL_EAST,"
						" NVL(T2.THICK_POOL_WEST, 0) STD_THICK_POOL_WEST,"
						" NVL(T2.SLAGLINE_HEAD_WEST, 0) STD_SLAGLINE_HEAD_WEST,"
						" NVL(T2.SLAGLINE_HEAD_EAST, 0) STD_SLAGLINE_HEAD_EAST,"
						" NVL(T2.SLAGLINE_BACK_EAST, 0) STD_SLAGLINE_BACK_EAST,"
						" NVL(T2.SLAGLINE_BACK_WEST, 0) STD_SLAGLINE_BACK_WEST,"
						" NVL(T2.THICK_BOTTOM_MIN, 0) STD_THICK_BOTTOM_MIN,"
						" NVL(T2.THICK_BOTTOM_MAX, 0) STD_THICK_BOTTOM_MAX,"
						" NVL(T2.THICK_TRUNNION_EAST, 0) STD_THICK_TRUNNION_EAST,"
						" NVL(T2.THICK_TRUNNION_WEST, 0) STD_THICK_TRUNNION_WEST,"
						" NVL(T2.THICK_CAP_EAST_MAX, 0) STD_THICK_CAP_EAST_MAX,"
						" NVL(T2.THICK_CAP_WEST_MAX, 0) STD_THICK_CAP_WEST_MAX,"
						" NVL(T2.BOTTOM_BLOW_HOLE_NUM, 0) STD_BOTTOM_BLOW_HOLE_NUM,"
						" (T1.THICK_AREA_HEAD - NVL(t2.THICK_AREA_HEAD, 0)) DIFF_AREA_HEAD,"
						" (T1.THICK_AREA_BACK - NVL(t2.THICK_AREA_BACK, 0)) DIFF_AREA_BACK,"
						" (T1.THICK_POOL_HEAD - NVL(t2.THICK_POOL_HEAD, 0)) DIFF_POOL_HEAD,"
						" (T1.THICK_POOL_BACK - NVL(t2.THICK_POOL_BACK, 0)) DIFF_POOL_BACK,"
						" (T1.THICK_POOL_EAST - NVL(t2.THICK_POOL_EAST, 0)) DIFF_POOL_EAST,"
						" (T1.THICK_POOL_WEST - NVL(t2.THICK_POOL_WEST, 0)) DIFF_POOL_WEST,"
						" (T1.SLAGLINE_HEAD_WEST - NVL(t2.SLAGLINE_HEAD_WEST, 0)) DIFF_SLAGLINE_HEAD_WEST,"
						" (T1.SLAGLINE_HEAD_EAST - NVL(t2.SLAGLINE_HEAD_EAST, 0)) DIFF_SLAGLINE_HEAD_EAST,"
						" (T1.SLAGLINE_BACK_EAST - NVL(t2.SLAGLINE_BACK_EAST, 0)) DIFF_SLAGLINE_BACK_EAST,"
						" (T1.SLAGLINE_BACK_WEST - NVL(t2.SLAGLINE_BACK_WEST, 0)) DIFF_SLAGLINE_BACK_WEST,"
						" (T1.THICK_BOTTOM_MIN - NVL(t2.THICK_BOTTOM_MIN, 0)) DIFF_BOTTOM_MIN,"
						" (T1.THICK_BOTTOM_MAX - NVL(t2.THICK_BOTTOM_MAX, 0)) DIFF_BOTTOM_MAX,"
						" (T1.THICK_TRUNNION_EAST - NVL(t2.THICK_TRUNNION_EAST, 0)) DIFF_TRUNNION_EAST,"
						" (T1.THICK_TRUNNION_WEST - NVL(t2.THICK_TRUNNION_WEST, 0)) DIFF_TRUNNION_WEST,"
						" (T1.THICK_CAP_EAST_MAX - NVL(t2.THICK_CAP_EAST_MAX, 0)) DIFF_CAP_EAST_MAX,"
						" (T1.THICK_CAP_WEST_MAX - NVL(t2.THICK_CAP_WEST_MAX, 0)) DIFF_CAP_WEST_MAX"
						" FROM TMMSM21B t1"
						" LEFT JOIN TQMTS04B t2 ON t1.FURNACE_AGE >= t2.FURNACE_AGE AND t1.FURNACE_AGE < t2.FURNACE_AGE_MAX"
						" WHERE 1 = 1"
					;
				if (heat_no != "")
				{
					sqlstr_temp += " AND T1.HEAT_NO LIKE '%" + heat_no + "%'";
				}
				if (station_no != "")
				{
					sqlstr_temp += " AND T1.STATION_NO = '" + station_no + "'";
				}
				if (prod_shift_no != "")
				{
					sqlstr_temp += " AND T1.PROD_SHIFT_NO = '" + prod_shift_no + "'";
				}
				if (prod_shift_group != "")
				{
					sqlstr_temp += " AND T1.PROD_SHIFT_GROUP = '" + prod_shift_group + "'";
				}
				if(calip_thick_time_f != "")
				{
					sqlstr_temp += " AND T1.CALIP_THICK_TIME >= '" + calip_thick_time_f + "'";
				}
				if(calip_thick_time_t != "")
				{
					sqlstr_temp += " AND T1.CALIP_THICK_TIME <= '" + calip_thick_time_t + "'";
				}

				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY T1.STATION_NO, T1.CALIP_THICK_TIME DESC";
				sqlstr = sqlstr + sqlstr_temp;
				break;
		}

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32(); 
		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],pageInfo.RecordFrom,pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL,"TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;	

	 }
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
