/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李振
Version:    1.0
Date:     2025-07-15
Description: 通过钢种计算密度
remark:本
***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
/***** C++ 的业务头文件部分 *****/




//外部函数声明
BM2_FUNCTION_EXPORT
int f_mmsm_get_density(CString ST_NO, CDecimal& MAT_DENSITY, CDbConnection* conn)
{
	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm_get_matwt";                //定义函数英文名称  
	CString FunctionCname = "获取材料重量";              //定义函数中文名称
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/****** 自定义变量 ***** */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;
	CString c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");          //当前时间
	CString v_func_id = "";
	CString v_lslab_no = "";
	CString v_slab_no = "";
	CString v_ingot_code = "";
	CDecimal v_slab_len = 0;
	CDecimal v_slab_wt = 0;
	CDecimal v_slab_num = 1;
	CDecimal t_slab_wt = 0;
	int fetchRowCount = 0;
	CString sqlstr = "";
	CDecimal mat_radius;
	const CDecimal density = 7.85;
	CDbCommand cmd_inq(conn); // 建立连接。



	try
	{
		CDecimal v_code_wt = 7.85;//计算重量的系数,默认7.85
		sqlstr = " select CODE_DESC_1_CONTENT,decode(trim(CODE_DESC_2_CONTENT),'',7.85,trim(CODE_DESC_2_CONTENT)) CODE_DESC_2_CONTENT from TWMSMZD02 where CODE_CLASS='MMSMDENS' ORDER BY CODE  ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			Log::Info("", __FUNCTION__, "ST_NO =[{0}]", ST_NO.Trim().SubstringNE(0, cmd_inq.GetString(1).GetLength()), cmd_inq.GetDecimal(2));
			if (ST_NO.Trim().SubstringNE(0, cmd_inq.GetString(1).GetLength()) == cmd_inq.GetString(1))
			{
				v_code_wt = cmd_inq.GetDecimal(2);
				break;
			}
		}
		cmd_inq.Close();

		MAT_DENSITY = v_code_wt;
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
	if (doFlag < 0)
	{
		;
	}
	return doFlag;

}
