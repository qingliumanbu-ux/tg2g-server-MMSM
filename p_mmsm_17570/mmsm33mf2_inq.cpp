 /*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: CC生产实绩查询
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
BM2F_ENTERACE(mmsm33mf2_inq)

int f_mmsm33mf2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr					= "";
	CString sqlstr_count			= "";
	CString sqlstr_temp				= "";
	int		TotalRecordCount		= 0  ;

	CString ch_start_time_f			= "";
	CString ch_start_time_t			= "";
	

	//系统的分页类信息。
	CPageInfo pageInfo; 
 
	CModel tmmsm33("TMMSM33");

	CDbCommand cmd_inq(conn);

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
		tmmsm33.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if(bcls_rec->Tables[0].Columns.Contains("START_TIME_F"))
			ch_start_time_f = bcls_rec->Tables[0].Rows[0]["START_TIME_F"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_T"))
			ch_start_time_t = bcls_rec->Tables[0].Rows[0]["START_TIME_T"].ToString();

		if(ch_start_time_f != "")
		{
			ch_start_time_f = ch_start_time_f.SubstringNE(0,8)+"000000";
		}

		if(ch_start_time_t != "")
		{
			ch_start_time_t = ch_start_time_t.SubstringNE(0,8)+"235959";
		}
		
		/* ***** 打印输入参数 ***** */
		////Log::Info("", __FUNCTION__, "FACTORY_DIV =[{0}]", tmmsm33["FACTORY_DIV"].ToString());
		////Log::Info("", __FUNCTION__, "PONO      =[{0}]", tmmsm33["PONO"].ToString());
		////Log::Info("", __FUNCTION__, "HEAT_NO   =[{0}]", tmmsm33["HEAT_NO"].ToString());
		////Log::Info("", __FUNCTION__, "STATION_NO=[{0}]", tmmsm33["STATION_NO"].ToString());
		////Log::Info("", __FUNCTION__, "CAST_NO     =[{0}]", tmmsm33["CAST_NO"].ToString());
		////Log::Info("", __FUNCTION__, "PROD_SHIFT_NO =[{0}]", tmmsm33["PROD_SHIFT_NO"].ToString());
		////Log::Info("", __FUNCTION__, "PROD_SHIFT_GROUP =[{0}]", tmmsm33["PROD_SHIFT_GROUP"].ToString());
		////Log::Info("", __FUNCTION__, "start_time_f  =[{0}]", ch_start_time_f);
		////Log::Info("", __FUNCTION__, "start_time_t  =[{0}]", ch_start_time_t);


		CString  c_sql_where = "  WHERE  1 = 1 "; //查询条件。
	
		CString c_sql_condition  =  " SELECT *        FROM tmmsm33  t" ; 
		CString c_sql_condition2 =	" SELECT COUNT(1) FROM tmmsm33  t";
		CString c_sql_orderBY    =  " ORDER BY t.START_TIME DESC";

	
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TMMSM33 "
					"  WHERE 1=1 "
					;
				sqlstr	 = " SELECT * "
					"   FROM TMMSM33 "
					"  WHERE 1=1 "
					;

				if (tmmsm33["FACTORY_DIV"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND FACTORY_DIV = @tmmsm33.FACTORY_DIV";
				}
			
				if(tmmsm33["HEAT_NO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND HEAT_NO = @tmmsm33.HEAT_NO"; 
				}
				if(tmmsm33["PONO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND PONO	= @tmmsm33.PONO"; 
				}
				if(tmmsm33["STATION_NO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND STATION_NO	= @tmmsm33.STATION_NO"; 
				}
				if(tmmsm33["CAST_NO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND CAST_NO = @tmmsm33.CAST_NO"; 
				}
				if(tmmsm33["PROD_SHIFT_NO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND PROD_SHIFT_NO	= @tmmsm33.PROD_SHIFT_NO"; 
				}
				if(tmmsm33["PROD_SHIFT_GROUP"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND PROD_SHIFT_GROUP = @tmmsm33.PROD_SHIFT_GROUP"; 
				}				
				if(ch_start_time_f.Trim() != "")
				{
					sqlstr_temp	+= " AND SLAB_CUT_TIME			>= @ch_start_time_f"; 
				}
				if(ch_start_time_t.Trim() != "")
				{
					sqlstr_temp	+= " AND SLAB_CUT_TIME			<= @ch_start_time_t"; 
				}

				sqlstr_temp += " ORDER BY SLAB_CUT_TIME DESC";
				 

				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr		 = sqlstr + sqlstr_temp;
				break;
		}
		   
		cmd_inq.Parameters.Set("tmmsm33.FACTORY_DIV", tmmsm33["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("tmmsm33.HEAT_NO"			, tmmsm33["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm33.PONO"				, tmmsm33["PONO"].ToString());
		cmd_inq.Parameters.Set("tmmsm33.STATION_NO"			, tmmsm33["STATION_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm33.CAST_NO"				, tmmsm33["CAST_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm33.PROD_SHIFT_NO"	,tmmsm33["PROD_SHIFT_NO"].ToString()); 
		cmd_inq.Parameters.Set("tmmsm33.PROD_SHIFT_GROUP",tmmsm33["PROD_SHIFT_GROUP"].ToString()); 
		cmd_inq.Parameters.Set("ch_start_time_f"		 ,ch_start_time_f); 
		cmd_inq.Parameters.Set("ch_start_time_t"		 ,ch_start_time_t); 


		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32(); 
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],pageInfo.RecordFrom,pageInfo.PageSize);
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
