/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:
Date:2016-12-14
Version:1.0
Description: 炼钢原辅料抛帐-投料产出：传入数据table_name，heat_no，flag
**************************************************/
//涉及函数f_mmsm19_proc  20 21 23 24 25 31
#include "stdafx.h"


int f_mmsm_acyfl_seq(CString& resume_seq_no, CDbConnection * conn);

int f_mmsm_acyfl_tlcc(EIClass * bcls_rec, EIClass * bcls_ret, CString& v_acjc_relation_id, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	int doFlag = 0;
	CString sqlstr = " ";
	CString cs_acjc_relation_id = " ";

	CString v_proc_div = "";
	CString resume_seq_no = "";
	CString prod_code_in = "";
	CString prod_code_out = "";
	CString prod_time = "";
	CString unit_code = "";
	CString heat_no = "";
	CString factory_div = "";
	CString whole_backlog_code = "";
	CDecimal mat_in_wt = 0;
	CDecimal mat_out_wt = 0;
	CString table_name = "";
	CString flag = "";
	CString company_code = "";
	CString company_name = "";

	CModel tmmsmac("TMMSMAC");
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	//Log::Trace("", __FUNCTION__, "s.svc_name=[{0}]", s.svc_name);
	try
	{
		//传入数据
		table_name = bcls_rec->Tables["MMSMAC"].Rows[0]["TABLE_NAME"].ToString().Trim();
		heat_no = bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"].ToString().Trim();
		flag = bcls_rec->Tables["MMSMAC"].Rows[0]["FLAG"].ToString().Trim();
		v_proc_div = bcls_rec->Tables["MMSMAC"].Rows[0]["PROC_DIV"].ToString().Trim();
		Log::Trace("", "", "table_name ={0} heat_no ={1}  v_proc_div ={2}", table_name, heat_no, v_proc_div);

		//取factory_div和whole_backlog_code
		cmd_inq.SetCommandText("SELECT FACTORY_DIV,substr(DEV_CODE,1,1) FROM TPSSM12 WHERE heat_no = @heat_no AND AREA_ID = '5'");
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteReader(); //执行读取
		if (cmd_inq.Read()){
			factory_div = cmd_inq.GetString(1);
			whole_backlog_code = "S" + cmd_inq.GetString(2);
			//Log::Trace("", "", "whole_backlog_code ={0} factory_div ={1}", whole_backlog_code, factory_div);
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
		
		//查对应表获取数据
		if (bcls_rec->Tables.IndexOf("MMSMAC_QUERY") < 0)
		{
			bcls_rec->Tables.Add("MMSMAC_QUERY");
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.SetCommandText("SELECT * FROM " + table_name + " WHERE heat_no = @heat_no ");
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteQuery(bcls_rec->Tables["MMSMAC_QUERY"]);
		cmd_inq.Close();

		if (bcls_rec->Tables["MMSMAC_QUERY"].Rows.get_Count() > 0){
			for (int i = 0; i < bcls_rec->Tables["MMSMAC_QUERY"].Rows.get_Count(); i++){
				if (table_name == "TMMSM19"){//中频炉实绩   只抛产出
					//mat_in_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["STEEL_NET_WT"];
					mat_out_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["STEEL_NET_WT"];//钢水重量
					//prod_code_in = "92HM";//铁水
					prod_code_out = "GS";//钢水
				}
				if (table_name == "TMMSM20"){//电炉实绩
					mat_in_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["MOLTIRON_WT"]; //铁水重量
					mat_out_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["STEEL_NET_WT"]; //钢水重量
					prod_code_in = "92HM";//铁水
					prod_code_out = "GS";//钢水
				}
				if (table_name == "TMMSM21"){//转炉实绩
					mat_in_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["MOLTIRON_WT"];//铁水重量
					mat_out_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["STEEL_NET_WT"];//钢水净重量
					prod_code_in = "92HM";///铁水
					prod_code_out = "GS";//钢水
				}
				if (table_name == "TMMSM23"){//RH实绩
					mat_in_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["INT_STEEL_WT"];//初始钢水量
					mat_out_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["FIN_STEEL_WT"];//最终钢水量
					prod_code_in = "GS";//钢水
					prod_code_out = "GS";//钢水
				}
				if (table_name == "TMMSM24"){//LF实绩
					mat_in_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["INT_STEEL_WT"];//初始钢水量
					mat_out_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["FIN_STEEL_WT"];//最终钢水量
					prod_code_in = "GS";//钢水
					prod_code_out = "GS";//钢水
				}
				if (table_name == "TMMSM25"){//VD实绩
					mat_in_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["INT_STEEL_WT"];//初始钢水量
					mat_out_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["FIN_STEEL_WT"];//最终钢水量
					prod_code_in = "GS";//钢水
					prod_code_out = "GS";//钢水
				}
				if (table_name == "TMMSM31"){//连铸作业实绩  只抛投入
					mat_in_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["LADLE_ARRIVE_WT"];//钢包到达重量
					mat_out_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["LADLE_LEAVE_WT"];//钢包离开重量
					mat_in_wt = mat_in_wt - mat_out_wt; //投料重量
					mat_out_wt = 0; //初始化 防止抛产出
					prod_code_in = "GS";//钢水
					//prod_code_out = "GS";//钢水
				}

				if (table_name == "TMMSM41"){//模铸炉次作业实绩 只抛投入
					mat_in_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["LADLE_ENTER_START_T"];//大包到位重量
					mat_out_wt = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["LADLE_W_EN"];//浇注完重量
					mat_in_wt = mat_in_wt - mat_out_wt; //投料重量
					mat_out_wt = 0; //初始化 防止抛产出
					prod_code_in = "GS";//钢水
					//prod_code_out = "GS";//钢水
				}

				prod_time = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["START_TIME"].ToString().Trim();
				unit_code = bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["STATION_ID"].ToString().Trim() + bcls_rec->Tables["MMSMAC_QUERY"].Rows[i]["STATION_NO"].ToString().Trim();

				
				tmmsmac["REC_CREATE_TIME"] = dateNow;
				tmmsmac["REC_CREATOR"] = s.userid;
				tmmsmac["COMPANY_CODE"] = company_code;
				tmmsmac["COMPANY_NAME"] = company_name;
				tmmsmac["ACJC_RELATION_ID"] = v_acjc_relation_id;
				tmmsmac["APP_TRNC_DATE"] = dateNow.Substring(0, 8);
				tmmsmac["APP_TRANSACTION_T"] = dateNow.Substring(0, 6);
				tmmsmac["EVENT_DATETIME"] = dateNow;
				tmmsmac["PROD_TIME"] = prod_time;
				tmmsmac["UNIT_CODE"] = unit_code;
				tmmsmac["HEAT_NO"] = heat_no;
				tmmsmac["FACTORY_DIV"] = factory_div;
				tmmsmac["WHOLE_BACKLOG_CODE"] = whole_backlog_code;
				tmmsmac["FUNC_ID"] = s.svc_name;
				tmmsmac["FUNCTION_CODE_AC"] = "N";
				tmmsmac["PLAN_NO"] = heat_no;

				//Log::Trace("", "", "flag={0}", flag);

				if (flag == "0" && (v_proc_div == "U" || v_proc_div == "D")){ //传入标志 0抛负数 修改或删除时触发
					if (mat_in_wt != 0){
						//投料
						f_mmsm_acyfl_seq(resume_seq_no, conn);
						tmmsmac["RESUME_SEQ_NO"] = resume_seq_no;
						tmmsmac["EVENT_ID"] = "MMF1";
						tmmsmac["TRANSACTION_CODE"] = "C";
						tmmsmac["EVENT_DESC"] = "原辅料消耗";
						tmmsmac["MAT_ACT_WT"] = mat_in_wt * (-1);
						tmmsmac["MAT_THEORY_WT"] = mat_in_wt * (-1);
						//tmmsmac["PROD_CODE"] = prod_code_in;
						tmmsmac.TrimOrBlank();
						tmmsmac.Insert();
					}
					if (mat_out_wt != 0){
						//产出
						f_mmsm_acyfl_seq(resume_seq_no, conn);
						tmmsmac["RESUME_SEQ_NO"] = resume_seq_no;
						tmmsmac["EVENT_ID"] = "MMF2";
						tmmsmac["TRANSACTION_CODE"] = "P";
						tmmsmac["EVENT_DESC"] = "机组产出（炼钢钢水）";
						tmmsmac["MAT_ACT_WT"] = mat_out_wt * (-1);
						tmmsmac["MAT_THEORY_WT"] = mat_out_wt * (-1);
						//tmmsmac["PROD_CODE"] = prod_code_out;
						tmmsmac.TrimOrBlank();
						tmmsmac.Insert();
					}
				}
				else if (flag == "1" && (v_proc_div == "I" || v_proc_div == "U"))
				{ //传入标志 1抛正数 新增或修改时触发

					//Log::Trace("", "", "mat_in_wt={0}", mat_in_wt);
					//Log::Trace("", "", "mat_out_wt={0}", mat_out_wt);

					if (mat_in_wt != 0){
						//投料
						f_mmsm_acyfl_seq(resume_seq_no, conn);
						tmmsmac["RESUME_SEQ_NO"] = resume_seq_no;
						tmmsmac["EVENT_ID"] = "MMF1";
						tmmsmac["TRANSACTION_CODE"] = "C";
						tmmsmac["EVENT_DESC"] = "原辅料消耗";
						tmmsmac["MAT_ACT_WT"] = mat_in_wt;
						tmmsmac["MAT_THEORY_WT"] = mat_in_wt;
						//tmmsmac["PROD_CODE"] = prod_code_in;
						tmmsmac.Print();
						tmmsmac.TrimOrBlank();
						tmmsmac.Insert();
					}
					if (mat_out_wt != 0){
						//产出
						f_mmsm_acyfl_seq(resume_seq_no, conn);
						tmmsmac["RESUME_SEQ_NO"] = resume_seq_no;
						tmmsmac["EVENT_ID"] = "MMF2";
						tmmsmac["TRANSACTION_CODE"] = "P";
						tmmsmac["EVENT_DESC"] = "机组产出（炼钢钢水）";
						tmmsmac["MAT_ACT_WT"] = mat_out_wt;
						tmmsmac["MAT_THEORY_WT"] = mat_out_wt;
						//tmmsmac["PROD_CODE"] = prod_code_out;
						tmmsmac.TrimOrBlank();
						tmmsmac.Insert();
					}
				}
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


