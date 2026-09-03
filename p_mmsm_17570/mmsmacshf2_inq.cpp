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
/// 1.根据时间范围,炉号等条件进行切断实绩查询。
///  更改查询表，从TMMSM01更改为TMMSM33，收货标记两个表都更新
/// </para>
/// <para>数据库表：TMMSM33(炉次切断实绩表)          </para>
/// <para>主调用函数：前台MMSMACSHS2N画面F2(查询)按钮         </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns> 切割实绩 </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsmacshf2_inq)

int f_mmsmacshf2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString v_prod_time_from = "";//开始时间
	CString v_prod_time_to = "";//结束时间
	CString v_heat_no = "";//熔炼号
	CString v_order_no = "";//合同号
	CString v_stock_no = "";//库区号
	CString v_in_flag = "";//入库标记
	CString v_st_no = "";//出钢记号
	CString v_product_flag = "";//成品标记
	CString v_rcv_mat_flag = "";//收货标记
	CString v_unit_code = "";//铸机号
	CString v_people_div = "";//自动查询开关  1 是自动   0 或空是手动
	CString v_man_proc_div = "";// Y 是人工查询  N是自动查询
	CString v_remainder_reason = "";//余材原因
	CString v_order_flag = "";//是否有合同号
	CString v_c_div = "";//碳锈区分

	CString sqlstr = "";
	CString sqlstr_zdsx = "";//查询自动刷新开关
	CString sqlstr_temp = "";
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
		Log::Info("", __FUNCTION__, "v_mat_no【{0}】", v_mat_no);

		if (bcls_rec->Tables[0].Columns.Contains("ARCHIVE_FLAG"))
			v_archive_flag = bcls_rec->Tables[0].Rows[0]["ARCHIVE_FLAG"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_DESTION"))
			v_mat_destion = bcls_rec->Tables[0].Rows[0]["MAT_DESTION"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_STATUS"))
			v_mat_status = bcls_rec->Tables[0].Rows[0]["MAT_STATUS"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			v_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().ToUpper().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_FROM"))
			v_prod_time_from = bcls_rec->Tables[0].Rows[0]["PROD_TIME_FROM"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_TO"))
			v_prod_time_to = bcls_rec->Tables[0].Rows[0]["PROD_TIME_TO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().ToUpper().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ORDER_NO"))
			v_order_no = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_NO"))
			v_stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("IN_FLAG"))
			v_in_flag = bcls_rec->Tables[0].Rows[0]["IN_FLAG"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().ToUpper().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PRODUCT_FLAG"))
			v_product_flag = bcls_rec->Tables[0].Rows[0]["PRODUCT_FLAG"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("RCV_MAT_FLAG"))
			v_rcv_mat_flag = bcls_rec->Tables[0].Rows[0]["RCV_MAT_FLAG"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("UNIT_CODE"))
			v_unit_code = bcls_rec->Tables[0].Rows[0]["UNIT_CODE"].ToString().Trim().Replace(",", "','");

		if (bcls_rec->Tables[0].Columns.Contains("PEOPLE_DIV"))
			v_people_div = bcls_rec->Tables[0].Rows[0]["PEOPLE_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAN_PROC_DIV"))
			v_man_proc_div = bcls_rec->Tables[0].Rows[0]["MAN_PROC_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("REMAINDER_REASON"))
			v_remainder_reason = bcls_rec->Tables[0].Rows[0]["REMAINDER_REASON"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("C_DIV"))
			v_c_div  = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString().Trim();

		//按照是否有合同号查询
		if (bcls_rec->Tables[0].Columns.Contains("ORDER_FLAG"))
			v_order_flag = bcls_rec->Tables[0].Rows[0]["ORDER_FLAG"].ToString().Trim();

		Log::Info("", __FUNCTION__, "自动开关【{0}】，人工查询标记【{1}】 v_order_flag 【{2}】", v_people_div, v_man_proc_div, v_order_flag);

		//当为人工查询 Y 或自动查询  1时，才可以查询，其他情况不允许查询
		if (v_man_proc_div == "Y" || v_people_div == "1")
		{

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				//DECODE(RECV_MAT_TIME,' ',to_char(sysdate,'yyyymmddhh24miss'),RECV_MAT_TIME) RECV_MAT_TIME 
				
				/*else if (v_archive_flag == "H")
				{
						sqlstr = " SELECT  CASE WHEN (SELECT PONO FROM TPSSM11 S WHERE S.PONO = T.PONO )  IS NULL and (SELECT PONO FROM TPSSM_PLAN_ST S WHERE S.PONO = T.PONO) IS NULL THEN "
							"	'8' WHEN T.PONO_SLAB <> ' ' THEN '9' ELSE '3' END AS SLAB_TYPE_OLD, T1.* FROM HMMSM01 T WHERE 1 =1  ";
				}*/


				if (v_order_flag == "1")
				{
					sqlstr_temp += " and T.ORDER_NO  <>' '";
				}
				else if (v_order_flag == "0")
				{
					sqlstr_temp += " and T.ORDER_NO  =' '";
				}

				if (v_mat_no != "") //可输入多条材料号查询
				{
					sqlstr_temp += " and  T.MAT_NO IN ('" + v_mat_no + "')";
				}
				if (v_mat_destion != "")
				{
					sqlstr_temp += " and  T.MAT_DESTION = '" + v_mat_destion + "'";
				}

				if (v_mat_status != "")
				{
					sqlstr_temp += " and  T.MAT_STATUS = '" + v_mat_status + "'";
				}
				if (v_pono != "")
				{
					sqlstr_temp += " and  T.PONO = '" + v_pono + "'";
				}
				if (v_prod_time_from != "")
				{
					sqlstr_temp += " and  T.PROD_TIME >= '" + v_prod_time_from + "'";
				}
				if (v_prod_time_to != "")
				{
					sqlstr_temp += " and  T.PROD_TIME <= '" + v_prod_time_to + "'";
				}
				if (v_heat_no != "")
				{
					sqlstr_temp += " and  T.HEAT_NO = '" + v_heat_no + "'";
				}
				if (v_order_no != "")
				{
					sqlstr_temp += " and  T.ORDER_NO = '" + v_order_no + "'";
				}
				if (v_stock_no != "")
				{
					sqlstr_temp += " and  T.STOCK_NO = '" + v_stock_no + "'";
				}
				if (v_in_flag != "")
				{
					sqlstr_temp += " and  T.IN_FLAG = '" + v_in_flag + "'";
				}
				if (v_st_no != "")
				{
					sqlstr_temp += " and  T.ST_NO = '" + v_st_no + "'";
				}
				if (v_product_flag != "")
				{
					sqlstr_temp += " and  T.PRODUCT_FLAG = '" + v_product_flag + "'";
				}
				if (v_rcv_mat_flag != "")
				{
					sqlstr_temp += " and  T.RCV_MAT_FLAG = '" + v_rcv_mat_flag + "'";
				}
				if (v_unit_code != "")
				{
					sqlstr_temp += " and  T.UNIT_CODE IN ('" + v_unit_code + "')";
				}
				if (v_remainder_reason != "")
				{
					sqlstr_temp += " and  T.REMAINDER_REASON = '" + v_remainder_reason + "'";
				}
				if (v_c_div != "")
				{
					sqlstr_temp += " and  T.C_DIV = '" + v_c_div + "'";
				}

				//sqlstr_temp += " ORDER BY PROD_TIME DESC";
				//sqlstr += sqlstr_temp;

				if (v_archive_flag == "T" || v_archive_flag == "")//在线数据先查TMMSM01表，历史数据查HMMSM01表
				{
					sqlstr = "SELECT * FROM ( SELECT CASE WHEN (SELECT PONO FROM TPSSM11 S WHERE S.PONO = T.PONO )  IS NULL and (SELECT PONO FROM TPSSM_PLAN_ST S WHERE S.PONO = T.PONO) IS NULL  THEN "
						"	'8' WHEN T.PONO_SLAB <> ' ' THEN '9' ELSE '3' END AS SLAB_TYPE_OLD,DECODE(substr(TQ01.ORDER_NO,0,1),'A',TQ01.TRNP_MODE_CODE,' ')  TRNP_MODE_CODE_1,TQ01.ORDER_THICK,TQ01.PROD_CLASS_DESC, decode(t2.MAT_NO, null, '0', '1') need_mend,nvl(t2.MEND_CAUSE, ' ')           MEND_CAUSE, T.*,CASE WHEN T.RCV_MAT_FLAG = 'N' THEN ' ' WHEN T.measure_wt = T.receive_weight THEN '1' ELSE '0' END AS avlb_flag1 FROM TMMSM01 T LEFT JOIN TQMOM01 TQ01 ON  T.ORDER_NO =TQ01.ORDER_NO LEFT JOIN get_mend_flag t2 ON T.mat_no = t2.MAT_NO  WHERE 1 = 1  " + sqlstr_temp + " "
						"	UNION ALL "
						"	SELECT CASE WHEN(SELECT PONO FROM TPSSM11 S WHERE S.PONO = T.PONO)  IS NULL and(SELECT PONO FROM TPSSM_PLAN_ST S WHERE S.PONO = T.PONO) IS NULL  THEN	"
						"	'8' WHEN T.PONO_SLAB <> ' ' THEN '9' ELSE '3' END AS SLAB_TYPE_OLD, DECODE(substr(TQ01.ORDER_NO,0,1),'A',TQ01.TRNP_MODE_CODE,' ') TRNP_MODE_CODE_1,TQ01.ORDER_THICK,TQ01.PROD_CLASS_DESC, decode(t2.MAT_NO, null, '0', '1') need_mend,nvl(t2.MEND_CAUSE, ' ')           MEND_CAUSE, T.*,CASE WHEN T.RCV_MAT_FLAG = 'N' THEN ' ' WHEN T.measure_wt = T.receive_weight THEN '1' ELSE '0' END AS avlb_flag1 FROM HMMSM01 T LEFT JOIN TQMOM01 TQ01 ON  T.ORDER_NO =TQ01.ORDER_NO LEFT JOIN get_mend_flag t2 ON T.mat_no = t2.MAT_NO WHERE 1 = 1 " + sqlstr_temp + "  "
						"	AND USAGE_DECISION <> '3001' )  ORDER BY PROD_TIME DESC ";
				}
			}

			Log::Info("", __FUNCTION__, "sqlstr【{0}】", sqlstr);
			//分页获取
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
			cmd_inq.Close();


			//返回分页总数量信息 
			bcls_ret->Tables.Add("PageInfo");
			bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
			bcls_ret->Tables["PageInfo"].Rows.Add();
			bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;


			bcls_ret->Tables.Add();

			////查询自动刷新的开关  并放在查询条件区域里的开关上
			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:				// MS SQL Server数据库
			//case DB_KIND_ORACLE:	        // Oracle 数据库
			//default:
			//	
			//	sqlstr_zdsx = " select CODE from TWMSMZD02 where CODE_CLASS = 'MMSHQY'";
			//}
			////分页获取
			//cmd_inq.SetCommandText(sqlstr_zdsx);
			//cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
			//cmd_inq.Close();
			//
			


		}
		else
		{
			Log::Info("", __FUNCTION__, "开关没开且不是人工查询的");
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
