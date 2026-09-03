/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    3.0
Date:     2014-05-8
Description: 查询可以做预处理实绩后备的计划信息
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
 

/***** C++ 的业务头文件部分 *****/ 
//#include "SQLDDL.h"




/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
///<summary>
///炼钢实绩新增-计划信息查询
///<para>
///1.根据前台传入的条件查询计划信息
///</para>
///<para>数据库表TPSSM12(炼钢作业计划编制子表)</para>
///</summary>
///<param name="FACTORY_DIV">主工序代码</param>
///<param name="STATION_ID">工位代码</param>
///<param name="STATION_NO">工位号</param>
///<returns>指定条件下的计划信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsmplanno_inq)

int f_mmsmplanno_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量***** */
	int doFlag = 0;

	int fetchRowCount;
	int blknum;
	int count = 0;
	CString v_factory_div = "";
	CString v_proc_no = "";
	CString v_pono    = "";
	CString v_station_id = "";
	CString v_station_no = "";
	CString v_dev_code = "";
	CDecimal  v_area_id = 0;
	CString v_table_name = "";
	CString v_item_name1 = "";
	CString v_item_name2 = "";
	CString v_slab_dest = "";
	CString v_hot_send_flag= "";
	CDecimal  v_slab_num = 0;
	CString   v_slab_dest_code = "";
	CString   v_slab_plan_dest = "";



	CString sqlstr = "";
	CString sqlstr2 = "";
	CString  sqlstr_inq = "";
	CString  sqlstr_total = "";
	CString  sqlstr_where = "";

	int     v_total_count    = 0;


	CDbCommand cmd_inq(conn); //与DB 建立连接。
	CDbCommand cmd_sql(conn); //与DB 建立连接。
  
	CModel tpssm11("TPSSM11");


	try
	{
		
		//获得输入参数
		//============
		if(bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
					
		 //Log::Info("", __FUNCTION__, "v_factory_div  =[{0}]", v_factory_div);
				
		sqlstr_inq = " SELECT * FROM TPSSM11 ";
						
		//sqlstr_where = " AND b."+ v_item_name1+ " NOT IN (SELECT "+ v_item_name2 + " FROM " + v_table_name+ " WHERE 1 = 1 )";

		sqlstr = sqlstr_inq + sqlstr_where;
		//Log::Info("", __FUNCTION__, "sqlstr      =[{0}]", sqlstr);
		
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Set("factory_div", v_factory_div);
		cmd_sql.ExecuteReader(); //执行读取

		
    if(!bcls_ret->Tables[0].Columns.Contains("CODE"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"CODE"); 
		}
		
			
		if(!bcls_ret->Tables[0].Columns.Contains("PONO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"PONO"); 
		}
		if(!bcls_ret->Tables[0].Columns.Contains("ST_NO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"ST_NO"); 
		}
	
			
	
		//相关信息初始化。
		//===============
		fetchRowCount = 0;
		while(cmd_sql.Read()) //只读取单记录，可用IF 语句。
		{		
			
			cmd_sql.Fetch(tpssm11);		 //整个表结构的获取。
				
			//Log::Info("", __FUNCTION__, " fetchRowCount =[{0}]",  fetchRowCount);

			//Log::Info("", __FUNCTION__, " tpssm11.PONO  =[{0}]",  tpssm11["PONO"].ToString());
			//Log::Info("", __FUNCTION__, " tpssm11.HEAT_NO  =[{0}]",  tpssm11["HEAT_NO"].ToString());
		
			//Log::Info("", __FUNCTION__, " tpssm11.ST_NO  =[{0}]",  tpssm11["ST_NO"].ToString());
	

		
			bcls_ret->Tables[0].Rows.Add();

			
			bcls_ret->Tables[0].Rows[fetchRowCount]["PONO"] = tpssm11["PONO"];
			bcls_ret->Tables[0].Rows[fetchRowCount]["ST_NO"] = tpssm11["ST_NO"]; 
			bcls_ret->Tables[0].Rows[fetchRowCount]["CODE"] = tpssm11["SM_PLAN_NO"];
	
		
							
			fetchRowCount++;
			

		}

		if(fetchRowCount==0)
		{
			bcls_ret->Tables[0].Rows.Add();
		}

		cmd_sql.Close(); //关闭游标



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

