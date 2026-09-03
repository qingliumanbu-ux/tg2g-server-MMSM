/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     20145-11-25
Description: 工序实绩查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */
int f_mmsm_confirm_flag(const CString& factory_div, const CString& heat_no, CString& heat_confirm_flag, CDbConnection * conn);


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
BM2F_ENTERACE(mmsmsjdc_inq)

int f_mmsmsjdc_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstrsa = " ";
	CString sqlstrsb = " ";
	CString sqlstr_sub = " ";
	CString sqlstr_sub1 = " ";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	int     i = 0;

	CString ch_start_time_f = "";
	CString ch_start_time_t = "";
	CString v_table_type = "";//表名称。
	CString v_proc_div = "";
	CString v_heat_no = "";
	CString v_heat_confirm_flag = "";


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm00("TMMSM00");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inqsa(conn);
	CDbCommand cmd_inqsb(conn);
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
		tmmsm00.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
			v_table_type = bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
			v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();

		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_F"))
			ch_start_time_f = bcls_rec->Tables[0].Rows[0]["START_TIME_F"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_T"))
			ch_start_time_t = bcls_rec->Tables[0].Rows[0]["START_TIME_T"].ToString().Trim();

		if (v_table_type.Trim() == "")
		{
			sprintf(s.msg, "【表名称】不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/* 设置开始时刻和结束时刻 */
		if (ch_start_time_f.Trim() != "")
		{
			ch_start_time_f = ch_start_time_f.Substring(0, 8);
			ch_start_time_f += "000000";
		}
		if (ch_start_time_t.Trim() != "")
		{
			ch_start_time_t = ch_start_time_t.Substring(0, 8);
			ch_start_time_t += "235959";
		}


		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "FACTORY_DIV =[{0}]", tmmsm00["FACTORY_DIV"].ToString());
		Log::Info("", __FUNCTION__, "PONO      =[{0}]", tmmsm00["PONO"].ToString());
		Log::Info("", __FUNCTION__, "HEAT_NO   =[{0}]", tmmsm00["HEAT_NO"].ToString());
		Log::Info("", __FUNCTION__, "PROC_NO   =[{0}]", tmmsm00["PROC_NO"].ToString());
		Log::Info("", __FUNCTION__, "STATION_NO=[{0}]", tmmsm00["STATION_NO"].ToString());
		////Log::Info("", __FUNCTION__, "PROD_SHIFT_NO =[{0}]", tmmsm00["PROD_SHIFT_NO"].ToString());
		////Log::Info("", __FUNCTION__, "PROD_SHIFT_GROUP =[{0}]", tmmsm00["PROD_SHIFT_GROUP"].ToString());
		////Log::Info("", __FUNCTION__, "st_no  =[{0}]", tmmsm00["ST_NO"].ToString());
		Log::Info("", __FUNCTION__, "start_time_f  =[{0}]", ch_start_time_f);
		Log::Info("", __FUNCTION__, "start_time_t  =[{0}]", ch_start_time_t);


		if (v_proc_div.Trim() != "")
		{
			if (tmmsm00["HEAT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "熔炼号不能为空，请先按F2查询!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}



		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//取消耗项，然后将消耗项设置为横向
			sqlstrsa = " select distinct mat_code from TMMSM2A"
				" where 1=1";
			cmd_inqsa.SetCommandText(sqlstrsa);
			cmd_inqsa.ExecuteReader();
			while (cmd_inqsa.Read())
			{
				sqlstr_sub = sqlstr_sub + ",sum(case when mat_code ='" + cmd_inqsa.GetString(1) + "' then DEVO_WT else 0 end) as use_" + cmd_inqsa.GetString(1);
			}
			cmd_inqsa.Close();

			//取测温项，然后将测温项设置为横向
			sqlstrsb = " select distinct PROC_COUNT from TMMSM2B "
				" where 1=1";
			cmd_inqsb.SetCommandText(sqlstrsb);
			cmd_inqsb.ExecuteReader();
			while (cmd_inqsb.Read())
			{
				sqlstr_sub1 = sqlstr_sub1 + ",max(case when PROC_COUNT ='" + cmd_inqsb.GetString(1) + "' then STEEL_TEMP else 0 end) as count_" + cmd_inqsb.GetString(1);
			}
			cmd_inqsb.Close();

			sqlstr = " select t1.*, t2.*, T3.* "
				" from " + v_table_type + " t1 "
				" left join( "
				" select heat_no as HEAT_NO2 " + sqlstr_sub + " "
				" from TMMSM2A  "
				" where 1 = 1 "
				" group by heat_no "
				" ) t2 on t1.heat_no = t2.HEAT_NO2 "
				" left join( "
				" select heat_no as HEAT_NO3 " + sqlstr_sub1 + " "
				" from TMMSM2B "
				" where 1 = 1 "
				" group by heat_no "
				" ) T3 on T2.HEAT_NO2 = T3.HEAT_NO3 "
				" where 1 = 1 ";

			sqlstr_count = " SELECT COUNT(*)  FROM " + v_table_type + " WHERE 1=1 ";
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);

			if (tmmsm00["HEAT_NO"].ToString().Trim() != "")
			{
				if (v_proc_div.Trim() == "")
				{
					sqlstr_temp += " AND T1.HEAT_NO			= '" + tmmsm00["HEAT_NO"].ToString() + "'  ";
				}
				else if (v_proc_div.Trim() == "UP")
				{
					sqlstr_temp += " AND T1.HEAT_NO = (SELECT MAX(HEAT_NO) FROM " + v_table_type + " WHERE HEAT_NO < @tmmsm00.HEAT_NO)";
				}
				else if (v_proc_div.Trim() == "DOWN")
				{
					sqlstr_temp += " AND T1.HEAT_NO = (SELECT MIN(HEAT_NO) FROM " + v_table_type + " WHERE HEAT_NO > @tmmsm00.HEAT_NO) ";
				}
			}

			if (tmmsm00["FACTORY_DIV"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND T1.FACTORY_DIV = @tmmsm00.FACTORY_DIV";
			}

			if (tmmsm00["PONO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND T1.PONO = @tmmsm00.PONO";
			}

			if (tmmsm00["PROC_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND T1.PROC_NO = @tmmsm00.PROC_NO";
			}

			if (tmmsm00["STATION_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND T1.STATION_NO	= @tmmsm00.STATION_NO";
			}
			if (tmmsm00["PROD_SHIFT_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND T1.PROD_SHIFT_NO	= @tmmsm00.PROD_SHIFT_NO";
			}
			if (tmmsm00["PROD_SHIFT_GROUP"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND T1.PROD_SHIFT_GROUP= @tmmsm00.PROD_SHIFT_GROUP";
			}
			if (tmmsm00["ST_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND  T1.ST_NO			= @tmmsm00.ST_NO ";
			}
			if (ch_start_time_f.Trim() != "")
			{
				sqlstr_temp += " AND T1.START_TIME			>= '" + ch_start_time_f + "'";
			}
			if (ch_start_time_t.Trim() != "")
			{
				sqlstr_temp += " AND T1.START_TIME			<= '" + ch_start_time_t + "'";
			}


			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr = sqlstr + sqlstr_temp;

			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			break;
		}

		cmd_inq.Parameters.Set("tmmsm00.FACTORY_DIV", tmmsm00["FACTORY_DIV"].ToString());
		//cmd_inq.Parameters.Set("tmmsm00.HEAT_NO", tmmsm00["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm00.PONO", tmmsm00["PONO"].ToString());
		cmd_inq.Parameters.Set("tmmsm00.PROC_NO", tmmsm00["PROC_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm00.STATION_NO", tmmsm00["STATION_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm00.PROD_SHIFT_NO", tmmsm00["PROD_SHIFT_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm00.PROD_SHIFT_GROUP", tmmsm00["PROD_SHIFT_GROUP"].ToString());
		cmd_inq.Parameters.Set("tmmsm00.ST_NO", tmmsm00["ST_NO"].ToString());


		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
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
