/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料主信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm831_inq_t83)

int f_mmsm831_inq_t83(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString mat_code1 = "";
	CString mat_code2 = "";
	int		TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm83("TMMSM83");

	CDbCommand cmd_inq(conn);

	try
	{
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
		tmmsm83.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Info("", __FUNCTION__, "BIN_SRART_TIME =[{0}]", tmmsm83["BIN_SRART_TIME"].ToString());
		Log::Info("", __FUNCTION__, "BIN_END_TIME =[{0}]", tmmsm83["BIN_END_TIME"].ToString());
		Log::Info("", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm83["BUNKER_NO"].ToString());
		Log::Info("", __FUNCTION__, "BUNKER_NO_ORIGINAL =[{0}]", tmmsm83["BUNKER_NO_ORIGINAL"].ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM83 "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT BUNKER_NO,BUNKER_NAME,BUNKER_NO_ORIGINAL,BUNKER_NAME_ORIGINAL,BIN_SRART_TIME,BIN_END_TIME,REAL_WEIGHT*1000 REAL_WEIGHT,BELT,PROD_SHIFT_GROUP,QUALITY_BATCH_NO,FLAG1,SEQ_CODE,MAT_CODE,MAT_NAME,ACTUAL_WEIGHT*1000 ACTUAL_WEIGHT "
				"   FROM TMMSM83 "
				"  WHERE 1=1  AND SEQ_CODE != ' '  "
				;
			if (tmmsm83["BIN_SRART_TIME"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND BIN_SRART_TIME	>= @BIN_SRART_TIME";
			}
			if (tmmsm83["BIN_END_TIME"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND BIN_END_TIME <= @BIN_END_TIME";
			}
			if (tmmsm83["BUNKER_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND BUNKER_NO like @BUNKER_NO || '%'";
			}

			if (tmmsm83["BUNKER_NO_ORIGINAL"].ToString().Trim() != "" )
			{
				sqlstr_temp += " AND BUNKER_NO_ORIGINAL	like @BUNKER_NO_ORIGINAL || '%'";
			}
			if (tmmsm83["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_CODE like @MAT_CODE || '%'";
			}

			if (tmmsm83["MAT_NAME"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_NAME	like @MAT_NAME || '%'";
			}
			if (tmmsm83["FLAG1"].ToString().Trim() == "0")
			{
				sqlstr_temp += " AND FLAG1 IN('0',' ')";
			}
			if (tmmsm83["FLAG1"].ToString().Trim() == "1" || tmmsm83["FLAG1"].ToString().Trim() == "2")
			{
				sqlstr_temp += " AND FLAG1 = @FLAG1 ";
			} 			
			sqlstr_temp += " ORDER BY FLAG1 ASC,REC_CREATE_TIME DESC,SEQ_CODE  DESC ";

			//sqlstr_count = sqlstr_count + sqlstr_temp;
			//sqlstr_temp += " ORDER BY SEQ_NO DESC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}

		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);

		cmd_inq.Parameters.Set("BIN_SRART_TIME", tmmsm83["BIN_SRART_TIME"].ToString());
		cmd_inq.Parameters.Set("BIN_END_TIME", tmmsm83["BIN_END_TIME"].ToString());
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm83["BUNKER_NO"].ToString());
		cmd_inq.Parameters.Set("BUNKER_NO_ORIGINAL", tmmsm83["BUNKER_NO_ORIGINAL"].ToString());
		cmd_inq.Parameters.Set("MAT_CODE", tmmsm83["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("MAT_NAME", tmmsm83["MAT_NAME"].ToString());
		cmd_inq.Parameters.Set("FLAG1", tmmsm83["FLAG1"].ToString());
		

		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		if (!bcls_ret->Tables[0].Columns.Contains("FLAG2"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "FLAG2");
		}
		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			sqlstr = " select mat_code from tmmsm60 where BUNKER_NO = @BUNKER_NO";
			cmd_inq.Parameters.Set("BUNKER_NO", bcls_ret->Tables[0].Rows[i]["BUNKER_NO_ORIGINAL"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				mat_code1 = cmd_inq.GetString(1);
				if (bcls_ret->Tables[0].Rows[i]["BUNKER_NO"].ToString() == mat_code1)
				{
					bcls_ret->Tables[0].Rows[i]["FLAG2"] = "0";
				}
				else
				{
					bcls_ret->Tables[0].Rows[i]["FLAG2"] = "1";
				}
				
			}
			else
			{
				bcls_ret->Tables[0].Rows[i]["FLAG2"] = "1";
			}
			cmd_inq.Close();

			sqlstr = " select mat_code from tmmsm60 where BUNKER_NO = @BUNKER_NO";
			cmd_inq.Parameters.Set("BUNKER_NO", bcls_ret->Tables[0].Rows[i]["BUNKER_NO"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				mat_code2 = cmd_inq.GetString(1);
				if (bcls_ret->Tables[0].Rows[i]["BUNKER_NO"].ToString() == mat_code2)
				{
					bcls_ret->Tables[0].Rows[i]["FLAG2"] = "0";
				}
				else
				{
					bcls_ret->Tables[0].Rows[i]["FLAG2"] = "1";
				}
			}
			else
			{
				bcls_ret->Tables[0].Rows[i]["FLAG2"] = "1";
			}
			cmd_inq.Close();
			if (mat_code1.Trim() != "" && mat_code2.Trim() != "")
			{
				if (mat_code1 == mat_code2)
				{
					bcls_ret->Tables[0].Rows[i]["FLAG2"] = "0";
				}
				else
				{
					bcls_ret->Tables[0].Rows[i]["FLAG2"] = "1";
				}
			}
			else
			{
				bcls_ret->Tables[0].Rows[i]["FLAG2"] = "1";
			}
			

		}
		
		Log::Info("", __FUNCTION__, "count(1) =[{0}]", bcls_ret->Tables[0].Rows.get_Count());

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
