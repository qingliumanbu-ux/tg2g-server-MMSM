/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2016-7-25
Description: 按PONO查询物料
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/
#include "tmmsm01.h"


/* ***** 静态函数申明 ***** */


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// CC工序实绩查询
/// <para>
///
///
/// </para>
/// <para>数据库表：          </para>
/// <para>主调用函数：        </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns> CC实绩 </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm01g1_inq);

int f_mmsm01g1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_t = "";
	CString sqlstr_h = "";
	CString t_sqlsr = "";
	CString h_sqlsr = "";

	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	int     i = 0;

	CString ch_start_time_f = "";
	CString ch_start_time_t = "";
	CString v_table_type = "";//表名称。
	CString v_proc_div = "";
	CString v_heat_no = "";
	CString v_pono_n = "";
	CString v_pono = "";
	CString v_order_no = "";
	CString v_cast_lot_no = "";
	CString v_archive_flag = "";
	int blkNum = 0;

	//系统的分页类信息。
	CPageInfo pageInfo;

	CTMMSM01 tmmsm01(conn);


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_sql1(conn);


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


		//--------------------------------
		//获取传入参数

		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			v_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString();
		/*if (bcls_rec->Tables[0].Columns.Contains("CAST_LOT_NO"))
			v_cast_lot_no = bcls_rec->Tables[0].Rows[0]["CAST_LOT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("ORDER_NO"))
			v_order_no = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString();*/
		if (bcls_rec->Tables[0].Columns.Contains("ARCHIVE_FLAG"))
			v_archive_flag = bcls_rec->Tables[0].Rows[0]["ARCHIVE_FLAG"].ToString();

		if (v_archive_flag.Trim() == "T")//在线
		{
			v_table_type = "TMMSM01";
		}
		else if (v_archive_flag.Trim() == "H")//历史
		{
			v_table_type = "HMMSM01";
		}


		/* ***** 打印输入参数 ***** */

		Log::Info("", __FUNCTION__, "PONO      =[{0}]", v_pono);
		/*Log::Info("", __FUNCTION__, "CAST_LOT_NO      =[{0}]", v_cast_lot_no);
		Log::Info("", __FUNCTION__, "ORDER_NO         =[{0}]", v_order_no);*/
		Log::Info("", __FUNCTION__, "ARCHIVE_FLAG      =[{0}]", v_archive_flag);


		/*blkNum = bcls_ret->Tables.IndexOf("MMSMTJ");
		if (blkNum < 0)
		{
		bcls_ret->Tables.Add("MMSMTJ");

		}*/

		blkNum = bcls_ret->Tables.IndexOf("MMSMC");
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("MMSMC");
		}



		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库(未开Oracle兼容)
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库(开Oracle兼容)
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			if (v_archive_flag.Trim() != "")
			{
				sqlstr = " SELECT  DISTINCT  A.PONO, A.HEAT_NO, A.ORDER_NO, "
					" A.CAST_NO AS CAST_NUM_1,A.CAST_DIV_NO  AS CAST_NUM_2, A.ST_NO, A.HOT_CHARGE_FLAG,"
					"A.HOT_SEND_FLAG,A.CC_NO FROM  " + v_table_type + " A Where  1 = 1  ";
				if (v_pono.Trim() != "")
				{
					sqlstr += " AND A.PONO = @pono";
				}

			/*	if (v_order_no.Trim() != "")
				{
					sqlstr += " AND A.ORDER_NO = @order_no";
				}

				if (v_cast_lot_no.Trim() != "")
				{
					sqlstr += " AND A.PONO IN (SELECT PONO FROM TPSSM01 WHERE CAST_LOT_NO	= @cast_lot_no)";

				}*/
				sqlstr = " SELECT A.*, SUM(B.MAT_WT) AS NOM_WT_2, SUM(B.MAT_NUM) AS NOM_NUM_2 "
					" FROM( "
					" SELECT A.*,SUM(B.MAT_WT) AS NOM_WT_1, SUM(B.MAT_NUM) AS NOM_NUM_1 "
					" FROM( "
					" SELECT A.*,SUM(E.MAT_ACT_WT) as HC_WT, SUM(E.MAT_NUM) as HC_NUM "
					" FROM( "
					" SELECT A.*,SUM(E.MAT_ACT_WT) AS MAT_WT, SUM(E.MAT_NUM) AS MAT_NUM  FROM "
					" ( " + sqlstr + " ) AS A LEFT JOIN " + v_table_type + " AS E ON(A.PONO = E.PONO)"
					" Group by   A.PONO, A.HEAT_NO, A.ORDER_NO, A.CAST_NUM_1, A.CAST_NUM_2, A.ST_NO, A.HOT_CHARGE_FLAG, A.HOT_SEND_FLAG, A.CC_NO "
					" ) "
					" AS A  LEFT JOIN " + v_table_type + " AS E ON(A.PONO = E.PONO AND E.HOT_SEND_FLAG = '1') "
					" GROUP BY A.PONO, A.HEAT_NO, A.ORDER_NO, A.CAST_NUM_1, A.CAST_NUM_2, A.ST_NO, A.HOT_CHARGE_FLAG, A.HOT_SEND_FLAG, A.CC_NO, "
					" A.MAT_WT, A.MAT_NUM "
					" ) AS A LEFT JOIN TPMOM01 AS B ON A.PONO = B.PONO "
					" GROUP BY A.PONO, A.HEAT_NO, A.ORDER_NO, A.CAST_NUM_1, A.CAST_NUM_2, A.ST_NO, A.HOT_CHARGE_FLAG, A.HOT_SEND_FLAG, A.CC_NO, "
					" A.MAT_WT, A.MAT_NUM, A.HC_WT, A.HC_NUM "
					" )AS A LEFT JOIN HPMOM01 AS B ON A.PONO = B.PONO"
					" GROUP BY A.PONO, A.HEAT_NO, A.ORDER_NO, A.CAST_NUM_1, A.CAST_NUM_2, A.ST_NO, A.HOT_CHARGE_FLAG, A.HOT_SEND_FLAG, A.CC_NO, "
					" A.MAT_WT, A.MAT_NUM, A.HC_WT, A.HC_NUM, A.NOM_WT_1, A.NOM_NUM_1 "
					" ORDER BY A.PONO, A.ORDER_NO DESC ";

			}
			else
			{
				t_sqlsr = " SELECT  DISTINCT  A.PONO, A.HEAT_NO, A.ORDER_NO, "
					" A.CAST_NO AS CAST_NUM_1, A.CAST_DIV_NO  AS CAST_NUM_2, A.ST_NO, A.HOT_CHARGE_FLAG,"
					" A.HOT_SEND_FLAG, A.CC_NO "
					" FROM  TMMSM01 A Where  1 = 1  ";

				if (v_pono.Trim() != "")
				{
					t_sqlsr += " AND A.PONO = @pono";
				}

				//if (v_order_no.Trim() != "")
				//{
				//	t_sqlsr += " AND A.ORDER_NO = @order_no";
				//}

				//if (v_cast_lot_no.Trim() != "")
				//{
				//	t_sqlsr += " AND A.PONO IN (SELECT PONO FROM TPSSM01 WHERE CAST_LOT_NO	= @cast_lot_no)";
				//}
			

				h_sqlsr =" SELECT  DISTINCT  A.PONO, A.HEAT_NO, A.ORDER_NO, "
					" A.CAST_NO AS CAST_NUM_1, A.CAST_DIV_NO  AS CAST_NUM_2, A.ST_NO, A.HOT_CHARGE_FLAG,"
					" A.HOT_SEND_FLAG, A.CC_NO "
					" FROM  HMMSM01 A Where  1 = 1  ";
				if (v_pono.Trim() != "")
				{
					h_sqlsr += " AND A.PONO = @pono ";
				}

				/*if (v_order_no.Trim() != "")
				{
					h_sqlsr += " AND A.ORDER_NO = @order_no ";
				}

				if (v_cast_lot_no.Trim() != "")
				{
					h_sqlsr += " AND A.PONO IN (SELECT PONO FROM TPSSM01 WHERE CAST_LOT_NO	= @cast_lot_no) ";
				}*/
			

				sqlstr = " SELECT A.*,SUM(B.MAT_WT) AS NOM_WT_2, SUM(B.MAT_NUM) AS NOM_NUM_2 "
					" FROM( "
					" SELECT A.* ,SUM(B.MAT_WT) AS NOM_WT_1, SUM(B.MAT_NUM) AS NOM_NUM_1 "
					" FROM(" 

					" SELECT A.*,SUM(E.MAT_ACT_WT) as HC_WT_2, SUM(E.MAT_NUM) as HC_NUM_2 "
					" FROM( "
					" SELECT A.*,SUM(E.MAT_ACT_WT) as HC_WT_1, SUM(E.MAT_NUM) as HC_NUM_1 "
					" FROM( "
					" SELECT A.*,SUM(E.MAT_ACT_WT)  AS MAT_WT_2, SUM(E.MAT_NUM) AS MAT_NUM_2 "
					" FROM("
					" SELECT A.*,SUM(E.MAT_ACT_WT)  AS MAT_WT_1, SUM(E.MAT_NUM) AS MAT_NUM_1 "
					" FROM( "
					  + t_sqlsr + " UNION "+h_sqlsr+
				    " ) AS A  LEFT JOIN  TMMSM01 AS E  ON A.PONO = E.PONO "
					" GROUP BY  A.PONO, A.HEAT_NO, A.ORDER_NO, A.CAST_NUM_1, A.CAST_NUM_2, A.ST_NO,"
					" A.HOT_CHARGE_FLAG, A.HOT_SEND_FLAG, A.CC_NO "
					" )AS A  LEFT JOIN  HMMSM01 AS E ON A.PONO = E.PONO"
					" GROUP BY  A.PONO, A.HEAT_NO, A.ORDER_NO, A.CAST_NUM_1, A.CAST_NUM_2, A.ST_NO,"
					" A.HOT_CHARGE_FLAG, A.HOT_SEND_FLAG, A.CC_NO, A.MAT_WT_1, A.MAT_NUM_1 "
					" )AS A  LEFT JOIN TMMSM01 AS E ON(A.PONO = E.PONO AND E.HOT_SEND_FLAG = '1') "
					" GROUP BY  A.PONO, A.HEAT_NO, A.ORDER_NO, A.CAST_NUM_1, A.CAST_NUM_2, A.ST_NO,"
					" A.HOT_CHARGE_FLAG, A.HOT_SEND_FLAG, A.CC_NO, A.MAT_WT_1, A.MAT_NUM_1 ,"
					" A.MAT_WT_2, A.MAT_NUM_2"
					" )AS A  LEFT JOIN HMMSM01 AS E ON(A.PONO = E.PONO AND E.HOT_SEND_FLAG = '1') "
					" GROUP BY  A.PONO, A.HEAT_NO, A.ORDER_NO, A.CAST_NUM_1, A.CAST_NUM_2, A.ST_NO,"
					" A.HOT_CHARGE_FLAG, A.HOT_SEND_FLAG, A.CC_NO, A.MAT_WT_1, A.MAT_NUM_1 ,"
					" A.MAT_WT_2, A.MAT_NUM_2,A.HC_WT_1, A.HC_NUM_1 "


					" )"
					" AS A LEFT JOIN TPMOM01 AS B ON A.PONO = B.PONO "
					" GROUP BY A.PONO, A.HEAT_NO, A.ORDER_NO, A.CAST_NUM_1, A.CAST_NUM_2,"
					" A.ST_NO, A.HOT_CHARGE_FLAG, A.HOT_SEND_FLAG, A.CC_NO,  A.MAT_WT_1, A.MAT_NUM_1 ,"
					" A.MAT_WT_2, A.MAT_NUM_2,A.HC_WT_1, A.HC_NUM_1,A.HC_WT_2, A.HC_NUM_2 "
					" ) AS A LEFT JOIN HPMOM01 AS B ON A.PONO = B.PONO "
					" GROUP BY A.PONO, A.HEAT_NO, A.ORDER_NO, A.CAST_NUM_1, A.CAST_NUM_2,"
					" A.ST_NO, A.HOT_CHARGE_FLAG, A.HOT_SEND_FLAG, A.CC_NO,A.MAT_WT_1, A.MAT_NUM_1 ,"
					" A.MAT_WT_2, A.MAT_NUM_2,A.HC_WT_1, A.HC_NUM_1,A.HC_WT_2, A.HC_NUM_2 , A.NOM_WT_1, A.NOM_NUM_1 "
					" ORDER BY A.PONO, A.ORDER_NO DESC";
				
				    
			}


			sqlstr_count = "SELECT COUNT(*) FROM ( " + sqlstr + " )";

			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			break;
		}

		
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("pono", v_pono);
			/*cmd_inq.Parameters.Set("order_no", v_order_no);
			cmd_inq.Parameters.Set("cast_lot_no", v_cast_lot_no);*/
			/*cmd_inq.ExecuteReader();*/

			blkNum = bcls_ret->Tables.IndexOf("MMSMC0");
			if (blkNum < 0)
			{
				bcls_ret->Tables.Add("MMSMC0");
			}

			cmd_inq.ExecuteQuery(bcls_ret->Tables["MMSMC0"]);
			cmd_inq.Close();
		
		
		Log::Info("", __FUNCTION__, "Page.RecordForm=[{0}],Page.PageSize=[{1}]", pageInfo.RecordFrom, pageInfo.PageSize);

		int rowCount = bcls_ret->Tables["MMSMC0"].Rows.get_Count();
		Log::Info("", __FUNCTION__, "获得的行数   =[{0}]", rowCount);

		bcls_ret->Tables["MMSMC"].Clone(bcls_ret->Tables["MMSMC0"]);

		for (int index = 0, cnt = 0; index < rowCount; index++)
		{

			CString S_PONO = bcls_ret->Tables["MMSMC0"].Rows[index]["PONO"].ToString();
			Log::Info("", __FUNCTION__, "tmmsm01.PONO   =[{0}]", S_PONO);
			Log::Info("", __FUNCTION__, "v_pono_n    =[{0}]", v_pono_n);

			if (strcmp(v_pono_n, S_PONO) <0)
			{



				Log::Info("", __FUNCTION__, "压入Tables[\"MMSMC\"]时：tmmsm01.PONO  =[{0}]", S_PONO);
				Log::Info("", __FUNCTION__, "v_pono_n    =[{0}]", v_pono_n);
				Log::Info("", __FUNCTION__, "添加第【{0}】行", cnt);
				Log::Info("", __FUNCTION__, "位置0");
				bcls_ret->Tables["MMSMC"].Rows.Add();
				/*	bcls_ret->Tables["MMSMC"].Rows[cnt]["MAT_NO"] = bcls_ret->Tables["MMSMC0"].Rows[index]["MAT_NO"];*/
				bcls_ret->Tables["MMSMC"].Rows[cnt]["PONO"] = bcls_ret->Tables["MMSMC0"].Rows[index]["PONO"];
				bcls_ret->Tables["MMSMC"].Rows[cnt]["HEAT_NO"] = bcls_ret->Tables["MMSMC0"].Rows[index]["HEAT_NO"];
				bcls_ret->Tables["MMSMC"].Rows[cnt]["ORDER_NO"] = bcls_ret->Tables["MMSMC0"].Rows[index]["ORDER_NO"];
				bcls_ret->Tables["MMSMC"].Rows[cnt]["ST_NO"] = bcls_ret->Tables["MMSMC0"].Rows[index]["ST_NO"];
				bcls_ret->Tables["MMSMC"].Rows[cnt]["HOT_CHARGE_FLAG"] = bcls_ret->Tables["MMSMC0"].Rows[index]["HOT_CHARGE_FLAG"];
				bcls_ret->Tables["MMSMC"].Rows[cnt]["HOT_SEND_FLAG"] = bcls_ret->Tables["MMSMC0"].Rows[index]["HOT_SEND_FLAG"];
				bcls_ret->Tables["MMSMC"].Rows[cnt]["CC_NO"] = bcls_ret->Tables["MMSMC0"].Rows[index]["CC_NO"];

				if (v_archive_flag.Trim() != ""){
					bcls_ret->Tables["MMSMC"].Rows[cnt]["MAT_WT"] = bcls_ret->Tables["MMSMC0"].Rows[index]["MAT_WT"];
					bcls_ret->Tables["MMSMC"].Rows[cnt]["MAT_NUM"] = bcls_ret->Tables["MMSMC0"].Rows[index]["MAT_NUM"];
					bcls_ret->Tables["MMSMC"].Rows[cnt]["HC_WT"] = bcls_ret->Tables["MMSMC0"].Rows[index]["HC_WT"];
					bcls_ret->Tables["MMSMC"].Rows[cnt]["HC_NUM"] = bcls_ret->Tables["MMSMC0"].Rows[index]["HC_NUM"];
				}
				else{
					if (bcls_ret->Tables["MMSMC"].Columns.IndexOf("MAT_WT")<0){
						bcls_ret->Tables["MMSMC"].Columns.Add(DT_DOUBLE, "MAT_WT");
					}
					if (bcls_ret->Tables["MMSMC"].Columns.IndexOf("MAT_NUM")<0){
						bcls_ret->Tables["MMSMC"].Columns.Add(DT_INT32, "MAT_NUM");
					}
					if (bcls_ret->Tables["MMSMC"].Columns.IndexOf("HC_WT")<0){
						bcls_ret->Tables["MMSMC"].Columns.Add(DT_DOUBLE, "HC_WT");
					}
					if (bcls_ret->Tables["MMSMC"].Columns.IndexOf("HC_NUM")<0){
						bcls_ret->Tables["MMSMC"].Columns.Add(DT_INT32, "HC_NUM");
					}
					bcls_ret->Tables["MMSMC"].Rows[cnt]["MAT_WT"] =
						bcls_ret->Tables["MMSMC0"].Rows[index]["MAT_WT_1"].ToDouble() + bcls_ret->Tables["MMSMC0"].Rows[index]["MAT_WT_2"].ToDouble();
					bcls_ret->Tables["MMSMC"].Rows[cnt]["HC_WT"] =
						bcls_ret->Tables["MMSMC0"].Rows[index]["HC_WT_1"].ToDouble() + bcls_ret->Tables["MMSMC0"].Rows[index]["HC_WT_2"].ToDouble();
					bcls_ret->Tables["MMSMC"].Rows[cnt]["MAT_NUM"] =
						bcls_ret->Tables["MMSMC0"].Rows[index]["MAT_NUM_1"].ToDouble() + bcls_ret->Tables["MMSMC0"].Rows[index]["MAT_NUM_2"].ToDouble();
					bcls_ret->Tables["MMSMC"].Rows[cnt]["HC_NUM"] =
						bcls_ret->Tables["MMSMC0"].Rows[index]["HC_NUM_1"].ToDouble() + bcls_ret->Tables["MMSMC0"].Rows[index]["HC_NUM_2"].ToDouble();
				}
				if (bcls_ret->Tables["MMSMC"].Columns.IndexOf("NOM_WT")<0){
					bcls_ret->Tables["MMSMC"].Columns.Add(DT_STRING, "NOM_WT");
				}
				if (bcls_ret->Tables["MMSMC"].Columns.IndexOf("NOM_NUM")<0){
					bcls_ret->Tables["MMSMC"].Columns.Add(DT_STRING, "NOM_NUM");
				}
				if (bcls_ret->Tables["MMSMC0"].Rows[index]["NOM_WT_1"].ToString().Trim() != "0"){
					bcls_ret->Tables["MMSMC"].Rows[cnt]["NOM_WT"] = bcls_ret->Tables["MMSMC0"].Rows[index]["NOM_WT_1"];
					Log::Info("", __FUNCTION__, "1申请量：{0} ", bcls_ret->Tables["MMSMC0"].Rows[index]["NOM_WT_1"].ToString());
				}
				else if (bcls_ret->Tables["MMSMC0"].Rows[index]["NOM_WT_2"].ToString().Trim() != "0")
				{
					bcls_ret->Tables["MMSMC"].Rows[cnt]["NOM_WT"] = bcls_ret->Tables["MMSMC0"].Rows[index]["NOM_WT_2"];
					Log::Info("", __FUNCTION__, "2申请量：{0} ", bcls_ret->Tables["MMSMC0"].Rows[index]["NOM_WT_2"].ToString());
				}
				if (bcls_ret->Tables["MMSMC0"].Rows[index]["NOM_NUM_1"].ToString().Trim() != "0"){
					bcls_ret->Tables["MMSMC"].Rows[cnt]["NOM_NUM"] = bcls_ret->Tables["MMSMC0"].Rows[index]["NOM_NUM_1"];
					Log::Info("", __FUNCTION__, "1申请块：{0} ", bcls_ret->Tables["MMSMC0"].Rows[index]["NOM_NUM_1"].ToString());
				}
				else if (bcls_ret->Tables["MMSMC0"].Rows[index]["NOM_NUM_2"].ToString().Trim() != "0")
				{
					bcls_ret->Tables["MMSMC"].Rows[cnt]["NOM_NUM"] = bcls_ret->Tables["MMSMC0"].Rows[index]["NOM_NUM_2"];
					Log::Info("", __FUNCTION__, "2申请块：{0} ", bcls_ret->Tables["MMSMC0"].Rows[index]["NOM_NUM_2"].ToString());
				}
			
				if (bcls_ret->Tables["MMSMC"].Columns.IndexOf("CAST_NUM")<0){
					bcls_ret->Tables["MMSMC"].Columns.Add(DT_STRING, "CAST_NUM");
				}
				
				/*	Log::Info("", __FUNCTION__, "位置6");*/
				if (bcls_ret->Tables["MMSMC0"].Rows[index]["CAST_NUM_1"].ToString().Trim() != ""&& bcls_ret->Tables["MMSMC0"].Rows[index]["CAST_NUM_2"].ToString()){
					bcls_ret->Tables["MMSMC"].Rows[cnt]["CAST_NUM"] = bcls_ret->Tables["MMSMC0"].Rows[index]["CAST_NUM_1"].ToString() + "-" +
						bcls_ret->Tables["MMSMC0"].Rows[index]["CAST_NUM_2"].ToString() ;
				}
				else{
					bcls_ret->Tables["MMSMC"].Rows[cnt]["CAST_NUM"] = "";
				}
				/*Log::Info("", __FUNCTION__, "位置7");*/
				cnt++;
			}


			v_pono_n = S_PONO;

		}
		bcls_ret->Tables.Remove("MMSMC0");
		

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
