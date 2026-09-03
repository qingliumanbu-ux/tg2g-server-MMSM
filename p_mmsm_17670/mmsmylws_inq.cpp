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
BM2F_ENTERACE(mmsmylws_inq)

int f_mmsmylws_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	EIClass EITable;
	EITable.Clear();
	EITable.Tables.Add();
	CDecimal zpl_devo_wt_2 = 0; // 中频炉(碳钢)
	CDecimal dl_devo_wt_2 = 0; // 电炉(碳钢)
	CDecimal bof_devo_wt_2 = 0; // 转炉(碳钢)
	CDecimal rh_devo_wt_2 = 0; // RH(碳钢)
	CDecimal lf_devo_wt_2 = 0; // LF(碳钢)
	CDecimal sum_devo_wt_2 = 0; // 合计(碳钢)
	CDecimal sum_devo_wt_1 = 0; // 合计(不锈钢)
	CDecimal cy_devo_wt_2 = 0; // 库存和合计的差异(碳钢)
	CDecimal lf_devo_wt_1 = 0;  // LF(不锈钢)
	CDecimal lts_devo_wt_1 = 0; // LTS(不锈钢)
	CDecimal rh_devo_wt_1 = 0;	// RH(不锈钢)
	CDecimal aod_devo_wt_1 = 0; // AOD(不锈钢)
	CDecimal dl_devo_wt_1 = 0;  //电路(不锈钢)
	CDecimal vod_devo_wt_1 = 0;	//VOD(不锈钢)
	CDecimal zpl_devo_wt_1 = 0;	//中频炉(不锈钢)	

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsmws("TMMSMWS");

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
		tmmsmws.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		
		if (tmmsmws["DATE_C"].ToString().GetLength()>= 6)
		{
			tmmsmws["DATE_C"] = tmmsmws["DATE_C"].ToString().SubstringNE(0, 6); 
		}
		Log::Info("", __FUNCTION__, "DATE_C =[{0}]", tmmsmws["DATE_C"].ToString());
		Log::Info("", __FUNCTION__, "MAT	_CODE =[{0}]", tmmsmws["MAT_CODE"].ToString());


		//自动导入上月期末作为 期初库存
		sqlstr = "SELECT * FROM TMMSMWS WHERE  DATE_C LIKE '%" + tmmsmws["DATE_C"].ToString() + "%'";



		//条件WHERE OPERATOR !='加工厂用户' AND TOSTK not like 'EP%'  AND TYPE IN (IN进厂，石灰进厂)
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = 
				" SELECT B.MAT_CODE,B.MAT_NAME, B.DATE_C,B.STOCK_INI_WT,B.IN_STOCK_WT,B.STOCK_WGT,B.OUT_STOCK_WT,"
				" B.STOCK_INI_WT + B.IN_STOCK_WT AS STOCK_END_WT, A.* FROM "
				" (SELECT  STAT_DATE AS DATE_C,MAT_CODE,MAX(MAT_NAME) AS MAT_NAME, SUM(STOCK_WT_QC) AS STOCK_INI_WT, SUM(IN_STOCK_WT2) AS IN_STOCK_WT ,"
				"        SUM(OUT_STOCK_WT2) AS OUT_STOCK_WT,SUM(STOCK_WT) AS STOCK_WGT,SUM( STOCK_WT_QC + IN_STOCK_WT2)  AS STOCK_END_WT FROM"
				"(SELECT MAT_CODE,MAT_NAME,STAT_DATE,STOCK_WT_QC, (IN_STOCK_WT2+IN_STOCK_WT3+IN_STOCK_WT5+IN_STOCK_WT6+IN_STOCK_WT9+IN_STOCK_WT10+IN_STOCK_WT12+IN_STOCK_WT14+IN_STOCK_WT16 ) IN_STOCK_WT2,OUT_STOCK_WT2,STOCK_WT FROM TMMSM57C WHERE STOCK_CODE='6241' AND  exists   (SELECT 1 FROM TMMSM50  where MAT_CODE = TMMSM57C.MAT_CODE  and SYSTEM_ID_MAT='C' ) UNION"
				" SELECT MAT_CODE,MAT_NAME,STAT_DATE,STOCK_WT_QC,IN_STOCK_WT,OUT_STOCK_WT1,STOCK_WT FROM TMMSM57D where exists   (SELECT 1 FROM TMMSM50  where MAT_CODE = TMMSM57D.MAT_CODE  and SYSTEM_ID_MAT='B' )) GROUP BY STAT_DATE,MAT_CODE ) "
				" B LEFT JOIN"
				"( SELECT MAT_CODE,MAX(MAT_NAME) MAT_NAME,STAT_DATE,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'B' AND SUBSTR(ST_NO,1,1) IN ('2','4')  THEN DEVO_WT ELSE 0 END)/1000,4) BOF_DEVO_WT_2,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'R' AND SUBSTR(ST_NO,1,1) IN ('2','4') THEN DEVO_WT ELSE 0 END)/1000,4) RH_DEVO_WT_2,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'F' AND SUBSTR(ST_NO,1,1) IN ('2','4') THEN DEVO_WT ELSE 0 END)/1000,4) LF_DEVO_WT_2,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'Z' AND SUBSTR(ST_NO,1,1) IN ('2','4') THEN DEVO_WT ELSE 0 END)/1000,4) ZPL_DEVO_WT_2,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'E' AND SUBSTR(ST_NO,1,1) IN ('2','4') THEN DEVO_WT ELSE 0 END)/1000,4) DL_DEVO_WT_2,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) IN('Z', 'E', 'B', 'R', 'F') AND SUBSTR(ST_NO,1,1) IN ('2','4') THEN DEVO_WT ELSE 0 END)/1000,4) SUM_DEVO_WT_2,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'F' AND SUBSTR(ST_NO,1,1) IN ('1','3','5')  THEN DEVO_WT ELSE 0 END)/1000,4) LF_DEVO_WT_1,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'S' AND SUBSTR(ST_NO,1,1) IN ('1','3','5')  THEN DEVO_WT ELSE 0 END)/1000,4) LTS_DEVO_WT_1,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'R' AND SUBSTR(ST_NO,1,1) IN ('1','3','5')  THEN DEVO_WT ELSE 0 END)/1000,4) RH_DEVO_WT_1,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'A' AND SUBSTR(ST_NO,1,1) IN ('1','3','5')  THEN DEVO_WT ELSE 0 END)/1000,4) AOD_DEVO_WT_1,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'B' AND SUBSTR(ST_NO,1,1) IN ('1','3','5')  THEN DEVO_WT ELSE 0 END)/1000,4) TL_DEVO_WT_1,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'E' AND SUBSTR(ST_NO,1,1) IN ('1','3','5')  THEN DEVO_WT ELSE 0 END)/1000,4) DL_DEVO_WT_1,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'V' AND SUBSTR(ST_NO,1,1) IN ('1','3','5')  THEN DEVO_WT ELSE 0 END)/1000,4) VOD_DEVO_WT_1,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) = 'Z' AND SUBSTR(ST_NO,1,1) IN ('1','3','5')  THEN DEVO_WT ELSE 0 END)/1000,4) ZPL_DEVO_WT_1,"
				"  ROUND(SUM(CASE WHEN SUBSTR(DEV_CODE, 1, 1) IN('F', 'S', 'R', 'A','B','E', 'V', 'Z') AND SUBSTR(ST_NO,1,1) IN ('1','3','5')  THEN DEVO_WT ELSE 0 END)/1000,4) SUM_DEVO_WT_1,"
				"  ROUND(SUM(DEVO_WT)/1000,4) FROM TMMSM2A_SEND WHERE RTN_FLAG<>'1' AND SEND_FLAG='1'  "
				" GROUP BY  MAT_CODE,STAT_DATE) A ON A.MAT_CODE = B.MAT_CODE AND A.STAT_DATE=B.DATE_C WHERE 1=1  AND  B.MAT_CODE<>' ' ";



			if (tmmsmws["DATE_C"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND DATE_C = '" + tmmsmws["DATE_C"].ToString() + "'";
			}
			if (tmmsmws["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND B.MAT_CODE LIKE '%" + tmmsmws["MAT_CODE"].ToString()+"%'";
			}
			sqlstr = sqlstr + sqlstr_temp;
			sqlstr_count = "SELECT COUNT(1) FROM (" + sqlstr+")";
			break;
		}

		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);


		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取

		// ZPL_DEVO_WT_2 DL_DEVO_WT_2 BOF_DEVO_WT_2 RH_DEVO_WT_2 LF_DEVO_WT_2 SUM_DEVO_WT_2
		Log::Trace(" ", __FUNCTION__, "sqlstr={[0]}", sqlstr);
		cmd_inq.SetCommandText(sqlstr);

		cmd_inq.ExecuteQuery(EITable.Tables[0]);
		cmd_inq.Close();
		bcls_ret->Tables[0].Copy(EITable.Tables[0]);
		if (EITable.Tables[0].Rows.get_Count()>0)
		{
			for (int i = 0; i < EITable.Tables[0].Rows.get_Count(); i++)
			{
				if (EITable.Tables[0].Rows[i]["OUT_STOCK_WT"].ToDecimal()>0 )
				{
					/*if (EITable.Tables[0].Rows[i]["OUT_STOCK_WT"].ToDecimal() - EITable.Tables[0].Rows[i]["SUM_DEVO_WT_2"].ToDecimal() - EITable.Tables[0].Rows[i]["SUM_DEVO_WT_1"].ToDecimal() != 0)
					{
						zpl_devo_wt_2 = EITable.Tables[0].Rows[i]["ZPL_DEVO_WT_2"].ToDecimal();
						dl_devo_wt_2 = EITable.Tables[0].Rows[i]["DL_DEVO_WT_2"].ToDecimal();
						bof_devo_wt_2 = EITable.Tables[0].Rows[i]["BOF_DEVO_WT_2"].ToDecimal();
						rh_devo_wt_2 = EITable.Tables[0].Rows[i]["RH_DEVO_WT_2"].ToDecimal();
						lf_devo_wt_2 = EITable.Tables[0].Rows[i]["LF_DEVO_WT_2"].ToDecimal();
						sum_devo_wt_2 = EITable.Tables[0].Rows[i]["SUM_DEVO_WT_2"].ToDecimal();
						sum_devo_wt_1 = EITable.Tables[0].Rows[i]["SUM_DEVO_WT_1"].ToDecimal();
						
						lf_devo_wt_1 = EITable.Tables[0].Rows[i]["LF_DEVO_WT_1"].ToDecimal();
						lts_devo_wt_1 = EITable.Tables[0].Rows[i]["LTS_DEVO_WT_1"].ToDecimal();
						rh_devo_wt_1 = EITable.Tables[0].Rows[i]["RH_DEVO_WT_1"].ToDecimal();
						aod_devo_wt_1 = EITable.Tables[0].Rows[i]["AOD_DEVO_WT_1"].ToDecimal();
						dl_devo_wt_1 = EITable.Tables[0].Rows[i]["DL_DEVO_WT_1"].ToDecimal();
						vod_devo_wt_1 = EITable.Tables[0].Rows[i]["VOD_DEVO_WT_1"].ToDecimal();
						zpl_devo_wt_1 = EITable.Tables[0].Rows[i]["ZPL_DEVO_WT_1"].ToDecimal();

						cy_devo_wt_2 = EITable.Tables[0].Rows[i]["STOCK_WGT"].ToDecimal() - EITable.Tables[0].Rows[i]["SUM_DEVO_WT_2"].ToDecimal() - EITable.Tables[0].Rows[i]["SUM_DEVO_WT_1"].ToDecimal();
						if (cy_devo_wt_2 == 0) return 0;
						if (zpl_devo_wt_2 != 0 && sum_devo_wt_2 + sum_devo_wt_1!=0)
						{
							zpl_devo_wt_2 = zpl_devo_wt_2 / (sum_devo_wt_2 + sum_devo_wt_1) * cy_devo_wt_2 + zpl_devo_wt_2;
							bcls_ret->Tables[0].Rows[i]["ZPL_DEVO_WT_2"] = zpl_devo_wt_2;
						}
						if (dl_devo_wt_2 != 0 && sum_devo_wt_2 + sum_devo_wt_1 != 0)
						{
							dl_devo_wt_2 = dl_devo_wt_2 / (sum_devo_wt_2 + sum_devo_wt_1) * cy_devo_wt_2 + dl_devo_wt_2;
							bcls_ret->Tables[0].Rows[i]["DL_DEVO_WT_2"] = dl_devo_wt_2;
						}
						if (bof_devo_wt_2 != 0 && sum_devo_wt_2 + sum_devo_wt_1 != 0)
						{
							bof_devo_wt_2 = bof_devo_wt_2 / (sum_devo_wt_2 + sum_devo_wt_1) * cy_devo_wt_2 + bof_devo_wt_2;
							bcls_ret->Tables[0].Rows[i]["BOF_DEVO_WT_2"] = bof_devo_wt_2;
						}
						if (rh_devo_wt_2 != 0 && sum_devo_wt_2 + sum_devo_wt_1 != 0)
						{
							rh_devo_wt_2 = rh_devo_wt_2 / (sum_devo_wt_2 + sum_devo_wt_1) * cy_devo_wt_2 + rh_devo_wt_2;
							bcls_ret->Tables[0].Rows[i]["RH_DEVO_WT_2"] = rh_devo_wt_2;
						}
						if (lf_devo_wt_2 != 0 && sum_devo_wt_2 + sum_devo_wt_1 != 0)
						{
							lf_devo_wt_2 = lf_devo_wt_2 / (sum_devo_wt_2 + sum_devo_wt_1) * cy_devo_wt_2 + lf_devo_wt_2;
							bcls_ret->Tables[0].Rows[i]["LF_DEVO_WT_2"] = lf_devo_wt_2;
						}


						if (lf_devo_wt_1 != 0 && sum_devo_wt_2 + sum_devo_wt_1 != 0)
						{
							lf_devo_wt_1 = lf_devo_wt_1 / (sum_devo_wt_2 + sum_devo_wt_1) * cy_devo_wt_2 + lf_devo_wt_1;
							bcls_ret->Tables[0].Rows[i]["LF_DEVO_WT_1"] = lf_devo_wt_1;
						}
						if (lts_devo_wt_1 != 0 && sum_devo_wt_2 + sum_devo_wt_1 != 0)
						{
							lts_devo_wt_1 = lts_devo_wt_1 / (sum_devo_wt_2 + sum_devo_wt_1) * cy_devo_wt_2 + lts_devo_wt_1;
							bcls_ret->Tables[0].Rows[i]["LTS_DEVO_WT_1"] = lts_devo_wt_1;
						}
						if (rh_devo_wt_1 != 0 && sum_devo_wt_2 + sum_devo_wt_1 != 0)
						{
							rh_devo_wt_1 = rh_devo_wt_1 / (sum_devo_wt_2 + sum_devo_wt_1) * cy_devo_wt_2 + rh_devo_wt_1;
							bcls_ret->Tables[0].Rows[i]["RH_DEVO_WT_1"] = rh_devo_wt_1;
						}
						if (aod_devo_wt_1 != 0 && sum_devo_wt_2 + sum_devo_wt_1 != 0)
						{
							aod_devo_wt_1 = aod_devo_wt_1 / (sum_devo_wt_2 + sum_devo_wt_1) * cy_devo_wt_2 + aod_devo_wt_1;
							bcls_ret->Tables[0].Rows[i]["AOD_DEVO_WT_1"] = aod_devo_wt_1;
						}
						if (dl_devo_wt_1 != 0 && sum_devo_wt_2 + sum_devo_wt_1 != 0)
						{
							dl_devo_wt_1 = dl_devo_wt_1 / (sum_devo_wt_2 + sum_devo_wt_1) * cy_devo_wt_2 + dl_devo_wt_1;
							bcls_ret->Tables[0].Rows[i]["DL_DEVO_WT_1"] = dl_devo_wt_1;
						}
						if (vod_devo_wt_1 != 0 && sum_devo_wt_2 + sum_devo_wt_1 != 0)
						{
							vod_devo_wt_1 = vod_devo_wt_1 / (sum_devo_wt_2 + sum_devo_wt_1) * cy_devo_wt_2 + vod_devo_wt_1;
							bcls_ret->Tables[0].Rows[i]["VOD_DEVO_WT_1"] = vod_devo_wt_1;
						}
						if (zpl_devo_wt_1 != 0 && sum_devo_wt_2 + sum_devo_wt_1 != 0)
						{
							zpl_devo_wt_1 = zpl_devo_wt_1 / (sum_devo_wt_2 + sum_devo_wt_1) * cy_devo_wt_2 + zpl_devo_wt_1;
							bcls_ret->Tables[0].Rows[i]["ZPL_DEVO_WT_1"] = zpl_devo_wt_1;
						}

					}*/
				}
			}
		}
		

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

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