/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56  
Description: 铁水领料实绩查询
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
#include<math.h>
/***** C++ 的业务头文件部分 *****/ 
//#include "tmmsm15.h"


// service入口
BM2F_ENTERACE(mmsm50af2_inq)

int f_mmsm50af2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr					= "";
	CString sqlstr_count			= "";
	CString sqlstr_temp				= "";
	CString sqlstr_temp1            = ""; 
	int		TotalRecordCount		= 0  ;

	CString ch_start_time_f			= "";
	CString ch_start_time_t			= "";
	
	int total_num = 0;
	CDecimal total_wgt = 0;

	//系统的分页类信息。
	CPageInfo pageInfo; 
 
	//CTMMSM15 tmmsm15(conn);
	CModel tmmsm15("TMMSM15");

	CDbCommand cmd_inq(conn);

	try
	{
		//try
		//{//获取前台DEV控件传入的分页信息
		//	pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		//}
		//catch(CException& ce)
		//{
		//	pageInfo.RecordFrom = 0;
		//	pageInfo.PageSize   = 1000;
		//}


		////--------------------------------
		////获取传入参数
		//tmmsm15.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		//tmmsm15.Print();
		//if(bcls_rec->Tables[0].Columns.Contains("START_TIME_F"))
		//	ch_start_time_f = bcls_rec->Tables[0].Rows[0]["START_TIME_F"].ToString();
		//if(bcls_rec->Tables[0].Columns.Contains("START_TIME_T"))
		//	ch_start_time_t = bcls_rec->Tables[0].Rows[0]["START_TIME_T"].ToString();
		//if (bcls_rec->Tables[0].Columns.Contains("FLAG_2"))
		//	tmmsm15["USE_UP_TIME"] = bcls_rec->Tables[0].Rows[0]["FLAG_2"].ToString();
		//if (bcls_rec->Tables[0].Columns.Contains("FLAG_1"))
		//	tmmsm15["ARRIVE_REAL_TIME"] = bcls_rec->Tables[0].Rows[0]["FLAG_1"].ToString();
		//if (bcls_rec->Tables[0].Columns.Contains("IRON_DEST"))
		//	tmmsm15["IRON_DEST"] = bcls_rec->Tables[0].Rows[0]["IRON_DEST"].ToString();
		///* ***** 打印输入参数 ***** */
		//Log::Info("", __FUNCTION__, "tmmsm15.IRON_DEST  =[{0}]", tmmsm15["IRON_DEST"]);
		//Log::Info("", __FUNCTION__, "tmmsm15.CFID  =[{0}]", tmmsm15["CFID"]);
		//Log::Info("", __FUNCTION__, "start_time_f  =[{0}]", ch_start_time_f);
		//Log::Info("", __FUNCTION__, "start_time_t  =[{0}]", ch_start_time_t);

	
		//switch(conn->DatabaseKind)
		//{
		//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:				// MS SQL Server数据库
		//	case DB_KIND_ORACLE:	        // Oracle 数据库
		//	default:

		//		sqlstr_count = " SELECT COUNT(1) REC_COUNT, SUM(NET_WT) TOTAL_WT "
		//			"   FROM TMMSM15 "
		//			"  WHERE 1=1 "
		//			;
		//		sqlstr	 = " SELECT * "
		//			"   FROM TMMSM15 "
		//			"  WHERE 1=1 "
		//			;
		//		if (tmmsm15["IRON_NO"].ToString().Trim() != "")
		//		{
		//			sqlstr_temp	+= " AND IRON_NO			= @tmmsm15.IRON_NO"; 
		//		}
		//		Log::Info("", __FUNCTION__, "tmmsm15.CFID  =[{0}]", tmmsm15["CFID"]);
		//		if (tmmsm15["CFID"].ToString().Trim() != "")
		//		{
		//			sqlstr_temp += " AND CFID			= @tmmsm15.CFID";
		//		}

		//		if (tmmsm15["BFID"].ToString().Trim() != "")
		//		{
		//			sqlstr_temp += " AND BFID			= @tmmsm15.BFID";
		//		}
		//		if(ch_start_time_f.Trim() != "")
		//		{
		//			sqlstr_temp	+= " AND REC_CREATE_TIME			>= @ch_start_time_f"; 
		//		}
		//		if(ch_start_time_t.Trim() != "")
		//		{
		//			sqlstr_temp	+= " AND REC_CREATE_TIME			<= @ch_start_time_t"; 
		//		}
		//		if (tmmsm15["USE_UP_TIME"].ToString().Trim() != "")
		//		{
		//			sqlstr_temp += " AND USE_UP_TIME	!= ' '";
		//		}
		//		if (tmmsm15["ARRIVE_REAL_TIME"].ToString().Trim() != "")
		//		{
		//			sqlstr_temp += " AND ARRIVE_REAL_TIME	!= ' '";
		//		}
		//		if (tmmsm15["IRON_DEST"].ToString().Trim() != "")
		//		{
		//			sqlstr_temp += " AND IRON_DEST	 = @tmmsm15.IRON_DEST";
		//		}
		//		sqlstr_temp1= " ORDER BY  IRON_NO";

		//		sqlstr_count = sqlstr_count + sqlstr_temp;
		//		sqlstr = sqlstr + sqlstr_temp + sqlstr_temp1;
		//		break;
		//}
		//cmd_inq.Parameters.Set("tmmsm15.IRON_NO", tmmsm15["IRON_NO"]);
		//cmd_inq.Parameters.Set("tmmsm15.CFID", tmmsm15["CFID"]);
		//cmd_inq.Parameters.Set("tmmsm15.BFID", tmmsm15["BFID"]);
		//cmd_inq.Parameters.Set("ch_start_time_f"		 ,ch_start_time_f); 
		//cmd_inq.Parameters.Set("ch_start_time_t"		 ,ch_start_time_t); 
		//cmd_inq.Parameters.Set("tmmsm15.IRON_DEST", tmmsm15["IRON_DEST"]);
		//
		//cmd_inq.SetCommandText(sqlstr_count);
		///*TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32(); */
		//
		//cmd_inq.ExecuteReader();
		//
		//if (cmd_inq.Read())
		//{
		//	TotalRecordCount = cmd_inq.GetInt32(1);
		//	total_num = ceil(double(TotalRecordCount) / double(pageInfo.PageSize));
		//	total_wgt = cmd_inq.GetDecimal(2);
		//}
		//cmd_inq.Close();
		//
		////分页获取
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.ExecuteQuery(bcls_ret->Tables[0],pageInfo.RecordFrom,pageInfo.PageSize);
		//cmd_inq.Close();
		//Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);
		//Log::Info("", __FUNCTION__, "total_num  =[{0}]", total_num);
		//Log::Info("", __FUNCTION__, "total_wgt  =[{0}]", total_wgt);

		////返回分页总数量信息 
		//bcls_ret->Tables.Add("PageInfo");
		//bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL,"TotalRecordCount");
		//bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TOTAL_NUM");
		//bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TOTAL_WGT");
		//bcls_ret->Tables["PageInfo"].Rows.Add();
		//bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;	
		//bcls_ret->Tables["PageInfo"].Rows[0]["TOTAL_NUM"] = total_num;
		//bcls_ret->Tables["PageInfo"].Rows[0]["TOTAL_WGT"] = total_wgt;
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
