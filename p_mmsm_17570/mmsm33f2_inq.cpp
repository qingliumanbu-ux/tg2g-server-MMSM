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
BM2F_ENTERACE(mmsm33f2_inq)

int f_mmsm33f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_tpssm03 = "";
	CString sqlstr_tmmsm33 = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString sqlstr_order_by = "";
	int		TotalRecordCount = 0;
	int   fetchRowCount = 0;
	int   fetchRowCount1 = 0;
	int   fetchRowCount2 = 0;

	CString v_pono = "";
	CString v_heat_no = "";
	CString v_cc_mach_no = "";
	CString v_billet_type = "";
	CString v_ingot_code = "";
	CString v_history_flag = "";
	CString v_slab_dest = "";
	CString v_dev_code = "";
	CString v_station_id = "";
	CString v_station_no = "";
	CString v_factory_div = "";
	CString v_heat_confm_time = "";
	CString v_heat_confm_time_1 = "";
	CDecimal v_plan_slab_num = 0;
	CDecimal v_cut_slab_num = 0;

	CDecimal v_slab_num = 0;
	CDecimal v_slab_cut_num = 0;

	CString v_lslab_no = "";
	CString v_lslab_no_pre = "";
	
	CString psTableName = "";
	CString ps2TableName = "";

	CString sqlh = "";
	CString sqlhcount = "";



	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm31("TMMSM31");
	CModel tpssm11("TPSSM11");
	CModel tpssm12("TPSSM12");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_tpssm03(conn);
	CDbCommand cmd_inq_tmmsm33(conn);

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
		/*tpssm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);*/

		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			v_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
			v_station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_NO"))
			v_station_no = bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HISTORY_FLAG"))
			v_history_flag = bcls_rec->Tables[0].Rows[0]["HISTORY_FLAG"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_CONFM_TIME"))
			v_heat_confm_time = bcls_rec->Tables[0].Rows[0]["HEAT_CONFM_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_CONFM_TIME_1"))
			v_heat_confm_time_1 = bcls_rec->Tables[0].Rows[0]["HEAT_CONFM_TIME_1"].ToString();
		if (!bcls_ret->Tables[0].Columns.Contains("FACTORY_DIV"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
		}
		    v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString();
		
		if (!bcls_ret->Tables[0].Columns.Contains("BILLET_TYPE"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "BILLET_TYPE");
		}
		    v_billet_type = bcls_rec->Tables[0].Rows[0]["BILLET_TYPE"].ToString();

		Log::Info("", __FUNCTION__, "v_factory_div      =[{0}]", v_factory_div);
		Log::Info("", __FUNCTION__, "v_pono      =[{0}]", v_pono);
		Log::Info("", __FUNCTION__, "v_billet_type      =[{0}]", v_billet_type);

		if (v_station_no.Trim() != "")
		{
			v_dev_code = v_station_id + v_station_no;
			//Log::Info("", __FUNCTION__, "v_dev_code11      =[{0}]", v_dev_code);
		}

		//Log::Info("", __FUNCTION__, "v_dev_code22      =[{0}]", v_dev_code);

		if (!bcls_ret->Tables[0].Columns.Contains("PROC_NO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "PROC_NO");
		}

		if (!bcls_ret->Tables[0].Columns.Contains("START_TIME_REAL"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "START_TIME_REAL");
		}

		if (!bcls_ret->Tables[0].Columns.Contains("END_TIME_REAL"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "END_TIME_REAL");
		}
		
		if (!bcls_ret->Tables[0].Columns.Contains("BILLET_TYPE"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "BILLET_TYPE");
		}

		if (!bcls_ret->Tables[0].Columns.Contains("INGOT_CODE"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "INGOT_CODE");
		}

		if (!bcls_ret->Tables[0].Columns.Contains("SLAB_PLAN_DEST"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "SLAB_PLAN_DEST");
		}

		if (!bcls_ret->Tables[0].Columns.Contains("PLAN_SLAB_NUM"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "PLAN_SLAB_NUM");
		}

		if (!bcls_ret->Tables[0].Columns.Contains("CUT_SLAB_NUM"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "CUT_SLAB_NUM");
		}
		if (!bcls_ret->Tables[0].Columns.Contains("HEAT_CONFM_TIME"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_CONFM_TIME");
		}
		if (!bcls_ret->Tables[0].Columns.Contains("HEAT_CONFM_TIME_1"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_CONFM_TIME_1");
		}


	
		/* ***** 打印输入参数 ***** */
		//Log::Info("", __FUNCTION__, "PONO      =[{0}]", v_pono);
		//Log::Info("", __FUNCTION__, "HEAT_NO   =[{0}]", v_heat_no);
		//Log::Info("", __FUNCTION__, "CC_MACH_NO=[{0}]", v_station_no);


	
		v_history_flag = bcls_rec->Tables[0].Rows[0]["HISTORY_FLAG"].ToString().Trim();
		if (v_history_flag == "1")
		{
			psTableName = "TPSSM41";
			ps2TableName = "TPSSM42";
		}
		else
		{
			psTableName = "TPSSM11";
			ps2TableName = "TPSSM12";
		}
		Log::Info("", __FUNCTION__, "HISTORY_FLAG=[{0}]", v_history_flag);

		if (v_billet_type == "1")
		{
			if (v_history_flag == "0" || v_history_flag == "1"){
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:

					sqlstr = " SELECT a.*, b.proc_no, b.dev_code,b.START_TIME_REAL,b.END_TIME_REAL  "
						" FROM " + psTableName + " a," + ps2TableName + "  b"
						" WHERE a.heat_no = b.heat_no"
						" AND  b.area_id = 5 "
						" AND  b.dev_code IN('C6','C7') "

						//" AND substr(b.dev_code,1,1) =@station_id "
						" AND  a.run_status >= '52'";

					sqlstr_count = " SELECT COUNT(1) "
						" FROM " + psTableName + " a," + ps2TableName + "  b"
						" WHERE a.heat_no = b.heat_no"
						" AND  b.area_id = 5 "
						" AND  b.dev_code IN('C6','C7') "

						//" AND substr(b.dev_code,1,1) =@station_id "
						" AND  a.run_status >= '52'";

					if (v_heat_no.Trim() != "")
					{
						sqlstr_temp += " AND a.heat_no	= @heat_no";
					}
					if (v_station_id.Trim() != "")
					{
						sqlstr_temp += " AND substr(b.dev_code,1,1) =@station_id ";
					}
					if (v_pono.Trim() != "")
					{
						sqlstr_temp += " AND a.pono	= @pono";
					}
					if (v_dev_code.Trim() != "")
					{
						sqlstr_temp += " AND b.dev_code	= @dev_code";
					}
					if (v_factory_div.Trim() != "")
					{
						sqlstr_temp += " AND a.factory_div	= @v_factory_div ";
					}
					if (v_heat_confm_time.Trim() != ""&&v_heat_confm_time_1.Trim() == "")
					{
						sqlstr_temp += " AND b.START_TIME_REAL	>= @v_heat_confm_time ";
					}
					if (v_heat_confm_time.Trim() != ""&&v_heat_confm_time_1.Trim() != "")
					{
						sqlstr_temp += " AND b.START_TIME_REAL	>= @v_heat_confm_time AND b.START_TIME_REAL	<= @v_heat_confm_time_1 ";
					}

					sqlstr_order_by = " ORDER BY b.PROC_NO DESC";

					sqlstr_count = sqlstr_count + sqlstr_temp;
					sqlstr = sqlstr + sqlstr_temp + sqlstr_order_by;

					//Log::Info("", __FUNCTION__, "sqlstr_count  =[{0}]", sqlstr_count);
					Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);
					break;
				}
			}
			else{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:

					//在线表查询
					sqlstr = " SELECT a.*, b.proc_no, b.dev_code,b.START_TIME_REAL,b.END_TIME_REAL  "
						" FROM  TPSSM11 a,TPSSM12  b"
						" WHERE a.heat_no = b.heat_no"
						" AND  b.area_id = 5 "
						" AND  b.dev_code NOT IN('C6','C7') "
						" AND  a.run_status >= '52'";
					//历史查询
					sqlh = " SELECT a.*, b.proc_no, b.dev_code,b.START_TIME_REAL,b.END_TIME_REAL  "
						" FROM  TPSSM41 a,TPSSM42  b"
						" WHERE a.heat_no = b.heat_no"
						" AND  b.area_id = 5 "
						" AND  b.dev_code  NOT IN('C6','C7') "
						" AND  a.run_status >= '52'";

					if (v_heat_no.Trim() != "")
					{
						sqlstr_temp += " AND a.heat_no	= @heat_no";
					}
					if (v_station_id.Trim() != "")
					{
						sqlstr_temp += " AND substr(b.dev_code,1,1) =@station_id ";
					}
					if (v_pono.Trim() != "")
					{
						sqlstr_temp += " AND a.pono	= @pono";
					}
					if (v_dev_code.Trim() != "")
					{
						sqlstr_temp += " AND b.dev_code	= @dev_code";
					}
					if (v_factory_div.Trim() != "")
					{
						sqlstr_temp += " AND a.factory_div	= @v_factory_div";
					}
					if (v_heat_confm_time.Trim() != ""&&v_heat_confm_time_1.Trim() == "")
					{
						sqlstr_temp += " AND b.START_TIME_REAL	>= '" + v_heat_confm_time.Trim() + "' ";
					}
					if (v_heat_confm_time.Trim() != ""&&v_heat_confm_time_1.Trim() != "")
					{
						sqlstr_temp += " AND b.START_TIME_REAL	>= '" + v_heat_confm_time.Trim() + "' AND b.START_TIME_REAL	<= '" + v_heat_confm_time_1.Trim() + "' ";
					}

					sqlstr_order_by = " ORDER BY PROC_NO DESC";

					sqlstr = sqlstr + sqlstr_temp + "  UNION ALL " + sqlh + sqlstr_temp + sqlstr_order_by;
					sqlstr_count = "select COUNT(*) from ( " + sqlstr + " )";

					Log::Info("", __FUNCTION__, "sqlstr_count  =[{0}]", sqlstr_count);
					Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);
					break;
				}
			}
		}
		else
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr = " SELECT a.*, b.proc_no, b.dev_code,b.START_TIME_REAL,b.END_TIME_REAL  "
					" FROM " + psTableName + " a," + ps2TableName + "  b"
					" WHERE a.heat_no = b.heat_no"
					" AND  b.area_id = 5 "
					" AND  b.dev_code not IN('C6','C7') "

					//" AND substr(b.dev_code,1,1) =@station_id "
					" AND  a.run_status >= '52'";

				sqlstr_count = " SELECT COUNT(1) "
					" FROM " + psTableName + " a," + ps2TableName + "  b"
					" WHERE a.heat_no = b.heat_no"
					" AND  b.area_id = 5 "
					" AND  b.dev_code not in('C6','C7') "

					//" AND substr(b.dev_code,1,1) =@station_id "
					" AND  a.run_status >= '52'";

				if (v_heat_no.Trim() != "")
				{
					sqlstr_temp += " AND a.heat_no	= @heat_no";
				}
				if (v_station_id.Trim() != "")
				{
					sqlstr_temp += " AND substr(b.dev_code,1,1) =@station_id ";
				}
				if (v_pono.Trim() != "")
				{
					sqlstr_temp += " AND a.pono	= @pono";
				}
				if (v_dev_code.Trim() != "")
				{
					sqlstr_temp += " AND b.dev_code	= @dev_code";
				}
				if (v_factory_div.Trim() != "")
				{
					sqlstr_temp += " AND a.factory_div	= @v_factory_div";
				}
				if (v_heat_confm_time.Trim() != ""&&v_heat_confm_time_1.Trim() == "")
				{
					sqlstr_temp += " AND a.HEAT_CONFM_TIME	>= @v_heat_confm_time ";
				}
				if (v_heat_confm_time.Trim() != ""&&v_heat_confm_time_1.Trim() != "")
				{
					sqlstr_temp += " AND a.HEAT_CONFM_TIME	>= @v_heat_confm_time AND a.HEAT_CONFM_TIME	<= @v_heat_confm_time_1 ";
				}

				sqlstr_order_by = " ORDER BY b.PROC_NO DESC";

				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr = sqlstr + sqlstr_temp + sqlstr_order_by;

				//Log::Info("", __FUNCTION__, "sqlstr_count  =[{0}]", sqlstr_count);
				Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);
				break;
			}
		}
		

		


		cmd_inq.Parameters.Set("heat_no", v_heat_no);
		cmd_inq.Parameters.Set("pono",v_pono);
		cmd_inq.Parameters.Set("dev_code", v_dev_code);
		cmd_inq.Parameters.Set("station_id", v_station_id);
		Log::Info("", __FUNCTION__, "v_station_id  =[{0}]", v_station_id);
		Log::Info("", __FUNCTION__, "sql  =[{0}]", sqlstr_count);
		cmd_inq.Parameters.Set("v_factory_div", v_factory_div);
		cmd_inq.Parameters.Set("v_heat_confm_time", v_heat_confm_time);
		cmd_inq.Parameters.Set("v_heat_confm_time_1", v_heat_confm_time_1);

		//获取查询返回的信息。(多记录获取模式)
		//============
		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();

		cmd_inq.SetCommandText(sqlstr);// 设置执行的SQL语句

		//逐行读取
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			
			fetchRowCount++;
			Log::Info("", __FUNCTION__, "fetchRowCount  =[{0}]", fetchRowCount);
			Log::Info("", __FUNCTION__, "pageInfo.RecordFrom  =[{0}]", pageInfo.RecordFrom);
			Log::Info("", __FUNCTION__, "pageInfo.PageSize  =[{0}]", pageInfo.PageSize);
			if (fetchRowCount > (pageInfo.RecordFrom + pageInfo.PageSize))
			{
				break;
			}
			if (!((fetchRowCount > pageInfo.RecordFrom) && (fetchRowCount <= (pageInfo.RecordFrom + pageInfo.PageSize))))
			{//若不在设定的行号范围内，则继续下一个循环。
				continue;
			}

			fetchRowCount1 = 0;
			v_plan_slab_num = 0;
			v_slab_num = 0;
		
		
			cmd_inq.Fetch(tpssm11);
			cmd_inq.Fetch(tpssm12);
			
			
			//Log::Info("", __FUNCTION__, "tpssm11.PONO  =[{0}]", tpssm11["PONO"].ToString());

			tpssm11.MergeTo(bcls_ret->Tables[0], false);
			

			sqlstr_tpssm03 = " SELECT BILLET_TYPE,SLAB_DEST,SLAB_NUM,INGOT_CODE,LSLAB_NO"
				"   FROM tpssm03 "
				"   WHERE PONO        = @pono"
				"   ORDER BY SLAB_NO";

			cmd_inq_tpssm03.SetCommandText(sqlstr_tpssm03);
			cmd_inq_tpssm03.Parameters.Clear();
			cmd_inq_tpssm03.Parameters.Set("pono", tpssm11["PONO"].ToString());
			cmd_inq_tpssm03.ExecuteReader();

			while (cmd_inq_tpssm03.Read())
			{
				
				if (fetchRowCount1 == 0)
				{
					v_billet_type = cmd_inq_tpssm03.GetString(1);
					v_slab_dest = cmd_inq_tpssm03.GetString(2);
					v_ingot_code = cmd_inq_tpssm03.GetString(4);

				}
				
				//厚板向特殊处理,按照长坯号块数
				if (v_slab_dest.Trim() == "18")
				{
					v_lslab_no = cmd_inq_tpssm03.GetString(5);
				
					if (v_lslab_no.Trim() != v_lslab_no_pre.Trim())
					{
						v_slab_num = v_slab_num + 1;
					}
					v_lslab_no_pre = v_lslab_no;

					v_plan_slab_num =  v_slab_num;

			
				}
				else
				{
					v_slab_num = cmd_inq_tpssm03.GetDecimal(3);
					v_plan_slab_num = v_plan_slab_num + v_slab_num;
				}
			
					
				fetchRowCount1++;
			}
			cmd_inq_tpssm03.Close();



			sqlstr_tmmsm33 = " SELECT sum(mat_tube)"
				"   FROM tmmsm33 "
				"   WHERE PONO        = @pono";

			cmd_inq_tmmsm33.SetCommandText(sqlstr_tmmsm33);
			cmd_inq_tmmsm33.Parameters.Clear();
			cmd_inq_tmmsm33.Parameters.Set("pono", tpssm11["PONO"].ToString());
			cmd_inq_tmmsm33.ExecuteReader();

			while (cmd_inq_tmmsm33.Read())
			{
				v_cut_slab_num = cmd_inq_tmmsm33.GetDecimal(1);
			}
			cmd_inq_tmmsm33.Close();

			
			if (bcls_ret->Tables[0].Columns.Contains("PROC_NO"))
			{
				bcls_ret->Tables[0].Rows[fetchRowCount2]["PROC_NO"] = tpssm12["PROC_NO"];
			}

			if (bcls_ret->Tables[0].Columns.Contains("START_TIME_REAL"))
			{
				bcls_ret->Tables[0].Rows[fetchRowCount2]["START_TIME_REAL"] = tpssm12["START_TIME_REAL"];
			}

			if (bcls_ret->Tables[0].Columns.Contains("END_TIME_REAL"))
			{
				bcls_ret->Tables[0].Rows[fetchRowCount2]["END_TIME_REAL"] = tpssm12["END_TIME_REAL"];
			}

			if (bcls_ret->Tables[0].Columns.Contains("BILLET_TYPE"))
			{
				bcls_ret->Tables[0].Rows[fetchRowCount2]["BILLET_TYPE"] = v_billet_type;
			}

			if (bcls_ret->Tables[0].Columns.Contains("INGOT_CODE"))
			{
				bcls_ret->Tables[0].Rows[fetchRowCount2]["INGOT_CODE"] = v_ingot_code;
			}

			if (bcls_ret->Tables[0].Columns.Contains("SLAB_PLAN_DEST"))
			{
				bcls_ret->Tables[0].Rows[fetchRowCount2]["SLAB_PLAN_DEST"] = v_slab_dest;
			}

			if (bcls_ret->Tables[0].Columns.Contains("PLAN_SLAB_NUM"))
			{
				bcls_ret->Tables[0].Rows[fetchRowCount2]["PLAN_SLAB_NUM"] = v_plan_slab_num;
			}

			if (bcls_ret->Tables[0].Columns.Contains("CUT_SLAB_NUM"))
			{
				bcls_ret->Tables[0].Rows[fetchRowCount2]["CUT_SLAB_NUM"] = v_cut_slab_num;
			}

			fetchRowCount2++;

		}
		cmd_inq.Close(); //关闭游标

	


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
