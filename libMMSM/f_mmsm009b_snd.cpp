/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     王佳倩
Version:    1.0
Date:       2022-10-26
Description: 炼钢实绩电文发送（智慧质量）
**************************************************/
//框架头文件
#include "stdafx.h" 
#include "epex.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢切断实绩电文发送
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

BM2_FUNCTION_EXPORT
 int f_mmsm009b_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	int j	= 0;

	/* 业务变量 */
	CString	datetime("");    
	int		fetchRowCount = 0;
	CString v_table_type = "";
	CString v_proc_div = "";
	CString v_heat_no = "";
	CString v_proc_no = "";
	CString v_event_id = "";
	CString cs_main_mat_no = "";
	CString v_tc_backlog = "";
	CDecimal v_proc_count = 0;


	/* 创建电文处理对象 */
	EPEX epex(&s,conn);

	/* 实体类定义 */
	CModel tmm009a("TMM009A");
	CModel tmmsm33("TMMSM33");
	

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{	
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMSMSND 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
    
	
		/* 获取输入参数 */	
		if(bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
			v_tc_backlog = bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"].ToString().Trim();
		if(bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_DIV"))
			v_proc_div = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"].ToString().Trim();

		if (bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_COUNT"))
			v_proc_count = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_COUNT"].ToDecimal();

		Log::Trace("", __FUNCTION__, "传入参数,v_proc_count= [{0}] [{1}],[{2}]", v_tc_backlog, v_proc_count, v_proc_count);
	
		tmm009a["MAT_KIND"] = "SM";
		tmm009a["EVENT_ID"] = "MM9B";
		tmm009a["EVENT_LINE_TYPE"] = "00";
		tmm009a["TC_BACKLOG"] = v_tc_backlog;
	

		/* 检查输入参数合法性 */
		if(tmm009a["TC_BACKLOG"].ToString().Trim() == "")
		{
			strcpy(s.msg,"电文工序值不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
		/* 查询电文号 */
		tmm009a.Query("MAT_KIND,EVENT_LINE_TYPE,EVENT_ID,TC_BACKLOG");
		tmm009a.TrimOrBlank();

		Log::Trace("",__FUNCTION__,"查询电文号 tmm009a[TC_NO] = [{0}]",tmm009a["TC_NO"].ToString());	  	
	  	

		/* 实绩表配置电文号则发送电文 */
		if (tmm009a["TC_SEND_FLAG"].ToString().Trim() == "Y")	//Y-发送电文
		{
			if(tmm009a["TC_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"设置发送电文的配置，电文号不能为空!请在MM0097A1画面维护。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 发送电文开始 */
			//初始化
			if(epex.Initialize(tmm009a["TC_NO"].ToString()) < 0)
			{
				strcpy(s.msg,"初始化电文[" + tmm009a["TC_NO"].ToString() + "]失败，原因[" + epex.GetMsg()+"]。");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}			

			//切断实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM33")
			{
				tmmsm33.Reset();
				tmmsm33["MAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"].ToString().Trim();
				v_proc_div = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"].ToString().Trim(); 
				tmmsm33.Query();
				tmmsm33.Print();
				tmmsm33.TrimOrBlank();

				if (epex.SetValue(0, tmmsm33) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			if (epex.SendTele() < 0)
			{
				strcpy(s.msg,"电文发送失败。");
				throw CApplicationException(-1, s.msg, log.Location); 
			}

			epex.Uninitialize();
				
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




