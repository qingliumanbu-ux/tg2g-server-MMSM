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
BM2F_ENTERACE(mmsmws_inq)

int f_mmsmws_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	CString tabFlag = "";
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm57c("TMMSM57C");

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
		tmmsm57c.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (bcls_rec->Tables[0].Columns.Contains("tabFlag"))
		{
			tabFlag = bcls_rec->Tables[0].Rows[0]["tabFlag"];
		}
		Log::Trace("", __FUNCTION__, "tabFlag[{0}]  ", tabFlag);
		tmmsm57c["STAT_DATE"] = tmmsm57c["STAT_DATE"].ToString().SubstringNE(0, 6);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			if (tabFlag == "3")
			{
				sqlstr = " SELECT STAT_DATE AS DATE_C,MAT_CODE,MAX(MAT_NAME) AS MAT_NAME,'LG1' AS FACTORY_DIV,'6241' AS STORE_PLACE,'TON' AS  UNIT,SUM(DEVO_WT/1000)  DEVO_WT"
					" ,sum(case when  substr(st_no,1,1) in ('2','3','5') then DEVO_WT/1000 else 0 end)  tz_wt_1  "
					" ,sum(case when  substr(st_no,1,1) not in ('2','3','5') then DEVO_WT/1000 else 0 end)  tz_wt_2  "
					" FROM tmmsm2a_send   WHERE HANDLE_DIV='F' and SEND_FLAG = '1' and  RTN_FLAG != '1' "
					" AND STAT_DATE = @stat_date"
					;
				if (tmmsm57c["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr += " AND MAT_CODE		like '%'|| @MAT_CODE||'%'";
				}
				if (tmmsm57c["MAT_NAME"].ToString().Trim() != "")
				{
					sqlstr += " AND MAT_NAME	like '%'|| @MAT_NAME||'%'";
				}
				sqlstr += " GROUP BY STAT_DATE,MAT_CODE  ORDER BY MAT_CODE ";
			
			}
			else if (tabFlag == "hc")
			{
				sqlstr = 
					" SELECT MAT_CODE,MAX(MAT_NAME) MAT_NAME ,SUM(STOCK_WT)  STOCK_INI_WT FROM TMMSM60"
					" WHERE BUNKER_TYPE='TRAIN' "
					;

				if (tmmsm57c["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr += " AND MAT_CODE LIKE '%" + tmmsm57c["MAT_CODE"].ToString() + "%'";
				}
				if (tmmsm57c["MAT_NAME"].ToString().Trim() != "")
				{
					sqlstr += " AND MAT_NAME LIKE '%" + tmmsm57c["MAT_NAME"].ToString() + "%'";
				}
				sqlstr += " GROUP BY MAT_CODE  ORDER BY MAT_CODE ";
			}
			else if (tabFlag == "ge")
			{
				sqlstr =
					" SELECT MAT_CODE,MAX(MAT_NAME) MAT_NAME ,SUM(STOCK_WT)  STOCK_INI_WT FROM TMMSM60"
					" WHERE BUNKER_TYPE='SCRAPALLOY' AND  BUNKER_NO NOT IN('E19','E20','E21','E22','F19','F20','F21','F22')"
				    ;  
				if (tmmsm57c["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr += " AND MAT_CODE LIKE '%" + tmmsm57c["MAT_CODE"].ToString() + "%'";
				}
				if (tmmsm57c["MAT_NAME"].ToString().Trim() != "")
				{
					sqlstr += " AND MAT_NAME LIKE '%" + tmmsm57c["MAT_NAME"].ToString() + "%'";
				}
				sqlstr += " GROUP BY MAT_CODE  ORDER BY MAT_CODE ";
			}
			else
			{
				sqlstr = " SELECT  @stat_date AS DATE_C,MAT_CODE,MAX(MAT_NAME) AS MAT_NAME,'LG1' AS FACTORY_DIV,'6241' AS STORE_PLACE,'TON' AS  UNIT,"
					" SUM(IN_STOCK_WT) AS STOCK_INI_WT, SUM(IN_STOCK_WT) AS IN_STOCK_WT ,"
					"  SUM(OUT_STOCK_WT) AS OUT_STOCK_WT,SUM(STOCK_WT) AS STOCK_WGT"
					", SUM(MES_WT) AS MES_WT, SUM(MES_WT_1) AS MES_WT_1, SUM(MES_WT_2) AS MES_WT_2"
					", SUM(OUT_STOCK_WT)-SUM(MES_WT) AS DIF_WT"
					" FROM ("
					"  SELECT MAT_CODE,MAT_NAME,STOCK_WT_QC, (IN_STOCK_WT2+IN_STOCK_WT3+IN_STOCK_WT5+IN_STOCK_WT6+IN_STOCK_WT9+IN_STOCK_WT10+IN_STOCK_WT12+IN_STOCK_WT14+IN_STOCK_WT16) IN_STOCK_WT,OUT_STOCK_WT2 AS OUT_STOCK_WT,STOCK_WT"
					" ,0 MES_WT,0 MES_wt_1 ,0 MES_wt_2"
					"  FROM TMMSM57C"
					"  WHERE STOCK_CODE='6241' "
					"  AND  MAT_CODE in  (SELECT MAT_CODE FROM TMMSM50  where SYSTEM_ID_MAT='C' ) "
					"  and STOCK_CODE='6241'  "
					"  and stat_date in ( select max(stat_date) from tmmsm57c where stat_date like  @stat_date||'%'  and STOCK_CODE='6241' ) "
					"  UNION ALL"
					"  SELECT MAT_CODE,MAT_NAME,STOCK_WT_QC,IN_STOCK_WT,OUT_STOCK_WT1 AS OUT_STOCK_WT,STOCK_WT "
					" ,0 MES_WT,0 MES_wt_1 ,0 MES_wt_2"
					" FROM TMMSM57D "
					" where MAT_CODE in  (SELECT MAT_CODE FROM TMMSM50  where  SYSTEM_ID_MAT='B' )"
					" and stat_date in(select max(stat_date) from tmmsm57d where stat_date like  @stat_date || '%') "
					"  UNION ALL"
					"  SELECT MAT_CODE,MAT_NAME,0 STOCK_WT_QC,0 IN_STOCK_WT,0 AS OUT_STOCK_WT,0 STOCK_WT "
					", SUM(DEVO_WT / 1000)  MES_WT	"
					" ,sum(case when  substr(st_no,1,1) in ('2','3','5') then DEVO_WT/1000 else 0 end)  MES_wt_1  "
					" ,sum(case when  substr(st_no,1,1) not in ('2','3','5') then DEVO_WT/1000 else 0 end)  MES_wt_2  "
					" FROM tmmsm2a_send "
					" WHERE  SEND_FLAG = '1' and  RTN_FLAG != '1' "
					" AND STAT_DATE = @stat_date "
					" GROUP BY MAT_CODE,MAT_NAME "
					" ) WHERE 1=1 "
					;
				if (tmmsm57c["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr += " AND MAT_CODE		like '%'|| @MAT_CODE||'%'";
				}
				if (tmmsm57c["MAT_NAME"].ToString().Trim() != "")
				{
					sqlstr += " AND MAT_NAME	like '%'|| @MAT_NAME||'%'";
				}

				sqlstr += " GROUP BY MAT_CODE  ORDER BY MAT_CODE ";
			}

			break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
		cmd_inq.Parameters.Set("stat_date", tmmsm57c["STAT_DATE"].ToString());
		cmd_inq.Parameters.Set("MAT_CODE", tmmsm57c["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("MAT_NAME", tmmsm57c["MAT_NAME"].ToString());  
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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
