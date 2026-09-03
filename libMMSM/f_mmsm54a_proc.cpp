/***********************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 原辅料计划增删改（领用计划)
*************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

 



//外部函数声明
//int f_cm_pam1j1_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_cm_pampm1_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2_FUNCTION_EXPORT
int f_mmsm54a_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm54a_proc";                //定义函数英文名称  
	CString FunctionCname = "原辅料领料计划增删改";          //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   blkNum;

	CString sqlstr = "";
	CString v_proc_div = "";
	CString v_factory_div = "";
	CString v_pract_rcv_flag = "";
	CString v_mat_type = "";

	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		CPageInfo pageInfo;

		/*数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
	CModel tmmsm54("TMMSM54");

		//初始化实体类

		tmmsm54.Reset();


		/*判断是否存在指定块*/
		if (bcls_rec->Tables["PARA"].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
			v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();

		if (bcls_rec->Tables["PARA"].Columns.Contains("FACTORY_DIV"))  //厂别
			v_factory_div = bcls_rec->Tables["PARA"].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank().ToUpper();

		if (bcls_rec->Tables[0].Columns.Contains("MAT_TYPE"))//材料类型 有可能是1，2拼在一块传到后台
			v_mat_type = bcls_rec->Tables[0].Rows[0]["MAT_TYPE"].ToString().Trim();

		//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
		
		////发送领用计划电文 黄华添加 2016-4-26
		//CString sendtable_name = "";
		//if (bcls_rec->Tables[0].Rows[0]["MAT_TYPE"].ToString().Trim() == "2")
		//{
		//	sendtable_name = "PAM1J1";
		//}
		//else
		//{

		//	sendtable_name = "PAMPM1";
		//}
		//bcls_rec->Tables.Add(sendtable_name);
		//bcls_rec->Tables[sendtable_name].Clone(bcls_rec->Tables[0]);
		//bcls_rec->Tables[sendtable_name].Columns.Add(DT_STRING, "REC_CREATE_TIME");
		//bcls_rec->Tables[sendtable_name].Columns.Add(DT_STRING, "REC_CREATOR");

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			tmmsm54.Reset();
			tmmsm54.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm54["FACTORY_DIV"] = v_factory_div;
			tmmsm54.TrimOrBlank();

			//Log::Trace("", __FUNCTION__, "tmmsm54["FACTORY_DIV"] =[{0}]", tmmsm54["FACTORY_DIV"].ToString());
			//Log::Trace("", __FUNCTION__, "tmmsm54["PLAN_STATUS"] =[{0}]", tmmsm54["PLAN_STATUS"].ToString());


			if (tmmsm54["MAT_CODE"].ToString().Trim() == "")
			{
				strcpy(s.msg, "物料代码不能为空!");
			}

			if (v_proc_div == "Y") //领用计划发送
			{

				tmmsm54.Reset();
				tmmsm54.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tmmsm54.TrimOrBlank();

				if (tmmsm54["PLAN_NO_Y"].ToString().Trim() == "")
				{
					strcpy(s.msg, "计划号不能为空!");
				}
				/* ***** 打印输入参数 ***** */
				//Log::Info("", __FUNCTION__, "PLAN_NO  =[{0}]", tmmsm54["PLAN_NO_Y"].ToString());
				//Log::Info("", __FUNCTION__, "MAT_CODE  =[{0}]", tmmsm54["MAT_CODE"].ToString());

				tmmsm54["REC_REVISE_TIME"] = dateNow;
				tmmsm54["REC_REVISOR"] = s.userid;
				tmmsm54["PLAN_STATUS"] = "2";
				tmmsm54["PLAN_SEND_TIME"] = dateNow;

				tmmsm54.Update("REC_REVISOR, REC_REVISE_TIME, PLAN_STATUS,PLAN_SEND_TIME", "PLAN_NO_Y,MAT_CODE");

				////发送领用计划电文 黄华添加 2016-4-26
				//bcls_rec->Tables[sendtable_name].Clear();
				//tmmsm54.MergeTo(bcls_rec->Tables[sendtable_name], false);
				//bcls_rec->Tables[sendtable_name].Rows[0]["REC_CREATE_TIME"] = tmmsm54["REC_REVISE_TIME"];
				//bcls_rec->Tables[sendtable_name].Rows[0]["REC_CREATOR"] = tmmsm54["REC_REVISOR"];
				//bcls_rec->Tables[sendtable_name].Rows[0]["FACTORY_DIV"] = tmmsm54.UNLOAD_POS_CODE;
				//if (tmmsm54["MAT_TYPE"].ToString() == "2")
				//{

				//	doFlag = f_cm_pam1j1_snd(bcls_rec, bcls_ret, conn);
				//}
				//else
				//{

				//	doFlag = f_cm_pampm1_snd(bcls_rec, bcls_ret, conn);
				//}
			}


			if (v_proc_div == "I")//新增
			{
				tmmsm54["REC_CREATE_TIME"] = dateNow;
				tmmsm54["REC_CREATOR"] = s.userid;
				tmmsm54["PLAN_STATUS"] = '1';

				if (tmmsm54["PLAN_MAKE_TIME"].ToString().Trim() == "")
				{
					tmmsm54["PLAN_MAKE_TIME"] = dateNow;
				}

				//如果材料类型为空，勾连物料主数据中的材料类型。

				if (v_mat_type.Trim() == "" || tmmsm54["MAT_TYPE"].ToString().Trim().GetLength() > 1)
				{
					sqlstr = "SELECT  MAT_TYPE "
						" FROM    TMMSM50 "
						" WHERE   MAT_CODE = @mat_code";

					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.Parameters.Clear();
					cmd_sql.Parameters.Set("mat_code", tmmsm54["MAT_CODE"].ToString());
					cmd_sql.ExecuteReader();
					if (cmd_sql.Read())
					{
						tmmsm54["MAT_TYPE"] = cmd_sql.GetString(1);
					}
					cmd_sql.Close();

				}

				//Log::Trace("", __FUNCTION__, "tmmsm54["MAT_TYPE"] =[{0}]", tmmsm54["MAT_TYPE"].ToString());

				//计划号生成
				tmmsm54["PLAN_NO_Y"] = "L" + dateNow;

				//Log::Trace("", __FUNCTION__, "tmmsm54.PLAN_NO1111=[{0}]", tmmsm54["PLAN_NO_Y"].ToString());
				//tmmsm54.Print();
				tmmsm54.Insert();

			}

			if (v_proc_div == "U")//修改
			{
				if (tmmsm54["PLAN_STATUS"].ToString() != '1')
				{
					strcpy(s.msg, "该计划不为编制状态，不允许修改!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR,PLAN_STATUS"
					"	   FROM	TMMSM54 "
					"      WHERE  PLAN_NO_Y = @plan_no_y";

				//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("plan_no_y", tmmsm54["PLAN_NO_Y"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsm54["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsm54["REC_CREATOR"] = cmd_sql.GetString(2);
					tmmsm54["PLAN_STATUS"] = cmd_sql.GetString(3);
				}
				cmd_sql.Close();

				tmmsm54["REC_REVISE_TIME"] = dateNow;
				tmmsm54["REC_REVISOR"] = s.userid;

				tmmsm54.Delete();
				tmmsm54.Insert();
			}


			if (v_proc_div == "D")
			{
				if (tmmsm54["PLAN_STATUS"].ToString() == '2')
				{
					strcpy(s.msg, "该计划已经发送，不允许修改!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				tmmsm54.Delete();

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

