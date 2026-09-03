/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2017-06-14
Description: 炼钢化渣炉实绩处理
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢化渣炉实绩处理
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件    
  

//外部函数声明


BM2_FUNCTION_EXPORT
 int f_mmsm7101_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义
	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	
	/* 业务变量 */
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_proc_div = "";
	CString v_pract_coll_mode = "";
	CString v_factory_div = "";
	int v_seq = 0;

	/* 实体类定义 */
	CModel tmmsm71("TMMSM71");
	
	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_sql(conn);

	try
	{
		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
		v_pract_coll_mode = bcls_rec->Tables[0].Rows[0]["PRACT_COLL_MODE"].ToString();
		v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank().ToUpper();

		//Log::Trace("", __FUNCTION__, "v_factory_div=[{0}]", v_factory_div);
		//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);

		//Log::Trace("", __FUNCTION__, "bcls_rec->Tables[0].Rows.get_Count()=[{0}]", bcls_rec->Tables[0].Rows.get_Count());

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm71.Reset();
			tmmsm71.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm71.TrimOrBlank();

			//Log::Trace("", __FUNCTION__, "tmmsm71["SLAG_PROC_NO"] =[{0}]", tmmsm71["SLAG_PROC_NO"].ToString());
		
			if (v_proc_div == "I")
			{
				tmmsm71["REC_CREATE_TIME"] = dateNow;
				tmmsm71["REC_CREATOR"] = s.userid;

				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）

					sqlstr = "SELECT MAX(SUBSTR(SLAG_PROC_NO, 5, 5)) + 1 FROM TMMSM71 WHERE  SUBSTR(SLAG_PROC_NO, 1, 2) = (SELECT substr(to_char(current date,'yyyy'),3,2) FROM sysibm.sysdummy1)";

					break;

				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = "SELECT MAX(SUBSTR(SLAG_PROC_NO, 5, 5)) + 1 FROM TMMSM71 WHERE  SUBSTR(SLAG_PROC_NO, 1, 2) = (SELECT substr(to_char(current date,'yyyy'),3,2)  FROM DUAL)";

					break;
				}

			
				//Log::Info("", __FUNCTION__, " sqlstr =[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.ExecuteReader();
				if (cmd_sql.Read())
				{
					v_seq = cmd_sql.GetInt16(1);
				}
				else
				{
					v_seq = 1;
				}
				cmd_sql.Close();


				//Log::Info("", __FUNCTION__, "v_seq=[{0}]", v_seq);

				tmmsm71["SLAG_PROC_NO"] = tmmsm71["SLAG_PROC_NO"].ToString().Format("%s%s%.5d", (const char*)dateNow.Trim().SubstringNE(2, 2), "HZ", v_seq);

			

				//Log::Trace("", __FUNCTION__, "tmmsm71["SLAG_PROC_NO"] =[{0}]", tmmsm71["SLAG_PROC_NO"].ToString());

				tmmsm71.Insert();
			}
			else if (v_proc_div == "U")
			{
				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM	TMMSM71 "
					"   WHERE  SLAG_PROC_NO	= @tmmsm71.SLAG_PROC_NO";

				//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("tmmsm71.SLAG_PROC_NO", tmmsm71["SLAG_PROC_NO"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					
					tmmsm71["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsm71["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsm71["REC_REVISE_TIME"] = dateNow;
				tmmsm71["REC_REVISOR"] = s.userid;

				tmmsm71.Delete();
				tmmsm71.Insert();

			}
			else if (v_proc_div == "D")
			{
				tmmsm71.Delete();
			}


		}


	
		
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错,sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台,与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1,事务将被回滚
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
	cmd_sql.Close();
	//返回-1时事务将回滚,返回为0是事务将提交
	return doFlag;
}
