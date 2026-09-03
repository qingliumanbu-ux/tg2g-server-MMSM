/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-11-25
Description: 投料实绩明细实绩增删改
===========================================================</remark>*/


/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件




//外部函数声明
int f_mmsm53_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_count(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_confirm_flag(const CString& factory_div, const CString& heat_no, CString& heat_confirm_flag, CDbConnection * conn);
int f_mmsm_get_seq_no(CString& resume_seq_no, CDbConnection * conn);
int f_mmsm2a_trace(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //原辅料消耗履历写入
int f_t82306_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//投料
#if  defined _SYS_PES
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号

int f_mmsm2ahj_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//料仓

BM2_FUNCTION_EXPORT
int f_mmsm2a_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm2a_proc";                //定义函数英文名称  
	CString FunctionCname = "投料实绩明细实绩增删改";              //定义函数中文名称

	//
	//CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int ret = 0;
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   n_count = 0;
	int   blkNum;

	CString sqlstr = "";
	CString v_heat_confirm_flag = "";
	CString v_area_id = "";
	CString v_proc_div = "";
	CString v_station_id = "";
	CString v_station_no = "";
	CDecimal newWt = 0;
	CString newHandWorkMark = "";
	CString newCollMode = "";
	CString v_acjc_relation_id = "";
	CString resume_seq_no = "";
	CString table = "";
	CDbCommand cmd_inq(conn);
	CString v_resume_seq_no = " ";
	CString seq("");

	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass in_mmsm2atrace;  //调用履历函数

	try
	{
		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。



		/* 实体类定义 */
		CModel tmmsm2a("TMMSM2A");
		CModel tmmsm53("TMMSM53");
		CModel tmmsm52("TMMSM52");
		if (bcls_rec->Tables["MMSM2A"].Columns.Contains("ACJC_RELATION_ID"))
		{
			v_acjc_relation_id = bcls_rec->Tables["MMSM2A"].Rows[0]["ACJC_RELATION_ID"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM2A"].Columns.Contains("NEW_WT"))
		{
			newWt = bcls_rec->Tables["MMSM2A"].Rows[0]["NEW_WT"].ToDecimal();
		}
		if (bcls_rec->Tables["MMSM2A"].Columns.Contains("NEW_HANDWORK_MARK"))
		{
			newHandWorkMark = bcls_rec->Tables["MMSM2A"].Rows[0]["NEW_HANDWORK_MARK"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM2A"].Columns.Contains("NEW_COLL_MODE"))
		{
			newCollMode = bcls_rec->Tables["MMSM2A"].Rows[0]["NEW_COLL_MODE"].ToString().Trim();
		}
		v_proc_div = bcls_rec->Tables["MMSM2A"].Rows[0]["PROC_DIV"].ToString().Trim();

		Log::Info("", __FUNCTION__, "newHandWorkMark =[{0}]", newHandWorkMark);
		Log::Info("", __FUNCTION__, "newWt =[{0}]", newWt);

		tmmsm2a.MergeFrom(bcls_rec->Tables["MMSM2A"].Rows[0]);
		tmmsm2a.TrimOrBlank();

		//用来存放单位为吨的重量数据
		if (tmmsm2a["MAT_AMOUNT1"].ToDecimal() == 0 && tmmsm2a["DEVO_WT"].ToDecimal() > 0 )
		{
			tmmsm2a["MAT_AMOUNT1"] = tmmsm2a["DEVO_WT"].ToDecimal() / 1000;
		}

		Log::Info("", __FUNCTION__, "tmmsm2a.HEAT_NO =[{0}]", tmmsm2a["HEAT_NO"].ToString());
		Log::Info("", __FUNCTION__, "bcls..HEAT_NO =[{0}]", bcls_rec->Tables["MMSM2A"].Rows[0]["HEAT_NO"].ToString());

		in_mmsm2atrace.Tables[0].set_TableName("TRACE");
		in_mmsm2atrace.Tables[0].Clone(tmmsm52);

		//已炉次确定则返回
		Log::Trace("", __FUNCTION__, "FACTORY_DIV=[{0}]", tmmsm2a["FACTORY_DIV"].ToString());

		doFlag = f_mmsm_confirm_flag(tmmsm2a["FACTORY_DIV"].ToString(), tmmsm2a["HEAT_NO"].ToString(), v_heat_confirm_flag, conn);

		Log::Trace("", __FUNCTION__, "0111111HEAT_NO=[{0}]", tmmsm2a["HEAT_NO"].ToString());

		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (v_heat_confirm_flag != "0" && v_heat_confirm_flag != "")
		{
			strcpy(s.msg, "该制造命令号已经炉次确定!!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/*if (tmmsm2a["PROC_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "处理号不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		if (tmmsm2a["MAT_CODE"].ToString().Trim() == "")
		{
			strcpy(s.msg, "物料代码不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", __FUNCTION__, "00  HEAT_NO=[{0}]", tmmsm2a["HEAT_NO"].ToString());
		/*if (tmmsm2a["MAT_TYPE"].ToString().Trim() == "")
		{
			cmd_sql.SetCommandText("SELECT MAT_TYPE FROM TMMSM50 WHERE MAT_CODE = @MAT_CODE ");
			cmd_sql.Parameters.Set("MAT_CODE", tmmsm2a["MAT_CODE"].ToString());
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read()) tmmsm2a["MAT_TYPE"] = cmd_sql.GetString(1);
			cmd_sql.Close();
		}*/
		if (tmmsm2a["STATION_ID"].ToString() == "B") table = "TMMSM21";
		else if (tmmsm2a["STATION_ID"].ToString() == "Y") table = "TMMSM21";//不锈钢转炉-预溶液
		else if (tmmsm2a["STATION_ID"].ToString() == "Z") table = "TMMSM19";//IF
		else if (tmmsm2a["STATION_ID"].ToString() == "E") table = "TMMSM20";
		else if (tmmsm2a["STATION_ID"].ToString() == "X") table = "TMMSM20";//不锈钢电炉-预溶液
		else if (tmmsm2a["STATION_ID"].ToString() == "A") table = "TMMSM27";//AOD
		else if (tmmsm2a["STATION_ID"].ToString() == "F") table = "TMMSM24";
		else if (tmmsm2a["STATION_ID"].ToString() == "R") table = "TMMSM23";
		else if (tmmsm2a["STATION_ID"].ToString() == "S") table = "TMMSM26";//LTS
		else if (tmmsm2a["STATION_ID"].ToString() == "V") table = "TMMSM25";
		else if (tmmsm2a["STATION_ID"].ToString() == "D") table = "TMMSM14";//增加脱硫加废钢功能 mfj  20230905
		Log::Trace("", __FUNCTION__, "table=[{0}]", table);
		CModel tmmsmaa(table);
		Log::Trace("", __FUNCTION__, "1table=[{0}]", table);
		tmmsmaa["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString();
		Log::Trace("", __FUNCTION__, "2table=[{0}]", table);
		tmmsmaa["PROC_NO"] = tmmsm2a["PROC_NO"].ToString();
		tmmsmaa["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString();
		Log::Trace("", __FUNCTION__, "1HEAT_NO=[{0}]", tmmsm2a["HEAT_NO"].ToString());
		tmmsmaa.Query("HEAT_NO,L2_PROC_NO");
		Log::Trace("", __FUNCTION__, "2HEAT_NO=[{0}]", tmmsm2a["HEAT_NO"].ToString());
		doFlag = f_mm0011("TMMSM2A_SEQ", 8, v_resume_seq_no, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", "dateNow", "dateNow = {0}", dateNow);
		Log::Trace("", "v_resume_seq_no", "v_resume_seq_no = {0}", v_resume_seq_no);
		seq = dateNow + v_resume_seq_no;
		tmmsm2a["PROD_SEQ_NO"] = seq;
		Log::Trace("", "RESUME_SEQ_NO", "RESUME_SEQ_NO = {0}", seq);
		Log::Trace("", "RESUME_SEQ_NO", "RESUME_SEQ_NO = {0}", tmmsm2a["PROD_SEQ_NO"].ToString());
		/*tmmsm2a["PROD_SHIFT_NO"] = tmmsmaa["PROD_SHIFT_NO"].ToString();
		tmmsm2a["PROD_SHIFT_GROUP"] = tmmsmaa["PROD_SHIFT_GROUP"].ToString();*/

		Log::Trace("", __FUNCTION__, "3HEAT_NO=[{0}]", tmmsm2a["HEAT_NO"].ToString());
		if (v_proc_div == "I") //新增
		{
			Log::Trace("", __FUNCTION__, "4HEAT_NO=[{0}]", tmmsm2a["HEAT_NO"].ToString());
			//修改库存表
			cmd_sql.SetCommandText("UPDATE TMMSM51 SET REC_REVISE_TIME = @dateNow,REC_REVISOR = @s.userid,STOCK_WT = STOCK_WT - @tmmsm2a.DEVO_WT WHERE MAT_CODE = @tmmsm2a.MAT_CODE AND FACTORY_DIV = @tmmsm2a.FACTORY_DIV");
			cmd_sql.Parameters.Set("dateNow", dateNow);
			cmd_sql.Parameters.Set("s.userid", s.userid);
			cmd_sql.Parameters.Set("tmmsm2a.DEVO_WT", tmmsm2a["DEVO_WT"].ToDecimal());
			cmd_sql.Parameters.Set("tmmsm2a.MAT_CODE", tmmsm2a["MAT_CODE"].ToString());
			cmd_sql.Parameters.Set("tmmsm2a.FACTORY_DIV", tmmsm2a["FACTORY_DIV"].ToString());
			cmd_sql.ExecuteNonQuery();
			cmd_sql.Close();


			//写收发明细表
			f_mmsm_get_seq_no(resume_seq_no, conn);
			tmmsm53["RESUME_SEQ_NO"] = resume_seq_no;
			tmmsm53["REC_CREATE_TIME"] = dateNow;
			tmmsm53["REC_CREATOR"] = s.userid;
			tmmsm53["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"];
			tmmsm53["HANDLE_DIV"] = "7";
			tmmsm53["OTHER_BILL_NO"] = tmmsm2a["PROC_NO"];
			tmmsm53["MAT_CODE"] = tmmsm2a["MAT_CODE"];
			tmmsm53["MAT_NAME"] = tmmsm2a["MAT_NAME"];
			tmmsm53["STOCK_WT"] = tmmsm2a["DEVO_WT"];
			tmmsm53["SMELT_DIV"] = tmmsm2a["STATION_ID"];

			tmmsm53.Insert();

			//新增消耗实绩表
			tmmsm2a["REC_CREATE_TIME"] = dateNow;
			tmmsm2a["DEVO_TIME"] = dateNow;
			tmmsm2a["REC_CREATOR"] = s.userid;

			blkNum = bcls_rec->Tables.IndexOf("PROCCOUNT");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("PROCCOUNT");
				bcls_rec->Tables["PROCCOUNT"].Columns.Add(DT_STRING, "TABLE_TYPE");
				bcls_rec->Tables["PROCCOUNT"].Columns.Add(DT_STRING, "HEAT_NO");
				bcls_rec->Tables["PROCCOUNT"].Columns.Add(DT_STRING, "PROC_NO");
			}
			if (bcls_rec->Tables["PROCCOUNT"].Rows.get_Count() <= 0)
			{
				bcls_rec->Tables["PROCCOUNT"].Rows.Add();
			}
			bcls_rec->Tables["PROCCOUNT"].Rows[0]["TABLE_TYPE"] = "TMMSM2A";
			bcls_rec->Tables["PROCCOUNT"].Rows[0]["HEAT_NO"] = tmmsm2a["HEAT_NO"];
			bcls_rec->Tables["PROCCOUNT"].Rows[0]["PROC_NO"] = tmmsm2a["PROC_NO"];

			doFlag = f_mmsm_count(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm2a["PROC_NO"].ToString() != " " || tmmsm2a["PROC_NO"].ToString() != ""){
				cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2a where  HEAT_NO ='" + tmmsm2a["HEAT_NO"].ToString().Trim() + "' ");
			}
			else{
				cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2a where  HEAT_NO ='" + tmmsm2a["HEAT_NO"].ToString().Trim() + "'  AND PROC_NO = '" + tmmsm2a["PROC_NO"].ToString() + "' ");
			}
			
			tmmsm2a["PROC_COUNT"] = cmd_inq.ExecuteScalar();
			cmd_inq.Close();
			//tmmsm2a["PROC_COUNT"] = bcls_ret->Tables[0].Rows[0]["PROC_COUNT"];
			Log::Trace("", "", "tmmsm2a11.newWt = {0}", newWt);
			tmmsm2a.Print();
			tmmsm2a.TrimOrBlank();
			Log::Trace("", "", "tmmsm2a11.222 = {0}", newWt);
			tmmsm2a.Insert();


		}
		if (v_proc_div == "U")
		{
			if (newWt != tmmsm2a["DEVO_WT"].ToDecimal())
			{
				//修改库存表
				cmd_sql.SetCommandText("UPDATE TMMSM51 SET REC_REVISE_TIME = @dateNow,REC_REVISOR = @s.userid,STOCK_WT = STOCK_WT + @tmmsm2a.DEVO_WT  - @newWt WHERE MAT_CODE = @tmmsm2a.MAT_CODE AND FACTORY_DIV = @tmmsm2a.FACTORY_DIV");
				cmd_sql.Parameters.Set("dateNow", dateNow);
				cmd_sql.Parameters.Set("s.userid", s.userid);
				cmd_sql.Parameters.Set("tmmsm2a.DEVO_WT", tmmsm2a["DEVO_WT"].ToDecimal());
				cmd_sql.Parameters.Set("newWt", newWt);
				cmd_sql.Parameters.Set("tmmsm2a.MAT_CODE", tmmsm2a["MAT_CODE"].ToString());
				cmd_sql.Parameters.Set("tmmsm2a.FACTORY_DIV", tmmsm2a["FACTORY_DIV"].ToString());
				cmd_sql.ExecuteNonQuery();
				cmd_sql.Close();

				//写收发明细表  先将上一次的重量写一笔负，然后本次重量写一笔正。

				f_mmsm_get_seq_no(resume_seq_no, conn);
				tmmsm53["RESUME_SEQ_NO"] = resume_seq_no;

				tmmsm53["REC_CREATE_TIME"] = dateNow;
				tmmsm53["REC_CREATOR"] = s.userid;
				tmmsm53["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"];
				tmmsm53["HANDLE_DIV"] = "7";
				tmmsm53["OTHER_BILL_NO"] = tmmsm2a["PROC_NO"];
				tmmsm53["MAT_CODE"] = tmmsm2a["MAT_CODE"];
				tmmsm53["MAT_NAME"] = tmmsm2a["MAT_NAME"];
				tmmsm53["STOCK_WT"] = tmmsm2a["DEVO_WT"].ToDecimal() * (-1);
				tmmsm53["SMELT_DIV"] = tmmsm2a["STATION_ID"];
				tmmsm53["SUB_RECORD_NO"] = tmmsm53["RESUME_SEQ_NO"];
				tmmsm53.Print();
				tmmsm53.Insert();


				//写收发明细表
				//f_mmsm_get_seq_no(tmmsm53["RESUME_SEQ_NO"].ToString(), conn);
				f_mmsm_get_seq_no(resume_seq_no, conn);
				tmmsm53["RESUME_SEQ_NO"] = resume_seq_no;

				tmmsm53["REC_CREATE_TIME"] = dateNow;
				tmmsm53["REC_CREATOR"] = s.userid;
				tmmsm53["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"];
				tmmsm53["HANDLE_DIV"] = "7";
				tmmsm53["OTHER_BILL_NO"] = tmmsm2a["PROC_NO"];
				tmmsm53["MAT_CODE"] = tmmsm2a["MAT_CODE"];
				tmmsm53["MAT_NAME"] = tmmsm2a["MAT_NAME"];
				tmmsm53["STOCK_WT"] = newWt;
				tmmsm53["SMELT_DIV"] = tmmsm2a["STATION_ID"];
				tmmsm53.Print();
				tmmsm53.Insert();

			}

			//修改消耗实绩表
			tmmsm2a["REC_REVISE_TIME"] = dateNow;
			tmmsm2a["REC_REVISOR"] = s.userid;
			tmmsm2a["DEVO_WT"] = newWt;
			tmmsm2a["HANDWORK_MARK"] = newHandWorkMark;
			tmmsm2a["PRACT_COLL_MODE"] = newCollMode;
			tmmsm2a.TrimOrBlank();
			//Log::Trace("", "", "tmmsm2a.newWt = {0}", newWt);
			Log::Info("", __FUNCTION__, "22newHandWorkMark =[{0}]", newHandWorkMark);
			Log::Info("", __FUNCTION__, "22newWt =[{0}]", newWt);
			Log::Info("", __FUNCTION__, "22newCollMode =[{0}]", newCollMode);
			tmmsm2a.Update("REC_REVISE_TIME,REC_REVISOR,DEVO_WT,HANDWORK_MARK,PRACT_COLL_MODE", "PROC_NO,MAT_CODE,PROC_COUNT");

		}
		if (v_proc_div == "D")//删除
		{
			//修改库存表
			cmd_sql.SetCommandText("UPDATE TMMSM51 SET REC_REVISE_TIME = @dateNow,REC_REVISOR = @s.userid,STOCK_WT = STOCK_WT + @tmmsm2a.DEVO_WT WHERE MAT_CODE = @tmmsm2a.MAT_CODE AND FACTORY_DIV = @tmmsm2a.FACTORY_DIV");
			cmd_sql.Parameters.Set("dateNow", dateNow);
			cmd_sql.Parameters.Set("s.userid", s.userid);
			cmd_sql.Parameters.Set("tmmsm2a.DEVO_WT", tmmsm2a["DEVO_WT"].ToDecimal());
			cmd_sql.Parameters.Set("tmmsm2a.MAT_CODE", tmmsm2a["MAT_CODE"].ToString());
			cmd_sql.Parameters.Set("tmmsm2a.FACTORY_DIV", tmmsm2a["FACTORY_DIV"].ToString());
			cmd_sql.ExecuteNonQuery();
			cmd_sql.Close();


			//写收发明细表
			//f_mmsm_get_seq_no(tmmsm53["RESUME_SEQ_NO"].ToString(), conn);
			f_mmsm_get_seq_no(resume_seq_no, conn);
			tmmsm53["RESUME_SEQ_NO"] = resume_seq_no;

			tmmsm53["REC_CREATE_TIME"] = dateNow;
			tmmsm53["REC_CREATOR"] = s.userid;
			tmmsm53["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"];
			tmmsm53["HANDLE_DIV"] = "7";
			tmmsm53["OTHER_BILL_NO"] = tmmsm2a["PROC_NO"];
			tmmsm53["MAT_CODE"] = tmmsm2a["MAT_CODE"];
			tmmsm53["MAT_NAME"] = tmmsm2a["MAT_NAME"];
			tmmsm53["STOCK_WT"] = tmmsm2a["DEVO_WT"].ToDecimal() * (-1);
			tmmsm53["SMELT_DIV"] = tmmsm2a["STATION_ID"];
			tmmsm53.Insert();
			//Log::Trace("", "", "tmmsm2a.newWt = {0}", newWt);
			//删除消耗实绩表
			tmmsm2a.Delete("PROC_NO,MAT_CODE,PROC_COUNT");

		}

		Log::Trace("", "", "tmmsm2a.PROC_NO = {0}", tmmsm2a["PROC_NO"].ToString());

#if defined(_SYS_PES)
		//调用发送电文
		blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSMSND");
		}

		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
		}

		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_DIV");
		}

		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}

		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("MAT_CODE"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAT_CODE");
		}
		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROD_SEQ_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROD_SEQ_NO");
		}

		bcls_rec->Tables["MMSMSND"].Rows.Add();
		bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM2A";
		bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString();
		bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString();
		bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_CODE"] = tmmsm2a["MAT_CODE"].ToString();
		bcls_rec->Tables["MMSMSND"].Rows[0]["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString();
		Log::Trace("", "", "tmmsm2a.MAT_CODE = {0}", bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_CODE"].ToString());

		doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
#endif


		if (v_proc_div == "I") tmmsm52["LOG_TYPE"] = "3";
		else if (v_proc_div == "U") tmmsm52["LOG_TYPE"] = "4";
		else tmmsm52["LOG_TYPE"] = "5";

		tmmsm52["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"].ToString();
		tmmsm52["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString();
		tmmsm52["PROC_NO"] = tmmsm2a["PROC_NO"].ToString();
		Log::Trace("", "", "tmmsm2a.PROC_COUNT = {0}", tmmsm2a["PROC_COUNT"].ToDecimal());
		tmmsm52["PROC_COUNT"] = tmmsm2a["PROC_COUNT"].ToDecimal();
		tmmsm52.MergeTo(in_mmsm2atrace.Tables[0], false);
		Log::Trace("", "", "tmmsm52.PROC_COUNT = {0}", tmmsm2a["PROC_COUNT"].ToDecimal());
		//记录物料消耗的履历
		ret = 0;
		//ret = f_mmsm2a_trace(&in_mmsm2atrace, bcls_ret, conn);
		if (ret < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//调用发送电文
		blkNum = bcls_rec->Tables.IndexOf("T823");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("T823");
		}

		if (!bcls_rec->Tables["T823"].Columns.Contains("DEAL_FLAG"))
		{
			bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "DEAL_FLAG");
		}

		if (!bcls_rec->Tables["T823"].Columns.Contains("PROC_COUNT"))
		{
			bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "PROC_COUNT");
		}

		if (!bcls_rec->Tables["T823"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}


		if (!bcls_rec->Tables["T823"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "HEAT_NO");
		}

		bcls_rec->Tables["T823"].Rows.Add();
		bcls_rec->Tables["T823"].Rows[0]["DEAL_FLAG"] = "I";
		bcls_rec->Tables["T823"].Rows[0]["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
		bcls_rec->Tables["T823"].Rows[0]["HEAT_NO"] = tmmsm2a["HEAT_NO"];
		bcls_rec->Tables["T823"].Rows[0]["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"];

		doFlag = f_t82306_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		/*设置系统返回参数*/
		strcpy(s.msg, _RES("GCRSS0000002"));//处理成功。  

		bcls_rec->Tables.Clear();
		tmmsm2a.MergeTo(bcls_rec->Tables.Add());
		doFlag = f_mmsm2ahj_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			strcpy(s.msg, "调用函数报错!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

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

