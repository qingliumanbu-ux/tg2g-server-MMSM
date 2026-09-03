/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/


// service入口
BM2F_ENTERACE(mmsmwt_inq)

int f_mmsmwt_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString factory_div = "";
	CString cc_mach_no = "";
	CString st_no = "";
	CString sg_sign = "";
	int		TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsmwt("TMMSMWT");


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


		//--------------------------------
		//获取传入参数
		tmmsmwt.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr =
				"select tmmsmwt.* from tmmsmwt where 1=1 ";
			sqlstr_count = " SELECT COUNT(1) FROM tmmsmwt where 1=1 ";
			if (tmmsmwt["INGOT_NAME"].ToString().Trim() != "")
			{
				sqlstr_temp += "AND INGOT_NAME = @tmmsmwt.INGOT_NAME     ";
			}
			if (tmmsmwt["INGOT_CODE"].ToString().Trim() != "")
			{
				sqlstr_temp += "AND INGOT_CODE  = @tmmsmwt.INGOT_CODE    ";
			}
			if (tmmsmwt["FACTORY_DIV"].ToString().Trim() != "")
			{
				sqlstr_temp += "AND FACTORY_DIV  = @tmmsmwt.FACTORY_DIV    ";
			}
			sqlstr += sqlstr_temp;
			sqlstr_count += sqlstr_temp;
			sqlstr += "ORDER BY RESUME_SEQ_NO DESC";
			break;
		}

		//Log::Trace("", __FUNCTION__, "tmmsmwt["INGOT_NAME"] = [{0}]  tmmsmwt["INGOT_CODE"] = [{1}] tmmsmwt["FACTORY_DIV"] = [{2}] ", tmmsmwt["INGOT_NAME"].ToString(), tmmsmwt["INGOT_CODE"].ToString(), tmmsmwt["FACTORY_DIV"].ToString());
		
		//Log::Trace("", __FUNCTION__, "sqlstr			= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmmsmwt.INGOT_NAME", tmmsmwt["INGOT_NAME"].ToString());
		cmd_inq.Parameters.Set("tmmsmwt.INGOT_CODE", tmmsmwt["INGOT_CODE"].ToString());
		cmd_inq.Parameters.Set("tmmsmwt.FACTORY_DIV", tmmsmwt["FACTORY_DIV"].ToString());
		/*cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();*/

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
