/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 电渣锭计划查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */
int f_mmsm_matno_catch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

// service入口
BM2F_ENTERACE(mmsm70matno_inq)

int f_mmsm70matno_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;

	CString ch_start_time_f = "";
	CString ch_start_time_t = "";
	CString ch_mat_no = "";

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tep0002("TEP0002");

	CDbCommand cmd_inq(conn);

	try
	{
		tep0002["CODE_CLASS"] = "M00M";
		tep0002["CODE_DESC_2_CONTENT"] = "H";
		tep0002["CODE_DESC_4_CONTENT"] = "1";
		tep0002.Query("CODE_CLASS,CODE_DESC_2_CONTENT,CODE_DESC_4_CONTENT");
		//Log::Trace("", __FUNCTION__, "CODE=[{0}]", tep0002["CODE"].ToString());

		if (!bcls_rec->Tables[0].Columns.Contains("FUNC_ID"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
		}
		bcls_rec->Tables[0].Rows[0]["FUNC_ID"] = tep0002["CODE"];
		doFlag = f_mmsm_matno_catch(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(doFlag, s.msg, log.Location);
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
