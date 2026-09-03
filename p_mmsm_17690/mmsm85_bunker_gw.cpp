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
BM2F_ENTERACE(mmsm85_bunker_gw)

int f_mmsm85_bunker_gw(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString s_bunker_no = "";
	int		TotalRecordCount = 0;
	int i_idx = 0;
	EIClass EITable;


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm85("TMMSM85");

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
		EITable.Tables.Add();
		EITable.Tables.Add();
		EITable.Tables.Add();
		EITable.Tables.Add();

		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		//--------------------------------
		//获取传入参数
		tmmsm85.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		Log::Info("", __FUNCTION__, "BUNKER_TYPE =[{0}]", tmmsm85["BUNKER_TYPE"].ToString());
		Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm85["MAT_CODE"].ToString());
		Log::Info("", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm85["BUNKER_NO"].ToString());
		//tmmsm85["BUNKER_NO"]
		s_bunker_no = tmmsm85["BUNKER_NO"].ToString().Trim();
		if (s_bunker_no.GetLength()>0)
		{
			i_idx = s_bunker_no.Find('-');
		}
		s_bunker_no = s_bunker_no.Substring(0, i_idx);
		Log::Info("", __FUNCTION__, "i_idx =[{0}],s_bunker_no =[{1}]", i_idx, s_bunker_no);

		sqlstr = "  SELECT   (BUNKER_NO||'-'||MAT_NAME) BUNKER_NO , BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
			"  WHERE 1=1  AND FLAG1 = 'H' "
			;
			
		if (tmmsm85["MAT_CODE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND MAT_CODE			= @tmmsm85.MAT_CODE";
		}
		//sqlstr_temp += " AND MAT_CODE			=  'AT000385'";
		sqlstr_temp += " order by BUNKER_NO ";
		sqlstr_count = sqlstr_count + sqlstr_temp;
		sqlstr = sqlstr + sqlstr_temp;

		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmmsm85.MAT_CODE", tmmsm85["MAT_CODE"].ToString());
		cmd_inq.ExecuteQuery(EITable.Tables[0]);
		cmd_inq.Close(); 

		// 地下料仓
		sqlstr = "  SELECT   (BUNKER_NO||'-'||MAT_NAME) BUNKER_NO , BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
			"  WHERE 1=1  AND BUNKER_TYPE IN ('AUTO','TRAIN','TRAINN','VIRT') "
			;

		sqlstr_temp = " ";
		if (tmmsm85["MAT_CODE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND MAT_CODE			= @tmmsm85.MAT_CODE";
		}
		
		sqlstr_temp += " order by BUNKER_TYPE,BACK_N_1 ";
		sqlstr_count = sqlstr_count + sqlstr_temp;
		sqlstr = sqlstr + sqlstr_temp;

		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmmsm85.MAT_CODE", tmmsm85["MAT_CODE"].ToString());
		cmd_inq.ExecuteQuery(EITable.Tables[1]);
		cmd_inq.Close();

		// 镍板库
		sqlstr = "  SELECT   (BUNKER_NO||'-'||MAT_NAME) BUNKER_NO , BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
			"  WHERE 1=1  AND BUNKER_TYPE IN ('NICKEL') "
			;

		sqlstr_temp = " ";
		if (tmmsm85["MAT_CODE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND MAT_CODE			= @tmmsm85.MAT_CODE";
		}
		sqlstr_temp += " order by BUNKER_NO ";
		sqlstr_count = sqlstr_count + sqlstr_temp;
		sqlstr = sqlstr + sqlstr_temp;

		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmmsm85.MAT_CODE", tmmsm85["MAT_CODE"].ToString());
		cmd_inq.ExecuteQuery(EITable.Tables[2]);
		cmd_inq.Close();

		// 络铁库
		sqlstr = "  SELECT   (BUNKER_NO||'-'||MAT_NAME) BUNKER_NO , BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
			"  WHERE 1=1  AND BUNKER_TYPE IN ('FECR','SCRAPALLOY') "
			;

		sqlstr_temp = " ";
		if (tmmsm85["MAT_CODE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND MAT_CODE			= @tmmsm85.MAT_CODE";
		}
		sqlstr_temp += " order by BUNKER_NO ";
		sqlstr_count = sqlstr_count + sqlstr_temp;
		sqlstr = sqlstr + sqlstr_temp;

		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmmsm85.MAT_CODE", tmmsm85["MAT_CODE"].ToString());
		cmd_inq.ExecuteQuery(EITable.Tables[3]);
		cmd_inq.Close();

		// 废钢坑
		sqlstr = "  SELECT   (BUNKER_NO||'-'||MAT_NAME) BUNKER_NO , BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
			"  WHERE 1=1  AND BUNKER_TYPE IN ('COOL','CARBON','STAINLESS','VS') "
			;
		sqlstr_temp = " ";
		if (tmmsm85["MAT_CODE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND MAT_CODE			= @tmmsm85.MAT_CODE";
		}
		sqlstr_temp += " order by BUNKER_NO ";
		sqlstr_count = sqlstr_count + sqlstr_temp;
		sqlstr = sqlstr + sqlstr_temp;

		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmmsm85.MAT_CODE", tmmsm85["MAT_CODE"].ToString());
		cmd_inq.ExecuteQuery(EITable.Tables[4]);
		cmd_inq.Close();

		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", 11111);

		if (EITable.Tables[0].Rows.get_Count() > 1)
		{
			bcls_ret->Tables[0].Clone(EITable.Tables[0]);
			for (int i = 0; i < EITable.Tables[0].Rows.get_Count() + 1; i++)
			{
				bcls_ret->Tables[0].Rows.Add();
				if (i < 1)
				{
					if (i == 0)
					{
						bcls_ret->Tables[0].Rows[0]["BUNKER_NO"] = " ";
						bcls_ret->Tables[0].Rows[0]["BUNKER_NAME"] = " ";
						bcls_ret->Tables[0].Rows[0]["BUNKER_TYPE"] = " ";
						bcls_ret->Tables[0].Rows[0]["MAT_CODE"] = " ";
						bcls_ret->Tables[0].Rows[0]["MAT_NAME"] = " ";
						bcls_ret->Tables[0].Rows[0]["MAT_TYPE"] = " ";
						bcls_ret->Tables[0].Rows[0]["STOCK_WT"] = 0;
						bcls_ret->Tables[0].Rows[0]["RATE"] = 0;

					}
				}
				else
				{
					bcls_ret->Tables[0].Rows[i]["BUNKER_NO"] = EITable.Tables[0].Rows[i - 1]["BUNKER_NO"];
					bcls_ret->Tables[0].Rows[i]["BUNKER_NAME"] = EITable.Tables[0].Rows[i - 1]["BUNKER_NAME"];
					bcls_ret->Tables[0].Rows[i]["BUNKER_TYPE"] = EITable.Tables[0].Rows[i - 1]["BUNKER_TYPE"];
					bcls_ret->Tables[0].Rows[i]["MAT_CODE"] = EITable.Tables[0].Rows[i - 1]["MAT_CODE"];
					bcls_ret->Tables[0].Rows[i]["MAT_TYPE"] = EITable.Tables[0].Rows[i - 1]["MAT_TYPE"];
					bcls_ret->Tables[0].Rows[i]["MAT_NAME"] = EITable.Tables[0].Rows[i - 1]["MAT_NAME"];
					bcls_ret->Tables[0].Rows[i]["STOCK_WT"] = EITable.Tables[0].Rows[i - 1]["STOCK_WT"];
					bcls_ret->Tables[0].Rows[i]["RATE"] = EITable.Tables[0].Rows[i - 1]["RATE"];
				}
			}
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", 22222);

		if (EITable.Tables[1].Rows.get_Count() > 0)
		{
			bcls_ret->Tables[1].Clone(EITable.Tables[1]);
			for (int i = 0; i < EITable.Tables[1].Rows.get_Count() + 1; i++)
			{
				bcls_ret->Tables[1].Rows.Add();
				if (i < 1)
				{
					if (i == 0)
					{
						bcls_ret->Tables[1].Rows[0]["BUNKER_NO"] = " ";
						bcls_ret->Tables[1].Rows[0]["BUNKER_NAME"] = " ";
						bcls_ret->Tables[1].Rows[0]["BUNKER_TYPE"] = " ";
						bcls_ret->Tables[1].Rows[0]["MAT_CODE"] = " ";
						bcls_ret->Tables[1].Rows[0]["MAT_NAME"] = " ";
						bcls_ret->Tables[1].Rows[0]["MAT_TYPE"] = " ";
						bcls_ret->Tables[1].Rows[0]["STOCK_WT"] = 0;
						bcls_ret->Tables[1].Rows[0]["RATE"] = 0;

					}
				}
				else
				{
					bcls_ret->Tables[1].Rows[i]["BUNKER_NO"] = EITable.Tables[1].Rows[i - 1]["BUNKER_NO"];
					bcls_ret->Tables[1].Rows[i]["BUNKER_NAME"] = EITable.Tables[1].Rows[i - 1]["BUNKER_NAME"];
					bcls_ret->Tables[1].Rows[i]["BUNKER_TYPE"] = EITable.Tables[1].Rows[i - 1]["BUNKER_TYPE"];
					bcls_ret->Tables[1].Rows[i]["MAT_CODE"] = EITable.Tables[1].Rows[i - 1]["MAT_CODE"];
					bcls_ret->Tables[1].Rows[i]["MAT_TYPE"] = EITable.Tables[1].Rows[i - 1]["MAT_TYPE"];
					bcls_ret->Tables[1].Rows[i]["MAT_NAME"] = EITable.Tables[1].Rows[i - 1]["MAT_NAME"];
					bcls_ret->Tables[1].Rows[i]["STOCK_WT"] = EITable.Tables[1].Rows[i - 1]["STOCK_WT"];
					bcls_ret->Tables[1].Rows[i]["RATE"] = EITable.Tables[1].Rows[i - 1]["RATE"];
				}
			}
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", 33333);

		if (EITable.Tables[2].Rows.get_Count() > 0)
		{
			bcls_ret->Tables[2].Clone(EITable.Tables[2]);
			for (int i = 0; i < EITable.Tables[2].Rows.get_Count() + 1; i++)
			{
				bcls_ret->Tables[2].Rows.Add();
				if (i < 1)
				{
					if (i == 0)
					{
						bcls_ret->Tables[2].Rows[0]["BUNKER_NO"] = " ";
						bcls_ret->Tables[2].Rows[0]["BUNKER_NAME"] = " ";
						bcls_ret->Tables[2].Rows[0]["BUNKER_TYPE"] = " ";
						bcls_ret->Tables[2].Rows[0]["MAT_CODE"] = " ";
						bcls_ret->Tables[2].Rows[0]["MAT_NAME"] = " ";
						bcls_ret->Tables[2].Rows[0]["MAT_TYPE"] = " ";
						bcls_ret->Tables[2].Rows[0]["STOCK_WT"] = 0;
						bcls_ret->Tables[2].Rows[0]["RATE"] = 0;

					}
				}
				else
				{
					bcls_ret->Tables[2].Rows[i]["BUNKER_NO"] = EITable.Tables[2].Rows[i - 1]["BUNKER_NO"];
					bcls_ret->Tables[2].Rows[i]["BUNKER_NAME"] = EITable.Tables[2].Rows[i - 1]["BUNKER_NAME"];
					bcls_ret->Tables[2].Rows[i]["BUNKER_TYPE"] = EITable.Tables[2].Rows[i - 1]["BUNKER_TYPE"];
					bcls_ret->Tables[2].Rows[i]["MAT_CODE"] = EITable.Tables[2].Rows[i - 1]["MAT_CODE"];
					bcls_ret->Tables[2].Rows[i]["MAT_TYPE"] = EITable.Tables[2].Rows[i - 1]["MAT_TYPE"];
					bcls_ret->Tables[2].Rows[i]["MAT_NAME"] = EITable.Tables[2].Rows[i - 1]["MAT_NAME"];
					bcls_ret->Tables[2].Rows[i]["STOCK_WT"] = EITable.Tables[2].Rows[i - 1]["STOCK_WT"];
					bcls_ret->Tables[2].Rows[i]["RATE"] = EITable.Tables[2].Rows[i - 1]["RATE"];
				}
			}
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", 44444);

		if (EITable.Tables[3].Rows.get_Count() > 0)
		{
			bcls_ret->Tables[3].Clone(EITable.Tables[3]);
			for (int i = 0; i < EITable.Tables[3].Rows.get_Count() + 1; i++)
			{
				bcls_ret->Tables[3].Rows.Add();
				if (i < 1)
				{
					if (i == 0)
					{
						bcls_ret->Tables[3].Rows[0]["BUNKER_NO"] = " ";
						bcls_ret->Tables[3].Rows[0]["BUNKER_NAME"] = " ";
						bcls_ret->Tables[3].Rows[0]["BUNKER_TYPE"] = " ";
						bcls_ret->Tables[3].Rows[0]["MAT_CODE"] = " ";
						bcls_ret->Tables[3].Rows[0]["MAT_NAME"] = " ";
						bcls_ret->Tables[3].Rows[0]["MAT_TYPE"] = " ";
						bcls_ret->Tables[3].Rows[0]["STOCK_WT"] = 0;
						bcls_ret->Tables[3].Rows[0]["RATE"] = 0;

					}
				}
				else
				{
					bcls_ret->Tables[3].Rows[i]["BUNKER_NO"] = EITable.Tables[3].Rows[i - 1]["BUNKER_NO"];
					bcls_ret->Tables[3].Rows[i]["BUNKER_NAME"] = EITable.Tables[3].Rows[i - 1]["BUNKER_NAME"];
					bcls_ret->Tables[3].Rows[i]["BUNKER_TYPE"] = EITable.Tables[3].Rows[i - 1]["BUNKER_TYPE"];
					bcls_ret->Tables[3].Rows[i]["MAT_CODE"] = EITable.Tables[3].Rows[i - 1]["MAT_CODE"];
					bcls_ret->Tables[3].Rows[i]["MAT_TYPE"] = EITable.Tables[3].Rows[i - 1]["MAT_TYPE"];
					bcls_ret->Tables[3].Rows[i]["MAT_NAME"] = EITable.Tables[3].Rows[i - 1]["MAT_NAME"];
					bcls_ret->Tables[3].Rows[i]["STOCK_WT"] = EITable.Tables[3].Rows[i - 1]["STOCK_WT"];
					bcls_ret->Tables[3].Rows[i]["RATE"] = EITable.Tables[3].Rows[i - 1]["RATE"];
				}
			}
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", 55555);

		if (EITable.Tables[4].Rows.get_Count() > 0)
		{
			bcls_ret->Tables[4].Clone(EITable.Tables[4]);
			for (int i = 0; i < EITable.Tables[4].Rows.get_Count() + 1; i++)
			{
				bcls_ret->Tables[4].Rows.Add();
				if (i < 1)
				{
					if (i == 0)
					{
						bcls_ret->Tables[4].Rows[0]["BUNKER_NO"] = " ";
						bcls_ret->Tables[4].Rows[0]["BUNKER_NAME"] = " ";
						bcls_ret->Tables[4].Rows[0]["BUNKER_TYPE"] = " ";
						bcls_ret->Tables[4].Rows[0]["MAT_CODE"] = " ";
						bcls_ret->Tables[4].Rows[0]["MAT_NAME"] = " ";
						bcls_ret->Tables[4].Rows[0]["MAT_TYPE"] = " ";
						bcls_ret->Tables[4].Rows[0]["STOCK_WT"] = 0;
						bcls_ret->Tables[4].Rows[0]["RATE"] = 0;

					}
				}
				else
				{
					bcls_ret->Tables[4].Rows[i]["BUNKER_NO"] = EITable.Tables[4].Rows[i - 1]["BUNKER_NO"];
					bcls_ret->Tables[4].Rows[i]["BUNKER_NAME"] = EITable.Tables[4].Rows[i - 1]["BUNKER_NAME"];
					bcls_ret->Tables[4].Rows[i]["BUNKER_TYPE"] = EITable.Tables[4].Rows[i - 1]["BUNKER_TYPE"];
					bcls_ret->Tables[4].Rows[i]["MAT_CODE"] = EITable.Tables[4].Rows[i - 1]["MAT_CODE"];
					bcls_ret->Tables[4].Rows[i]["MAT_TYPE"] = EITable.Tables[4].Rows[i - 1]["MAT_TYPE"];
					bcls_ret->Tables[4].Rows[i]["MAT_NAME"] = EITable.Tables[4].Rows[i - 1]["MAT_NAME"];
					bcls_ret->Tables[4].Rows[i]["STOCK_WT"] = EITable.Tables[4].Rows[i - 1]["STOCK_WT"];
					bcls_ret->Tables[4].Rows[i]["RATE"] = EITable.Tables[4].Rows[i - 1]["RATE"];
				}
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
