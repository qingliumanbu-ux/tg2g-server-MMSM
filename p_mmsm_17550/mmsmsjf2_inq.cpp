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
BM2F_ENTERACE(mmsmsjf2_inq)

int f_mmsmsjf2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr					= "";
	CString sqlstr_count			= "";
	CString sqlstr_temp				= "";
	int		TotalRecordCount		= 0  ;
	int     i = 0;

	CString ch_start_time_f			= "";
	CString ch_start_time_t			= "";
	CString ch_end_time_f = "";
	CString ch_end_time_t = "";
	CString v_table_type = "";//表名称。
	CString v_proc_div = "";
	CString v_heat_no = "";
	CString v_st_no = "";
	CString l2_proc_no = "";
	CString v_heat_confirm_flag = "";
	CString h_mmsm = "";
	

 
	CModel tmmsm00("TMMSM00");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	try
	{



		//--------------------------------
		//获取传入参数
		tmmsm00.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
			v_table_type = bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
			v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("L2_PROC_NO"))
		l2_proc_no = bcls_rec->Tables[0].Rows[0]["L2_PROC_NO"].ToString();
		if(bcls_rec->Tables[0].Columns.Contains("START_TIME_F"))
			ch_start_time_f = bcls_rec->Tables[0].Rows[0]["START_TIME_F"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_T"))
			ch_start_time_t = bcls_rec->Tables[0].Rows[0]["START_TIME_T"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("H_MMSM"))
			h_mmsm = bcls_rec->Tables[0].Rows[0]["H_MMSM"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_F"))
			ch_end_time_f = bcls_rec->Tables[0].Rows[0]["END_TIME_F"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_F"))
			ch_end_time_f = bcls_rec->Tables[0].Rows[0]["END_TIME_F"].ToString().Trim();
		Log::Info("", __FUNCTION__, "h_mmsm =[{0}]", h_mmsm);
		if (v_table_type.Trim() == "")
		{
			sprintf(s.msg, "【表名称】不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		} 
		if (h_mmsm == "1"){
			CString d = v_table_type.Substring(1, 6);
			v_table_type = "H" + d;
		}
		/* 设置开始时刻和结束时刻 */
		if (ch_start_time_f.Trim() != "")
		{
			ch_start_time_f = ch_start_time_f.Substring(0,8);
			ch_start_time_f += "000000";
		}
		if (ch_start_time_t.Trim() != "")
		{
			ch_start_time_t = ch_start_time_t.Substring(0, 8);
			ch_start_time_t += "235959";
		}
		//马建国要求
		if (ch_end_time_f.Trim() != "")
		{
			ch_end_time_f = ch_end_time_f.Substring(0, 8);
			ch_end_time_f += "000000";
		}
		if (ch_end_time_t.Trim() != "")
		{
			ch_end_time_t = ch_end_time_t.Substring(0, 8);
			ch_end_time_t += "235959";
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

		
	
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1)  FROM " + v_table_type + " WHERE 1=1 ";
				sqlstr = " SELECT *  FROM " + v_table_type + " WHERE 1=1 ";

			
				if (tmmsm00["HEAT_NO"].ToString().Trim() != "")
				{
					if (v_proc_div.Trim() == "")
					{
						sqlstr_temp += " AND HEAT_NO			= @tmmsm00.HEAT_NO";
					}
					else if (v_proc_div.Trim() == "UP")
					{
						sqlstr_temp += " AND HEAT_NO = (SELECT MAX(HEAT_NO) FROM " + v_table_type + " WHERE HEAT_NO < @tmmsm00.HEAT_NO)";
					}
					else if (v_proc_div.Trim() == "DOWN")
					{
						sqlstr_temp += " AND HEAT_NO = (SELECT MIN(HEAT_NO) FROM " + v_table_type + " WHERE HEAT_NO > @tmmsm00.HEAT_NO) ";
					}
				}

				if (tmmsm00["FACTORY_DIV"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND FACTORY_DIV = @tmmsm00.FACTORY_DIV";
				}
				
				if(tmmsm00["PONO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND PONO = @tmmsm00.PONO"; 
				}

				if (tmmsm00["PROC_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND PROC_NO = @tmmsm00.PROC_NO";
				}

				if(tmmsm00["STATION_NO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND STATION_NO	= @tmmsm00.STATION_NO"; 
				}
				if (tmmsm00["DEV_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND DEV_CODE	= @tmmsm00.DEV_CODE";
				}
				if(tmmsm00["PROD_SHIFT_NO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND PROD_SHIFT_NO	= @tmmsm00.PROD_SHIFT_NO"; 
				}
				if(tmmsm00["PROD_SHIFT_GROUP"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND PROD_SHIFT_GROUP= @tmmsm00.PROD_SHIFT_GROUP"; 
				}		
				if (tmmsm00["ST_NO"].ToString().Trim() != "")
				{
					v_st_no = tmmsm00["ST_NO"].ToString().Trim();
					sqlstr_temp += " AND  ST_NO	 LIKE '" + v_st_no + "%'";
					
				}
				
				if(ch_start_time_f.Trim() != "")
				{
					sqlstr_temp	+= " AND START_TIME			>= @ch_start_time_f"; 
				}
				if(ch_start_time_t.Trim() != "")
				{
					sqlstr_temp	+= " AND START_TIME			<= @ch_start_time_t"; 
				}
				if (l2_proc_no.Trim() != ""){
					sqlstr_temp += " AND L2_PROC_NO			=@L2_PROC_NO";
				}
				if (ch_end_time_f.Trim() != "")
				{
					sqlstr_temp += " AND END_TIME			>= @ch_end_time_f";
				}
				if (ch_end_time_t.Trim() != "")
				{
					sqlstr_temp += " AND END_TIME			<= @ch_end_time_t";
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;

				sqlstr_temp += " ORDER BY START_TIME DESC";
				sqlstr		 = sqlstr + sqlstr_temp;

				Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
				break;
		}
		   
		cmd_inq.Parameters.Set("tmmsm00.FACTORY_DIV", tmmsm00["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("tmmsm00.HEAT_NO"	, tmmsm00["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm00.PONO"		, tmmsm00["PONO"].ToString());
		cmd_inq.Parameters.Set("tmmsm00.PROC_NO", tmmsm00["PROC_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm00.STATION_NO"	, tmmsm00["STATION_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm00.DEV_CODE", tmmsm00["DEV_CODE"].ToString());
		cmd_inq.Parameters.Set("tmmsm00.PROD_SHIFT_NO"	,tmmsm00["PROD_SHIFT_NO"].ToString()); 
		cmd_inq.Parameters.Set("tmmsm00.PROD_SHIFT_GROUP",tmmsm00["PROD_SHIFT_GROUP"].ToString()); 
		//cmd_inq.Parameters.Set("tmmsm00.ST_NO", tmmsm00["ST_NO"].ToString());
		cmd_inq.Parameters.Set("ch_start_time_f"		 ,ch_start_time_f); 
		cmd_inq.Parameters.Set("ch_start_time_t"		 ,ch_start_time_t); 
		cmd_inq.Parameters.Set("ch_end_time_f", ch_end_time_f);
		cmd_inq.Parameters.Set("ch_end_time_t", ch_end_time_t);
		//L2_PROC_NO
		cmd_inq.Parameters.Set("L2_PROC_NO", l2_proc_no);
		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32(); 
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();


		//增加非表中字段
		if (!bcls_ret->Tables[0].Columns.Contains("HEAT_CONFIRM_FLAG"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_CONFIRM_FLAG");
		}


		for (i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			v_heat_no = bcls_ret->Tables[0].Rows[i]["HEAT_NO"].ToString();

			////Log::Info("", __FUNCTION__, "v_heat_no =[{0}]", v_heat_no);
			
			//炉次确定标记
			doFlag = f_mmsm_confirm_flag(tmmsm00["FACTORY_DIV"].ToString(), v_heat_no, v_heat_confirm_flag, conn);

			////Log::Info("", __FUNCTION__, "v_heat_confirm_flag =[{0}]", v_heat_confirm_flag);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			bcls_ret->Tables[0].Rows[i]["HEAT_CONFIRM_FLAG"] = v_heat_confirm_flag;
		}
		


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
