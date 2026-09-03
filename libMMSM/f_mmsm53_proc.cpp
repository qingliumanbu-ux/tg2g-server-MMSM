/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 实绩物料消耗时的原辅料出入库履历和库存处理
<para>

***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

  
  
  

//外部函数声明
int f_mmsm_get_seq_no(CString& resume_seq_no, CDbConnection * conn);
int f_mmsm_acyfl(EIClass * bcls_rec, CString & flag, EIClass * bcls_ret, CString& v_acjc_relation_id, CDbConnection * conn);
int f_mmsm_acyfl_seq(CString& v_acjc_relation_id, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm53_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm53_proc";           //定义函数英文名称  
	CString FunctionCname = "原辅料出入库履历处理";              //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   blkNum;
	CString sqlstr = "";
	CString  v_proc_div = "";
	CString v_acjc_relation_id = "";
	CString resume_seq_no = "";
	CDecimal newWt = 0;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm51("TMMSM51");
	CModel tmmsm53("TMMSM53");

		//初始化实体类
		tmmsm2a.Reset();
		tmmsm51.Reset();
		tmmsm53.Reset();


		/* 获取输入参数*/

		blkNum = bcls_rec->Tables.IndexOf("MMSM53");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMSM53 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/*获取输入参数 */
		tmmsm2a.MergeFrom(bcls_rec->Tables["MMSM53"].Rows[0]);
		tmmsm2a.TrimOrBlank();

		if (bcls_rec->Tables["MMSM53"].Columns.Contains("PROC_DIV"))   //增删改区分
			v_proc_div = bcls_rec->Tables["MMSM53"].Rows[0]["PROC_DIV"].ToString().Trim();
		
		//Log::Trace("", __FUNCTION__, "tmmsm2a["MAT_CODE"] =[{0}]", tmmsm2a["MAT_CODE"].ToString());


		//判断该物料代码是否存在，如果不存在，新增。存在，报错
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "SELECT STOCK_WT "
				"		FROM TMMSM51 "
				"		WHERE FACTORY_DIV =  @factory_div";
			"		AND   MAT_CODE =  @mat_code";

			break;
		}
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Clear();
		cmd_sql.Parameters.Set("factory_div", tmmsm2a["FACTORY_DIV"].ToString());
		cmd_sql.Parameters.Set("mat_code", tmmsm2a["MAT_CODE"].ToString());
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			tmmsm51["STOCK_WT"] = cmd_sql.GetDecimal(1);
		}
		else
		{
			/*strcpy(s.msg, "该物料代码库存为0，投料出错!");
			throw CApplicationException(-1, s.msg, log.Location);*/
		}
		cmd_sql.Close();

		
		//调用公共函数返回履历序号
		f_mmsm_get_seq_no(resume_seq_no, conn);

		tmmsm53["RESUME_SEQ_NO"] = resume_seq_no;
		if (v_proc_div == "I") //新增
		{
			//写库存表
			tmmsm51["REC_REVISE_TIME"] = dateNow;
			tmmsm51["REC_REVISOR"] = s.userid;
			tmmsm51["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"];
			tmmsm51["MAT_CODE"] = tmmsm2a["MAT_CODE"];
			tmmsm51["STOCK_WT"] = tmmsm51["STOCK_WT"].ToDecimal() - tmmsm2a["DEVO_WT"];

			tmmsm51.Update("REC_REVISOR, REC_REVISE_TIME,STOCK_WT", "FACTORY_DIV,STOCK_PLACE_NO,MAT_CODE");


			//写收发明细表
			tmmsm53["REC_CREATE_TIME"] = dateNow;
			tmmsm53["REC_CREATOR"] = s.userid;
			tmmsm53["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"];
			tmmsm53["HANDLE_DIV"] = "7";
			tmmsm53["OTHER_BILL_NO"] = tmmsm2a["PROC_NO"];
			tmmsm53["MAT_CODE"] = tmmsm2a["MAT_CODE"];
			tmmsm53["MAT_NAME"] = tmmsm2a["MAT_NAME"];
			tmmsm53["STOCK_WT"] = tmmsm2a["DEVO_WT"];

			tmmsm53.Insert();
		}
		//else if (v_proc_div == "U") //修改
		//{
		//	//写库存表
		//	tmmsm51["REC_REVISE_TIME"] = dateNow;
		//	tmmsm51["REC_REVISOR"] = s.userid;
		//	tmmsm51["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"];
		//	tmmsm51["MAT_CODE"] = tmmsm2a["MAT_CODE"];
		//	tmmsm51["STOCK_WT"] = tmmsm51["STOCK_WT"].ToDecimal() + v_devo_wt_old - tmmsm2a["DEVO_WT"];

		//	tmmsm51.Update("REC_REVISOR, REC_REVISE_TIME,STOCK_WT", "FACTORY_DIV,STOCK_PLACE_NO,MAT_CODE");

		//	//写收发明细表  先将上一次的重量写一笔负，然后本次重量写一笔正。
		//	tmmsm53["REC_CREATE_TIME"] = dateNow;
		//	tmmsm53["REC_CREATOR"] = s.userid;
		//	tmmsm53["RESUME_SEQ_NO"] = dateNow;
		//	tmmsm53["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"];
		//	tmmsm53["HANDLE_DIV"] = "7";
		//	tmmsm53["OTHER_BILL_NO"] = tmmsm2a["PROC_NO"];
		//	tmmsm53["MAT_CODE"] = tmmsm2a["MAT_CODE"];
		//	tmmsm53["MAT_NAME"] = tmmsm2a["MAT_NAME"];
		//	tmmsm53["STOCK_WT"] = 0 - v_devo_wt_old;

		//	tmmsm53.Insert();

		//	//写收发明细表
		//	tmmsm53["REC_CREATE_TIME"] = dateNow;
		//	tmmsm53["REC_CREATOR"] = s.userid;
		//	tmmsm53["RESUME_SEQ_NO"] = dateNow;
		//	tmmsm53["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"];
		//	tmmsm53["HANDLE_DIV"] = "7";
		//	tmmsm53["OTHER_BILL_NO"] = tmmsm2a["PROC_NO"];
		//	tmmsm53["MAT_CODE"] = tmmsm2a["MAT_CODE"];
		//	tmmsm53["MAT_NAME"] = tmmsm2a["MAT_NAME"];
		//	tmmsm53["STOCK_WT"] = tmmsm2a["DEVO_WT"];

		//	tmmsm53.Insert();


		//}
		else if (v_proc_div == "D")//删除
		{
			//写库存表
			tmmsm51["REC_REVISE_TIME"] = dateNow;
			tmmsm51["REC_REVISOR"] = s.userid;
			tmmsm51["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"];
			tmmsm51["MAT_CODE"] = tmmsm2a["MAT_CODE"];
			tmmsm51["STOCK_WT"] = tmmsm51["STOCK_WT"].ToDecimal() + tmmsm2a["DEVO_WT"].ToDecimal();

			tmmsm51.Update("REC_REVISOR, REC_REVISE_TIME,STOCK_WT", "FACTORY_DIV,STOCK_PLACE_NO,MAT_CODE");


			//写收发明细表
			tmmsm53["REC_CREATE_TIME"] = dateNow;
			tmmsm53["REC_CREATOR"] = s.userid;
			tmmsm53["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"];
			tmmsm53["HANDLE_DIV"] = "7";
			tmmsm53["OTHER_BILL_NO"] = tmmsm2a["PROC_NO"];
			tmmsm53["MAT_CODE"] = tmmsm2a["MAT_CODE"];
			tmmsm53["MAT_NAME"] = tmmsm2a["MAT_NAME"];
			tmmsm53["STOCK_WT"] = 0 - tmmsm2a["DEVO_WT"].ToDecimal();

			tmmsm53.Insert();

			f_mmsm_acyfl_seq(v_acjc_relation_id, conn);
			//写抛帐表
			CString v_acjc_flag = "2";
			CString resume_seq_no = "";
			CString v_acjc_relation_id = "";
			if (bcls_rec->Tables.IndexOf("MMSM2A") < 0)
			{
				bcls_rec->Tables.Add("MMSM2A");
			}
			bcls_rec->Tables["MMSM2A"].Rows.Add();
			if (!bcls_rec->Tables["MMSM2A"].Columns.Contains("HEAT_NO"))
			{
				bcls_rec->Tables["MMSM2A"].Columns.Add(DT_STRING, "HEAT_NO");
			}
			if (!bcls_rec->Tables["MMSM2A"].Columns.Contains("PROC_NO"))
			{
				bcls_rec->Tables["MMSM2A"].Columns.Add(DT_STRING, "PROC_NO");
			}
			if (!bcls_rec->Tables["MMSM2A"].Columns.Contains("PROC_COUNT"))
			{
				bcls_rec->Tables["MMSM2A"].Columns.Add(DT_STRING, "PROC_COUNT");
			}
			bcls_rec->Tables["MMSM2A"].Rows[0]["HEAT_NO"] = tmmsm2a["HEAT_NO"];
			bcls_rec->Tables["MMSM2A"].Rows[0]["PROC_NO"] = tmmsm2a["PROC_NO"]; 
			bcls_rec->Tables["MMSM2A"].Rows[0]["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
			doFlag = f_mmsm_acyfl(bcls_rec, v_acjc_flag, bcls_ret, v_acjc_relation_id, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
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

