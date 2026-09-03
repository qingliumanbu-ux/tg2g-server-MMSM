/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   wzn
Version:    1.0
Date:     2018-01-31
Description: 铸坯单重参数录入
***********************************************************************/


#include "stdafx.h"
#include "epex.h"





BM2_FUNCTION_EXPORT


int f_mmsm01_st_no_update(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 变量定义 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString s_formname = "";
	CString v_fields_str = "";
	CString ingotCode = "";
	CString sgSign = "";
	CDecimal p_seq_id = 0;
	CString i_func_id = "";
	CString i_resume_seq_no = "";
	CModel tmmsmwt("TMMSMWT");
	CModel tmmsmwt_pre("TMMSMWT");
	CModel tmmsmwtb("TMMSMWTB");
	CDbCommand cmd_inq(conn);
	CString cs_heat_no = "";
	CString cs_st_no = "";
	try
	{
		cs_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		cs_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();
		sqlstr = "SELECT * FROM TMMSM33 WHERE HEAT_NO = @heat_no ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", cs_heat_no);
		cmd_inq.ExecuteQuery(tb_tpssm11.Tables[0]);
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


