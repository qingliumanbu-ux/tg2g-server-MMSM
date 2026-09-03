/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料进料汇总查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmylgxhz_save)

int f_mmsmylgxhz_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int count = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString datetime = "";
	CString prodtime = "";
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsmw9("TMMSMW9");

	CDbCommand cmd_inq(conn);

	try
	{
		try{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}


		//--------------------------------
		//获取传入参数

		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		prodtime = datetime.SubstringNE(0, 6);
		tmmsmw9["PROD_DATE"] = prodtime;
		tmmsmw9["HEAT_NO1"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_A1_S"].ToString();
		tmmsmw9["HEAT_NO2"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_A2_S"].ToString();
		tmmsmw9["HEAT_NO3"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_A0_S"].ToString();
		tmmsmw9["HEAT_NO4"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_A6_S"].ToString();
		tmmsmw9["BACK_CODE_1"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_B1_S"].ToString();
		tmmsmw9["BACK_CODE_2"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_B2_S"].ToString();
		tmmsmw9["BACK_CODE_3"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_B0_S"].ToString();

		tmmsmw9["BACK_COL_1"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_E1_S"].ToString();
		tmmsmw9["BACK_COL_2"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_E2_S"].ToString();
		tmmsmw9["BACK_C1"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F1_S"].ToString();
		tmmsmw9["BACK_C2"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F2_S"].ToString();
		tmmsmw9["BACK_C3"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F3_S"].ToString();
		tmmsmw9["BACK_C4"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F4_S"].ToString();
		tmmsmw9["BACK_C5"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F5_S"].ToString();
		tmmsmw9["BACK_C6"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F6_S"].ToString();
		tmmsmw9["BACK_C7"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F7_S"].ToString();
		tmmsmw9["BACK_C8"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F8_S"].ToString();
		tmmsmw9.TrimOrBlank();

		if (tmmsmw9.QueryCount("PROD_DATE") ==0)
		{
			tmmsmw9["REC_CREATOR"] = s.userid;
			tmmsmw9["REC_CREATE_TIME"] = datetime;
			tmmsmw9.Insert();
		}
		else
		{
			tmmsmw9.Update("HEAT_NO1,HEAT_NO2,HEAT_NO3,HEAT_NO4,BACK_CODE_1,BACK_CODE_2,BACK_CODE_3,BACK_COL_1,BACK_COL_2,BACK_C1,BACK_C2,BACK_C3,BACK_C4,BACK_C5,BACK_C6,BACK_C7,BACK_C8", "PROD_DATE");
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