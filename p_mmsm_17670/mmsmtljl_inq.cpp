/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料进料汇总查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmtljl_inq)

int f_mmsmtljl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int count = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString begin_time = "";
	CString end_time = "";
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	CString prod_shift_no = "";
	CString prod_shift_group = "";
	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm81("TMMSM81");
	CModel tmmsm89("TMMSM89");
	CModel tmmsm81_s("TMMSM81_S");
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
		tmmsm81.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();
		if (bcls_rec->Tables.Contains("PAGEINFO"))
		{
			if (bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_NUM") && bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_SIZE"))
			{
				current_page_no = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_NUM"].ToDecimal().ToInt32() + 1;
				record_count_per_page = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_SIZE"];
			}
			else {
				record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
				current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
			}
		}
		else {
			record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
			current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
		}
		Log::Info("", __FUNCTION__, "BEGIN_TIME =[{0}]", begin_time);
		Log::Info("", __FUNCTION__, "END_TIME =[{0}]", end_time);
		Log::Trace("", __FUNCTION__, "record_count_per_page	= [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "current_page_no			= [{0}]", current_page_no);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM tmmsm81 T "
				"  WHERE 1=1 "
				;
			sqlstr =" SELECT T.*,GREATEST(NVL(C,0),NVL(SI,0), NVL(MN,0),NVL(P,0),NVL(S,0),NVL(CR,0),NVL(NI,0),NVL(TI,0),NVL(AI,0),NVL(MO,0),NVL(CU,0),NVL(V,0),NVL(CO,0),NVL(PB,0),"
                    " NVL(SIC,0),NVL(SIO2,0),NVL(AL2O3,0),NVL(CAO,0),NVL(CAF2,0),NVL(CA,0),NVL(CR2O3,0),NVL(N,0),NVL(LN,0),NVL(W,0),NVL(NB,0),NVL(B,0),NVL(FE,0),NVL(CE,0),NVL(RE,0)) AS MAX_VALUE "
                    " FROM (SELECT A.WEIGH_NO,A.LOT_NO,A.MAT_NAME,A.STOCK_WT,A.MAT_RCV_TIME,A.PROD_SHIFT_GROUP,"
                    " A.REC_CREATOR,  SUBSTR(A.MAT_RCV_TIME, 0, 6) MON_DATA,SUBSTR(A.MAT_RCV_TIME, 0, 8) PROD_DATE, A.REC_REVISOR,A.BUNKER_NO,A.GROSS_WT,A.TARE_WT,A.NET_WT,A.RAW_WEIGHT,"
                    " A.BUCKLE_WT,A.DEDUCT_WGT, A.SECOND_NET_WT,A.FACTORY_DIV,A.SHIP_NAME, A.VEHICLE_NO,A.QUALITY_BATCH_NO,A.MAT_CODE,A.REMARK,A.I_BILLTYPE,A.MANUFAC_NAME,"
                    " DECODE(C.BUNKER_TYPE,'AUTO','汽车库A/B','TRAIN','火车库C/D/VT','TRAINN','火车库新扩VT','SCRAPALLOY','铬库E/F/VA','NICKEL','镍板库N') AS STOCK_ROOM ,"
                    " B.*  FROM "
					" (SELECT WEIGH_NO,LOT_NO,MAT_NAME,STOCK_WT,MAT_RCV_TIME,PROD_SHIFT_GROUP,REC_CREATOR,REC_REVISOR,BUNKER_NO,GROSS_WT,TARE_WT,NET_WT,"
					"        RAW_WEIGHT,BUCKLE_WT,DEDUCT_WGT,SECOND_NET_WT,FACTORY_DIV,SHIP_NAME,VEHICLE_NO,QUALITY_BATCH_NO,MAT_CODE,REMARK,I_BILLTYPE,MANUFAC_NAME FROM TMMSM81"
					" UNION SELECT"
					"       WEIGH_NO,LOT_NO,MAT_NAME,STOCK_WT,MAT_RCV_TIME,PROD_SHIFT_GROUP,REC_CREATOR,REC_REVISOR,BUNKER_NO,GROSS_WT,TARE_WT,NET_WT,"
					"       RAW_WEIGHT,BUCKLE_WT,DEDUCT_WGT,SECOND_NET_WT,FACTORY_DIV,SHIP_NAME,VEHICLE_NO,QUALITY_BATCH_NO,MAT_CODE,' ' as REMARK,0 as I_BILLTYPE, ' 'as MANUFAC_NAME FROM TMMSM81_S) "
					" A  LEFT JOIN"
                    " (SELECT  * FROM ( SELECT  QUALITY_BATCH_NO AS QUALITY_BATCH_NO1 ,MAT_CODE AS MAT_CODE1,ELM_NAME,ELM_VALUE FROM  TMMSM81AL ) PIVOT ( SUM(ELM_VALUE) FOR ELM_NAME IN"
                    " ( 'C' AS C ,'Si' AS SI, 'Mn' AS MN,'P' AS P, 'S' AS S,'Cr' AS CR ,'Ni' AS NI, 'Ti' AS TI,'Ai' AS AI, 'Mo' AS MO, 'Cu' AS CU ,'V' AS V, 'Co' AS CO,"
                    " 'Pb' AS PB, 'SiC' AS SIC,'SiO2' AS SiO2 ,'Al2O3' AS Al2O3, 'Ca0' AS CAO,'CaF2' AS CAF2, 'Ca' AS CA, 'Cr2O3' AS CR2O3 , 'N' AS N,'Ln' AS LN,"
                    " 'W' AS W,'Nb' AS NB ,'B' AS B, 'Fe' AS FE, 'Ce' AS CE,'Re' AS RE ) )) B ON  A.QUALITY_BATCH_NO=B.QUALITY_BATCH_NO1 AND A.MAT_CODE=B.MAT_CODE1"
                    " LEFT JOIN TMMSM60  C ON   A.BUNKER_NO=C.BUNKER_NO"
                    " ) T WHERE 1=1   ";

			if (begin_time.Trim() != "")
			{
				sqlstr_temp += " AND T.MAT_RCV_TIME>=@begin_time";
			}
			if (end_time.Trim() != "")
			{
				sqlstr_temp += " AND T.MAT_RCV_TIME<=@end_time";
			}
			if (tmmsm81["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND T.MAT_CODE LIKE '%" + tmmsm81["MAT_CODE"].ToString() + "%'";
			}
			if (tmmsm81["BUNKER_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND T.BUNKER_NO LIKE '%" + tmmsm81["BUNKER_NO"].ToString() + "%'";
			}
			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += "  ORDER BY T.MAT_CODE  ";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		
		
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("MAT_CODE", tmmsm81["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm81["BUNKER_NO"].ToString());
		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		//分页获取
		cmd_inq.SetCommandText(sqlstr);

		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();


		//刷班次班组
		/*sqlstr = "   SELECT EVENT_TIME,RESUME_SEQ_NO FROM TMMSM89  WHERE FUNC_ID='mmsm831_upd2' AND SUBSTR(BUNKER_NO,1,2)   IN ('AL','BL','EL') ORDER BY REC_CREATE_TIME DESC";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		Log::Info("", __FUNCTION__, "SW2 sqlstr =[{0}]", sqlstr);
		while (cmd_inq.Read())
		{
			tmmsm89.Reset();
			cmd_inq.Fetch(tmmsm89);
			if (tmmsm89["EVENT_TIME"].ToString().Trim() != "")
			{
				f_epep_get_shift_group("SMCP", tmmsm89["EVENT_TIME"].ToString(), prod_shift_no, prod_shift_group, conn);
		
			}
			tmmsm89["SHIFT_NO"] = prod_shift_no;
			tmmsm89["SHIFT_GROUP"] = prod_shift_group;
			tmmsm89.Update("SHIFT_NO,SHIFT_GROUP", "RESUME_SEQ_NO");
		}

		
		cmd_inq.Close();*/

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