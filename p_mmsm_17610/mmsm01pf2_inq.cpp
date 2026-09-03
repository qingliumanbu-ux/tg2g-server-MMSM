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
BM2F_ENTERACE(mmsm01pf2_inq)

int f_mmsm01pf2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr2 = "";
	CString sqlstr_count			= "";
	CString sqlstr_temp				= "";
	int		TotalRecordCount		= 0  ;
	int     i = 0;

	CString ch_start_time_f			= "";
	CString ch_start_time_t			= "";
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

	CModel tmmsm01("TMMSM01");
 
	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch(CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize   = 1000;
		}


		//--------------------------------
		//获取传入参数
		
		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			v_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("CAST_LOT_NO"))
			v_cast_lot_no = bcls_rec->Tables[0].Rows[0]["CAST_LOT_NO"].ToString();
		if(bcls_rec->Tables[0].Columns.Contains("ORDER_NO"))
			v_order_no = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString();
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
	
		////Log::Info("", __FUNCTION__, "PONO      =[{0}]",v_pono);
		////Log::Info("", __FUNCTION__, "CAST_LOT_NO      =[{0}]", v_cast_lot_no);
		////Log::Info("", __FUNCTION__, "ORDER_NO         =[{0}]", v_order_no);
		////Log::Info("", __FUNCTION__, "ARCHIVE_FLAG      =[{0}]", v_archive_flag);


		blkNum = bcls_ret->Tables.IndexOf("MMSMTJ");
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("MMSMTJ");

		}

		blkNum = bcls_ret->Tables.IndexOf("MMSMC");
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("MMSMC");
		}

		

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				if (v_archive_flag.Trim() != "")
				{
					sqlstr = " SELECT *  FROM " + v_table_type + " WHERE 1=1 ";

					if (v_pono.Trim() != "")
					{
						sqlstr += " AND PONO = @pono";
					}

					if (v_order_no.Trim() != "")
					{
						sqlstr += " AND ORDER_NO = @order_no";
					}

					if (v_cast_lot_no.Trim() != "")
					{
						sqlstr+= " AND PONO IN (SELECT PONO FROM TPSSM01 WHERE CAST_LOT_NO	= @cast_lot_no)";
					}

				}
				else
				{
					sqlstr = " SELECT *  FROM  TMMSM01  WHERE 1=1 ";

					if (v_pono.Trim() != "")
					{
						sqlstr += " AND PONO = @pono";
					}

					if (v_order_no.Trim() != "")
					{
						sqlstr += " AND ORDER_NO = @order_no";
					}

					if (v_cast_lot_no.Trim() != "")
					{
						sqlstr += " AND PONO IN (SELECT PONO FROM TPSSM01 WHERE CAST_LOT_NO	= @cast_lot_no)";
					}

					
					sqlstr2 += " UNION SELECT *  FROM HMMSM01  WHERE 1=1 ";

					if (v_pono.Trim() != "")
					{
						sqlstr2 += " AND PONO = @pono";
					}

					if (v_order_no.Trim() != "")
					{
						sqlstr2 += " AND ORDER_NO = @order_no";
					}

					if (v_cast_lot_no.Trim() != "")
					{
						sqlstr2 += " AND PONO IN (SELECT PONO FROM TPSSM01 WHERE CAST_LOT_NO	= @cast_lot_no)";
					}

					sqlstr = sqlstr + sqlstr2;

				}
	

				sqlstr_count = "SELECT COUNT(*) FROM ( " + sqlstr + " )";

				//Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
				break;
		}
		   
	
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("pono", v_pono);
		cmd_inq.Parameters.Set("order_no", v_order_no);
		cmd_inq.Parameters.Set("cast_lot_no", v_cast_lot_no);
		cmd_inq.ExecuteReader();

		while (cmd_inq.Read())
		{
			
			cmd_inq.Fetch(tmmsm01);

			////Log::Info("", __FUNCTION__, "tmmsm01.PONO      =[{0}]", tmmsm01["PONO"].ToString());
			////Log::Info("", __FUNCTION__, "v_pono_n    =[{0}]", v_pono_n);
			
			if (strcmp(v_pono_n, tmmsm01["PONO"].ToString())<0)
			{
				tmmsm01.MergeTo(bcls_ret->Tables["MMSMC"], false);
			}

			v_pono_n = tmmsm01["PONO"];
			
		}
		cmd_inq.Close();
	


		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL,"TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;	

	 }
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
