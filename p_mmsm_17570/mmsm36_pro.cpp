/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2014-07-08
Description: 缓冷退火实绩增删改
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件
   

//外部函数声明
int f_mmsm3601_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn); 


BM2F_ENTERACE(mmsm36_pro)

int f_mmsm36_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */

	/* 实体类定义 */
	
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";
	try
	{	
		CString maker = "";
		if (bcls_rec->Tables[0].Columns.Contains("INSPECT_MAKER"))
		{
			maker = bcls_rec->Tables[0].Rows[0]["INSPECT_MAKER"].ToString();
			if (maker!=""&&maker!=" "&&maker.Trim()!="")
			{
				try{
					Log::Trace("", __FUNCTION__, "content = [{0}]  ", maker);
					CString strsql = " select CODE,CODE_DESC_1_CONTENT from TWMSMZD02  where CODE_CLASS = 'INSPECT_MAKER' and code =  " + maker + "";
					CString content = "";
					cmd_inq.SetCommandText(strsql);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						content = cmd_inq.GetString(2);

					}
					cmd_inq.Close();
					Log::Trace("", __FUNCTION__, "content = [{0}]  ", content);
					bcls_rec->Tables[0].Rows[0]["INSPECT_MAKER"] = content;
				}
				catch (CDbException& ex){
					cmd_inq.Close();
				}
				
			}
			
		}
		//实绩收集标记  0-后备   1-电文
		//if(!bcls_rec->Tables[0].Columns.Contains("PRACT_COLL_MODE"))
		//{
		//	bcls_rec->Tables[0].Columns.Add(DT_STRING,"PRACT_COLL_MODE"); 
		//}
	
		//bcls_rec->Tables[0].Rows[0]["PRACT_COLL_MODE"] = "0";


		//电文需要增加厂别区分
	/*if(!bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV"); 
		}
	
		bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"] = " ";
  */
		Log::Trace("", sqlstr, "sqlstr[{0}]  ", "--");
		doFlag = f_mmsm3601_proc(bcls_rec, bcls_ret,conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}



	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
} 


