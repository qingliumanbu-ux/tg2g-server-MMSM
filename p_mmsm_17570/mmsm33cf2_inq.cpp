/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-05-25
Description: 板坯切断炉次查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/





/* ***** 静态函数申明 ***** */


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// CC实绩查询
/// <para>
/// 1.根据时间范围,炉号等条件进行CC实绩查询。
///
/// </para>
/// <para>数据库表：TMMSM31(CC炉次实绩表)          </para>
/// <para>主调用函数：前台MMSM31画面F2(查询)按钮         </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns> CC实绩 </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm33cf2_inq)

int f_mmsm33cf2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	

	int		TotalRecordCount = 0;
	CString v_mat_no = "";
	CString v_archive_flag = "";//记录类型   T  在线数据    H 历史数据
	CString v_mat_destion = "";//材料去向
	CString v_mat_status = "";//材料状态
	CString v_pono = "";//制造命令号
	CString v_start_time = "";//开始时间
	CString v_end_time = "";//结束时间
	CString v_heat_no = "";//熔炼号
	CString v_order_no = "";//合同号
	CString v_stock_no = "";//库区号
	CString v_in_flag = "";//入库标记
	CString v_st_no = "";//出钢记号
	CString v_product_flag = "";//成品标记
	CString v_table_type = "";//表名

	CString sqlstr = "";
	CString sqlstr_temp = "";
	CString sqlstr_grup = "";

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm01("TMMSM01");


	CDbCommand cmd_inq(conn);


	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().ToUpper().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			v_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().ToUpper().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().ToUpper().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().ToUpper().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
			v_table_type = bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"].ToString().Trim();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			
			if (v_table_type == "TMMSM38"){
				sqlstr = " select * from TMMSM38 WHERE 1=1 ";
				if (v_mat_no != "")
				{
					sqlstr_temp += " and  MAT_NO = '" + v_mat_no + "'";
				}

				if (v_pono != "")
				{
					sqlstr_temp += " and  PONO = '" + v_pono + "'";
				}
				if (v_start_time != "")
				{
					sqlstr_temp += " and  PROD_TIME >= '" + v_start_time + "'";
				}
				if (v_end_time != "")
				{
					sqlstr_temp += " and  PROD_TIME <= '" + v_end_time + "'";
				}
				if (v_heat_no != "")
				{
					sqlstr_temp += " and  HEAT_NO = '" + v_heat_no + "'";
				}
				if (v_table_type == "TMMSM39")
				{
					sqlstr_temp += " and  USE_LOGO <> '1'";
				}

				if (v_table_type == "TMMSM96")
				{
					sqlstr_temp += " AND EVENT_ID  = 'MM33' ";
				}

			}
			else if (v_table_type == "TMMSM39"){
				sqlstr = " select T1.ST_NO, sum(T1.CUT_SCRAP_WT) - max(NVL(T2.MAT_ACT_WT,0)) AS CUT_SCRAP_WT "
					" from TMMSM39 T1 "
					" LEFT JOIN (select sum(MAT_ACT_WT) MAT_ACT_WT,ST_NO  from tmmsm38 group by ST_NO) T2 ON T1.ST_NO = T2.ST_NO ";
				if (v_mat_no != "")
				{
					sqlstr_temp += " and  T1.MAT_NO = '" + v_mat_no + "'";
				}

				if (v_pono != "")
				{
					sqlstr_temp += " and  T1.PONO = '" + v_pono + "'";
				}
				if (v_start_time != "")
				{
					sqlstr_temp += " and  T1.PROD_TIME >= '" + v_start_time + "'";
				}
				if (v_end_time != "")
				{
					sqlstr_temp += " and  T1.PROD_TIME <= '" + v_end_time + "'";
				}
				if (v_heat_no != "")
				{
					sqlstr_temp += " and  T1.HEAT_NO = '" + v_heat_no + "'";
				}
				if (v_table_type == "TMMSM39")
				{
					sqlstr_temp += " and  T1.USE_LOGO <> '1'";
				}

				if (v_table_type == "TMMSM96")
				{
					sqlstr_temp += " AND T1.EVENT_ID  = 'MM33' ";
				}

				sqlstr_grup = " GROUP BY T1.ST_NO ";
			}


			sqlstr=sqlstr+sqlstr_temp+sqlstr_grup;

			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		}
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();
	

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
