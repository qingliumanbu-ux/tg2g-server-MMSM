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


BM2F_ENTERACE(mmsm11k_inq)

int f_mmsm11k_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 		//服务调用日志输出

	int doFlag = 0;					//服务调用返回值

	/* 分页信息定义 */
	CPageInfo pageInfo;
	int	TotalRecordCount = 0;

	CModel tmmsm11h1("TMMSM11H1");	//中位硅1表

	CDbCommand cmd_inq(conn);			//数据库操作对象定义
	CString ay_type = "";
	/* sql语句变量定义 */
	CString sqlstr;
	CString sqlstr_count = "";
	CString sqlstr_temp = "";

	try
	{
		//sql语句赋初值
		
		ay_type = bcls_rec->Tables[0].Rows[0]["AY_TYPE"].ToString();
		if (ay_type=="1")
		{
			sqlstr_temp += " AND A.POS_DIR_CODE	IN('1','2')  ORDER BY A.POS_DIR_CODE ASC ";
		}
		else if(ay_type == "2")
		{
			sqlstr_temp += " AND A.POS_DIR_CODE	IN('3','4')  ORDER BY A.POS_DIR_CODE ASC  ";
		}
		sqlstr = "   SELECT(SELECT  ELM_VALUE  FROM  tmmsm81aL  WHERE   ELM_NAME = 'Si' AND  LOT_NO = B.TPC_ID ) AS  ELEM_SI,   "
			"     A.TORPEDO_BATCH_NUMBER, A.TORPEDO_NUMBER, A.C_DELIVERY, A.POS_DIR_CODE, A.HM_WEIGHT,B.AC_WT,B.ASSIGNEDVOLUME	 FROM TMMSM11K A  LEFT JOIN TMMSM11 B ON A.TICODE = B.TICODE  WHERE 1 = 1 ";
		sqlstr = sqlstr + sqlstr_temp;

		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
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