/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 原辅料出库
<para>

***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

  
  
  



//外部函数声明
int f_mmsm_get_seq_no(CString& resume_seq_no, CDbConnection * conn);
int f_mmsm55_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm56_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm56_proc";           //定义函数英文名称  
	CString FunctionCname = "原辅料出库";              //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   blkNum;
	CString sqlstr = "";
	CString  v_proc_div = "";
	CString  resume_seq_no = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		CPageInfo pageInfo;

		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
	CModel tmmsm56("TMMSM56");
	CModel tmmsm51("TMMSM51");
	CModel tmmsm53("TMMSM53");
		if (!bcls_rec->Tables.Contains("MMSM56"))
		{
			bcls_rec->Tables[0].set_TableName("MMSM56");
		}
		for (int i = 0; i < bcls_rec->Tables["MMSM56"].Rows.get_Count(); i++)
		{
			/* 获取输入参数*/
			tmmsm56.Reset();
			tmmsm56.MergeFrom(bcls_rec->Tables["MMSM56"].Rows[i]);
			tmmsm56.TrimOrBlank();

			if (bcls_rec->Tables["PARA"].Columns.Contains("PROC_DIV"))   //增删改区分
				v_proc_div = bcls_rec->Tables["PARA"].Rows[i]["PROC_DIV"].ToString().Trim();

			if (bcls_rec->Tables["MMSM56"].Columns.Contains("MAT_TYPE"))   //材料类型
				tmmsm56["MAT_TYPE"] = bcls_rec->Tables["MMSM56"].Rows[i]["MAT_TYPE"].ToString().Trim();

			//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
			//Log::Trace("", __FUNCTION__, "tmmsm56["FACTORY_DIV"] =[{0}]", tmmsm56["FACTORY_DIV"].ToString());
			//Log::Trace("", __FUNCTION__, "tmmsm56["MAT_CODE"] =[{0}]", tmmsm56["MAT_CODE"].ToString());
			//Log::Trace("", __FUNCTION__, "tmmsm56["HANDLE_DIV"] =[{0}]", tmmsm56["HANDLE_DIV"].ToString());
			//Log::Trace("", __FUNCTION__, "tmmsm56["OUT_STOCK_NO"] =[{0}]", tmmsm56["OUT_STOCK_NO"].ToString());
			//Log::Trace("", __FUNCTION__, "tmmsm56["PLAN_NO_Y"] =[{0}]", tmmsm56["PLAN_NO_Y"].ToString());

			//如果材料类型为空，勾连物料主数据中的材料类型。

			if (tmmsm56["MAT_TYPE"].ToString().Trim() == "")
			{
				sqlstr = "SELECT  MAT_TYPE "
					" FROM    TMMSM50 "
					" WHERE   MAT_CODE = @mat_code";

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("mat_code", tmmsm56["MAT_CODE"].ToString());
				cmd_sql.ExecuteReader();
				if (cmd_sql.Read())
				{
					tmmsm56["MAT_TYPE"] = cmd_sql.GetString(1);
				}
				else
				{
					strcpy(s.msg, "该物料代码对应的材料类型不存在!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				cmd_sql.Close();

			}

			/****** 程序处理 ******/
			if (v_proc_div == "I")
			{
				tmmsm56["REC_CREATE_TIME"] = dateNow;
				tmmsm56["REC_CREATOR"] = s.userid;

				//出库单号生成

				tmmsm56["OUT_STOCK_NO"] = "C" + dateNow;
				tmmsm56["AFFIRM_FLAG"] = "0";
				tmmsm56.Insert();
			}
			else if (v_proc_div == "U")
			{

				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR,AFFIRM_FLAG"
					"	   FROM	TMMSM56 "
					"      WHERE  OUT_STOCK_NO	= @out_stock_no";

				//Log::Info("", __FUNCTION__, "sqlstr=[{0}]  tmmsm56["OUT_STOCK_NO"] =[{1}]", sqlstr,tmmsm56["OUT_STOCK_NO"].ToString());

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("out_stock_no", tmmsm56["OUT_STOCK_NO"].ToString());

				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsm56["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsm56["REC_CREATOR"] = cmd_sql.GetString(2);
					tmmsm56["AFFIRM_FLAG"] = cmd_sql.GetString(3);
				}
				cmd_sql.Close();

				//已经出库确认的不允许修改
				if (tmmsm56["AFFIRM_FLAG"].ToString() == "1")
				{
					strcpy(s.msg, "该出库单号已经出库确认，不允许修改!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				tmmsm56["REC_REVISE_TIME"] = dateNow;
				tmmsm56["REC_REVISOR"] = s.userid;

				tmmsm56.Delete();
				tmmsm56.Insert();
			}
			else if (v_proc_div == "D")
			{
				//判断是否出库确认，如果确认，不允许删除
				sqlstr = " SELECT AFFIRM_FLAG"
					" FROM TMMSM56"
					" WHERE OUT_STOCK_NO = @out_stock_no";

				//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("out_stock_no", tmmsm56["OUT_STOCK_NO"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsm56["AFFIRM_FLAG"] = cmd_sql.GetString(1);
				}
				cmd_sql.Close();

				//已经出库确认的不允许修改
				if (tmmsm56["AFFIRM_FLAG"].ToString() == "1")
				{
					strcpy(s.msg, "该出库单号已经出库确认，不允许删除!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				tmmsm56.Delete();
			}
			else if (v_proc_div == "Y") //出库确认
			{
				//修正出库表
				tmmsm56["REC_REVISE_TIME"] = dateNow;
				tmmsm56["REC_REVISOR"] = s.userid;
				//tmmsm56["OUT_STOCK_TIME"] = dateNow;
				tmmsm56["AFFIRM_TIME"] = dateNow;
				tmmsm56["CONFM_BY"] = s.userid;
				tmmsm56["AFFIRM_FLAG"] = "1";

				tmmsm56.Update("REC_REVISOR, REC_REVISE_TIME, AFFIRM_TIME,CONFM_BY,AFFIRM_FLAG", "OUT_STOCK_NO");
				

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
						"		WHERE FACTORY_DIV =  @factory_div"
						"		AND   MAT_CODE =  @mat_code";

					break;
				}
				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("factory_div", tmmsm56["FACTORY_DIV"].ToString());
				cmd_sql.Parameters.Set("mat_code", tmmsm56["MAT_CODE"].ToString());
				cmd_sql.ExecuteReader();
				if (cmd_sql.Read())
				{
					tmmsm51["STOCK_WT"] = cmd_sql.GetDecimal(1);
				}
				else
				{
					strcpy(s.msg, "该物料代码库存为0，出库确认出错!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				cmd_sql.Close();

				//写库存表
				tmmsm51["REC_REVISE_TIME"] = dateNow;
				tmmsm51["REC_REVISOR"] = s.userid;
				tmmsm51["FACTORY_DIV"] = tmmsm56["FACTORY_DIV"];
				tmmsm51["STOCK_PLACE_NO"] = tmmsm56["STOCK_PLACE_NO"];
				tmmsm51["MAT_CODE"] = tmmsm56["MAT_CODE"];
				tmmsm51["STOCK_WT"] = tmmsm51["STOCK_WT"].ToDecimal() - tmmsm56["OUT_STOCK_WT"];

				tmmsm51.Update("REC_REVISOR, REC_REVISE_TIME,STOCK_WT", "FACTORY_DIV,STOCK_PLACE_NO,MAT_CODE");


				//写收发明细表
				//f_mmsm_get_seq_no(tmmsm53["RESUME_SEQ_NO"].ToString(), conn);
				f_mmsm_get_seq_no(resume_seq_no , conn);
				tmmsm53["IN_STOCK_NO"] = resume_seq_no;

				tmmsm53["REC_CREATE_TIME"] = dateNow;
				tmmsm53["REC_CREATOR"] = s.userid;
				tmmsm53["FACTORY_DIV"] = tmmsm56["FACTORY_DIV"];
				tmmsm53["COMPANY_DIV"] = tmmsm56["TO_FACTORY"];

				tmmsm53["HANDLE_DIV"] = tmmsm56["HANDLE_DIV"];
				tmmsm53["OTHER_BILL_NO"] = tmmsm56["OUT_STOCK_NO"];
				tmmsm53["MAT_CODE"] = tmmsm56["MAT_CODE"];
				tmmsm53["MAT_NAME"] = tmmsm56["MAT_NAME"];
				tmmsm53["MAT_TYPE"] = tmmsm56["MAT_TYPE"];
				tmmsm53["STOCK_WT"] = tmmsm56["OUT_STOCK_WT"];

				tmmsm53.Insert();
			}
			else if (v_proc_div == "N")//出库确认回退
			{
				//修正出库表
				tmmsm56["REC_REVISE_TIME"] = dateNow;
				tmmsm56["REC_REVISOR"] = s.userid;
				tmmsm56["OUT_STOCK_TIME"] = " ";
				tmmsm56["AFFIRM_TIME"] = " ";
				tmmsm56["CONFM_BY"] = " ";
				tmmsm56["AFFIRM_FLAG"] = "0";

				tmmsm56.Update("REC_REVISOR, REC_REVISE_TIME, AFFIRM_TIME,CONFM_BY,AFFIRM_FLAG", "OUT_STOCK_NO");

				//修正库存表
				//对应物料代码对应的原库存
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	    // Oracle 数据库
				default:
					sqlstr = "SELECT STOCK_WT "
						"		FROM TMMSM51 "
						"		WHERE FACTORY_DIV =  @factory_div"
						"		AND   MAT_CODE =  @mat_code";

					break;
				}
				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("factory_div", tmmsm56["FACTORY_DIV"].ToString());
				cmd_sql.Parameters.Set("mat_code", tmmsm56["MAT_CODE"].ToString());
				cmd_sql.ExecuteReader();
				if (cmd_sql.Read())
				{
					tmmsm51["STOCK_WT"] = cmd_sql.GetDecimal(1);
				}

				cmd_sql.Close();

				tmmsm51["REC_REVISE_TIME"] = dateNow;
				tmmsm51["REC_REVISOR"] = s.userid;
				tmmsm51["FACTORY_DIV"] = tmmsm56["FACTORY_DIV"];
				tmmsm51["STOCK_PLACE_NO"] = tmmsm56["STOCK_PLACE_NO"];
				tmmsm51["MAT_CODE"] = tmmsm56["MAT_CODE"];
				tmmsm51["STOCK_WT"] = tmmsm51["STOCK_WT"].ToDecimal() + tmmsm56["OUT_STOCK_WT"].ToDecimal();

				tmmsm51.Update("REC_REVISOR, REC_REVISE_TIME,STOCK_WT", "FACTORY_DIV,STOCK_PLACE_NO,MAT_CODE");

				//写收发明细表
				//f_mmsm_get_seq_no(tmmsm53["RESUME_SEQ_NO"].ToString(), conn);
				f_mmsm_get_seq_no(resume_seq_no, conn);
				tmmsm53["IN_STOCK_NO"] = resume_seq_no;
				tmmsm53["REC_CREATE_TIME"] = dateNow;
				tmmsm53["REC_CREATOR"] = s.userid;
				tmmsm53["FACTORY_DIV"] = tmmsm56["FACTORY_DIV"];
				tmmsm53["COMPANY_DIV"] = tmmsm56["TO_FACTORY"];

				tmmsm53["HANDLE_DIV"] = tmmsm56["HANDLE_DIV"];
				tmmsm53["OTHER_BILL_NO"] = tmmsm56["OUT_STOCK_NO"];
				tmmsm53["MAT_CODE"] = tmmsm56["MAT_CODE"];
				tmmsm53["MAT_NAME"] = tmmsm56["MAT_NAME"];
				tmmsm53["MAT_TYPE"] = tmmsm56["MAT_TYPE"];
				tmmsm53["STOCK_WT"] = 0 - tmmsm56["OUT_STOCK_WT"].ToDecimal();

				tmmsm53.Insert();

			}
			else if (v_proc_div == "UY")
			{
				//修正出库表
				tmmsm56["REC_REVISE_TIME"] = dateNow;
				tmmsm56["REC_REVISOR"] = s.userid;
				//tmmsm56["OUT_STOCK_TIME"] = dateNow;
				tmmsm56["AFFIRM_TIME"] = dateNow;
				tmmsm56["CONFM_BY"] = s.userid;
				tmmsm56["AFFIRM_FLAG"] = "1";

				tmmsm56.Update("REC_REVISOR, REC_REVISE_TIME,OUT_STOCK_TIME,AFFIRM_TIME,CONFM_BY,AFFIRM_FLAG,VEHICLE_NO", "OUT_STOCK_NO,MAT_CODE");

				//查询最新的TMMSM56信息

				tmmsm56.Query("OUT_STOCK_NO,MAT_CODE");

				/*tmmsm56.Print();*/


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
						"		WHERE FACTORY_DIV =  @factory_div"
						"		AND   MAT_CODE =  @mat_code";

					break;
				}
				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("factory_div", tmmsm56["FACTORY_DIV"].ToString());
				cmd_sql.Parameters.Set("mat_code", tmmsm56["MAT_CODE"].ToString());
				cmd_sql.ExecuteReader();
				if (cmd_sql.Read())
				{
					tmmsm51["STOCK_WT"] = cmd_sql.GetDecimal(1);
				}
				else
				{
					strcpy(s.msg, "该物料代码库存为0，出库确认出错!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				cmd_sql.Close();

				//写库存表
				tmmsm51["REC_REVISE_TIME"] = dateNow;
				tmmsm51["REC_REVISOR"] = s.userid;
				tmmsm51["FACTORY_DIV"] = tmmsm56["FACTORY_DIV"];
				tmmsm51["STOCK_PLACE_NO"] = tmmsm56["STOCK_PLACE_NO"];
				tmmsm51["MAT_CODE"] = tmmsm56["MAT_CODE"];
				tmmsm51["STOCK_WT"] = tmmsm51["STOCK_WT"].ToDecimal() - tmmsm56["OUT_STOCK_WT"];
				tmmsm51.Update("REC_REVISOR, REC_REVISE_TIME,STOCK_WT", "FACTORY_DIV,STOCK_PLACE_NO,MAT_CODE");


				//写收发明细表
				//f_mmsm_get_seq_no(tmmsm53["RESUME_SEQ_NO"].ToString(), conn);
				f_mmsm_get_seq_no(resume_seq_no, conn);
				tmmsm53["IN_STOCK_NO"] = resume_seq_no;
				tmmsm53["REC_CREATE_TIME"] = dateNow;
				tmmsm53["REC_CREATOR"] = s.userid;
				tmmsm53["FACTORY_DIV"] = tmmsm56["FACTORY_DIV"];
				tmmsm53["COMPANY_DIV"] = tmmsm56["TO_FACTORY"];
				tmmsm53["HANDLE_DIV"] = tmmsm56["HANDLE_DIV"];
				tmmsm53["OTHER_BILL_NO"] = tmmsm56["OUT_STOCK_NO"];
				tmmsm53["MAT_CODE"] = tmmsm56["MAT_CODE"];
				tmmsm53["MAT_NAME"] = tmmsm56["MAT_NAME"];
				tmmsm53["MAT_TYPE"] = tmmsm56["MAT_TYPE"];
				tmmsm53["STOCK_WT"] = tmmsm56["OUT_STOCK_WT"];
				tmmsm53["PLAN_NO_Y"] = tmmsm56["PLAN_NO_Y"];
				tmmsm53.Insert();
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

