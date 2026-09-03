/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 原料模板画面查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm835d_inq)

int f_mmsm835d_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int		TotalRecordCount = 0;
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString v_table_name = "";//表名称。
	CString v_mat_code = "";
	CString v_mat_id = "";
	CString v_dev_code = "";
	CString v_mat_name = "";
	CString begin_time = "";
	CString end_time = "";
	CDbCommand cmd_inq(conn);
	//系统的分页类信息。
	CPageInfo pageInfo;
	try
	{
		CModel tmmsm89("TMMSM89");
		tmmsm89.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();
		Log::Info("", __FUNCTION__, "TABLE_NAME =[{0}]", v_table_name);

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
				" WHERE  EVENT_DESC='发送L2'"
				" AND MAT_CODE LIKE 'F%'"
				" AND substr(MAT_CODE, 1, 3) NOT in ('F06')"
				" AND  MAT_CODE NOT IN ( SELECT CODE FROM TEP0002 WHERE CODE_CLASS='MMLC01')"
				" AND  not exists (SELECT 1 FROM"
				" ( SELECT MISSING_NO FROM  TMMSM89"
				" WHERE EVENT_DESC='上传302'"
				" AND MAT_CODE LIKE 'F%'"
				" AND substr(MAT_CODE, 1, 3) NOT in ('F06')"
				" AND  MAT_CODE NOT IN ( SELECT CODE FROM TEP0002 WHERE CODE_CLASS='MMLC01')) t2  WHERE  t1.MISSING_NO=t2.MISSING_NO) ";

			sqlstr_count = " SELECT COUNT(1) FROM (" + sqlstr + ") WHERE 1=1 ";
			break;
		}
		if (tmmsm89["SERIAL_NUMBER"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND SERIAL_NUMBER		like '%'|| @SERIAL_NUMBER||'%'";
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
		if (begin_time.Trim() != "")
		{
			sqlstr_temp += " AND EVENT_TIME>=@begin_time";
		}
		if (end_time.Trim() != "")
		{
			sqlstr_temp += " AND EVENT_TIME<=@end_time";
		}
		sqlstr_count = sqlstr_count + sqlstr_temp;

		sqlstr_temp += " ORDER BY REC_CREATE_TIME DESC";

		sqlstr = sqlstr + sqlstr_temp;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		Log::Info("", __FUNCTION__, "sqlstr_count =[{0}]", sqlstr_count);

		cmd_inq.Parameters.Set("SERIAL_NUMBER", tmmsm89["SERIAL_NUMBER"].ToString());
		cmd_inq.Parameters.Set("MAT_CODE", tmmsm89["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm89["BUNKER_NO"].ToString());
		cmd_inq.Parameters.Set("WEIGH_NO", tmmsm89["WEIGH_NO"].ToString());
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
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
