/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */
// service入口
BM2F_ENTERACE(mmsmnx2_inq)

int f_mmsmnx2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString from_time = " ";
	CString to_time = " ";

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsmnx("TMMSMNX02");

	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("FROM_TIME"))
			from_time = bcls_rec->Tables[0].Rows[0]["FROM_TIME"].ToString().SubstringNE(0, 8);
		if (bcls_rec->Tables[0].Columns.Contains("TO_TIME"))
			to_time = bcls_rec->Tables[0].Rows[0]["TO_TIME"].ToString().SubstringNE(0, 8);
		try{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}


		//--------------------------------
		//获取传入参数
		tmmsmnx.MergeFrom(bcls_rec->Tables[0].Rows[0]);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			if (tmmsmnx["MANUFAC_NAME"].ToString().Trim() == ""&&tmmsmnx["MAT_CODE"].ToString().Trim() == ""&&from_time.Trim() == ""&&to_time.Trim() == "")
			{
				sqlstr_count = " SELECT COUNT(1) "
					"   FROM NX_V_PUR_CENTER_BASE2 "
					"  WHERE 1=1 "
					;

				sqlstr = " SELECT * "
					"   FROM NX_V_PUR_CENTER_BASE2"
					"  WHERE 1=1 "
					;
			}
			else
			{
				sqlstr_count = " SELECT COUNT(1) "
					"   FROM tmmsmnx02 "
					"  WHERE 1=1 ";

				sqlstr = " SELECT * "
					"   FROM tmmsmnx02"
					"  WHERE 1=1 ";
				if (tmmsmnx["MANUFAC_NAME"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MANUFAC_NAME		like '%'|| @tmmsmnx.MANUFAC_NAME||'%'";
				}
				if (tmmsmnx["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_CODE		like '%'|| @tmmsmnx.MAT_CODE||'%'";
				}
				if (from_time.Trim() != "")
				{
					sqlstr_temp += " AND TIME_STAMPS >= @from_time";
				}
				if (to_time.Trim() != "")
				{
					sqlstr_temp += " AND TIME_STAMPS <= @to_time";
				}
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY group_class_desc,MAT_CODE ";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		cmd_inq.Parameters.Set("tmmsmnx.MANUFAC_NAME", tmmsmnx["MANUFAC_NAME"].ToString());
		cmd_inq.Parameters.Set("tmmsmnx.MAT_CODE", tmmsmnx["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("to_time", to_time);
		cmd_inq.Parameters.Set("from_time", from_time);

		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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
