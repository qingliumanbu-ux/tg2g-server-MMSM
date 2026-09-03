/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 原辅料主信息新增、删除、修改
<para>

***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

  



//外部函数声明
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm50a_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm50_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	/****** 定义函数名称 ***** */
CString FunctionEname = "f_mmsm50_pro";                //定义函数英文名称  
CString FunctionCname = "原辅料信息_信息后备";              //定义函数中文名称


CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
  

  //程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   n_count = 0;
	int   blkNum;

	CString sqlstr="";
	CString   v_proc_div = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_update = "";  //修改的字段信息
	CString v_condi  = "";  //过滤的字段信息。
	CString v_func_id = ""; //功能号。 
	CString  v_item_ename = "";
	CString c_sql_condition  = "";

		   
  
	try
	{
		CPageInfo pageInfo;

		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
	CModel tmmsm50("TMMSM50");

		//初始化实体类
		tmmsm50.Reset();

		/* 获取输入参数*/

		//Log::Trace("", __FUNCTION__, "v_proc_div11111111111111[{0}] ", v_proc_div);

		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();

		//Log::Trace("", __FUNCTION__, "v_proc_div222222222222[{0}] ", v_proc_div);

		blkNum = bcls_rec->Tables.IndexOf("MMSM50A");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSM50A");
		}

		blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSMSND");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_DIV");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_COUNT");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAT_CODE");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAT_NAME");
		}
		

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm50.Reset();
			tmmsm50.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm50.TrimOrBlank();
			

			if (v_proc_div == "I")
			{
				if (tmmsm50.QueryCount("MAT_CODE") == 1)
				{
					sprintf(s.msg, "物料代码重复");
					throw CApplicationException(-1, s.msg, log.Location);
				};
				if (tmmsm50["MAT_CODE"].ToString().Trim() == "")
				{
					sprintf(s.msg, "物料代码不能为空"); //系统错误信息
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm50["REC_CREATE_TIME"] = dateNow;
				tmmsm50["REC_CREATOR"] = s.userid;
				tmmsm50["MAT_STATION_D"] = tmmsm50["MAT_STATION"];
				//tmmsm50.Print();
				tmmsm50.Insert();

		
				//新增TMMSM50A

				bcls_rec->Tables["MMSM50A"].Rows.Clear();
				tmmsm50.MergeTo(bcls_rec->Tables["MMSM50A"], false);
						
				doFlag = f_mmsm50a_proc(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

			}
			else if (v_proc_div == "U")
			{
				
				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM	TMMSM50 "
					"		WHERE  MAT_CODE	= @mat_code";

				//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("mat_code", tmmsm50["MAT_CODE"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsm50["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsm50["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsm50["REC_REVISE_TIME"] = dateNow;
				tmmsm50["REC_REVISOR"] = s.userid;

				tmmsm50.Delete();
				tmmsm50.Insert();

			}
			else if (v_proc_div == "D")
			{
				tmmsm50.Delete();

				bcls_rec->Tables["MMSM50A"].Rows.Clear();
				tmmsm50.MergeTo(bcls_rec->Tables["MMSM50A"], false);

				doFlag = f_mmsm50a_proc(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

			}
			bcls_rec->Tables["MMSMSND"].Rows.Clear();
			bcls_rec->Tables["MMSMSND"].Rows.Add();
			bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM50";
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_COUNT"] = "";
			bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_CODE"] = tmmsm50["MAT_CODE"];
			bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NAME"] = tmmsm50["MAT_NAME"];
			doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		
	
		
		/*设置系统返回参数*/
		strcpy(s.msg,  _RES("GCRSS0000002"));//处理成功。  

	}



	/*捕获数据库操作异常*/
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		//LogTrace(1,1,"%s",(const char*)sqlstr);
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = "DB error:" + sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		 
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

	//LogTrace(1,1,"doFlag[%d]s.msg[%s],s.sysmsg[%s]",doFlag,s.msg,s.sysmsg);
	////LogTrace(1, 1, " **************%s end*****************", (const char*)FunctionEname);
	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;

} 

