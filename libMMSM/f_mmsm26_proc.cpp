/***********************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: LTS实绩增删改
*************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件








//外部函数声明
int f_mmsm53_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_qmts_yc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);
int f_mmsm_sj_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm10_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_acyfl_tlcc(EIClass * bcls_rec, EIClass * bcls_ret, CString& v_acjc_relation_id, CDbConnection * conn);
int f_mmsm_acyfl_seq(CString& v_acjc_relation_id, CDbConnection * conn);

#if  defined _SYS_MES   || defined _SYS_PES
int f_pssm12_mm_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

#if  defined _SYS_PES
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

int f_mmsm009c_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//集控大屏写表
int f_mmsm_gyins2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//原料函数
int f_mmsm_hjjrl_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//宝武推送

BM2_FUNCTION_EXPORT
int f_mmsm26_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm26_proc";                //定义函数英文名称  
	CString FunctionCname = "LTS信息增删改";          //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   blkNum;
	EIClass mmsmbwlt;

	CString sqlstr = "";
	CString v_proc_div = "";
	CString v_pract_rcv_flag = "";
	CString v_factory_div = "";
	CString v_station_id = "";
	CString v_acjc_relation_id = "";

	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");


	EIClass inBlock1;
	EIClass outBlock1;
	EIClass mmsmgy06;


	try
	{
		CPageInfo pageInfo;

		/*数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。
		CDbCommand cmd_inq(conn);
		CDbCommand cmd_id(conn);

		CString PROD_SHIFT_NO = "";
		CString PROD_SHIFT_GROUP = "";
		/* 实体类定义 */
		CModel tmmsm26("TMMSM26");
		CModel hmmsm26("HMMSM26");
		CModel tmmsm26_old("TMMSM26");
		CModel tmmsm2a("TMMSM2A");
		CModel tmmsm2b("TMMSM2B");
		CModel tmmsm00("TMMSM00");


		//初始化实体类

		tmmsm26.Reset();
		tmmsm2a.Reset();
		tmmsm2b.Reset();
		tmmsm00.Reset();


		//调用校验及公共处理函数,针对有共性的字段进行赋值

		if (!bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "TABLE_TYPE");
		}

		bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"] = "TMMSM26";

		doFlag = f_mmsm_sj_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		tmmsm00.MergeFrom(bcls_ret->Tables["TMMSM00"].Rows[0]);
		tmmsm00.TrimOrBlank();


		/*如果是电文调用需要在电文接收service里对厂别和设备类型进行赋值*/
		if (bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
			v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))  //厂别
			v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_ID"))  //设备类型
			v_station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString().TrimOrBlank().ToUpper();

		//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
		//Log::Trace("", __FUNCTION__, "v_factory_div=[{0}]", v_factory_div);
		//Log::Trace("", __FUNCTION__, "v_station_id=[{0}]", v_station_id);

		tmmsm26.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm26.TrimOrBlank();
		hmmsm26.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		hmmsm26.TrimOrBlank();
		hmmsm26.Insert();
		if (tmmsm26["L2_PROC_NO"].ToString() == " "){
			tmmsm26["L2_PROC_NO"] = tmmsm26["PROC_NO"];
		}
		if (tmmsm26["PROC_NO"].ToString() == " "){
			tmmsm26["PROC_NO"] = tmmsm26["L2_PROC_NO"];
		}
		tmmsm26["FACTORY_DIV"] = "LG1";
		tmmsm26.CopyFrom(tmmsm00);
		tmmsm26.Print();

		//生产日期暂时定为取开始时刻。  mfj  20231120
		if (tmmsm26["START_TIME"].ToString().Trim() != "" && tmmsm26["PROD_DATE"].ToString().Trim() == "")
		{
			tmmsm26["PROD_DATE"] = tmmsm26["START_TIME"].ToString().SubstringNE(0, 8);
		}

		if (tmmsm26["PROD_DATE"].ToString().Trim() != "")
		{
			tmmsm26["PROD_DATE"] = tmmsm26["PROD_DATE"].ToString().Substring(0, 8);
		}

		f_mmsm_acyfl_seq(v_acjc_relation_id, conn);
		//抛帐Start
		if (bcls_rec->Tables.IndexOf("MMSMAC") < 0)
		{
			bcls_rec->Tables.Add("MMSMAC");
		}
		bcls_rec->Tables["MMSMAC"].Rows.Add();
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("TABLE_NAME"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "TABLE_NAME");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("FLAG"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "FLAG");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("ACJC_RELATION_ID"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "ACJC_RELATION_ID");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "PROC_DIV");
		}
		bcls_rec->Tables["MMSMAC"].Rows[0]["TABLE_NAME"] = "TMMSM26";
		bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		bcls_rec->Tables["MMSMAC"].Rows[0]["FLAG"] = "0"; //0抛负数 1抛正数
		bcls_rec->Tables["MMSMAC"].Rows[0]["ACJC_RELATION_ID"] = dateNow + "0000";
		bcls_rec->Tables["MMSMAC"].Rows[0]["PROC_DIV"] = v_proc_div;
		//Log::Trace("", __FUNCTION__, "bcls_rec->MMSMAC->HEAT_NO=[{0}]", bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"].ToString().Trim());
		//doFlag = f_mmsm_acyfl_tlcc(bcls_rec, bcls_ret, v_acjc_relation_id, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//抛帐End

		if (tmmsm26["PROD_SHIFT_NO"].ToString().Trim() == "" )
		{
			f_epep_get_shift_group("SMDD", tmmsm26["END_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
			tmmsm26["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
		}

		if (v_proc_div == "I")
		{
		
			//产品化三明模式，避免弹窗里保存按钮点击多次
			tmmsm26_old["HEAT_NO"] = tmmsm26["HEAT_NO"].ToString();
			tmmsm26_old["PROC_NO"] = tmmsm26["PROC_NO"].ToString();
			if (tmmsm26_old.QueryCount("HEAT_NO,PROC_NO") > 0)
			{
				strcpy(s.msg, "主实绩已保存!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm26["ID_SJ"].ToString() == " " || tmmsm26["ID_SJ"].ToString() == ""){
				cmd_id.SetCommandText(" SELECT LPAD(TO_CHAR(TEST_ID_XMY.NEXTVAL), 9, '0') AS ID FROM DUAl ");
				cmd_id.ExecuteReader();
				if (cmd_id.Read())
				{
					tmmsm26["ID_SJ"] = cmd_id.GetString(1);
				}
				cmd_id.Close();
			}
			CString f = "-";
			tmmsm26["ID_SJ"] = f + tmmsm26["ID_SJ"].ToString();
			tmmsm26.Insert();
			v_pract_rcv_flag = "1";

			//炉次等级计算
			inBlock1.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			inBlock1.Tables[0].Columns.Add(DT_STRING, "WHOLE_BACKLOG_CODE");
			inBlock1.Tables[0].Columns.Add(DT_STRING, "ST_NO");
			inBlock1.Tables[0].Columns.Add(DT_STRING, "PROC_NO");

			inBlock1.Tables[0].Rows.Add();
			inBlock1.Tables[0].Rows[0]["HEAT_NO"] = tmmsm26["HEAT_NO"];
			inBlock1.Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"] = tmmsm00["STATION_ID"];
			inBlock1.Tables[0].Rows[0]["ST_NO"] = tmmsm26["ST_NO"];
			inBlock1.Tables[0].Rows[0]["PROC_NO"] = tmmsm26["PROC_NO"];

			//doFlag = f_qmts_yc(&inBlock1, &outBlock1,conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else if (v_proc_div == "U")
		{
			Log::Trace("","", "11111");
			CString heat_no = " ";
			CString proc_no = " ";
			cmd_inq.SetCommandText(" select HEAT_NO,PROC_NO from TMMSM26 WHERE L2_PROC_NO='" + tmmsm26["L2_PROC_NO"].ToString() + "' ");
			cmd_inq.ExecuteReader();
			Log::Trace("", __FUNCTION__, "v0=[{0}]", tmmsm26["L2_PROC_NO"].ToString());

			if (cmd_inq.Read())
			{
				heat_no = cmd_inq.GetString(1);
				proc_no = cmd_inq.GetString(2);
				if (heat_no == " "){
					tmmsm26.Delete("L2_PROC_NO");
				}
				else
				{
					tmmsm26["HEAT_NO"] = heat_no;
					tmmsm26["PROC_NO"] = proc_no;
					tmmsm26.Delete("HEAT_NO,PROC_NO");
					tmmsm26["HEAT_NO"] = heat_no;
					tmmsm26["PROC_NO"] = proc_no;
				}
			}
			Log::Trace("", __FUNCTION__, "v1=[{0}]", proc_no);
			cmd_inq.Close();
			tmmsm26.Insert();
			v_pract_rcv_flag = "1";


		}
		else if (v_proc_div == "D")
		{
			tmmsm26.Delete("PROC_NO");

			tmmsm2a["HEAT_NO"] = tmmsm26["HEAT_NO"];
			tmmsm2b["HEAT_NO"] = tmmsm26["HEAT_NO"];
			tmmsm2a["PROC_NO"] = tmmsm26["PROC_NO"];
			tmmsm2b["PROC_NO"] = tmmsm26["PROC_NO"];

			blkNum = bcls_rec->Tables.IndexOf("MMSM53");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("MMSM53");
			}


			if (!bcls_rec->Tables["MMSM53"].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables["MMSM53"].Columns.Add(DT_STRING, "PROC_DIV");
			}

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr = " SELECT * FROM TMMSM2A "
					"  WHERE PROC_NO = @proc_no ";

				break;
			}

			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Set("proc_no", tmmsm26["PROC_NO"].ToString());
			cmd_sql.ExecuteReader();

			while (cmd_sql.Read())
			{
				cmd_sql.Fetch(tmmsm2a);

				bcls_rec->Tables["MMSM53"].Rows.Clear();
				tmmsm2a.MergeTo(bcls_rec->Tables["MMSM53"], false);
				bcls_rec->Tables["MMSM53"].Rows[0]["PROC_DIV"] = v_proc_div;

				doFlag = f_mmsm53_proc(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

			}
			cmd_sql.Close();


			tmmsm2a.Delete("HEAT_NO,PROC_NO");
			tmmsm2b.Delete("HEAT_NO,PROC_NO");

			v_pract_rcv_flag = " ";

		}

		//抛帐Start
		if (bcls_rec->Tables.IndexOf("MMSMAC") < 0)
		{
			bcls_rec->Tables.Add("MMSMAC");
		}
		bcls_rec->Tables["MMSMAC"].Rows.Add();
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("TABLE_NAME"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "TABLE_NAME");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("FLAG"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "FLAG");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("ACJC_RELATION_ID"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "ACJC_RELATION_ID");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "PROC_DIV");
		}if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("PONO"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "PONO");
		}
		bcls_rec->Tables["MMSMAC"].Rows[0]["TABLE_NAME"] = "TMMSM26";
		bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		bcls_rec->Tables["MMSMAC"].Rows[0]["FLAG"] = "1"; //0抛负数 1抛正数
		bcls_rec->Tables["MMSMAC"].Rows[0]["ACJC_RELATION_ID"] = dateNow + "0000";
		bcls_rec->Tables["MMSMAC"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["MMSMAC"].Rows[0]["PONO"] = tmmsm26["PONO"].ToString();
		//Log::Trace("", __FUNCTION__, "bcls_rec->MMSMAC->HEAT_NO=[{0}]", bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"].ToString().Trim());
		//doFlag = f_mmsm_acyfl_tlcc(bcls_rec, bcls_ret, v_acjc_relation_id, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//抛帐End


		/****** 调用炼钢实绩总表处理函数 ***** */
		if (!bcls_rec->Tables.Contains("MMSM10"))
		{
			bcls_rec->Tables.Add("MMSM10");
		}

		//炉号
		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "HEAT_NO");
		}

		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "PROC_NO");
		}

		//工位标识
		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("STATION_ID"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "STATION_ID");
		}
		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "PROC_DIV");
		}
		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("PONO"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "PONO");
		}
		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}

		bcls_rec->Tables["MMSM10"].Rows.Add();
		bcls_rec->Tables["MMSM10"].Rows[0]["HEAT_NO"] = tmmsm26["HEAT_NO"];
		bcls_rec->Tables["MMSM10"].Rows[0]["PROC_NO"] = tmmsm26["PROC_NO"];
		bcls_rec->Tables["MMSM10"].Rows[0]["STATION_ID"] = tmmsm00["STATION_ID"];
		bcls_rec->Tables["MMSM10"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["MMSM10"].Rows[0]["PONO"] = tmmsm26["PONO"];
		bcls_rec->Tables["MMSM10"].Rows[0]["L2_PROC_NO"] = tmmsm26["L2_PROC_NO"];
		doFlag = f_mmsm10_trace(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}


#if  defined _SYS_MES   || defined _SYS_PES
		/****** 调用炼钢计划函数 ***** */
		if (!bcls_rec->Tables.Contains("PSSM12"))
		{
			bcls_rec->Tables.Add("PSSM12");
		}

		//实绩接受标记
		if (!bcls_rec->Tables["PSSM12"].Columns.Contains("PRACT_RCV_FLAG"))
		{
			bcls_rec->Tables["PSSM12"].Columns.Add(DT_STRING, "PRACT_RCV_FLAG");
		}
		if (!bcls_rec->Tables["PSSM12"].Columns.Contains("SM_PLAN_NO"))
		{
			bcls_rec->Tables["PSSM12"].Columns.Add(DT_STRING, "SM_PLAN_NO");
		}

		tmmsm00.MergeTo(bcls_rec->Tables["PSSM12"], false);
		bcls_rec->Tables["PSSM12"].Rows[0]["PRACT_RCV_FLAG"] = v_pract_rcv_flag;
		bcls_rec->Tables["PSSM12"].Rows[0]["SM_PLAN_NO"] = tmmsm26["SM_PLAN_NO"].ToString();
		bcls_rec->Tables["PSSM12"].Rows[0]["FACTORY_DIV"] = v_factory_div;
		Log::Trace("", __FUNCTION__, "FACTORY_DIV=[{0}]", bcls_rec->Tables["PSSM12"].Rows[0]["FACTORY_DIV"].ToString());
		doFlag = f_pssm12_mm_rcv(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
#endif

		//调用发送电文
		Log::Trace("", __FUNCTION__, "集控大屏=[{0}]", v_proc_div);
		blkNum = bcls_rec->Tables.IndexOf("JKDP");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("JKDP");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "PROC_DIV");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "PROC_NO");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "HEAT_NO");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("TABLE_NAME"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "TABLE_NAME");
		}
		if (!bcls_rec->Tables["JKDP"].Columns.Contains("ID_SJ"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "ID_SJ");
		}
		if (!bcls_rec->Tables["JKDP"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}
		bcls_rec->Tables["JKDP"].Rows.Add();
		bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tmmsm26["PROC_NO"].ToString();
		bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"] = tmmsm26["HEAT_NO"].ToString();
		bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_LTS_PROD_SUMMARY";
		bcls_rec->Tables["JKDP"].Rows[0]["ID_SJ"] = tmmsm26["ID_SJ"].ToString();
		bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"] = tmmsm26["L2_PROC_NO"].ToString();
		doFlag = f_mmsm009c_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}


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

		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_NO");
		}

		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}
		bcls_rec->Tables["MMSMSND"].Rows.Add();
		bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM26";
		bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tmmsm26["PROC_NO"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tmmsm26["HEAT_NO"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tmmsm26["L2_PROC_NO"];

		doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
#endif

		mmsmgy06.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		mmsmgy06.Tables[0].Columns.Add(DT_STRING, "SM_PLAN_NOL2");
		mmsmgy06.Tables[0].Rows.Add();
		mmsmgy06.Tables[0].Rows[0]["HEAT_NO"] = tmmsm26["HEAT_NO"];
		mmsmgy06.Tables[0].Rows[0]["SM_PLAN_NOL2"] = tmmsm26["SM_PLAN_NOL2"];
		doFlag = f_mmsm_gyins2(&mmsmgy06, bcls_ret, conn);

		if (tmmsm26["ST_NO"].ToString() != " "&&tmmsm26["NEXT_DEV_CODE"].ToString().SubstringNE(0, 1) == "C"){
			mmsmbwlt.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			mmsmbwlt.Tables[0].Columns.Add(DT_STRING, "STATION_ID");
			mmsmbwlt.Tables[0].Columns.Add(DT_STRING, "STATION_CODE");
			mmsmbwlt.Tables[0].Columns.Add(DT_STRING, "ST_NO");
			mmsmbwlt.Tables[0].Rows.Add(); 
			mmsmbwlt.Tables[0].Rows[0]["HEAT_NO"] = tmmsm26["HEAT_NO"];
			mmsmbwlt.Tables[0].Rows[0]["STATION_ID"] = tmmsm26["STATION_ID"];
			mmsmbwlt.Tables[0].Rows[0]["STATION_CODE"] = "LTS";
			mmsmbwlt.Tables[0].Rows[0]["ST_NO"] = tmmsm26["ST_NO"];
			doFlag = f_mmsm_hjjrl_proc(&mmsmbwlt, bcls_ret, conn);
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

