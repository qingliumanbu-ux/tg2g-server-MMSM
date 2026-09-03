/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:   1.0
Date:     2016-01-29 17:13:56  
Description: 原辅料出入库履历查询
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/




// service入口
BM2F_ENTERACE(mmsm53_inq)

int f_mmsm53_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr					= "";
	CString sqlstr_count			= "";
	CString sqlstr_temp				= "";
	int		TotalRecordCount		= 0  ;
	CString v_mat_kind = "";
	CString v_mat_type = "";
	CString v_mat_type1 = "";
	CString v_mat_type2 = "";
	
	

	//系统的分页类信息。
	CPageInfo pageInfo; 
 
	CModel tmmsm54("TMMSM54");
	CModel tmmsm53("TMMSM53");

	CDbCommand cmd_inq(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch(CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize   = 1000;
		}


		//--------------------------------
		//获取传入参数
		tmmsm53.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		tmmsm53.TrimOrBlank();

		if (bcls_rec->Tables[0].Columns.Contains("MAT_KIND"))//材料类型 有可能是1，2拼在一块传到后台
			v_mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
				
		/* ***** 打印输入参数 ***** */
		
		//Log::Info("", __FUNCTION__, "FACTORY_DIV  =[{0}]", tmmsm53["FACTORY_DIV"].ToString());
		//Log::Info("", __FUNCTION__, "HANDLE_DIV  =[{0}]", tmmsm53["HANDLE_DIV"].ToString());
		//Log::Info("", __FUNCTION__, "tmmsm53.MAT_TYPE  =[{0}]", tmmsm53["MAT_TYPE"].ToString());
		//Log::Info("", __FUNCTION__, "MAT_KIND  =[{0}]", v_mat_kind);
		
		//Log::Info("", __FUNCTION__, "tmmsm53["MAT_TYPE"] =[{0}]", tmmsm53["MAT_TYPE"].ToString().Trim());
	
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

			////如果查询条件物料类型为空，就取MMSMPAR里配置的物料类型
			//if (v_mat_kind.Trim() != "" && tmmsm53["MAT_TYPE"].ToString().Trim() == "")
			//{

			//	sqlstr_count = " SELECT COUNT(1) FROM "
			//		" (SELECT * FROM TMMSM53 a left join (SELECT * FROM TMMSM54) b"
			//		"  ON a.OTHER_BILL_NO = b.PLAN_NO_Y"
			//		"  WHERE a.FACTORY_DIV = nvl(@factory_div, a.FACTORY_DIV)"
			//		"  AND a.MAT_TYPE  in(" + v_mat_kind + ")"
			//		"  AND a.HANDLE_DIV = nvl(@handle_div, a.HANDLE_DIV))";

			//	sqlstr = " SELECT * FROM "
			//		" (SELECT * FROM TMMSM53 a left join (SELECT * FROM TMMSM54) b"
			//		"  ON a.OTHER_BILL_NO = b.PLAN_NO_Y"
			//		"  WHERE a.FACTORY_DIV = nvl(@factory_div, a.FACTORY_DIV)"
			//		"  AND a.MAT_TYPE  in(" + v_mat_kind + ")"
			//		"  AND a.HANDLE_DIV = nvl(@handle_div, a.HANDLE_DIV))";

			//}
			//else
			//{
			//	sqlstr_count = " SELECT COUNT(1) FROM "
			//	" (SELECT * FROM TMMSM53 a left join (SELECT * FROM TMMSM54) b"
			//	"  ON a.OTHER_BILL_NO = b.PLAN_NO_Y"
			//	"  WHERE a.FACTORY_DIV = nvl(@factory_div, a.FACTORY_DIV)"
			//	"  AND a.MAT_TYPE = nvl(@tmmsm53["MAT_TYPE"].ToString(), a.MAT_TYPE)"
			//	"  AND a.HANDLE_DIV = nvl(@handle_div, a.HANDLE_DIV))";

			//	sqlstr = " SELECT * FROM "
			//	" (SELECT * FROM TMMSM53 a left join (SELECT * FROM TMMSM54) b"
			//	"  ON a.OTHER_BILL_NO = b.PLAN_NO_Y"
			//	"  WHERE a.FACTORY_DIV = nvl(@factory_div, a.FACTORY_DIV)"
			//	"  AND a.MAT_TYPE = nvl(@tmmsm53["MAT_TYPE"].ToString(), a.MAT_TYPE)"
			//	"  AND a.HANDLE_DIV = nvl(@handle_div, a.HANDLE_DIV))";

			//}



			if (tmmsm53["MAT_TYPE"].ToString().Trim() != "") //优先级最高,如果查询条件中的材料类型为空，取EPESPARA配置的参数
			{
		
				sqlstr_count = " SELECT COUNT(1) FROM "
				" (SELECT * FROM TMMSM53 a left join (SELECT * FROM TMMSM54) b"
				"  ON a.OTHER_BILL_NO = b.PLAN_NO_Y"
				"  WHERE a.FACTORY_DIV = nvl(trim(@factory_div), a.FACTORY_DIV)"
				"  AND a.MAT_TYPE = nvl(@tmmsm53.MAT_TYPE, a.MAT_TYPE)"
				"  AND a.HANDLE_DIV = nvl(@handle_div, a.HANDLE_DIV))";

				sqlstr = " SELECT * FROM "
				" (SELECT * FROM TMMSM53 a left join (SELECT * FROM TMMSM54) b"
				"  ON a.OTHER_BILL_NO = b.PLAN_NO_Y"
				"  WHERE a.FACTORY_DIV = nvl(trim(@factory_div), a.FACTORY_DIV)"
				"  AND a.MAT_TYPE = nvl(@tmmsm53.MAT_TYPE, a.MAT_TYPE)"
				"  AND a.HANDLE_DIV = nvl(trim(@handle_div), a.HANDLE_DIV))";

			}
			else
			{
				if (v_mat_kind.Trim() != "")
				{
					sqlstr_count = " SELECT COUNT(1) FROM "
						" (SELECT * FROM TMMSM53 a left join (SELECT * FROM TMMSM54) b"
						"  ON a.OTHER_BILL_NO = b.PLAN_NO_Y"
						"  WHERE a.FACTORY_DIV = nvl(trim(@factory_div), a.FACTORY_DIV)"
						"  AND a.MAT_TYPE  in(" + v_mat_kind + ")"
						"  AND a.HANDLE_DIV = nvl(trim(@handle_div), a.HANDLE_DIV))";

					sqlstr = " SELECT * FROM "
						" (SELECT * FROM TMMSM53 a left join (SELECT * FROM TMMSM54) b"
						"  ON a.OTHER_BILL_NO = b.PLAN_NO_Y"
						"  WHERE a.FACTORY_DIV = nvl(trim(@factory_div), a.FACTORY_DIV)"
						"  AND a.MAT_TYPE  in(" + v_mat_kind + ")"
						"  AND a.HANDLE_DIV = nvl(trim(@handle_div), a.HANDLE_DIV))";

				}
				else
				{
					/*sqlstr_count = " SELECT COUNT(1) FROM "
						" (SELECT a.FACTORY_DIV FROM TMMSM53 a left join (SELECT * FROM TMMSM54) b"
						"  ON a.OTHER_BILL_NO = b.PLAN_NO_Y"
						"  WHERE a.FACTORY_DIV = nvl(trim(@factory_div), a.FACTORY_DIV)"
						"  AND a.HANDLE_DIV = nvl(trim(@handle_div), a.HANDLE_DIV))";*/


					sqlstr_count = " SELECT COUNT(1) FROM "
						" (SELECT a.FACTORY_DIV FROM TMMSM53 a left join (SELECT * FROM TMMSM54) b"
						"  ON a.OTHER_BILL_NO = b.PLAN_NO_Y"
						"  WHERE a.FACTORY_DIV = nvl(trim('" + tmmsm53["FACTORY_DIV"].ToString()
						+ "'), a.FACTORY_DIV) AND a.HANDLE_DIV = nvl(trim('" + tmmsm53["HANDLE_DIV"].ToString() + "'), a.HANDLE_DIV))";

					sqlstr = " SELECT * FROM "
						" (SELECT * FROM TMMSM53 a left join (SELECT * FROM TMMSM54) b"
						"  ON a.OTHER_BILL_NO = b.PLAN_NO_Y"
						"  WHERE a.FACTORY_DIV = nvl(trim('" + tmmsm53["FACTORY_DIV"].ToString() + "'), a.FACTORY_DIV)"
						"  AND a.HANDLE_DIV = nvl(trim('" + tmmsm53["FACTORY_DIV"].ToString() + "'), a.HANDLE_DIV))";
				}

			}

									
			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr = sqlstr + sqlstr_temp;

			//Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);
			break;
		}
		
		
		
		//Log::Info("", __FUNCTION__, "sqlstr_count  =[{0}]", sqlstr_count);
		cmd_inq.SetCommandText(sqlstr_count);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("factory_div", tmmsm53["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("handle_div", tmmsm53["HANDLE_DIV"].ToString());
		cmd_inq.Parameters.Set("tmmsm53.MAT_TYPE", tmmsm53["MAT_TYPE"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			TotalRecordCount = cmd_inq.GetInt32(1);
		}
		cmd_inq.Close();
		//TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32(); 

		//Log::Info("", __FUNCTION__, "TotalRecordCount  =[{0}]", TotalRecordCount);
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("factory_div", tmmsm53["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("handle_div", tmmsm53["HANDLE_DIV"].ToString());
		cmd_inq.Parameters.Set("tmmsm53.MAT_TYPE", tmmsm53["MAT_TYPE"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],pageInfo.RecordFrom,pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL,"TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;	

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
