/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-11-25
Description: MMS工序实绩查询(带统计项)
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
/// 1.根据时间范围,炉号等条件进行CC实绩查询。
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
BM2F_ENTERACE(mmsmsjf2_inqm)

int f_mmsmsjf2_inqm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	int     i = 0;
	int     j = 0;

	CString v_start_time_f = "";
	CString v_start_time_t = "";
	CString v_table_type = "";//表名称
	CString v_area_id = "";
	CString v_station_id = "";
	CString v_factory_div_p = "";
	CString v_dev_code_sj = "";

	CString v_dev_code[100] = { "" }; //设备代码数组  100行
	int     v_dev_code_sum[100] = { 0 }; //设备代码对应的总炉数
	CString v_dev_code_sum_e = "";  //各工序炉数英文名

	CDecimal v_dev_code_wt[100] = { 0.0000000000001 }; //
	CString v_dev_code_wt_e = "";  //


	int     v_dev_code_num = 0;   //工序总数

	CString v_factory_div[100] = { "" }; //厂别代码数组  100行
	int     v_factory_div_num = 0;   //工序总数

	CString v_factory_div_sum_e = "";
	int     v_factory_div_sum[100] = { 0 };

	CString v_factory_div_wt_e = "";
	CDecimal  v_factory_div_wt[100] = { 0 };

	CString v_factory_div_e = "";
	CString v_wt_name = "";
	CString v_block_name = "";
	int fetchrowcount = 0;


	CDecimal v_dev_wt_sj = 0;
	int v_dev_block_sj = 0;

	CString v_heat_no = "";


	int blkNum = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;




	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

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

		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			v_factory_div_p = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("AREA_ID"))
			v_area_id = bcls_rec->Tables[0].Rows[0]["AREA_ID"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
			v_station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
			v_table_type = bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_F"))
			v_start_time_f = bcls_rec->Tables[0].Rows[0]["START_TIME_F"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_T"))
			v_start_time_t = bcls_rec->Tables[0].Rows[0]["START_TIME_T"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("WT_NAME"))
			v_wt_name = bcls_rec->Tables[0].Rows[0]["WT_NAME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("BLOCK_NAME"))
			v_block_name = bcls_rec->Tables[0].Rows[0]["BLOCK_NAME"].ToString();

		////Log::Info("", __FUNCTION__, "v_wt_name =[{0}]", v_wt_name);
		////Log::Info("", __FUNCTION__, "v_block_name =[{0}]", v_block_name);




		//表名如果为空，根据传入的工序类型转换为对应的表名
		if (v_table_type.Trim() == "")
		{
			sqlstr = CString("SELECT T.CODE_DESC_3_CONTENT "
				"  FROM  TEP0002 T "
				"  WHERE T.CODE_CLASS = 'PSD1'"
				"  AND   T.CODE = @code");

			cmd_inq.Parameters.Set("code", v_station_id);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				v_table_type = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}



		/* ***** 打印输入参数 ***** */
		////Log::Info("", __FUNCTION__, "v_factory_div_p =[{0}]", v_factory_div_p);
		////Log::Info("", __FUNCTION__, "v_area_id =[{0}]", v_area_id);
		////Log::Info("", __FUNCTION__, "v_station_id =[{0}]", v_station_id);
		////Log::Info("", __FUNCTION__, "v_table_type =[{0}]", v_table_type);
		////Log::Info("", __FUNCTION__, "start_time_f  =[{0}]", v_start_time_f);
		////Log::Info("", __FUNCTION__, "start_time_t  =[{0}]", v_start_time_t);




		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//如果工序代码为空，表示查询所有的精炼表
			if (v_area_id.Trim() == "4" && v_station_id.Trim() == "")
			{

				sqlstr = " SELECT FACTORY_DIV, PROC_NO, HEAT_NO, PONO, ST_NO, START_TIME, END_TIME,STATION_ID,STATION_NO,PROD_SHIFT_NO,PROD_SHIFT_GROUP FROM TMMSM22 "
					" WHERE  FACTORY_DIV = nvl(@factory_div,FACTORY_DIV)"
					" AND START_TIME	>= @start_time_f"
					" AND START_TIME <= @start_time_t";
				sqlstr += " UNION "
					" SELECT FACTORY_DIV, PROC_NO, HEAT_NO, PONO, ST_NO, START_TIME, END_TIME,STATION_ID,STATION_NO,PROD_SHIFT_NO,PROD_SHIFT_GROUP FROM TMMSM23 "
					" WHERE  FACTORY_DIV = nvl(@factory_div,FACTORY_DIV)"
					" AND START_TIME	>= @start_time_f"
					" AND START_TIME <= @start_time_t";


				sqlstr += " UNION "
					" SELECT FACTORY_DIV, PROC_NO, HEAT_NO, PONO, ST_NO, START_TIME, END_TIME,STATION_ID,STATION_NO,PROD_SHIFT_NO,PROD_SHIFT_GROUP FROM TMMSM24 "
					" WHERE  FACTORY_DIV = nvl(@factory_div,FACTORY_DIV)"
					" AND START_TIME	>= @start_time_f"
					" AND START_TIME <= @start_time_t";


				sqlstr += " UNION "
					" SELECT FACTORY_DIV, PROC_NO, HEAT_NO, PONO, ST_NO, START_TIME, END_TIME,STATION_ID,STATION_NO,PROD_SHIFT_NO,PROD_SHIFT_GROUP FROM TMMSM25 "
					" WHERE  FACTORY_DIV = nvl(@factory_div,FACTORY_DIV)"
					" AND START_TIME	>= @start_time_f"
					" AND START_TIME <= @start_time_t";


				////Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);

			}
			else
			{


				sqlstr = " SELECT * FROM " + v_table_type + " WHERE  FACTORY_DIV = nvl(@factory_div, FACTORY_DIV)"
					" AND START_TIME	>= @start_time_f"
					" AND START_TIME <= @start_time_t";

			}


			if (v_area_id == '6')
			{
				sqlstr_temp += " ORDER BY MAT_NO DESC";
			}
			else
			{
				sqlstr_temp += " ORDER BY HEAT_NO DESC";
			}


			sqlstr = sqlstr + sqlstr_temp;

			sqlstr_count = "SELECT COUNT(*) FROM ( " + sqlstr + " )";

			//Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			break;
		}

		cmd_inq.Parameters.Set("factory_div", v_factory_div_p);
		cmd_inq.Parameters.Set("start_time_f", v_start_time_f);
		cmd_inq.Parameters.Set("start_time_t", v_start_time_t);

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();



		//-------------------------------------------------------
		//将设备代码放入数组，以提高效率
		sqlstr = CString("SELECT  DISTINCT(T.DEV_CODE)  "
			"  FROM  TPSSMD1 T "
			"  WHERE T.AREA_ID = @area_id "
			" AND    T.STATION_ID = NVL(@station_id,STATION_ID)"
			"  ORDER BY T.DEV_CODE ");

		cmd_inq.Parameters.Set("area_id", v_area_id);
		cmd_inq.Parameters.Set("station_id", v_station_id);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			v_dev_code[i] = cmd_inq.GetString(1);

			v_dev_code_num++;    //该厂别的设备个数
			i++;

			//Log::Trace(" ", __FUNCTION__, "v_dev_code[i]=[{0}]", v_dev_code[i]);
		}
		cmd_inq.Close();

		//Log::Trace(" ", __FUNCTION__, "v_dev_code_num =[{0}]", v_dev_code_num);


		sqlstr = CString("SELECT CODE "
			"  FROM  TEP0002  "
			"  WHERE CODE_CLASS = 'M00F' "
			"  AND  CODE_DESC_2_CONTENT = 'SM'");


		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			v_factory_div[j] = cmd_inq.GetString(1);
			//Log::Trace(" ", __FUNCTION__, "v_factory_div[j] =[{0}]", v_factory_div[j]);

			v_factory_div_num++;    //几个炼钢厂
			j++;
		}
		cmd_inq.Close();

		//Log::Trace(" ", __FUNCTION__, "v_factory_div_num =[{0}]", v_factory_div_num);


		//定义返回参数

		blkNum = bcls_ret->Tables.IndexOf("MMSMTJ");
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("MMSMTJ");
		}

		for (i = 0; i < v_dev_code_num; i++)
		{

			v_dev_code_sum_e = v_dev_code[i] + "_SUM"; //每个设备的炉次总数(即为前台ED54功能号对应的英文名)

			bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, v_dev_code_sum_e);

			if (v_wt_name.Trim() != "")
			{
				v_dev_code_wt_e = v_dev_code[i] + "_WT"; //每个设备的良坯量

				bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, v_dev_code_wt_e);
			}

		}

		for (j = 0; j < v_factory_div_num; j++)
		{
			v_factory_div_sum_e = v_factory_div[j] + "_SUM";

			//Log::Trace(" ", __FUNCTION__, "v_factory_div_sum_e =[{0}]", v_factory_div_sum_e);

			bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, v_factory_div_sum_e);

			if (v_wt_name.Trim() != "")
			{
				v_factory_div_wt_e = v_factory_div[j] + "_WT";

				bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, v_factory_div_wt_e);
			}
		}


		bcls_ret->Tables["MMSMTJ"].Rows.Add(); // 创建一行

		for (j = 0; j < v_factory_div_num; j++)
		{

			//Log::Trace(" ", __FUNCTION__, "j =[{0}]", j);

			v_factory_div_e = v_factory_div[j];

			v_factory_div_sum[j] = 0;
			v_factory_div_wt[j] = 0;

			//Log::Trace(" ", __FUNCTION__, "v_factory_div_e =[{0}]", v_factory_div_e);

			//计算炉数
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:


				if (v_area_id.Trim() == "4" && v_station_id.Trim() == "")
				{

					sqlstr = "SELECT STATION_ID||STATION_NO,PROC_NO FROM TMMSM22"
						" WHERE  FACTORY_DIV = nvl(@factory_div,FACTORY_DIV)"
						" AND START_TIME	>= @start_time_f"
						" AND START_TIME <= @start_time_t";
					sqlstr += " UNION  "
						"SELECT  STATION_ID||STATION_NO,PROC_NO FROM TMMSM23"
						" WHERE  FACTORY_DIV = nvl(@factory_div,FACTORY_DIV)"
						" AND START_TIME	>= @start_time_f"
						" AND START_TIME <= @start_time_t";
					sqlstr += " UNION  "
						"SELECT  STATION_ID||STATION_NO,PROC_NO FROM TMMSM24"
						" WHERE  FACTORY_DIV = nvl(@factory_div,FACTORY_DIV)"
						" AND START_TIME	>= @start_time_f"
						" AND START_TIME <= @start_time_t";
					sqlstr += " UNION  "
						"SELECT STATION_ID||STATION_NO ,PROC_NO  FROM TMMSM25"
						" WHERE  FACTORY_DIV = nvl(@factory_div,FACTORY_DIV)"
						" AND START_TIME	>= @start_time_f"
						" AND START_TIME <= @start_time_t";


					////Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);

				}
				else if (v_area_id == '6')
				{
					sqlstr = " SELECT  MAT_NO,MAT_ACT_WT, MAT_TUBE  FROM " + v_table_type + " WHERE  FACTORY_DIV = nvl(@factory_div, FACTORY_DIV)"
						" AND START_TIME	>= @start_time_f"
						" AND START_TIME <= @start_time_t";
				}
				else
				{
					if (v_block_name.Trim() != "") //切断
					{
						sqlstr = " SELECT  STATION_ID||STATION_NO,SLAB_WT, MAT_TUBE  FROM " + v_table_type + " WHERE  FACTORY_DIV = nvl(@factory_div, FACTORY_DIV)"
							" AND START_TIME	>= @start_time_f"
							" AND START_TIME <= @start_time_t";

					}
					else
					{

						sqlstr = " SELECT STATION_ID||STATION_NO  FROM " + v_table_type + " WHERE  FACTORY_DIV = nvl(@factory_div, FACTORY_DIV)"
							//  " AND FACTORY_DIV = nvl(@factory_div_p, FACTORY_DIV) "
							" AND START_TIME	>= @start_time_f"
							" AND START_TIME <= @start_time_t"
							" ORDER BY HEAT_NO";



					}

				}


				//Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

				break;
			}

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("factory_div", v_factory_div_e);
			cmd_inq.Parameters.Set("start_time_f", v_start_time_f);
			cmd_inq.Parameters.Set("start_time_t", v_start_time_t);
			cmd_inq.ExecuteReader();

			while (cmd_inq.Read())
			{

				fetchrowcount++;
				v_dev_code_sj = cmd_inq.GetString(1);

				if (v_area_id == '6' || v_block_name.Trim() != "")
				{
					v_dev_wt_sj = cmd_inq.GetDecimal(2);
					v_dev_block_sj = cmd_inq.GetInt16(3);
				}


				//Log::Trace(" ", __FUNCTION__, "fetchrowcount =[{0}]", fetchrowcount);
				//Log::Trace(" ", __FUNCTION__, "fetchrowcount =[{0}]", fetchrowcount);
				//Log::Trace(" ", __FUNCTION__, "v_dev_code_sj =[{0}]", v_dev_code_sj);
				/*//Log::Trace(" ", __FUNCTION__, "v_dev_wt_sj =[{0}]", v_dev_wt_sj);
				//Log::Trace(" ", __FUNCTION__, "v_dev_block_sj =[{0}]", v_dev_block_sj);*/

				v_factory_div_sum[j] ++;

				v_factory_div_wt[j] = v_factory_div_wt[j] + v_dev_wt_sj;


				//Log::Trace(" ", __FUNCTION__, "v_dev_code_num =[{0}]", v_dev_code_num);

				//统计各工序炉次数量

				for (i = 0; i < v_dev_code_num; i++)
				{
					//Log::Trace(" ", __FUNCTION__, "v_dev_code[i] =[{0}]", v_dev_code[i]);

					if (v_dev_code_sj == v_dev_code[i])
					{
						if (v_block_name.Trim() != "")//块数
						{
							v_dev_code_sum[i] = v_dev_code_sum[i] + v_dev_block_sj;
						}
						else
						{
							v_dev_code_sum[i] ++;   //炉数
						}

						if (v_wt_name.Trim() != "")//重量
						{

							v_dev_code_wt[i] = v_dev_code_wt[i] + v_dev_wt_sj;

						}

					}

					/*	//Log::Trace(" ", __FUNCTION__, "v_dev_code[i] =[{0}]", v_dev_code[i]);
					//Log::Trace(" ", __FUNCTION__, "v_dev_code_sum[i] =[{0}]", v_dev_code_sum[i]);
					//Log::Trace(" ", __FUNCTION__, "v_dev_code_wt[i] =[{0}]", v_dev_code_wt[i]);*/


				}

			}
			cmd_inq.Close();

		}




		for (i = 0; i < v_dev_code_num; i++)
		{
			v_dev_code_sum_e = v_dev_code[i] + "_SUM"; //每个设备的炉次总数或者块数

			if (bcls_ret->Tables["MMSMTJ"].Columns.Contains(v_dev_code_sum_e))
			{
				bcls_ret->Tables["MMSMTJ"].Rows[0][v_dev_code_sum_e] = v_dev_code_sum[i];


			}

			v_dev_code_wt_e = v_dev_code[i] + "_WT"; //每个设备的重量

			if (bcls_ret->Tables["MMSMTJ"].Columns.Contains(v_dev_code_wt_e))
			{
				bcls_ret->Tables["MMSMTJ"].Rows[0][v_dev_code_wt_e] = v_dev_code_wt[i];

				/*	//Log::Trace(" ", __FUNCTION__, "i=[{0}]", i);
				//Log::Trace(" ", __FUNCTION__, "v_dev_code_wt[i] =[{0}]", v_dev_code_wt[i]);*/
			}

		}

		for (j = 0; j < v_factory_div_num; j++)
		{
			v_factory_div_sum_e = v_factory_div[j] + "_SUM"; //每个设备的炉次总数

			if (bcls_ret->Tables["MMSMTJ"].Columns.Contains(v_factory_div_sum_e))
			{
				bcls_ret->Tables["MMSMTJ"].Rows[0][v_factory_div_sum_e] = v_factory_div_sum[j];
			}

			v_factory_div_wt_e = v_factory_div[j] + "_WT";

			if (bcls_ret->Tables["MMSMTJ"].Columns.Contains(v_factory_div_wt_e))
			{
				bcls_ret->Tables["MMSMTJ"].Rows[0][v_factory_div_wt_e] = v_factory_div_wt[j];
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
