/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     Simon Li
Version:    1.0
Date:       2024-03-18
Description: 中位硅铁水罐查询
**************************************************/

//框架头文件
#include "stdafx.h"

//业务头文件


BM2F_ENTERACE(mmsm11k_upd)

int f_mmsm11k_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 		//服务调用日志输出

	int doFlag = 0;					//服务调用返回值

	/* 分页信息定义 */
	CPageInfo pageInfo;
	int	TotalRecordCount = 0;

	CModel tmmsm11("TMMSM11");	//中位硅1表
	CModel tmmsm11k("TMMSM11K");
	CDbCommand cmd_inq(conn);			//数据库操作对象定义
	CString ay_type = "";
	/* sql语句变量定义 */
	CString sqlstr;
	CString sqlstr_count = "";
	CString sqlstr_temp = "";

	try
	{
		//sql语句赋初值
		tmmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm11.Update("AC_WT,ASSIGNEDVOLUME", "TICODE");
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();

	return doFlag;		//返回-1时事务将回滚，返回为0是事务将提交
}