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
BM2F_ENTERACE(mmsm81fg_inq)

int f_mmsm81fg_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int count = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString cs_flag = "";

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
		//获取传入参数
		tmmsm60.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		cs_flag = bcls_rec->Tables[0].Rows[0]["CS_FLAG"].ToString();
		Log::Info("", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm60["BUNKER_NO"].ToString());
		Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm60["MAT_CODE"].ToString());
		
		if (cs_flag =="J")
		{
			sqlstr = " SELECT decode(t.MAT_CODE, '" + tmmsm60["MAT_CODE"].ToString().Replace("	", "").Trim() + "', 0, 1)  AS  FLAG,BUNKER_NO ,MAT_CODE,MAT_NAME,(SELECT MAT_CODE_L2 FROM TMMSM50 WHERE MAT_CODE = T.MAT_CODE) MAT_CODE_L2 ,STOCK_WT,BUNKER_NAME ,BUNKER_TYPE ,MAT_TYPE,BASE_NAME,BACK_C5,STOCK_WT_WARN ,GM_VALUE,UPPER_LIMIT_VALUE,STK_NO FROM TMMSM60 T WHERE BACK_C4 = 'MMSM518S2N' AND BACK_N_1  <= 20 ORDER BY BACK_N_1 ASC ";
		}
		else if (cs_flag == "K")
		{
			sqlstr = " SELECT decode(t.MAT_CODE, '" + tmmsm60["MAT_CODE"].ToString().Replace("	", "").Trim() + "', 0, 1)  AS  FLAG,BUNKER_NO ,MAT_CODE,MAT_NAME,(SELECT MAT_CODE_L2 FROM TMMSM50 WHERE MAT_CODE = T.MAT_CODE) MAT_CODE_L2 ,STOCK_WT,BUNKER_NAME ,BUNKER_TYPE ,MAT_TYPE,BASE_NAME,BACK_C5,STOCK_WT_WARN ,GM_VALUE,UPPER_LIMIT_VALUE,STK_NO FROM TMMSM60 T WHERE BACK_C4 = 'MMSM518S2N' AND BACK_N_1 > 20  AND BACK_N_1 <= 92     ORDER BY BACK_N_1 ASC ";
		}
		else if (cs_flag == "V")
		{
			sqlstr = " SELECT decode(t.MAT_CODE, '" + tmmsm60["MAT_CODE"].ToString().Replace("	", "").Trim() + "', 0, 1)  AS  FLAG,BUNKER_NO ,MAT_CODE,MAT_NAME,(SELECT MAT_CODE_L2 FROM TMMSM50 WHERE MAT_CODE = T.MAT_CODE) MAT_CODE_L2 ,STOCK_WT,BUNKER_NAME ,BUNKER_TYPE ,MAT_TYPE,BASE_NAME,BACK_C5,STOCK_WT_WARN ,GM_VALUE,UPPER_LIMIT_VALUE,STK_NO FROM TMMSM60 T WHERE BACK_C4 = 'MMSM518S2N' AND BACK_N_1 > 160  AND BACK_N_1 <= 187     ORDER BY BACK_N_1 ASC ";
		}
		else if (cs_flag == "M")
		{
			sqlstr = " SELECT decode(t.MAT_CODE, '" + tmmsm60["MAT_CODE"].ToString().Replace("	", "").Trim() + "', 0, 1)  AS  FLAG,BUNKER_NO ,MAT_CODE,MAT_NAME,(SELECT MAT_CODE_L2 FROM TMMSM50 WHERE MAT_CODE = T.MAT_CODE) MAT_CODE_L2 ,STOCK_WT,BUNKER_NAME ,BUNKER_TYPE ,MAT_TYPE,BASE_NAME,BACK_C5,STOCK_WT_WARN ,GM_VALUE,UPPER_LIMIT_VALUE,STK_NO FROM TMMSM60 T WHERE BACK_C4 = 'MMSM518S2N' AND BACK_N_1 IN ('123','124','125','126','127','128')    ORDER BY BACK_N_1 ASC ";
		}
		else if (cs_flag == "DX")
		{
			sqlstr = " SELECT decode(t.MAT_CODE, '" + tmmsm60["MAT_CODE"].ToString().Replace("	", "").Trim() + "', 0, 1)  AS  FLAG,BUNKER_NO ,MAT_CODE,MAT_NAME,(SELECT MAT_CODE_L2 FROM TMMSM50 WHERE MAT_CODE = T.MAT_CODE) MAT_CODE_L2 ,STOCK_WT,BUNKER_TYPE,BUNKER_NAME  ,MAT_TYPE,BASE_NAME,BACK_C5,STOCK_WT_WARN ,GM_VALUE,UPPER_LIMIT_VALUE,STK_NO FROM TMMSM60 T WHERE BUNKER_NO LIKE 'CL%'   ORDER BY BUNKER_NO ASC ";
		}
		else if (cs_flag == "BJ")
		{
			//料仓报警
			sqlstr = " SELECT ROUND(CASE WHEN STOCK_WT = 0 OR UPPER_LIMIT_VALUE = 0 THEN 0 else STOCK_WT/UPPER_LIMIT_VALUE END,12)*100 BL_WT ,CASE WHEN STOCK_WT < STOCK_WT_WARN THEN 'X' WHEN STOCK_WT > STOCK_WT_WARN THEN 'D' ELSE ' ' END BAOJI,100-ROUND(CASE WHEN STOCK_WT = 0 OR UPPER_LIMIT_VALUE = 0 THEN 0 else (STOCK_WT/UPPER_LIMIT_VALUE)*100 END,12) BL_WT1, decode(t.MAT_CODE, '" + tmmsm60["MAT_CODE"].ToString().Replace("	", "").Trim() + "', 0, 1)  AS  FLAG, BUNKER_NO, MAT_CODE, MAT_NAME, (SELECT MAT_CODE_L2 FROM TMMSM50 WHERE MAT_CODE = T.MAT_CODE) MAT_CODE_L2, STOCK_WT, BUNKER_TYPE, BUNKER_NAME, MAT_TYPE, BASE_NAME, BACK_C5, STOCK_WT_WARN, GM_VALUE, UPPER_LIMIT_VALUE,STK_NO FROM TMMSM60 T WHERE LENGTH(BUNKER_TYPE) = 2   ORDER BY BUNKER_NO ASC ";
		}
		else if (cs_flag == "HCK")
		{
			//料仓报警
			sqlstr = " SELECT ROUND(CASE WHEN STOCK_WT = 0 OR UPPER_LIMIT_VALUE = 0 THEN 0 else STOCK_WT/UPPER_LIMIT_VALUE END,12)*100 BL_WT ,CASE WHEN STOCK_WT < STOCK_WT_WARN THEN 'X' WHEN STOCK_WT > STOCK_WT_WARN THEN 'D' ELSE ' ' END BAOJI,100-BACK_N_2 BL_WT1, decode(t.MAT_CODE, '" + tmmsm60["MAT_CODE"].ToString().Replace("	", "").Trim() + "', 0, 1)  AS  FLAG, BUNKER_NO, MAT_CODE, MAT_NAME, (SELECT MAT_CODE_L2 FROM TMMSM50 WHERE MAT_CODE = T.MAT_CODE) MAT_CODE_L2, STOCK_WT, BUNKER_TYPE, BUNKER_NAME, MAT_TYPE, BASE_NAME, BACK_C5, STOCK_WT_WARN, GM_VALUE, UPPER_LIMIT_VALUE,STK_NO FROM TMMSM60 T WHERE BUNKER_TYPE = 'TRAIN'   ORDER BY BUNKER_NO ASC ";
		}

		//cmd_inq.SetCommandText(sqlstr_count);
		//TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		if (cs_flag == "BJ")
		{
			for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
			{
				if (bcls_ret->Tables[0].Rows[i]["BL_WT"].ToDecimal()>100)
				{
					bcls_ret->Tables[0].Rows[i]["BL_WT"] = 100;
					bcls_ret->Tables[0].Rows[i]["BL_WT1"] = 0;
				}
			}
		}
		Log::Info("", __FUNCTION__, "bcls_ret->Tables[0].Rows =[{0}]", bcls_ret->Tables[0].Rows.get_Count());

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