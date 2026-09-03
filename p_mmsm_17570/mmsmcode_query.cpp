/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2015-08-28
Version:1.0
Description: 精整相关信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */


/*<remark>=========================================================
/// <summary>
通过关键字，查询小代码信息
/// <returns>板坯信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsmcode_query)


int f_mmsmcode_query(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int rowCount = 0;
	CString sqlstr = "";
	CString key = "";
	
	CDbCommand cmd_sql(conn); //与DB 建立连接。


	try
	{	
		if (bcls_rec->Tables[0].Columns.Contains("KEY"))
		{
			key = bcls_rec->Tables[0].Rows[0]["KEY"].ToString();
			//库区号 小代码
			if (key=="STOCK_CODE")
			{
				sqlstr = "SELECT STOCK_NO,STOCK_NO_ANOTHER,stock_desc FROM TWM01 where 1=1";
			}
			//装点 小代码
			else if (key=="LOAD_CODE")
			{
				sqlstr = "SELECT LOAD_CODE,LOAD_NAME FROM TWMSM61A where 1=1 order by LOAD_CODE";
			}
			//卸点 小代码
			else if (key=="UNLOAD_CODE")
			{
				sqlstr = "SELECT UNLOAD_CODE,UNLOAD_NAME FROM TWMSM62A where 1=1 order by UNLOAD_CODE";
			}
			//车号 小代码
			else if (key=="VEHICLE_CODE")
			{
				sqlstr = "select CODE,CODE_DESC_1_CONTENT from TWMSMZD02  where CODE_CLASS='WM01'";
			}
			else if (key=="STOCK_NO")
			{
				CString value = bcls_rec->Tables[0].Rows[0]["VALUE"].ToString();
				sqlstr = " select stock_place_no,CODE_DESC,STOCK_NO from twm04 where 1=1 and STOCK_NO = '" + value + "' order by stock_place_no";
			}
			//金属去除速度和修磨工艺要求
			else if (key=="METALRATE")
			{
				CString value = bcls_rec->Tables[0].Rows[0]["VALUE"].ToString();
				//修磨机组不同区分修磨工艺要求
				CString mend_set = bcls_rec->Tables[0].Rows[0]["MEND_SET"].ToString();
				Log::Trace("", "", "mend_set={0}", mend_set);
				if (mend_set == "H1")
				{
					sqlstr = " SELECT t.CODE_DESC_1_CONTENT ,t.CODE_CLASS FROM TWMSMZD02 t WHERE 1 =1 and  (t.CODE_CLASS = 'METALRATE' or t.CODE_CLASS = 'PROCESS_S')  and t.CODE = '" + value + "' ";
				}
				else if (mend_set == "H5" || mend_set == "C3" || mend_set == "C7")
				{
					sqlstr = " SELECT t.CODE_DESC_2_CONTENT as CODE_DESC_1_CONTENT ,t.CODE_CLASS FROM TWMSMZD02 t WHERE 1 =1 and  (t.CODE_CLASS = 'METALRATE' or t.CODE_CLASS = 'PROCESS_S')  and t.CODE = '" + value + "' ";
				}
				else
				{
					sqlstr = " SELECT ' ' as CODE_DESC_1_CONTENT ,t.CODE_CLASS FROM TWMSMZD02 t WHERE 1 =1 and  (t.CODE_CLASS = 'METALRATE' or t.CODE_CLASS = 'PROCESS_S')  and t.CODE = '" + value + "' ";
				}
				/*{
					sqlstr = " SELECT t.CODE_DESC_2_CONTENT as CODE_DESC_1_CONTENT ,t.CODE_CLASS FROM TWMSMZD02 t WHERE 1 =1 and  (t.CODE_CLASS = 'METALRATE' or t.CODE_CLASS = 'PROCESS_S')  and t.CODE = '" + value + "' ";
				}*/
				
			}
			//砂轮使用要求和修磨放置时间要求
			else if (key == "WHEEL")
			{
				CString value = bcls_rec->Tables[0].Rows[0]["VALUE"].ToString();
				Log::Trace("", "", "WHEEL={0}", value);
				sqlstr = " SELECT t.CODE_DESC_3_CONTENT ,t.CODE_DESC_4_CONTENT FROM TWMSMZD02 t WHERE 1 =1 and t.CODE_CLASS = 'PROCESS_S' and t.CODE = '" + value + "' ";
			}
			else if (key == "IP")
			{
				//CString value = bcls_rec->Tables[0].Rows[0]["VALUE"].ToString();
				CString value = s.fore_ip;
				CString name = s.username;

				CString prodTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
				CString PROD_SHIFT_NO = "";
				CString PROD_SHIFT_GROUP = "";
				f_epep_get_shift_group("SMCP", prodTime, PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
				CString ipCount = " select count(*)  FROM TWMSMZD02 t  WHERE 1 = 1  AND t.CODE_CLASS = 'MEND_SET_IP'  AND t.CODE = '" + value + "' ";
				cmd_sql.SetCommandText(ipCount);
				CDecimal cd_count = cmd_sql.ExecuteScalar();
				Log::Trace("", "", "查询条数={0}", cd_count);
				if (cd_count==0)
				{
					value = "10.162.72.222";
					Log::Trace("", "", "IP={0}", value);
				}
				else
				{
					Log::Trace("", "", "IP={0}", value);
				}

				sqlstr = " SELECT t.CODE_DESC_1_CONTENT,T.CODE_DESC_2_CONTENT ,'" + name + "' as CODE_DESC_3_CONTENT , '" + PROD_SHIFT_GROUP + "'as CODE_DESC_4_CONTENT  FROM TWMSMZD02 t  WHERE 1 = 1  AND t.CODE_CLASS = 'MEND_SET_IP'  AND t.CODE = '" + value + "' ";
				
				
				
			}
			//指导去向
			else if (key=="GUIDE_DEST")
			{
				sqlstr = "select CODE,CODE_DESC_1_CONTENT from TWMSMZD02  where CODE_CLASS='WM02'";
			}
			//表面质量
			else if (key == "SURF_QUALITY")
			{
				sqlstr = "select CODE,CODE_DESC_1_CONTENT from TWMSMZD02  where CODE_CLASS='MMBMZL'";
			}
			//装点
			else if (key == "LOAD_CODE")
			{
				sqlstr = " SELECT LOAD_CODE,LOAD_NAME FROM TWMSM61A where 1=1 order by LOAD_CODE ";
			}
			//卸点
			else if (key == "UNLOAD_CODE")
			{
				sqlstr = " SELECT UNLOAD_CODE,UNLOAD_NAME FROM TWMSM62A where 1=1 order by UNLOAD_CODE ";
			}
			//
			else if (key=="LC03")
			{
				sqlstr = " select code,CODE_DESC_1_CONTENT from TEP0002 t where t.CODE_CLASS = 'LC03' ";
			}
			//原料收货卸点
			else if (key == "UNLOAD_POINT_CODE")
			{
				sqlstr = " select t.wlpot, t.description from Tmmsmpot t where t.kzkri = 'X' ";
			}
			//pda获取小代码
			else if (key == "PDA_CODE")
			{
				sqlstr = "  select CODE_CLASS,CODE ,CODE_DESC_1_CONTENT from TWMSMZD02  where CODE_CLASS in ('XMBMZL' ,'WM02') order by CODE_CLASS desc  ";
			}
			
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			cmd_sql.SetCommandText(sqlstr);			
			cmd_sql.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_sql.Close();

		}
		else
		{
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
