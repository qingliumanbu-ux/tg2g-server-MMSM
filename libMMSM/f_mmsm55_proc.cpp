/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 原辅料入库
<para>

***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

  
  
  
  



//外部函数声明
int f_mmsm_get_seq_no(CString& resume_seq_no, CDbConnection * conn);
//int f_posi_10(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_get_seq_no(CString& resume_seq_no, CString seq_name, CDecimal bit_num, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm55_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	/****** 定义函数名称 ***** */
CString FunctionEname = "f_mmsm55_proc";                //定义函数英文名称  
CString FunctionCname = "原辅料入库";              //定义函数中文名称


CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
  

  //程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   n_count = 0;
	int   blkNum;
	CString sqlstr="";
	CString  v_proc_div = "";
	CString  resume_seq_no = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
			   
  
	try
	{
		CPageInfo pageInfo;

		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
	CModel tmmsm55("TMMSM55");
	CModel tmmsm51("TMMSM51");
	CModel tmmsm53("TMMSM53");
	CModel tmmsm57("TMMSM57");
	CModel tmmsm55a("TMMSM55A");
		/* 获取输入参数*/
		if (!bcls_rec->Tables.Contains("MMSM55"))
		{
			bcls_rec->Tables[0].set_TableName("MMSM55");
		}
		for (int i = 0; i < bcls_rec->Tables["MMSM55"].Rows.get_Count(); i++)
		{
			tmmsm55.Reset();
			tmmsm55.MergeFrom(bcls_rec->Tables["MMSM55"].Rows[i]);
			tmmsm55.TrimOrBlank();

			if (bcls_rec->Tables["PARA"].Columns.Contains("PROC_DIV"))   //增删改区分
				v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString().Trim();


			if (bcls_rec->Tables["MMSM55"].Columns.Contains("MAT_TYPE"))   //材料类型
				tmmsm55["MAT_TYPE"] = bcls_rec->Tables["MMSM55"].Rows[0]["MAT_TYPE"].ToString().Trim();

			//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);

			/****** 程序处理 ******/
			if (v_proc_div == "I")
			{
				tmmsm55["REC_CREATE_TIME"] = dateNow;
				tmmsm55["REC_CREATOR"] = s.userid;

				if (tmmsm55["MAT_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg, "该批次号物料代码为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm55["IN_STOCK_TIME"].ToDecimal() == 0)
				{
					strcpy(s.msg, "入库时刻不能为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//如果材料类型为空，勾连物料主数据中的材料类型。

				if (tmmsm55["MAT_TYPE"].ToString().Trim() == "")
				{
					sqlstr = "SELECT  MAT_TYPE "
						" FROM    TMMSM50 "
						" WHERE   MAT_CODE = @mat_code";

					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.Parameters.Clear();
					cmd_sql.Parameters.Set("mat_code", tmmsm55["MAT_CODE"].ToString());
					cmd_sql.ExecuteReader();
					if (cmd_sql.Read())
					{
						tmmsm55["MAT_TYPE"] = cmd_sql.GetString(1);
					}
					cmd_sql.Close();

					//Log::Trace("", __FUNCTION__, "v_proc_div11111=[{0}]", v_proc_div);

				}

				//入库单号生成
				Log::Trace("", __FUNCTION__, "v_proc_div1111=[{0}]", v_proc_div);
				f_mmsm_get_seq_no(resume_seq_no, "MMSM_MATNO_SEQ", 4, conn);
				tmmsm55["IN_STOCK_NO"] = resume_seq_no;
				tmmsm55["IN_STOCK_NO"] = "R" +tmmsm55["IN_STOCK_NO"].ToString().SubstringNE(2);
				tmmsm55["AFFIRM_FLAG"] = "0";

				tmmsm55.Print();
				tmmsm55.Insert();

				//更新收发存
			}
			else if (v_proc_div == "U")
			{

				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR,AFFIRM_FLAG"
					"	   FROM	TMMSM55 "
					"      WHERE  IN_STOCK_NO	= @in_stock_no";

				//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("in_stock_no", tmmsm55["IN_STOCK_NO"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsm55["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsm55["REC_CREATOR"] = cmd_sql.GetString(2);
					tmmsm55["AFFIRM_FLAG"] = cmd_sql.GetString(3);
				}
				cmd_sql.Close();

				//已经入库确认的不允许修改
				if (tmmsm55["AFFIRM_FLAG"].ToString() == "1")
				{
					strcpy(s.msg, "该入库单号已经入库确认，不允许修改!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				tmmsm55["REC_REVISE_TIME"] = dateNow;
				tmmsm55["REC_REVISOR"] = s.userid;
				tmmsm55.Delete();
				tmmsm55.Insert();
			}
			else if (v_proc_div == "D")
			{
				//判断是否入库确认，如果确认，不允许删除
				sqlstr = " SELECT AFFIRM_FLAG"
					"	   FROM	TMMSM55 "
					"      WHERE  IN_STOCK_NO	= @in_stock_no";

				//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("in_stock_no", tmmsm55["IN_STOCK_NO"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsm55["AFFIRM_FLAG"] = cmd_sql.GetString(1);
				}
				cmd_sql.Close();

				//已经入库确认的不允许修改
				if (tmmsm55["AFFIRM_FLAG"].ToString() == "1")
				{
					strcpy(s.msg, "该入库单号已经入库确认，不允许删除!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				tmmsm55.Delete();

				tmmsm55a.Reset();
				tmmsm55a.MergeFrom(bcls_rec->Tables["MMSM55"].Rows[i]);

				tmmsm55a.TrimOrBlank();
				/* 删除事件信息 */
				tmmsm55a.Delete("MAT_CODE,IN_STOCK_NO");


			}
			else if (v_proc_div == "Y") //入库确认
			{
				tmmsm55["REC_REVISE_TIME"] = dateNow;
				tmmsm55["REC_REVISOR"] = s.userid;
				//tmmsm55["IN_STOCK_TIME"] = dateNow;
				tmmsm55["AFFIRM_TIME"] = dateNow;
				tmmsm55["CONFM_BY"] = s.userid;
				tmmsm55["AFFIRM_FLAG"] = "1";

				tmmsm55.Update("REC_REVISOR, REC_REVISE_TIME,IN_STOCK_TIME,AFFIRM_TIME,CONFM_BY,AFFIRM_FLAG", "IN_STOCK_NO,MAT_CODE");

				tmmsm55.Query("IN_STOCK_NO,MAT_CODE");

				//Log::Trace("", __FUNCTION__, "v_proc_div111111111111111=[{0}]", v_proc_div);

				//写即时库存表
				//判断该物料代码是否存在，如果不存在，新增。存在，修改
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
				cmd_sql.Parameters.Set("factory_div", tmmsm55["FACTORY_DIV"].ToString());
				cmd_sql.Parameters.Set("mat_code", tmmsm55["MAT_CODE"].ToString());
				cmd_sql.ExecuteReader();
				//Log::Trace("", __FUNCTION__, "tmmsm55["MAT_CODE"] =[{0}]", tmmsm55["MAT_CODE"].ToString());
				if (cmd_sql.Read() == true)
				{

					tmmsm51["STOCK_WT"] = cmd_sql.GetDecimal(1);

					tmmsm51["REC_REVISE_TIME"] = dateNow;
					tmmsm51["REC_REVISOR"] = s.userid;
					tmmsm51["FACTORY_DIV"] = tmmsm55["FACTORY_DIV"];
					tmmsm51["STOCK_PLACE_NO"] = tmmsm55["STOCK_PLACE_NO"];
					tmmsm51["MAT_CODE"] = tmmsm55["MAT_CODE"];
					tmmsm51["STOCK_WT"] = tmmsm51["STOCK_WT"].ToDecimal() + tmmsm55["IN_STOCK_WT"].ToDecimal();

					//tmmsm51.Print();
					//Log::Trace("", __FUNCTION__, "cmd_sql.Read1()=[{0}]", "1");
					tmmsm51.Update("REC_REVISOR, REC_REVISE_TIME,STOCK_WT", "FACTORY_DIV,MAT_CODE");
				}
				else
				{

					tmmsm51["REC_CREATE_TIME"] = dateNow;
					tmmsm51["REC_CREATOR"] = s.userid;
					tmmsm51["FACTORY_DIV"] = tmmsm55["FACTORY_DIV"];
					tmmsm51["STOCK_PLACE_NO"] = tmmsm55["STOCK_PLACE_NO"];
					tmmsm51["MAT_CODE"] = tmmsm55["MAT_CODE"];
					tmmsm51["MAT_NAME"] = tmmsm55["MAT_NAME"];
					tmmsm51["MAT_TYPE"] = tmmsm55["MAT_TYPE"];
					tmmsm51["STOCK_WT"] = tmmsm51["STOCK_WT"].ToDecimal() + tmmsm55["IN_STOCK_WT"].ToDecimal();
					//Log::Trace("", __FUNCTION__, "cmd_sql.Read2()=[{0}]", "2");
					//tmmsm51.Print();
					tmmsm51.Insert();
				}
				cmd_sql.Close();


				//写收发明细表
				tmmsm53["REC_CREATE_TIME"] = dateNow;
				tmmsm53["REC_CREATOR"] = s.userid;

				tmmsm53["FACTORY_DIV"] = tmmsm55["FACTORY_DIV"];
				tmmsm53["COMPANY_DIV"] = tmmsm55["FROM_FACTORY"];

				tmmsm53["HANDLE_DIV"] = tmmsm55["HANDLE_DIV"];
				tmmsm53["OTHER_BILL_NO"] = tmmsm55["IN_STOCK_NO"];
				tmmsm53["MAT_CODE"] = tmmsm55["MAT_CODE"];
				tmmsm53["MAT_NAME"] = tmmsm55["MAT_NAME"];
				tmmsm53["MAT_TYPE"] = tmmsm55["MAT_TYPE"];
				tmmsm53["STOCK_WT"] = tmmsm55["IN_STOCK_WT"];
				tmmsm53["PLAN_NO_Y"] = tmmsm55["PLAN_NO_Y"];
				//调用公共函数返回履历序号
				//f_mmsm_get_seq_no(tmmsm53["RESUME_SEQ_NO"].ToString(), conn);
				f_mmsm_get_seq_no(resume_seq_no, "MMSM_MATNO_SEQ", 4, conn);
				tmmsm53["IN_STOCK_NO"] = resume_seq_no;
				tmmsm53.Insert();

				//f_posi_10参数设置 //操作类别-operate_type(新增默认写submit 撤销默认写unsubmit);公司别-COMPANY_CODE; 磅单号 -IN_STOCK_APPLY_NO 库区 IN_STOCK_STOCK_NO 入库时间IN_STOCK_TIME
				if (!bcls_rec->Tables[0].Columns.Contains("OPERATE_TYPE"))
				{
					bcls_rec->Tables[0].Columns.Add(DT_STRING, "OPERATE_TYPE");
				}
				bcls_rec->Tables[0].Rows[0]["OPERATE_TYPE"] = "submit";
				if (!bcls_rec->Tables[0].Columns.Contains("COMPANY_CODE"))
				{
					bcls_rec->Tables[0].Columns.Add(DT_STRING, "COMPANY_CODE");
				}
				bcls_rec->Tables[0].Rows[0]["COMPANY_CODE"] = "JSKJ";
				if (!bcls_rec->Tables[0].Columns.Contains("IN_STOCK_APPLY_NO"))
				{
					bcls_rec->Tables[0].Columns.Add(DT_STRING, "IN_STOCK_APPLY_NO");
				}
				bcls_rec->Tables[0].Rows[0]["IN_STOCK_APPLY_NO"] = tmmsm55["WEIGH_NO"];
				if (!bcls_rec->Tables[0].Columns.Contains("IN_STOCK_STOCK_NO"))
				{
					bcls_rec->Tables[0].Columns.Add(DT_STRING, "IN_STOCK_STOCK_NO");
				}
				bcls_rec->Tables[0].Rows[0]["IN_STOCK_STOCK_NO"] = tmmsm55["RECV_DEPT_CODE"];
				if (!bcls_rec->Tables[0].Columns.Contains("IN_STOCK_TIME"))
				{
					bcls_rec->Tables[0].Columns.Add(DT_STRING, "IN_STOCK_TIME");
				}
				bcls_rec->Tables[0].Rows[0]["IN_STOCK_TIME"] = tmmsm55["IN_STOCK_TIME"];

				//Log::Trace("", __FUNCTION__, "操作类别OPERATE_TYPE=[{0}] ", bcls_rec->Tables[0].Rows[0]["OPERATE_TYPE"].ToString().Trim());
				//Log::Trace("", __FUNCTION__, "公司别COMPANY_CODE=[{0}] ", bcls_rec->Tables[0].Rows[0]["COMPANY_CODE"].ToString().Trim());
				//Log::Trace("", __FUNCTION__, "磅单号IN_STOCK_APPLY_NO=[{0}] ", bcls_rec->Tables[0].Rows[0]["IN_STOCK_APPLY_NO"].ToString().Trim());
				//Log::Trace("", __FUNCTION__, "库区IN_STOCK_STOCK_NO=[{0}] ", bcls_rec->Tables[0].Rows[0]["IN_STOCK_STOCK_NO"].ToString().Trim());
				//Log::Trace("", __FUNCTION__, "入库时间IN_STOCK_TIME=[{0}]", bcls_rec->Tables[0].Rows[0]["IN_STOCK_TIME"].ToString().Trim());

				//doFlag = f_posi_10(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			else if (v_proc_div == "N")//入库确认回退
			{
				//修正入库表
				tmmsm55["REC_REVISE_TIME"] = dateNow;
				tmmsm55["REC_REVISOR"] = s.userid;
				tmmsm55["IN_STOCK_TIME"] = " ";
				tmmsm55["AFFIRM_TIME"] = " ";
				tmmsm55["CONFM_BY"] = " ";
				tmmsm55["AFFIRM_FLAG"] = "0";

				tmmsm55.Update("REC_REVISOR, REC_REVISE_TIME, AFFIRM_TIME,CONFM_BY,AFFIRM_FLAG", "IN_STOCK_NO");

				//Log::Trace("", __FUNCTION__, "tmmsm55.MAT_CODE1111111111111111111=[{0}]", tmmsm55["MAT_CODE"].ToString());

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
						"		WHERE FACTORY_DIV =  @factory_div";
					"		AND   MAT_CODE =  @mat_code";

					break;
				}
				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("factory_div", tmmsm55["FACTORY_DIV"].ToString());
				cmd_sql.Parameters.Set("mat_code", tmmsm55["MAT_CODE"].ToString());
				cmd_sql.ExecuteReader();
				if (cmd_sql.Read())
				{
					tmmsm51["STOCK_WT"] = cmd_sql.GetDecimal(1);
				}

				tmmsm51["REC_REVISE_TIME"] = dateNow;
				tmmsm51["REC_REVISOR"] = s.userid;
				tmmsm51["FACTORY_DIV"] = tmmsm55["FACTORY_DIV"];
				tmmsm51["STOCK_PLACE_NO"] = tmmsm55["STOCK_PLACE_NO"];
				tmmsm51["MAT_CODE"] = tmmsm55["MAT_CODE"];
				tmmsm51["STOCK_WT"] = tmmsm51["STOCK_WT"].ToDecimal() - tmmsm55["IN_STOCK_WT"];

				tmmsm51.Update("REC_REVISOR, REC_REVISE_TIME,STOCK_WT", "FACTORY_DIV,STOCK_PLACE_NO,MAT_CODE");

				//Log::Trace("", __FUNCTION__, "tmmsm55.MAT_CODE22222222222222=[{0}]", tmmsm55["MAT_CODE"].ToString());

				//写收发明细表
				tmmsm53["REC_CREATE_TIME"] = dateNow;
				tmmsm53["REC_CREATOR"] = s.userid;

				tmmsm53["FACTORY_DIV"] = tmmsm55["FACTORY_DIV"];
				tmmsm53["COMPANY_DIV"] = tmmsm55["FROM_FACTORY"];

				tmmsm53["HANDLE_DIV"] = tmmsm55["HANDLE_DIV"];
				tmmsm53["OTHER_BILL_NO"] = tmmsm55["IN_STOCK_NO"];
				tmmsm53["MAT_CODE"] = tmmsm55["MAT_CODE"];
				tmmsm53["MAT_NAME"] = tmmsm55["MAT_NAME"];
				tmmsm53["MAT_TYPE"] = tmmsm55["MAT_TYPE"];
				tmmsm53["STOCK_WT"] = 0 - tmmsm55["IN_STOCK_WT"].ToDecimal();

				//调用公共函数返回履历序号
				//f_mmsm_get_seq_no(tmmsm53["RESUME_SEQ_NO"].ToString(), conn);
				f_mmsm_get_seq_no(resume_seq_no, "MMSM_MATNO_SEQ", 4, conn);
				tmmsm53["IN_STOCK_NO"] = resume_seq_no;
				//Log::Trace("", __FUNCTION__, "tmmsm55.MAT_COD3333333333333333333E=[{0}]", tmmsm55["MAT_CODE"].ToString());
				//tmmsm53["RESUME_SEQ_NO"] = 999;

				tmmsm53.Insert();

				//f_posi_10参数设置 //操作类别-operate_type(新增默认写submit 撤销默认写unsubmit);公司别-COMPANY_CODE; 磅单号 -IN_STOCK_APPLY_NO 库区 IN_STOCK_STOCK_NO 入库时间IN_STOCK_TIME
				if (!bcls_rec->Tables[0].Columns.Contains("OPERATE_TYPE"))
				{
					bcls_rec->Tables[0].Columns.Add(DT_STRING, "OPERATE_TYPE");
				}
				bcls_rec->Tables[0].Rows[0]["OPERATE_TYPE"] = "unsubmit";
				if (!bcls_rec->Tables[0].Columns.Contains("COMPANY_CODE"))
				{
					bcls_rec->Tables[0].Columns.Add(DT_STRING, "COMPANY_CODE");
				}
				bcls_rec->Tables[0].Rows[0]["COMPANY_CODE"] = "JSKJ";
				if (!bcls_rec->Tables[0].Columns.Contains("IN_STOCK_APPLY_NO"))
				{
					bcls_rec->Tables[0].Columns.Add(DT_STRING, "IN_STOCK_APPLY_NO");
				}
				bcls_rec->Tables[0].Rows[0]["IN_STOCK_APPLY_NO"] = tmmsm55["WEIGH_NO"];
				if (!bcls_rec->Tables[0].Columns.Contains("IN_STOCK_STOCK_NO"))
				{
					bcls_rec->Tables[0].Columns.Add(DT_STRING, "IN_STOCK_STOCK_NO");
				}
				bcls_rec->Tables[0].Rows[0]["IN_STOCK_STOCK_NO"] = tmmsm55["RECV_DEPT_CODE"];
				if (!bcls_rec->Tables[0].Columns.Contains("IN_STOCK_TIME"))
				{
					bcls_rec->Tables[0].Columns.Add(DT_STRING, "IN_STOCK_TIME");
				}
				bcls_rec->Tables[0].Rows[0]["IN_STOCK_TIME"] = tmmsm55["TARE_TIME"];

				//Log::Trace("", __FUNCTION__, "操作类别OPERATE_TYPE=[{0}] ", bcls_rec->Tables[0].Rows[0]["OPERATE_TYPE"].ToString().Trim());
				//Log::Trace("", __FUNCTION__, "公司别COMPANY_CODE=[{0}] ", bcls_rec->Tables[0].Rows[0]["COMPANY_CODE"].ToString().Trim());
				//Log::Trace("", __FUNCTION__, "磅单号IN_STOCK_APPLY_NO=[{0}] ", bcls_rec->Tables[0].Rows[0]["IN_STOCK_APPLY_NO"].ToString().Trim());
				//Log::Trace("", __FUNCTION__, "库区IN_STOCK_STOCK_NO=[{0}] ", bcls_rec->Tables[0].Rows[0]["IN_STOCK_STOCK_NO"].ToString().Trim());
				//Log::Trace("", __FUNCTION__, "入库时间IN_STOCK_TIME=[{0}]", bcls_rec->Tables[0].Rows[0]["IN_STOCK_TIME"].ToString().Trim());

				//doFlag = f_posi_10(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

			}
#pragma region 厂内转库更新入库重量  modify by ShiYong @20160521
			else if (v_proc_div == "UY_WT")
			{
	CModel hmmsm55("TMMSM55");
				hmmsm55["OTHER_BILL_NO"] = tmmsm55["OTHER_BILL_NO"];
				hmmsm55["MAT_CODE"] = tmmsm55["MAT_CODE"];
				//Log::Trace("", "", "hmmsm55["OTHER_BILL_NO"] = {0},hmmsm55["MAT_CODE"] = {1}", hmmsm55["OTHER_BILL_NO"].ToString(), hmmsm55["MAT_CODE"].ToString());
				hmmsm55.Query("OTHER_BILL_NO,MAT_CODE");
				//Log::Trace("", "", "hmmsm55["IN_STOCK_NO"] = {0},hmmsm55["AFFIRM_FLAG"] = {1}", hmmsm55["IN_STOCK_NO"].ToString(), hmmsm55["AFFIRM_FLAG"].ToString());

				//修正入库表
				tmmsm55["REC_REVISE_TIME"] = dateNow;
				tmmsm55["REC_REVISOR"] = s.userid;
				tmmsm55["IN_STOCK_NO"] = hmmsm55["IN_STOCK_NO"];
				tmmsm55.Update("REC_REVISOR,REC_REVISE_TIME,WT_ENTRUST,WT_TIME,GROSS_WT,IN_STOCK_WT,VEHICLE_NO", "IN_STOCK_NO,MAT_CODE");

				if (hmmsm55["AFFIRM_FLAG"].ToString() == "1" && tmmsm55["IN_STOCK_WT"].ToDecimal() != hmmsm55["IN_STOCK_WT"].ToDecimal())
				{
					//写库存表
					cmd_sql.SetCommandText("UPDATE TMMSM51 SET REC_REVISE_TIME = @dateNow,REC_REVISOR = @s.userid,STOCK_WT = STOCK_WT - @hmmsm55.IN_STOCK_WT + @tmmsm55.IN_STOCK_WT "
						"WHERE FACTORY_DIV = @hmmsm55.FACTORY_DIV AND STOCK_PLACE_NO = @hmmsm55.STOCK_PLACE_NO AND MAT_CODE = @tmmsm55.MAT_CODE");
					cmd_sql.Parameters.Set("dateNow", dateNow);
					cmd_sql.Parameters.Set("s.userid", s.userid);
					cmd_sql.Parameters.Set("hmmsm55.IN_STOCK_WT", hmmsm55["IN_STOCK_WT"].ToDecimal());
					cmd_sql.Parameters.Set("tmmsm55.IN_STOCK_WT", tmmsm55["IN_STOCK_WT"].ToDecimal());
					cmd_sql.Parameters.Set("hmmsm55.FACTORY_DIV", hmmsm55["FACTORY_DIV"].ToString());
					cmd_sql.Parameters.Set("hmmsm55.STOCK_PLACE_NO", hmmsm55["STOCK_PLACE_NO"].ToString());
					cmd_sql.Parameters.Set("tmmsm55.MAT_CODE", tmmsm55["MAT_CODE"].ToString());
					cmd_sql.ExecuteNonQuery();

					//写收发明细表
					//原相应入库明细冲
					//f_mmsm_get_seq_no(tmmsm53["RESUME_SEQ_NO"].ToString(), conn);
					f_mmsm_get_seq_no(resume_seq_no, "MMSM_MATNO_SEQ", 4, conn);
					tmmsm53["IN_STOCK_NO"] = resume_seq_no;
					tmmsm53["REC_CREATE_TIME"] = dateNow;
					tmmsm53["REC_CREATOR"] = s.userid;
					tmmsm53["FACTORY_DIV"] = hmmsm55["FACTORY_DIV"];
					tmmsm53["COMPANY_DIV"] = hmmsm55["FROM_FACTORY"];
					tmmsm53["HANDLE_DIV"] = hmmsm55["HANDLE_DIV"];
					tmmsm53["OTHER_BILL_NO"] = tmmsm55["IN_STOCK_NO"];
					tmmsm53["MAT_CODE"] = hmmsm55["MAT_CODE"];
					tmmsm53["MAT_NAME"] = hmmsm55["MAT_NAME"];
					tmmsm53["MAT_TYPE"] = hmmsm55["MAT_TYPE"];
					tmmsm53["STOCK_WT"] = hmmsm55["IN_STOCK_WT"].ToDecimal() * (-1);
					tmmsm53["PLAN_NO_Y"] = hmmsm55["PLAN_NO_Y"];
					tmmsm53["SUB_RECORD_NO"] = tmmsm53["RESUME_SEQ_NO"];
					tmmsm53.Insert();

					//新相应入库明细增
					//f_mmsm_get_seq_no(tmmsm53["RESUME_SEQ_NO"].ToString(), conn);
					f_mmsm_get_seq_no(resume_seq_no, "MMSM_MATNO_SEQ", 4, conn);
					tmmsm53["IN_STOCK_NO"] = resume_seq_no;
					tmmsm53["STOCK_WT"] = tmmsm55["IN_STOCK_WT"];
					tmmsm53.Insert();
				}
			}
#pragma endregion
#pragma region 接收外部系统入库信息处理 modify by ShiYong @20160522
			else if (v_proc_div == "RCV")
			{
				CString operDiv = bcls_rec->Tables[0].Rows[0]["OPER_DIV"].ToString().Trim();
				tmmsm55["OTHER_BILL_NO"] = bcls_rec->Tables[0].Rows[0]["OUT_STOCK_NO"].ToString().Trim();
				tmmsm55["PLAN_NO_Y"] = bcls_rec->Tables[0].Rows[0]["RETURN_PLAN"].ToString().Trim();
				tmmsm55["VEHICLE_NO"] = bcls_rec->Tables[0].Rows[0]["VEHICLE_NO"].ToString().Trim();
				tmmsm55["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["RET_DEPT"].ToString().Trim();
				tmmsm55["MAT_CODE"] = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
				tmmsm55["MAT_NAME"] = bcls_rec->Tables[0].Rows[0]["MAT_NAME"].ToString().Trim();
				tmmsm55["IN_STOCK_WT"] = bcls_rec->Tables[0].Rows[0]["MAT_ACT_WT"].ToDecimal();
				tmmsm55["WT_TIME"] = bcls_rec->Tables[0].Rows[0]["WEIGHT_TIME"].ToString().Trim();

	CModel hmmsm55("TMMSM55");
				hmmsm55["OTHER_BILL_NO"] = tmmsm55["OTHER_BILL_NO"];
				hmmsm55["MAT_CODE"] = tmmsm55["MAT_CODE"];
				bool ifExist = hmmsm55.Query("OTHER_BILL_NO,MAT_CODE");

				if (operDiv == "D" && ifExist == true)
				{
					if (hmmsm55["AFFIRM_FLAG"].ToString() == "1")
					{
						strcpy(s.msg, "该入库单号已经入库确认，不允许删除!");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					tmmsm55.Delete();
				}
				else if (operDiv != "D")
				{
					if (ifExist == false)
					{
						tmmsm55["REC_CREATE_TIME"] = dateNow;
						tmmsm55["REC_CREATOR"] = s.userid;
						//f_mmsm_get_seq_no(tmmsm55["IN_STOCK_NO"].ToString(), "MMSM_MATNO_SEQ", 4, conn);
						f_mmsm_get_seq_no(resume_seq_no, "MMSM_MATNO_SEQ", 4, conn);
						tmmsm55["IN_STOCK_NO"] = resume_seq_no;
						tmmsm55["IN_STOCK_NO"] = "R" + tmmsm55["IN_STOCK_NO"].ToString();
						if (tmmsm55["PLAN_NO_Y"].ToString().Substring(0, 1) == "L")
						{
							tmmsm55["HANDLE_DIV"] = "1";
						}
						else
						{
							tmmsm55["HANDLE_DIV"] = "2";
						}
						sqlstr = "SELECT MAT_NAME,MAT_TYPE FROM TMMSM50 WHERE MAT_CODE = @tmmsm55.MAT_CODE";
						cmd_sql.SetCommandText(sqlstr);
						cmd_sql.Parameters.Clear();
						cmd_sql.Parameters.Set("tmmsm55.MAT_CODE", tmmsm55["MAT_CODE"].ToString());
						cmd_sql.ExecuteReader();
						if (cmd_sql.Read())
						{
							if (tmmsm55["MAT_NAME"].ToString().Trim() == "")
							{
								tmmsm55["MAT_NAME"] = cmd_sql.GetString(1);
							}
							tmmsm55["MAT_TYPE"] = cmd_sql.GetString(2);
						}
						cmd_sql.Close();
						tmmsm55["ISSUE_TIME"] = dateNow;
						tmmsm55["AFFIRM_FLAG"] = "0";
						if (tmmsm55["MAT_TYPE"].ToString() == "2")
						{
							tmmsm55["FROM_FACTORY"] = "J";
						}
						else
						{
							tmmsm55["FROM_FACTORY"] = "W";
						}
						tmmsm55.TrimOrBlank();
						tmmsm55.Insert();
					}
					else
					{
						tmmsm55["REC_REVISE_TIME"] = dateNow;
						tmmsm55["REC_REVISOR"] = s.userid;
						tmmsm55["IN_STOCK_NO"] = hmmsm55["IN_STOCK_NO"];
						tmmsm55.Update("REC_REVISE_TIME,REC_REVISOR,PLAN_NO_Y,VEHICLE_NO,FACTORY_DIV,IN_STOCK_WT,WT_TIME", "IN_STOCK_NO,MAT_CODE");

						if (hmmsm55["AFFIRM_FLAG"].ToString() == "1")
						{
							//写库存表
							cmd_sql.SetCommandText("UPDATE TMMSM51 SET REC_REVISE_TIME = @dateNow,REC_REVISOR = @s.userid,STOCK_WT = STOCK_WT - @hmmsm55.IN_STOCK_WT + @tmmsm55.IN_STOCK_WT "
								"WHERE FACTORY_DIV = @hmmsm55.FACTORY_DIV AND STOCK_PLACE_NO = @hmmsm55.STOCK_PLACE_NO AND MAT_CODE = @tmmsm55.MAT_CODE");
							cmd_sql.Parameters.Set("dateNow", dateNow);
							cmd_sql.Parameters.Set("s.userid", s.userid);
							cmd_sql.Parameters.Set("hmmsm55.IN_STOCK_WT", hmmsm55["IN_STOCK_WT"].ToDecimal());
							cmd_sql.Parameters.Set("tmmsm55.IN_STOCK_WT", tmmsm55["IN_STOCK_WT"].ToDecimal());
							cmd_sql.Parameters.Set("hmmsm55.FACTORY_DIV", tmmsm55["FACTORY_DIV"].ToString());
							cmd_sql.Parameters.Set("hmmsm55.STOCK_PLACE_NO", hmmsm55["STOCK_PLACE_NO"].ToString());
							cmd_sql.Parameters.Set("tmmsm55.MAT_CODE", tmmsm55["MAT_CODE"].ToString());
							cmd_sql.ExecuteNonQuery();

							//写收发明细表
							//原相应入库明细冲
							//f_mmsm_get_seq_no(tmmsm53["RESUME_SEQ_NO"].ToString(), conn);
							f_mmsm_get_seq_no(resume_seq_no, "MMSM_MATNO_SEQ", 4, conn);
							tmmsm53["IN_STOCK_NO"] = resume_seq_no;
							tmmsm53["REC_CREATE_TIME"] = dateNow;
							tmmsm53["REC_CREATOR"] = s.userid;
							tmmsm53["FACTORY_DIV"] = hmmsm55["FACTORY_DIV"];
							tmmsm53["COMPANY_DIV"] = hmmsm55["FROM_FACTORY"];
							tmmsm53["HANDLE_DIV"] = hmmsm55["HANDLE_DIV"];
							tmmsm53["OTHER_BILL_NO"] = hmmsm55["OTHER_BILL_NO"];
							tmmsm53["MAT_CODE"] = hmmsm55["MAT_CODE"];
							tmmsm53["MAT_NAME"] = hmmsm55["MAT_NAME"];
							tmmsm53["MAT_TYPE"] = hmmsm55["MAT_TYPE"];
							tmmsm53["STOCK_WT"] = hmmsm55["IN_STOCK_WT"].ToDecimal() * (-1);
							tmmsm53["PLAN_NO_Y"] = hmmsm55["PLAN_NO_Y"];
							tmmsm53["SUB_RECORD_NO"] = tmmsm53["RESUME_SEQ_NO"];
							tmmsm53.Insert();

							//新相应入库明细增
							//f_mmsm_get_seq_no(tmmsm53["RESUME_SEQ_NO"].ToString(), conn);
							f_mmsm_get_seq_no(resume_seq_no, "MMSM_MATNO_SEQ", 4, conn);
							tmmsm53["IN_STOCK_NO"] = resume_seq_no;
							tmmsm53["STOCK_WT"] = tmmsm55["IN_STOCK_WT"];
							tmmsm53.Insert();
						}
					}
				}
			}
		
		}
		#pragma endregion

		
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

