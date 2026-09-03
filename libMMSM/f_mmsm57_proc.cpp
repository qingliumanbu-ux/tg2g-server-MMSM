/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 原辅料收发存新增、删除、修改
<para>

***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件



BM2_FUNCTION_EXPORT
int f_mmsm57_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm57_pro";                //定义函数英文名称  
	CString FunctionCname = "原辅料收发存后备";              //定义函数中文名称


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
		CModel tmmsm57("TMMSM57");

		//初始化实体类
		tmmsm57.Reset();

		/* 获取输入参数*/



		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();

		Log::Trace("", __FUNCTION__, "v_proc_div[{0}] ", v_proc_div);

	
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			tmmsm57.Reset();
			tmmsm57.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm57.TrimOrBlank();

			if (v_proc_div == "I")
			{
				if (tmmsm57["STAT_DATE"].ToString().GetLength()==14)
				{
					tmmsm57["STAT_DATE"] = tmmsm57["STAT_DATE"].ToString().SubstringNE(0, 8);
				}

				if (tmmsm57.QueryCount("STAT_DATE,PROD_SHIFT_GROUP,MAT_CODE") == 1)
				{
					sprintf(s.msg, "该统计日期、班组、物料代码已存在，请确认！");
					throw CApplicationException(-1, s.msg, log.Location);
				};
				if (tmmsm57["STAT_DATE"].ToString().Trim() == "" || tmmsm57["PROD_SHIFT_GROUP"].ToString().Trim() == "" || tmmsm57["MAT_CODE"].ToString().Trim() == "")
				{
					sprintf(s.msg, "统计日期、班组、物料代码不可为空，请确认"); //系统错误信息
					throw CApplicationException(-1, s.msg, log.Location);
				}
			
				tmmsm57["REC_CREATE_TIME"] = dateNow;
				tmmsm57["REC_CREATOR"] = s.userid;
				tmmsm57.Insert();
			}
			else if (v_proc_div == "U")
			{

				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM	tmmsm57 "
					"		WHERE  MAT_CODE	= @mat_code";

				Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("mat_code", tmmsm57["MAT_CODE"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsm57["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsm57["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsm57["REC_REVISE_TIME"] = dateNow;
				tmmsm57["REC_REVISOR"] = s.userid;
				if (tmmsm57["STAT_DATE"].ToString().GetLength() == 14)
				{
					tmmsm57["STAT_DATE"] = tmmsm57["STAT_DATE"].ToString().SubstringNE(0, 8);
				}
				Log::Trace("", __FUNCTION__, "MAT_CODE[{0}] ", tmmsm57["MAT_CODE"].ToString());
				Log::Trace("", __FUNCTION__, "STAT_DATE[{0}] ", tmmsm57["STAT_DATE"].ToString());
				tmmsm57.Delete();
				tmmsm57.Insert();

			}
			else if (v_proc_div == "D")
			{
				Log::Trace("", __FUNCTION__, "MAT_CODE[{0}] ", tmmsm57["MAT_CODE"]);
				if (tmmsm57["STAT_DATE"].ToString().GetLength() == 14)
				{
					tmmsm57["STAT_DATE"] = tmmsm57["STAT_DATE"].ToString().SubstringNE(0, 8);
				}
				tmmsm57.Print();
				tmmsm57.Delete();

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

