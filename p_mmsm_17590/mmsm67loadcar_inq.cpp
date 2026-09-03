
#include "stdafx.h"
//using namespace BM2;
//using namespace BM2::Data;
//using namespace BM2::Data::DbClient;


// service入口
BM2F_ENTERACE(mmsm67loadcar_inq)

int f_mmsm67loadcar_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	//实体类定义


	//数据库操作类定义
	CDbCommand cmd_inq(conn);


	// 数据库SQL操作字符串 
	CString  sqlstr("");
	CString  sqlstr_temp("");
	try
	{



		if (bcls_rec->Tables[0].Columns.Contains("PURCHASEDOCID"))
		{
			vapply = bcls_rec->Tables[0].Rows[0]["PURCHASEDOCID"];//申请号
		}
	

		if (vapply.Trim() != "")
		{
			/* 查询铁水分配信息 */
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr = " SELECT a.*  "
					"   FROM TWMSM61 a "
					"  WHERE 1=1 ";


				if (vapply.Trim() != "")
				{
					sqlstr_temp += " AND a.plan_no like @applyid || '%'";
				}
				Log::Trace("", __FUNCTION__, "传入参数vapply[{0}]", vapply);

				sqlstr_temp += " ORDER BY a.REC_CREATE_TIME DESC";
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}
			Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);

			cmd_inq.Parameters.Clear();

			cmd_inq.SetCommandText(sqlstr);


			cmd_inq.Parameters.Set("applyid", vapply.Trim());


			//cmd_inq.ExecuteReader();
			//返回记录给table
			bcls_ret->Tables[0].Clear();
			//cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);

			TotalRecordCount = cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);

			cmd_inq.Close();
		}


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
