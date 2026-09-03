
#include "stdafx.h"
//using namespace BM2;
//using namespace BM2::Data;
//using namespace BM2::Data::DbClient;


// service入口
BM2F_ENTERACE(mmsm65_inq)

int f_mmsm65_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 程序内部变量 ***** */
	int doFlag = 0;
	int fetchRowCount = 0;

	int TotalRecordCount = 0;

	//
	//系统的分页类信息。
	//CPageInfo pageInfo;


	/* ***** 业务变量 ***** */
	int    RowCount = 0;
	CString  vStartTime = "";
	CString  vEndTime("");
	CString  vapply("");
	CString  vmatno("");
	CString  status("");

	//实体类定义


	//数据库操作类定义
	CDbCommand cmd_inq(conn);


	// 数据库SQL操作字符串 
	CString  sqlstr("");
	CString  sqlstr_temp("");
	try
	{
		


		if (bcls_rec->Tables[0].Columns.Contains("S_DATETIME"))
		{
			vStartTime = bcls_rec->Tables[0].Rows[0]["S_DATETIME"];//开始时间
		}
		Log::Trace("", __FUNCTION__, "传入参数vStartTime[{0}]", vStartTime);

		if (bcls_rec->Tables[0].Columns.Contains("E_DATETIME"))
		{
			vEndTime = bcls_rec->Tables[0].Rows[0]["E_DATETIME"];//开始时间
		}
		Log::Trace("", __FUNCTION__, "传入参数vEndTime[{0}]", vEndTime);
		if (bcls_rec->Tables[0].Columns.Contains("PURCHASEDOCID"))
		{
			vapply = bcls_rec->Tables[0].Rows[0]["PURCHASEDOCID"];//申请号
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
		{
			vmatno = bcls_rec->Tables[0].Rows[0]["MAT_CODE"];//物料号
		}
		if (bcls_rec->Tables[0].Columns.Contains("STATUS"))
		{
			status = bcls_rec->Tables[0].Rows[0]["STATUS"];//上传状态
		}

		/* 查询铁水分配信息 */
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT a.*  "
				"   FROM TMMSM65 a "
				"  WHERE 1=1 ";

			if (vStartTime.Trim() != "")
			{
				sqlstr_temp += " AND a.S_DATETIME >=@starttime ";
			}

			if (vEndTime.Trim() != "")
			{
				sqlstr_temp += " AND a.E_DATETIME <=@endtime ";
			}
			if (vapply.Trim() != "")
			{
				sqlstr_temp += " AND a.PURCHASEDOCID like @applyid || '%'";
			}
			if (vmatno.Trim() != "")
			{
				sqlstr_temp +=  " AND a.mat_code like @matno || '%'"  ;
			}
			if (status.Trim() != "")
			{
				if (status=="3")
				{

				}
				else if (status == "4")
				{
					sqlstr_temp += " AND a.status IN ('0','2')";
				}
				else
				{
					sqlstr_temp += " AND a.status = @status ";
				}
				
			}

			sqlstr_temp += " ORDER BY a.REC_CREATE_TIME DESC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
	
		cmd_inq.Parameters.Clear();

		cmd_inq.SetCommandText(sqlstr);
		if (vStartTime.Trim() != "")
		{
			cmd_inq.Parameters.Set("starttime", vStartTime.Trim());
		}
		if (vEndTime.Trim() != "")
		{
			cmd_inq.Parameters.Set("endtime", vEndTime.Trim());
		}
		if (vapply.Trim() != "")
		{
			cmd_inq.Parameters.Set("applyid", vapply.Trim());
		}
		if (vmatno.Trim() != "")
		{
			cmd_inq.Parameters.Set("matno", vmatno.Trim());
		}
		if (status.Trim() != "")
		{
			if (status == "3")
			{

			}
			else if (status == "4")
			{

			}
			else
			{
				cmd_inq.Parameters.Set("status", status.Trim());
			}
		}
		//cmd_inq.ExecuteReader();
		//返回记录给table
		bcls_ret->Tables[0].Clear();
		//cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);

		TotalRecordCount = cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);

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
