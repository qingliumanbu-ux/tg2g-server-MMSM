/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:
Date:2016-12-08
Version:1.0
Description: 炼钢原辅料抛帐-投料 f_mmsm53_proc f_mmsm2a_save
**************************************************/
//涉及函数f_mmsm53_proc f_mmsm2a_save
#include "stdafx.h"

 

int f_mmsm_acyfl_seq(CString& resume_seq_no, CDbConnection * conn);

int f_mmsm_confirm_flag(const CString& factory_div, const CString& heat_no, CString& heat_confirm_flag, CDbConnection * conn);

int f_mmsm_acyfl(EIClass * bcls_rec, CString & flag, EIClass * bcls_ret, CString& v_acjc_relation_id, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	int doFlag = 0;
	CString sqlstr = " ";
	CString cs_acjc_relation_id = " ";

	CString v_proc_div = "";
	CDecimal newWt = 0;
	CString newHandWorkMark = "";
	CString newCollMode = "";
	CString resume_seq_no = "";
	CString v_heat_confirm_flag = "";
	CString whole_backlog_code = "";
	CString heat_no = "";
	CString factory_div = "";
	CString company_code = "";
	CString company_name = "";

	CModel tmmsmac("TMMSMAC");
	CModel tmmsm2a("TMMSM2A");
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		//Log::Trace("", "", "flagfirst ={0}", flag);

		if (flag == "0"){ //负数 f_mmsm2a_save
			//Log::Trace("", "", "flag ={0}", flag);
			heat_no = bcls_rec->Tables["MMSM2A_OLD"].Rows[0]["HEAT_NO"].ToString().Trim();
			//Log::Trace("", "", "MMSM2A_OLD_heat_no ={0}", heat_no);
			cmd_inq.SetCommandText("SELECT FACTORY_DIV,substr(DEV_CODE,1,1) FROM TPSSM12 WHERE heat_no = @heat_no AND AREA_ID = '5'");
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader(); //执行读取
			if (cmd_inq.Read()){
				factory_div = cmd_inq.GetString(1);
				whole_backlog_code = "S" + cmd_inq.GetString(2);
				//Log::Trace("", "", "whole_backlog_code ={0} factory_div ={0}", whole_backlog_code, factory_div);
			}
			cmd_inq.Close();

			//取company_code和company_name
			cmd_inq.SetCommandText(" SELECT CODE_DESC_3_CONTENT, CODE_DESC_4_CONTENT  FROM TEP0002  WHERE CODE_CLASS = 'M00F' AND CODE = @factory_div ");
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("factory_div", factory_div);
			cmd_inq.ExecuteReader(); //执行读取
			if (cmd_inq.Read()){
				company_code = cmd_inq.GetString(1);
				company_name = cmd_inq.GetString(2);
			}
			cmd_inq.Close();

			//Log::Trace("", "", "hanshu ={0}", bcls_rec->Tables["MMSM2A_OLD"].Rows.get_Count());

			for (int i = 0; i < bcls_rec->Tables["MMSM2A_OLD"].Rows.get_Count(); i++){
				v_proc_div = bcls_rec->Tables["MMSM2A_OLD"].Rows[0]["PROC_DIV"].ToString().Trim();
				//Log::Trace("", "", "v_proc_div22 ={0}", v_proc_div);

				if (v_proc_div == "U" || v_proc_div=="D"){
					tmmsm2a.MergeFrom(bcls_rec->Tables["MMSM2A_OLD"].Rows[i]);
					tmmsm2a.TrimOrBlank();
					f_mmsm_acyfl_seq(resume_seq_no,conn);
					tmmsmac["REC_CREATE_TIME"] = dateNow;
					tmmsmac["REC_CREATOR"] = s.userid;
					tmmsmac["COMPANY_CODE"] = company_code;
					tmmsmac["COMPANY_NAME"] = company_name;
					//tmmsmac["PROD_CODE"] = tmmsm2a["MAT_CODE"];  该字段长度不够
					tmmsmac["ACJC_RELATION_ID"] = v_acjc_relation_id;
					tmmsmac["RESUME_SEQ_NO"] = resume_seq_no;
					tmmsmac["APP_TRNC_DATE"] = dateNow.Substring(0, 8);
					tmmsmac["APP_TRANSACTION_T"] = dateNow.Substring(0, 6);
					tmmsmac["EVENT_ID"] = "MMF1";
					tmmsmac["TRANSACTION_CODE"] = "C";
					tmmsmac["EVENT_DESC"] = "原辅料消耗";
					tmmsmac["EVENT_DATETIME"] = dateNow;
					tmmsmac["PROD_TIME"] = tmmsm2a["DEVO_TIME"];
					tmmsmac["UNIT_CODE"] = tmmsm2a["STATION_ID"].ToString() + tmmsm2a["STATION_NO"].ToString();
					tmmsmac["HEAT_NO"] = tmmsm2a["HEAT_NO"];
					tmmsmac["FACTORY_DIV"] = factory_div;
					tmmsmac["WHOLE_BACKLOG_CODE"] = whole_backlog_code;
					tmmsmac["FUNC_ID"] = s.svc_name;
					tmmsmac["FUNCTION_CODE_AC"] = 'N';
					tmmsmac["PLAN_NO"] = tmmsm2a["HEAT_NO"];

					tmmsmac["MAT_ACT_WT"] = tmmsm2a["DEVO_WT"].ToDecimal() * (-1);
					tmmsmac["MAT_THEORY_WT"] = tmmsm2a["DEVO_WT"].ToDecimal() * (-1);
					tmmsmac.TrimOrBlank();
					tmmsmac.Print();
					tmmsmac.Insert();
				}
			}
		}
		else if (flag == "1"){//正数据 f_mmsm2a_save
			//Log::Trace("", "", "flag ={0}", flag);
			CString heat_no = bcls_rec->Tables[1].Rows[0]["HEAT_NO"];
			CString proc_no = bcls_rec->Tables[1].Rows[0]["PROC_NO"];

			//Log::Trace("", "", "heat_no ={0}", heat_no);

			cmd_inq.SetCommandText("SELECT FACTORY_DIV,substr(DEV_CODE,1,1) FROM TPSSM12 WHERE heat_no = @heat_no AND AREA_ID = '5'");
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader(); //执行读取
			if (cmd_inq.Read()){
				factory_div = cmd_inq.GetString(1);
				whole_backlog_code = "S" + cmd_inq.GetString(2);
				//Log::Trace("", "", "whole_backlog_code ={0} factory_div ={0}", whole_backlog_code, factory_div);
			}
			cmd_inq.Close();

			//取company_code和company_name
			cmd_inq.SetCommandText(" SELECT CODE_DESC_3_CONTENT, CODE_DESC_4_CONTENT  FROM TEP0002  WHERE CODE_CLASS = 'M00F' AND CODE = @factory_div ");
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("factory_div", factory_div);
			cmd_inq.ExecuteReader(); //执行读取
			if (cmd_inq.Read()){
				company_code = cmd_inq.GetString(1);
				company_name = cmd_inq.GetString(2);
			}
			cmd_inq.Close();

			if (!bcls_rec->Tables.Contains("MMSM2A_INSERT"))
			{
				bcls_rec->Tables.Add("MMSM2A_INSERT");
				bcls_rec->Tables["MMSM2A_INSERT"].Columns.Add(tmmsm2a);
			}

			cmd_inq.SetCommandText("SELECT * FROM TMMSM2A WHERE HEAT_NO = @heat_no AND PROC_NO = @proc_no");
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("proc_no", proc_no);
			cmd_inq.ExecuteQuery(bcls_rec->Tables["MMSM2A_INSERT"]);
			cmd_inq.Close();
			//Log::Trace("", "", "MMSM2A_INSERT={0}", bcls_rec->Tables["MMSM2A_INSERT"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["MMSM2A_INSERT"].Rows.get_Count();i++){
				tmmsm2a.MergeFrom(bcls_rec->Tables["MMSM2A_INSERT"].Rows[i]);
				tmmsm2a.TrimOrBlank();
				//Log::Trace("", "", "v_proc_divbb ={0}", v_proc_div);
				//Log::Trace("", "", "v_acjc_relation_id ={0}", v_acjc_relation_id);
				f_mmsm_acyfl_seq(resume_seq_no,conn);
				tmmsmac["REC_CREATE_TIME"] = dateNow;
				tmmsmac["REC_CREATOR"] = s.userid;
				tmmsmac["COMPANY_CODE"] = company_code;
				tmmsmac["COMPANY_NAME"] = company_name;
				//tmmsmac["PROD_CODE"] = tmmsm2a["MAT_CODE"]; 该字段长度不够
				tmmsmac["ACJC_RELATION_ID"] = v_acjc_relation_id;
				tmmsmac["RESUME_SEQ_NO"] = resume_seq_no;
				tmmsmac["APP_TRNC_DATE"] = dateNow.Substring(0, 8);
				tmmsmac["APP_TRANSACTION_T"] = dateNow.Substring(0, 6);
				tmmsmac["EVENT_ID"] = "MMF1";
				tmmsmac["TRANSACTION_CODE"] = "C";
				tmmsmac["EVENT_DESC"] = "原辅料消耗";
				tmmsmac["EVENT_DATETIME"] = dateNow;
				tmmsmac["PROD_TIME"] = tmmsm2a["DEVO_TIME"];
				tmmsmac["UNIT_CODE"] = tmmsm2a["STATION_ID"].ToString() + tmmsm2a["STATION_NO"].ToString();
				tmmsmac["HEAT_NO"] = tmmsm2a["HEAT_NO"];
				tmmsmac["FACTORY_DIV"] = factory_div;
				tmmsmac["WHOLE_BACKLOG_CODE"] = whole_backlog_code;
				tmmsmac["FUNC_ID"] = s.svc_name;
				tmmsmac["FUNCTION_CODE_AC"] = 'N';
				tmmsmac["PLAN_NO"] = tmmsm2a["HEAT_NO"];

				tmmsmac["MAT_ACT_WT"] = tmmsm2a["DEVO_WT"];
				tmmsmac["MAT_THEORY_WT"] = tmmsm2a["DEVO_WT"];
				tmmsmac.TrimOrBlank();
				tmmsmac.Print();
				tmmsmac.Insert();
			}
		}
		else if (flag == "2"){//删除实绩信息时同时抛负数 f_mmsm53_proc
			//Log::Trace("", "", "flag ={0}", flag);
			CString heat_no = bcls_rec->Tables["MMSM2A"].Rows[0]["HEAT_NO"].ToString().Trim();
			CString proc_no = bcls_rec->Tables["MMSM2A"].Rows[0]["PROC_NO"].ToString().Trim();
			CString proc_count = bcls_rec->Tables["MMSM2A"].Rows[0]["PROC_COUNT"].ToString().Trim();

			//Log::Trace("", "", "heat_no ={0}", heat_no);

			cmd_inq.SetCommandText("SELECT FACTORY_DIV,substr(DEV_CODE,1,1) FROM TPSSM12 WHERE heat_no = @heat_no AND AREA_ID = '5'");
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader(); //执行读取
			if (cmd_inq.Read()){
				factory_div = cmd_inq.GetString(1);
				whole_backlog_code = "S" + cmd_inq.GetString(2);
				//Log::Trace("", "", "whole_backlog_code ={0} factory_div ={0}", whole_backlog_code, factory_div);
			}
			cmd_inq.Close();

			//取company_code和company_name
			cmd_inq.SetCommandText(" SELECT CODE_DESC_3_CONTENT, CODE_DESC_4_CONTENT  FROM TEP0002  WHERE CODE_CLASS = 'M00F' AND CODE = @factory_div ");
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("factory_div", factory_div);
			cmd_inq.ExecuteReader(); //执行读取
			if (cmd_inq.Read()){
				company_code = cmd_inq.GetString(1);
				company_name = cmd_inq.GetString(2);
			}
			cmd_inq.Close();

			if (!bcls_rec->Tables.Contains("MMSM2A_INSERT"))
			{
				bcls_rec->Tables.Add("MMSM2A_INSERT");
				bcls_rec->Tables["MMSM2A_INSERT"].Columns.Add(tmmsm2a);
			}

			cmd_inq.SetCommandText("SELECT * FROM TMMSM2A WHERE HEAT_NO = @heat_no AND PROC_NO = @proc_no AND PROC_COUNT = @proc_count");
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("proc_no", proc_no);
			cmd_inq.Parameters.Set("proc_count", proc_count);
			cmd_inq.ExecuteQuery(bcls_rec->Tables["MMSM2A_INSERT"]);
			cmd_inq.Close();
			for (int i = 0; i < bcls_rec->Tables["MMSM2A_INSERT"].Rows.get_Count(); i++){
				tmmsm2a.MergeFrom(bcls_rec->Tables["MMSM2A_INSERT"].Rows[i]);
				tmmsm2a.TrimOrBlank();

				f_mmsm_acyfl_seq(resume_seq_no,conn);
				tmmsmac["REC_CREATE_TIME"] = dateNow;
				tmmsmac["REC_CREATOR"] = s.userid;
				tmmsmac["COMPANY_CODE"] = company_code;
				tmmsmac["COMPANY_NAME"] = company_name;
				//tmmsmac["PROD_CODE"] = tmmsm2a["MAT_CODE"];
				tmmsmac["ACJC_RELATION_ID"] = v_acjc_relation_id;
				tmmsmac["RESUME_SEQ_NO"] = resume_seq_no;
				tmmsmac["APP_TRNC_DATE"] = dateNow.Substring(0, 8);
				tmmsmac["APP_TRANSACTION_T"] = dateNow.Substring(0, 6);
				tmmsmac["EVENT_ID"] = "MMF1";
				tmmsmac["TRANSACTION_CODE"] = "C";
				tmmsmac["EVENT_DESC"] = "原辅料消耗";
				tmmsmac["EVENT_DATETIME"] = dateNow;
				tmmsmac["PROD_TIME"] = tmmsm2a["DEVO_TIME"];
				tmmsmac["UNIT_CODE"] = tmmsm2a["STATION_ID"].ToString() + tmmsm2a["STATION_NO"].ToString();
				tmmsmac["HEAT_NO"] = tmmsm2a["HEAT_NO"];
				tmmsmac["FACTORY_DIV"] = factory_div;
				tmmsmac["WHOLE_BACKLOG_CODE"] = whole_backlog_code;
				tmmsmac["FUNC_ID"] = s.svc_name;
				tmmsmac["FUNCTION_CODE_AC"] = 'N';
				tmmsmac["PLAN_NO"] = tmmsm2a["HEAT_NO"];

				tmmsmac["MAT_ACT_WT"] = tmmsm2a["DEVO_WT"].ToDecimal() * (-1);
				tmmsmac["MAT_THEORY_WT"] = tmmsm2a["DEVO_WT"].ToDecimal() * (-1);
				tmmsmac.TrimOrBlank();
				tmmsmac.Insert();
			}
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


