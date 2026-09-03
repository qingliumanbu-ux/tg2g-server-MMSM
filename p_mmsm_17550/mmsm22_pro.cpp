/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2014-07-08
Description: Ar生产实绩增删改
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件
   

//外部函数声明
int f_mmsm22_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn); 


BM2F_ENTERACE(mmsm22_pro)

int f_mmsm22_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString proc_div = "";
	CString factory_div = "";
	CString area_id = "";
	CString station_id = "";
	CString station_no = "";
	CString heat_no = "";

	/* 实体类定义 */
	
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{	
		//实绩收集标记  0-后备   1-电文
		if(!bcls_rec->Tables[0].Columns.Contains("PRACT_COLL_MODE"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING,"PRACT_COLL_MODE"); 
		}
		bcls_rec->Tables[0].Rows[0]["PRACT_COLL_MODE"] = "0";

		if (!bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
		}
		
		CString tableName = bcls_rec->Tables[0].get_TableName();
		Log::Info("", __FUNCTION__, "tableName  =[{0}]", tableName);
		if ("TMMSM22_ADD" == tableName){
			bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "I";//新增
		}
		else if ("TMMSM22_MODIFY" == tableName){
			bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "U";//修改
		}
		else if ("TMMSM22_DEL" == tableName){
			bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "D";//删除
		}

		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
			proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("AREA_ID"))
			area_id = bcls_rec->Tables[0].Rows[0]["AREA_ID"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
			station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_NO"))
			station_no = bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "proc_div  =[{0}]", proc_div);
		Log::Info("", __FUNCTION__, "factory_div  =[{0}]", factory_div);
		Log::Info("", __FUNCTION__, "area_id  =[{0}]", area_id);
		Log::Info("", __FUNCTION__, "station_id  =[{0}]", station_id);
		Log::Info("", __FUNCTION__, "station_no  =[{0}]", station_no);

		doFlag = f_mmsm22_proc(bcls_rec, bcls_ret,conn);
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


