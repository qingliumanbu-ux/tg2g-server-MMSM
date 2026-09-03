/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    1.0
Date:
Description: 导入报表
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsmjccjl_clear)

int f_mmsmjccjl_clear(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString table_name = " ";
	CString v_ccm_ld_1 = " ";

	CDbCommand cmd_inq(conn);

	try
	{ 

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			Log::Trace("", __FUNCTION__, "[{0}]", bcls_rec->Tables[0].get_TableName());

			if (bcls_rec->Tables[0].get_TableName() != "Table1")
			{
				if (bcls_rec->Tables[0].Columns.Contains("TABLE_NAME"))
					table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString().TrimOrBlank().ToUpper();
				if (bcls_rec->Tables[0].Columns.Contains("CCM_LD_1"))
					v_ccm_ld_1 = bcls_rec->Tables[0].Rows[0]["CCM_LD_1"].ToString().TrimOrBlank();
			}
			//if (bcls_rec->Tables[0].Columns.Contains("GRID_TAB"))
			//	v_grid_tab = bcls_rec->Tables[0].Rows[0]["GRID_TAB"].ToString();
			
			if (table_name == "TMMSMJCCJLADD1")
			{
				CModel tmmsmjccjladd1("TMMSMJCCJLADD1");

				for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
				{
					tmmsmjccjladd1.MergeFrom(bcls_rec->Tables[0].Rows[i]);

					Log::Trace("", __FUNCTION__, "删除[{0}]表中一条", table_name);

					/*sqlstr = " DELETE FROM TMMSMJCCJLADD1 WHERE 1=1 "
						"AND CCM_LD_1 = '" + v_ccm_ld_1 + "'";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();*/
					tmmsmjccjladd1.Delete("CCM_LD_1");
				}
			}
			else
			{
				sqlstr = " DELETE FROM TMMSMJCCJLADD1 WHERE 1=1 ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				sqlstr = " DELETE FROM TMMSMJCCJLADD2 WHERE 1=1 ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				Log::Trace("", "", "删除表TMMSMJCCJLADD1和TMMSMJCCJLADD2");
			}
			
		}
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
