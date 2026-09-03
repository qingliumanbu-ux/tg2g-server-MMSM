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
BM2F_ENTERACE(mmsm85nbk_inq)

int f_mmsm85nbk_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int countFlag = 0;
	int countFlag1 = 0;
	int count = 0;
	int k = 0, j = 0;


	CString sqlstr = "";
	CString table_type = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	EIClass EITable;
	EIClass EITable1;
	EITable1.Tables.Clear();
	EITable1.Tables.Add();
	EITable1.Tables.Add();

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm60("TMMSM60");

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
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		//获取传入参数
		tmmsm60.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (!bcls_ret->Tables[0].Columns.Contains("PURCHASEDOCID"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "PURCHASEDOCID");
		}
		if (!bcls_ret->Tables[0].Columns.Contains("BUNKER_NO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "BUNKER_NO");
		}

		if (!bcls_ret->Tables[1].Columns.Contains("BUNKER_NO"))
		{
			bcls_ret->Tables[1].Columns.Add(DT_STRING, "BUNKER_NO");
		}
		if (!bcls_ret->Tables[1].Columns.Contains("MAT_CODE"))
		{
			bcls_ret->Tables[1].Columns.Add(DT_STRING, "MAT_CODE");
		}
		if (!bcls_ret->Tables[1].Columns.Contains("STOCK_WT"))
		{
			bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "STOCK_WT");
		}
		if (!bcls_ret->Tables[1].Columns.Contains("QUALITY_BATCH_NO"))
		{
			bcls_ret->Tables[1].Columns.Add(DT_STRING, "QUALITY_BATCH_NO");
		}
		if (!bcls_ret->Tables[1].Columns.Contains("SEQ_NO"))
		{
			bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "SEQ_NO");
		}

		Log::Info("", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm60["BUNKER_NO"].ToString());
		Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm60["MAT_CODE"].ToString());

		//bcls_rec->Tables.IndexOf("MMLCSND");

		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
		{
			table_type = "1";
		}
		Log::Info("", __FUNCTION__, "table_type =[{0}]", table_type);

		//Log::Info("", __FUNCTION__, "BUNKER_NAME =[{0}]", tmmsm60["BUNKER_NAME"].ToString());
		//Log::Info("", __FUNCTION__, "BUNKER_TYPE =[{0}]", tmmsm60["BUNKER_TYPE"].ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr = " SELECT * "
				"   FROM tmmsm60 "
				"  WHERE 1=1 and BUNKER_TYPE IN ('EAFBOX','BOFBOX','AODBOX') and BACK_C2 = '1' ORDER BY BUNKER_NO"
				;
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);


		//cmd_inq.SetCommandText(sqlstr_count);
		//TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(EITable.Tables[0]);
		cmd_inq.Close();
		if (EITable.Tables[0].Rows.get_Count()>0 )
		{
			for (int n = 0; n < EITable.Tables[0].Rows.get_Count(); n++)
			{
				if (EITable.Tables[0].Rows[n]["STOCK_WT"].ToDecimal()>0)
				{
					countFlag = 1;
				}
				if (EITable.Tables[0].Rows[n]["STOCK_WT"].ToDecimal()==0)
				{
					countFlag1 = 1;
				}
			}
			if (countFlag==1)
			{
				sqlstr = " SELECT VOUCHER_ID PURCHASEDOCID,BUNKER_NO "
					"   FROM TMMSM85 "
					"  WHERE 1=1 AND BUNKER_NO IN (SELECT BUNKER_NO "
					"   FROM tmmsm60 "
					"  WHERE 1=1 and BUNKER_TYPE IN ('EAFBOX','BOFBOX','AODBOX') and BACK_C2 = '1' ) GROUP BY BUNKER_NO,VOUCHER_ID ORDER BY BUNKER_NO DESC "
					;

				Log::Info("", __FUNCTION__, "sqlstr1 =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				//cmd_inq.Parameters.Set("BUNKER_NO", tmmsm60["BUNKER_NO"].ToString());
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
				//cmd_inq.ExecuteQuery(EITable1.Tables[0]);
				cmd_inq.Close();
				/*if (bcls_ret->Tables[0].Rows.get_Count()>0)
				{
					for (size_t i = 0; i < length; i++)
					{

					}
				}
				else
				{
					bcls_ret->Tables[0].Copy(EITable1.Tables[0]);
				}*/

				if (table_type == "1")
				{

					sqlstr = " SELECT BUNKER_NO , MAT_CODE,STOCK_WT , QUALITY_BATCH_NO,SEQ_NO FROM TMMSM85 WHERE BUNKER_NO =@BUNKER_NO ";
					cmd_inq.Parameters.Set("BUNKER_NO", tmmsm60["BUNKER_NO"].ToString());
					Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
					cmd_inq.Close();
				}

			}
			if (countFlag1==1)
			{
				TotalRecordCount = bcls_ret->Tables[0].Rows.get_Count();
				if (EITable.Tables[0].Rows.get_Count()>0)
				{
					if (TotalRecordCount>0)
					{
						for (int i = 0; i < EITable.Tables[0].Rows.get_Count(); i++)
						{
							if (EITable.Tables[0].Rows[i]["STOCK_WT"].ToDecimal() == 0)
							{
								bcls_ret->Tables[0].Rows.Add();
								bcls_ret->Tables[0].Rows[TotalRecordCount + k]["BUNKER_NO"] = EITable.Tables[0].Rows[i]["BUNKER_NO"].ToString();
								bcls_ret->Tables[0].Rows[TotalRecordCount + k]["PURCHASEDOCID"] = " ";
								k++;
							}
							
						}
					}
					if (TotalRecordCount<1)
					{
						for (int i = 0; i < EITable.Tables[0].Rows.get_Count(); i++)
						{
							if (EITable.Tables[0].Rows[i]["STOCK_WT"].ToDecimal() == 0)
							{
								bcls_ret->Tables[0].Rows.Add();
								bcls_ret->Tables[0].Rows[j]["BUNKER_NO"] = EITable.Tables[0].Rows[i]["BUNKER_NO"].ToString();
								bcls_ret->Tables[0].Rows[j]["PURCHASEDOCID"] = " ";
								j++;
							}
						}
					}
				}
			}
			/*else
			{
				sqlstr = " SELECT * "
					"   FROM tmmsm60 "
					"  WHERE 1=1 and BUNKER_TYPE IN ('EAFBOX','BOFBOX','AODBOX') and BACK_C2 = '1' ORDER BY BUNKER_NO"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
				cmd_inq.Close();
			}*/
		}

		Log::Info("", __FUNCTION__, "bcls_ret->Tables[0].Rows =[{0}]", bcls_ret->Tables[0].Rows.get_Count());
		//返回分页总数量信息 
		//bcls_ret->Tables.Add("PageInfo");
		//bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		//bcls_ret->Tables["PageInfo"].Rows.Add();
		//bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

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