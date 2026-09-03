/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2016-01-22
Description: 缓冷退火实绩画面的材料信息查询
**************************************************/
//框架头文件
#include "stdafx.h"


//业务头文件


//外部函数声明
//int f_mmsm2e_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsm835d_inq029)

int f_mmsm835d_inq029(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* ***** 自定义变量 ***** */
	int		TotalRecordCount = 0;

	//系统的分页类信息。
	CPageInfo pageInfo;

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString tableName = "";
	CString begin_time = "";
	CString end_time = "";
	CString begin_time1 = "";
	CString serial_number = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

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

		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}


		CModel tmmsm89("TMMSM89");
		tmmsm89.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();

		serial_number = bcls_rec->Tables[0].Rows[0]["SERIAL_NUMBER"].ToString();


		sqlstr =
			" SELECT TO_CHAR("
			"               ADD_MONTHS(TO_DATE(@begin_time1, 'YYYYMMDDHH24MISS'), -2),"
			"               'YYYYMMDDHH24MISS'"
			"       ) AS begin_time1"
			" FROM DUAL ";

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("begin_time1", begin_time);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			begin_time1 = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "begin_time1[{0}]  ", begin_time1);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr =
				" SELECT EVENT_TIME,SERIAL_NUMBER,'6240'  FACTORY_CODE,'6241' DST_STOCK_CODE,'6062' SRC_STOCK_CODE,WEIGH_NO,"
				" RECEIVE_DATA_TIME,BUNKER_NO_ORIGINAL,BUNKER_NO,MAT_CODE,MAT_NAME,MISSING_NO,STOCK_WT,'KG' MEASURE_UNIT,"
				" RESUME_SEQ_NO,decode(BACK_CODE_1,'1','已冲销') BACK_CODE_1"
				" FROM TMMSM89 t1"
				" WHERE  1=1"
				" AND substr(MAT_CODE, 1, 3) NOT in ('F06')"
				" AND  MAT_CODE NOT IN ( SELECT CODE FROM TEP0002 WHERE CODE_CLASS='MMLC01')"
				" AND  MISSING_NO NOT IN "
				" ( SELECT MISSING_NO FROM  TMMSM89"
				" WHERE 1=1"
				" AND substr(MAT_CODE, 1, 3) NOT in ('F06')"
				" AND  MAT_CODE NOT IN ( SELECT CODE FROM TEP0002 WHERE CODE_CLASS='MMLC01')"
				" and EVENT_DESC = '上传302'"
				" AND MAT_CODE LIKE 'F%'"
				" AND EVENT_TIME <= @end_time "
				" AND EVENT_TIME>=@begin_time1  "
				")  "
				" and EVENT_DESC = '发送L2'"
				" AND MAT_CODE LIKE 'F%'"
				;			
			break;
		}
		if (tmmsm89["SERIAL_NUMBER"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND SERIAL_NUMBER		IN ( '"+ serial_number+ "')";
		}
		if (tmmsm89["MAT_CODE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND MAT_CODE		like '%'|| @MAT_CODE||'%'";
		}
		if (tmmsm89["BUNKER_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND BUNKER_NO		like '%'|| @BUNKER_NO||'%'";
		}
		if (tmmsm89["WEIGH_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND WEIGH_NO		like '%'|| @WEIGH_NO||'%'";
		}
		if (end_time.Trim() != "")
		{
			sqlstr_temp += " AND EVENT_TIME<=@end_time";
		}
		if (begin_time.Trim() != "")
		{
			sqlstr_temp += " AND EVENT_TIME>=@begin_time";
		}
		

		sqlstr_count = " SELECT COUNT(1) FROM (" + sqlstr + sqlstr_temp + ") WHERE 1=1 ";		

		sqlstr_temp += " ORDER BY REC_CREATE_TIME DESC";		

		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
		Log::Trace("", __FUNCTION__, "sqlstr_count[{0}]  ", sqlstr_count);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("SERIAL_NUMBER", tmmsm89["SERIAL_NUMBER"].ToString());
		cmd_inq.Parameters.Set("MAT_CODE", tmmsm89["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm89["BUNKER_NO"].ToString());
		cmd_inq.Parameters.Set("WEIGH_NO", tmmsm89["WEIGH_NO"].ToString());
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("begin_time1", begin_time1);
		cmd_inq.Parameters.Set("end_time", end_time);

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		sqlstr = sqlstr + sqlstr_temp;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();


		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;


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


