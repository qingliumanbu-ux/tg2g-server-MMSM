/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:
Date:2016-12-08
Version:1.0
Description: 炼钢原辅料抛帐-原辅料出库
**************************************************/
#include "stdafx.h"


int f_mmsm_acyfl_seq(CString& resume_seq_no, CDbConnection * conn);

int f_mmsm_acyfl_cnzy(EIClass * bcls_rec,  EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	int doFlag = 0;
	CString sqlstr = " ";
	CString  v_proc_div = "";
	CString  resume_seq_no = "";
	CString acjc_relation_id = "";
	CString company_code = "";
	CString company_name = "";

	CModel tmmsmac("TMMSMAC");
	CModel tmmsmac_old("TMMSMAC");
	CModel tmmsm55_old("TMMSM55");
	CModel tmmsm56_old("TMMSM56");
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{
		if (bcls_rec->Tables["PARA"].Columns.Contains("PROC_DIV"))   //增删改区分
			v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString().Trim();

		//取company_code和company_name
		cmd_inq.SetCommandText(" SELECT CODE_DESC_3_CONTENT, CODE_DESC_4_CONTENT  FROM TEP0002  WHERE CODE_CLASS = 'M00F' AND CODE = @factory_div ");
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("factory_div", "SU");
		cmd_inq.ExecuteReader(); //执行读取
		if (cmd_inq.Read()){
			company_code = cmd_inq.GetString(1);
			company_name = cmd_inq.GetString(2);
		}
		cmd_inq.Close();

		if (bcls_rec->Tables.Contains("MMSM55"))//入库
		{
			//原始数据tmmsm55_old 修改删除调用
			f_mmsm_acyfl_seq(resume_seq_no, conn);
			acjc_relation_id = resume_seq_no;
			tmmsmac_old["REC_CREATE_TIME"] = tmmsm55_old["REC_CREATE_TIME"];
			tmmsmac_old["REC_CREATOR"] = tmmsm55_old["REC_CREATOR"];
			tmmsmac_old["COMPANY_CODE"] = tmmsm55_old["COMPANY_CODE"];
			tmmsmac_old["COMPANY_NAME"] = tmmsm56_old["COMPANY_NAME"];
			tmmsmac_old["PROD_CODE"] = tmmsm55_old["MAT_CODE"]; //物料代码
			tmmsmac_old["MAT_NO"] = tmmsm55_old["WEIGH_NO"]; //磅单号
			tmmsmac_old["ACJC_RELATION_ID"] = acjc_relation_id;
			tmmsmac_old["RESUME_SEQ_NO"] = resume_seq_no;
			tmmsmac_old["APP_TRNC_DATE"] = dateNow.Substring(0, 8);
			tmmsmac_old["APP_TRANSACTION_T"] = dateNow.Substring(0, 6);
			tmmsmac_old["EVENT_ID"] = "MMF4";
			tmmsmac_old["TRANSACTION_CODE"] = "Z";
			tmmsmac_old["EVENT_DESC"] = "原辅料入库";
			tmmsmac_old["EVENT_DATETIME"] = dateNow;//当前时刻
			tmmsmac_old["PROD_TIME"] = tmmsm55_old["TARE_TIME"];//称皮时刻
			tmmsmac_old["MAT_ACT_WT"] = 0 - tmmsm55_old["NET_WT"].ToDecimal();//净重（新增抛正数，删除抛负数，修改先负再正）
			tmmsmac_old["MAT_THEORY_WT"] = 0 - tmmsm55_old["NET_WT"].ToDecimal();//净重（新增抛正数，删除抛负数，修改先负再正）
			tmmsmac_old["FUNC_ID"] = s.svc_name;
			tmmsmac_old["FUNCTION_CODE_AC"] = 'N';

			tmmsmac_old["IN_OUT_DIV"] = "1"; //1-入库2-出库
			tmmsmac_old["STOCK_NO"] = tmmsm55_old["SRC_STOCK_CODE"];  //来源库/去向库SRC_STOCK_CODE/DST_STOCK_CODE

			//新数据
			f_mmsm_acyfl_seq(resume_seq_no, conn);
			tmmsmac["REC_CREATE_TIME"] = dateNow;
			tmmsmac["REC_CREATOR"] = s.userid;
			tmmsmac["COMPANY_CODE"] = company_code;
			tmmsmac["COMPANY_NAME"] = company_name;
			tmmsmac["PROD_CODE"] = bcls_rec->Tables["MMSM55"].Rows[0]["MAT_CODE"]; //物料代码
			tmmsmac["MAT_NO"] = bcls_rec->Tables["MMSM55"].Rows[0]["WEIGH_NO"]; //磅单号
			tmmsmac["ACJC_RELATION_ID"] = acjc_relation_id;
			tmmsmac["RESUME_SEQ_NO"] = resume_seq_no;
			tmmsmac["APP_TRNC_DATE"] = dateNow.Substring(0, 8);
			tmmsmac["APP_TRANSACTION_T"] = dateNow.Substring(0, 6);
			tmmsmac["EVENT_ID"] = "MMF4";
			tmmsmac["TRANSACTION_CODE"] = "Z";
			tmmsmac["EVENT_DESC"] = "原辅料入库";
			tmmsmac["EVENT_DATETIME"] = dateNow;//当前时刻
			tmmsmac["PROD_TIME"] = bcls_rec->Tables["MMSM55"].Rows[0]["TARE_TIME"];//称皮时刻
			tmmsmac["MAT_ACT_WT"] = bcls_rec->Tables["MMSM55"].Rows[0]["NET_WT"];//净重（新增抛正数，删除抛负数，修改先负再正）
			tmmsmac["MAT_THEORY_WT"] = bcls_rec->Tables["MMSM55"].Rows[0]["NET_WT"];//净重（新增抛正数，删除抛负数，修改先负再正）
			tmmsmac["FUNC_ID"] = s.svc_name;
			tmmsmac["FUNCTION_CODE_AC"] = 'N';

			tmmsmac["IN_OUT_DIV"] = "1"; //1-入库2-出库
			tmmsmac["STOCK_NO"] = bcls_rec->Tables["MMSM55"].Rows[0]["SRC_STOCK_CODE"];  //来源库/去向库SRC_STOCK_CODE/DST_STOCK_CODE
		}
		else if (bcls_rec->Tables.Contains("MMSM56"))//出库
		{
			//原始数据tmmsm56_old 修改删除调用
			f_mmsm_acyfl_seq(resume_seq_no, conn);
			acjc_relation_id = resume_seq_no;
			tmmsmac_old["REC_CREATE_TIME"] = tmmsm56_old["REC_CREATE_TIME"];
			tmmsmac_old["REC_CREATOR"] = tmmsm56_old["REC_CREATOR"];
			tmmsmac_old["COMPANY_CODE"] = tmmsm56_old["COMPANY_CODE"];
			tmmsmac_old["COMPANY_NAME"] = tmmsm56_old["COMPANY_NAME"];
			tmmsmac_old["PROD_CODE"] = tmmsm56_old["MAT_CODE"]; //物料代码
			tmmsmac_old["MAT_NO"] = tmmsm56_old["WEIGH_NO"]; //磅单号
			tmmsmac_old["ACJC_RELATION_ID"] = acjc_relation_id;
			tmmsmac_old["RESUME_SEQ_NO"] = resume_seq_no;
			tmmsmac_old["APP_TRNC_DATE"] = dateNow.Substring(0, 8);
			tmmsmac_old["APP_TRANSACTION_T"] = dateNow.Substring(0, 6);
			tmmsmac_old["EVENT_ID"] = "MMF3";
			tmmsmac_old["TRANSACTION_CODE"] = "Z";
			tmmsmac_old["EVENT_DESC"] = "原辅料出库";
			tmmsmac_old["EVENT_DATETIME"] = dateNow;//当前时刻
			tmmsmac_old["PROD_TIME"] = tmmsm56_old["TARE_TIME"];//称皮时刻
			tmmsmac_old["MAT_ACT_WT"] = 0 - tmmsm56_old["NET_WT"].ToDecimal();//净重（新增抛正数，删除抛负数，修改先负再正）
			tmmsmac_old["MAT_THEORY_WT"] = 0 - tmmsm56_old["NET_WT"].ToDecimal();//净重（新增抛正数，删除抛负数，修改先负再正）
			tmmsmac_old["FUNC_ID"] = s.svc_name;
			tmmsmac_old["FUNCTION_CODE_AC"] = 'N';

			tmmsmac_old["IN_OUT_DIV"] = "1"; //1-入库2-出库
			tmmsmac_old["STOCK_NO"] = tmmsm56_old["SRC_STOCK_CODE"];  //来源库/去向库SRC_STOCK_CODE/DST_STOCK_CODE

			//新数据
			f_mmsm_acyfl_seq(resume_seq_no, conn);
			tmmsmac["REC_CREATE_TIME"] = dateNow;
			tmmsmac["REC_CREATOR"] = s.userid;
			tmmsmac["COMPANY_CODE"] = company_code;
			tmmsmac["COMPANY_NAME"] = company_name;
			tmmsmac["PROD_CODE"] = bcls_rec->Tables["MMSM56"].Rows[0]["MAT_CODE"]; //物料代码
			tmmsmac["MAT_NO"] = bcls_rec->Tables["MMSM56"].Rows[0]["WEIGH_NO"]; //磅单号
			tmmsmac["ACJC_RELATION_ID"] = acjc_relation_id;
			tmmsmac["RESUME_SEQ_NO"] = resume_seq_no;
			tmmsmac["APP_TRNC_DATE"] = dateNow.Substring(0, 8);
			tmmsmac["APP_TRANSACTION_T"] = dateNow.Substring(0, 6);
			tmmsmac["EVENT_ID"] = "MMF3";
			tmmsmac["TRANSACTION_CODE"] = "Z";
			tmmsmac["EVENT_DESC"] = "原辅料出库";
			tmmsmac["EVENT_DATETIME"] = dateNow;//当前时刻
			tmmsmac["PROD_TIME"] = bcls_rec->Tables["MMSM56"].Rows[0]["TARE_TIME"];//称皮时刻
			tmmsmac["MAT_ACT_WT"] = bcls_rec->Tables["MMSM56"].Rows[0]["NET_WT"];//净重（新增抛正数，删除抛负数，修改先负再正）
			tmmsmac["MAT_THEORY_WT"] = bcls_rec->Tables["MMSM56"].Rows[0]["NET_WT"];//净重（新增抛正数，删除抛负数，修改先负再正）
			tmmsmac["FUNC_ID"] = s.svc_name;
			tmmsmac["FUNCTION_CODE_AC"] = 'N';

			tmmsmac["IN_OUT_DIV"] = "2"; //1-入库2-出库
			tmmsmac["STOCK_NO"] = bcls_rec->Tables["MMSM56"].Rows[0]["DST_STOCK_CODE"];  //来源库/去向库SRC_STOCK_CODE/DST_STOCK_CODE
		}

		if (v_proc_div == "I"){
			tmmsmac.TrimOrBlank();
			tmmsmac.Insert();
		}
		else if (v_proc_div == "U"){
			//原始数据
			tmmsmac_old.TrimOrBlank();
			tmmsmac_old.Insert();

			//修改后数据
			tmmsmac.TrimOrBlank();
			tmmsmac.Insert();
		}
		else if (v_proc_div == "D"){
			tmmsmac_old.TrimOrBlank();
			tmmsmac_old.Insert();
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


