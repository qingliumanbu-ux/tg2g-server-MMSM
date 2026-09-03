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


BM2_FUNCTION_EXPORT
int f_mmsms1_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsms1_pro";                //定义函数英文名称  
	CString FunctionCname = "原辅料信息_信息后备";              //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   n_count = 0;
	int   blkNum;

	CString sqlstr = "";
	CString   v_proc_div = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_update = "";  //修改的字段信息
	CString v_condi = "";  //过滤的字段信息。
	CString v_func_id = ""; //功能号。 
	CString  v_item_ename = "";
	CString c_sql_condition = "";



	try
	{
		CPageInfo pageInfo;

		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
		CModel tmmsms1("TMMSMS1");

		//初始化实体类
		tmmsms1.Reset();

		/* 获取输入参数*/

		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsms1.Reset();
			tmmsms1.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsms1.TrimOrBlank();


			if (v_proc_div == "I")
			{
				if (tmmsms1.QueryCount("RUN_SIGNAL") == 1)
				{
					sprintf(s.msg, "操作事项重复");
					throw CApplicationException(-1, s.msg, log.Location);
				};
				tmmsms1["REC_CREATE_TIME"] = dateNow;
				tmmsms1["REC_CREATOR"] = s.userid;
				//tmmsms1.Print();
				tmmsms1.Insert();


			}
			else if (v_proc_div == "U")
			{

				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM	TMMSMS1 "
					"		WHERE  RUN_SIGNAL	= @RUN_SIGNAL";

				//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("RUN_SIGNAL", tmmsms1["RUN_SIGNAL"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsms1["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsms1["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsms1["REC_REVISE_TIME"] = dateNow;
				tmmsms1["REC_REVISOR"] = s.userid;

				tmmsms1.Delete();
				tmmsms1.Insert();

			}
			else if (v_proc_div == "D")
			{
				tmmsms1.Delete();

			}

		}



		/*设置系统返回参数*/
		strcpy(s.msg, _RES("GCRSS0000002"));//处理成功。  

	}



	/*捕获数据库操作异常*/
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		//LogTrace(1,1,"%s",(const char*)sqlstr);
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = "DB error:" + sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应

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

	//LogTrace(1,1,"doFlag[%d]s.msg[%s],s.sysmsg[%s]",doFlag,s.msg,s.sysmsg);
	////LogTrace(1, 1, " **************%s end*****************", (const char*)FunctionEname);
	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;

}

