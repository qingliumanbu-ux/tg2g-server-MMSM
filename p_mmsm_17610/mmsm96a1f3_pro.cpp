/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 厚板物料跟踪模拟抛帐
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 厚板物料跟踪模拟抛帐
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
#include "tmmsm96.h" 


//外部函数声明
int f_mmsm99(EIClass * bcls_rec,EIClass * bcls_ret,CDbConnection * conn);

BM2F_ENTERACE(mmsm96a1f3_pro)                                         

int f_mmsm96a1f3_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");    

	/* 实体类定义 */
	CTMMSM96 tmmsm96(conn);
	
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime	=	CDateTime::Now().ToString("yyyyMMddHHmmss");
		
		/* 设置块名 */
		bcls_rec->SetBlkName(1, "MM0099");

		/* 获取输入参数 */
		tmmsm96.MAT_KIND	= bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();

		/* 打印输入参数 */
		Log::Trace("", __FUNCTION__, "传入参数,tmmsm96.MAT_KIND	= [{0}]",tmmsm96.MAT_KIND);

		/* 调用物料函数 */
	
		if (tmmsm96.MAT_KIND.Trim() == "SM")
		{
			doFlag = f_mmsm99(bcls_rec,bcls_ret,conn);
			if(doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		
		
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch(CApplicationException& ex)
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
	
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
} 

