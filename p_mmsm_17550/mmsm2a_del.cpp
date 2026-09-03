//框架头文件
#include "stdafx.h" 


//业务头文件

BM2F_ENTERACE(mmsm2a_del)

int f_mmsm2a_del(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sql_temp;

	/* 数据库操作类定义 */
	CModel tmmsm2a("TMMSM2A");
	CDbCommand cmd_inq(conn);

	try
	{

		tmmsm2a.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " DELETE  "
				"FROM TMMSM2A "
				"WHERE 1 = 1 "
				;

			if (tmmsm2a["PROC_NO"].ToString().Trim() != "")
			{
				sql_temp += "AND PROC_NO = @tmmsm2a.PROC_NO";
			}
			sqlstr = sqlstr + sql_temp;
			break;
		}
		cmd_inq.Parameters.Set("tmmsm2a.PROC_NO", tmmsm2a["PROC_NO"].ToString());
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
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