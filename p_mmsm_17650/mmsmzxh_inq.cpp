/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   任习明
Version:    1.0
Date:     2024
Description: 总消耗查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmzxh_inq)

int f_mmsmzxh_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString start_time = " ";
	CString end_time_1 = " ";
	CString end_time = " ";
	CString heat_no1 = " ";
	CString heat_no2 = " ";
	CString heat_no3 = " ";
	CString heat_no4 = " ";
	CString heat_no5 = " ";
	CString heat_no6 = " ";
	CString heat_no7 = " ";
	CString heat_no8 = " ";
	CString heat_no9 = " ";
	CString heat_no10 = " ";
	CString heat_no11 = " ";
	CString heat_no12 = " ";
	CString heat_no13 = " ";
	CString heat_no14 = " ";
	CString heat_no15 = " ";
	CString heat_no16 = " ";
	CString nian = "";
	CString yue = "";
	CString ri = "";
	CString rec_create_time = " ";
	CString rec_creator = " ";
	CString baoji = " ";
	CDecimal cd_count = 0;
	CDecimal devo_wt = 0;
	CString biaoji = "0";
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	int i_idx = 0;
	int i_count = 0;
	CDecimal ni_wt = 0; // 成分*同炉同物料汇总重量Ni
	CDecimal cr_wt = 0; // 成分*同炉同物料汇总重量Cr
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CModel tmmsmzxhbb("TMMSMZXHBB");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EIClass EItables;
	CPageInfo pageInfo;

	try
	{
		

		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().TrimOrBlank().ToUpper();		

		sqlstr = " SELECT *"
			" from TMMSMZXHBB_LH"
			" where 1=1"
			" order by TIME_STAMPS desc"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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
