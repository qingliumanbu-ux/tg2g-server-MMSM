/***********************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 缓冷退火实绩增删改
*************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件







//外部函数声明

int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_pssm81_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#if defined(_SYS_PES)
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

//发切废电文
int f_mmsm_t82303_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天
BM2_FUNCTION_EXPORT
int f_mmsm3601_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm3601_proc";                //定义函数英文名称  
	//CString FunctionCname = "缓冷退火信息增删改";          //定义函数中文名称
	CString FunctionCname = "水爆信息增删改";          //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   blkNum;

	CString sqlstr = "";
	CString sqlstr1 = "";
	CString v_proc_div = "";
	CString v_pract_rcv_flag = "";
	CDecimal  v_cut_slab = 0;
	CDecimal  v_cut_charge = 0;
	CString v_factory_div = "";
	CString v_station_id = "";
	CString v_hot_flag = "";
	CString station_id = "";
	CString v_cold_v_hot_flag = "";
	CString v_plan_no = "";
	CString v_plan_backlog_code = "";
	CString casting_pre_judgment = " ";

	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass inBlock1;
	EIClass outBlock1;


	CString batch = " ";
	CString st_no = " ";
	CString unit_code = " ";
	CString mat_width = " ";
	CString deal_notion = " ";
	CString grind_main_reason = " ";


	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "BATCH");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "SG_SIGN");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "UNIT_CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "MAT_WIDTH");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DEAL_NOTION");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "GRIND_MAIN_REASON");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CASTING_PRE_JUDGMENT");

	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");


	try
	{
		CPageInfo pageInfo;

		/*数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
		CModel tmmsm01("TMMSM01");
		CModel tmmsm36("TMMSM36");
		CModel tmmsm96("TMMSM96");
		CDbCommand cmd_inq(conn);
		CDbCommand cmd_inqws(conn);

		CModel hmmsm01("HMMSM01");

		CString PROD_SHIFT_NO = "";
		CString PROD_SHIFT_GROUP = "";

		//初始化实体类
		tmmsm36.Reset();

		blkNum = bcls_rec->Tables.IndexOf("PSSM");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PSSM");
			bcls_rec->Tables["PSSM"].Columns.Add(DT_STRING, "FACTORY_DIV");
			bcls_rec->Tables["PSSM"].Columns.Add(DT_STRING, "PLAN_BACKLOG_CODE");
			bcls_rec->Tables["PSSM"].Columns.Add(DT_STRING, "PLAN_NO");
			bcls_rec->Tables["PSSM"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["PSSM"].Rows.Add();
		}


		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}

		/*如果是电文调用需要在电文接收service里对厂别和设备类型进行赋值*/
		if (bcls_rec->Tables["PARA"].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
			v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables["PARA"].Columns.Contains("STATION_ID"))
			v_station_id = bcls_rec->Tables["PARA"].Rows[0]["STATION_ID"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables["PARA"].Columns.Contains("HOT_FLAG"))
			v_hot_flag = bcls_rec->Tables["PARA"].Rows[0]["HOT_FLAG"].ToString().TrimOrBlank().ToUpper();


		tmmsm36.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm36["REC_CREATE_TIME"] = dateNow;
		tmmsm36["REC_CREATOR"] = s.userid;
		tmmsm36["STATION_ID"] = v_station_id;
		tmmsm36.TrimOrBlank();

		//Log::Trace("", __FUNCTION__, "Count()=[{0}]", bcls_rec->Tables["PARA"].Rows.get_Count());
		bcls_rec->Tables["MM0099"].Rows.Clear();
		bcls_rec->Tables["PSSM"].Rows.Clear();

		for (int i = 0; i < bcls_rec->Tables["PARA"].Rows.get_Count(); i++)
		{

			
			if (bcls_rec->Tables["PARA"].Columns.Contains("MAT_NO"))
				tmmsm36["MAT_NO"] = bcls_rec->Tables["PARA"].Rows[i]["MAT_NO"].ToString().TrimOrBlank().ToUpper();

			if (v_proc_div == "I")
			{
				int count = tmmsm36.QueryCount("MAT_NO, HEAT_NO");
				if (count == 1)
				{
					CFormattable arguments[] = { tmmsm36["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料{0}已经存在实绩信息,请修改或者删除实绩信息", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (v_hot_flag != "")
				{
					tmmsm36["HOT_FLAG"] = v_hot_flag;
				}

				/*--------------------------------------------------------------班次班组计算---------------------------------------------------------------------*/
				CString prodTime = CDateTime::Now().ToString("yyyyMMddHHmmss");//班次班组根据开始时刻计算
				Log::Trace("", "", "时间={0}", prodTime);
				f_epep_get_shift_group("SMCP", prodTime, PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);


				//f_mm0017(s.userid, PROD_SHIFT_NO, PROD_SHIFT_GROUP, prodTime, conn);
				tmmsm36["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
				tmmsm36["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;
				Log::Trace("", "", "计算班组={0}", PROD_SHIFT_GROUP);

				//2024-05-10
				if (tmmsm36["INSPECT_SHIFT"].ToString()!=" "&&tmmsm36["INSPECT_SHIFT"].ToString().Trim()!="")
				{
					tmmsm36["PROD_SHIFT_GROUP"] = tmmsm36["INSPECT_SHIFT"];
					Log::Trace("", "", "检验班组={0}", tmmsm36["PROD_SHIFT_GROUP"].ToString());
				}

				//2024-0509
				tmmsm36["CLIENT_IP"] = s.fore_ip;

				tmmsm36.Insert();
				tmmsm96["DEAL_NOTION"] = tmmsm36["DEAL_NOTION"];
				tmmsm96["GRIND_MAIN_REASON"] = tmmsm36["GRIND_MAIN_REASON"];
			}
			else if (v_proc_div == "U")
			{
				
				tmmsm36.Delete();

				//2024-05-10
				if (tmmsm36["INSPECT_SHIFT"].ToString() != " "&&tmmsm36["INSPECT_SHIFT"].ToString().Trim() != "")
				{
					tmmsm36["PROD_SHIFT_GROUP"] = tmmsm36["INSPECT_SHIFT"];
					Log::Trace("", "", "检验班组={0}", tmmsm36["PROD_SHIFT_GROUP"].ToString());
				}

				tmmsm36.Insert();
				

				tmmsm96["DEAL_NOTION"] = tmmsm36["DEAL_NOTION"];
				tmmsm96["GRIND_MAIN_REASON"] = tmmsm36["GRIND_MAIN_REASON"];
			}
			else if (v_proc_div == "D")
			{
				tmmsm36.Delete();
			}

			tmmsm96["MAT_NO"] = tmmsm36["MAT_NO"];

			tmmsm96["EVENT_ID"] = "MM36";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "f_mmsm3601_proc";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "热处理实绩产出";

			bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
			bcls_rec->Tables["MM0099"].Rows[i].Merge(tmmsm96);//框架后续会支持


			/*
				日期：20240516
				原因：对于出库的材料，无法上填水爆记录，需要判断一下，
				如果是在线--调用99函数；如果是归档--不调用99函数
			*/
			int count = tmmsm01.QueryCount("MAT_NO");
		
			if (count > 0)
			{
				doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else
			{
				if (v_proc_div == "I" || v_proc_div == "U")
				{
					Log::Trace("", "", "归档水爆={0}", "归档材料");
					hmmsm01["MAT_NO"] = tmmsm36["MAT_NO"];
					hmmsm01.Query();
					hmmsm01["DEAL_NOTION"] = tmmsm36["DEAL_NOTION"];
					hmmsm01["GRIND_MAIN_REASON"] = tmmsm36["GRIND_MAIN_REASON"];

					hmmsm01.Update("DEAL_NOTION,GRIND_MAIN_REASON","MAT_NO");

				}
				
				
			}
		
				//成品水爆坯检查结果 处置意见:不等于0时
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:

					sqlstr1 = "SELECT  B.BATCH, B.ST_NO, B.UNIT_CODE, B.MAT_WIDTH, A.DEAL_NOTION, A.GRIND_MAIN_REASON,A.CASTING_PRE_JUDGMENT  FROM TMMSM36 A "
						"LEFT JOIN TMMSM01 B ON A.MAT_NO = B.MAT_NO "
						"WHERE A.DEAL_NOTION <> '0'  AND B.MAT_NO='" + tmmsm36["MAT_NO"].ToString() + "'";
				}
				Log::Trace("", __FUNCTION__, "MAT_NO1 =[{0}]]", tmmsm36["MAT_NO"].ToString());
				Log::Trace("", __FUNCTION__, "sqlstr1 =[{0}]]", sqlstr1);
				cmd_inqws.SetCommandText(sqlstr1);
				cmd_inqws.ExecuteReader();
				
				if (cmd_inqws.Read())
				{
					batch = cmd_inqws.GetString(1);
					st_no = cmd_inqws.GetString(2);
					unit_code = cmd_inqws.GetString(3);
					mat_width = cmd_inqws.GetString(4);
					deal_notion = cmd_inqws.GetString(5);
					grind_main_reason = cmd_inqws.GetString(6);
					casting_pre_judgment = cmd_inqws.GetString(7);
				}
				Log::Trace("", __FUNCTION__, "batch[{0}]  ", batch);
				Log::Trace("", __FUNCTION__, "st_no[{0}]  ", st_no); 
				Log::Trace("", __FUNCTION__, "deal_notion[{0}]  ", deal_notion);
				cmd_inqws.Close();

				if (deal_notion.TrimOrBlank() != "0" && batch.TrimOrBlank() !=" ")
				{
					Log::Trace("", __FUNCTION__, "deal_notion1[{0}]  ", deal_notion);
				//发送宝武聊天
				bcls_rec_s.Tables[0].Rows.Add();
				bcls_rec_s.Tables[0].Rows[0]["BATCH"] = batch;
				bcls_rec_s.Tables[0].Rows[0]["SG_SIGN"] = st_no;
				bcls_rec_s.Tables[0].Rows[0]["UNIT_CODE"] = unit_code;
				bcls_rec_s.Tables[0].Rows[0]["MAT_WIDTH"] = mat_width;
				bcls_rec_s.Tables[0].Rows[0]["DEAL_NOTION"] = deal_notion;
				bcls_rec_s.Tables[0].Rows[0]["GRIND_MAIN_REASON"] = grind_main_reason;
				bcls_rec_s.Tables[0].Rows[0]["CASTING_PRE_JUDGMENT"] = casting_pre_judgment;
				bcls_rec_s.Tables[0].Rows[0]["CODE"] = "4";
				bcls_rec_s.Tables[0].Rows[0]["REMARK"] = batch;

				doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
				if (doFlag < 0)
				{
					strcpy(s.msg, "调用函数报错!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
				//成品水爆坯检查结果  修磨原因为：相机坏；处置意见:等于0时
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:

					sqlstr1 = "SELECT  B.BATCH, B.ST_NO, B.UNIT_CODE, B.MAT_WIDTH, A.DEAL_NOTION, A.GRIND_MAIN_REASON,A.CASTING_PRE_JUDGMENT  FROM TMMSM36 A "
						"LEFT JOIN TMMSM01 B ON A.MAT_NO = B.MAT_NO "
						"WHERE A.DEAL_NOTION = '0' AND A.GRIND_MAIN_REASON ='相机坏'  AND B.MAT_NO='" + tmmsm36["MAT_NO"].ToString() + "'";
				}
				Log::Trace("", __FUNCTION__, "MAT_NO1 =[{0}]]", tmmsm36["MAT_NO"].ToString());
				Log::Trace("", __FUNCTION__, "sqlstr1 =[{0}]]", sqlstr1);
				cmd_inqws.SetCommandText(sqlstr1);
				cmd_inqws.ExecuteReader();

				if (cmd_inqws.Read())
				{
					batch = cmd_inqws.GetString(1);
					st_no = cmd_inqws.GetString(2);
					unit_code = cmd_inqws.GetString(3);
					mat_width = cmd_inqws.GetString(4);
					deal_notion = cmd_inqws.GetString(5);
					grind_main_reason = cmd_inqws.GetString(6);
					casting_pre_judgment = cmd_inqws.GetString(7);
				}
				Log::Trace("", __FUNCTION__, "batch[{0}]  ", batch);
				Log::Trace("", __FUNCTION__, "st_no[{0}]  ", st_no);
				Log::Trace("", __FUNCTION__, "deal_notion[{0}]  ", deal_notion);
				cmd_inqws.Close();

				if (deal_notion.TrimOrBlank() == "0" && batch.TrimOrBlank() != " ")
				{
					Log::Trace("", __FUNCTION__, "deal_notion1[{0}]  ", deal_notion);
					//发送宝武聊天
					bcls_rec_s.Tables[0].Rows.Add();
					bcls_rec_s.Tables[0].Rows[0]["BATCH"] = batch;
					bcls_rec_s.Tables[0].Rows[0]["SG_SIGN"] = st_no;
					bcls_rec_s.Tables[0].Rows[0]["UNIT_CODE"] = unit_code;
					bcls_rec_s.Tables[0].Rows[0]["MAT_WIDTH"] = mat_width;
					bcls_rec_s.Tables[0].Rows[0]["DEAL_NOTION"] = deal_notion;
					bcls_rec_s.Tables[0].Rows[0]["GRIND_MAIN_REASON"] = grind_main_reason;
					bcls_rec_s.Tables[0].Rows[0]["CASTING_PRE_JUDGMENT"] = casting_pre_judgment;

					bcls_rec_s.Tables[0].Rows[0]["CODE"] = "4";
					bcls_rec_s.Tables[0].Rows[0]["REMARK"] = batch;

					doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
					if (doFlag < 0)
					{
						strcpy(s.msg, "调用函数报错!");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}



			//先判断材料号是否在计划中存在，如果存在，才调用计划跟踪函数

			//获得输入参数
			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//case DB_KIND_ORACLE:	    // Oracle 数据库
			//default:
			//	sqlstr = "SELECT  FACTORY_DIV,PLAN_NO,PLAN_BACKLOG_CODE "
			//		" FROM    TPSSM81 "
			//		" WHERE   MAT_NO = @mat_no";
			//	break;
			//}
			//cmd_sql.SetCommandText(sqlstr);
			//cmd_sql.Parameters.Clear();
			//cmd_sql.Parameters.Set("mat_no", tmmsm36["MAT_NO"].ToString());
			//cmd_sql.ExecuteReader();
			//if (cmd_sql.Read())
			//{

			//	v_factory_div = cmd_sql.GetString(1);
			//	v_plan_no = cmd_sql.GetString(2);
			//	v_plan_backlog_code = cmd_sql.GetString(3);

			//	//Log::Trace("", __FUNCTION__, "v_factory_div=[{0}]", v_factory_div);
			//	//Log::Trace("", __FUNCTION__, "v_plan_no=[{0}]", v_plan_no);

			//	/*if (bcls_rec->Tables["PSSM"].Rows.get_Count() <= 0)
			//	{
			//	bcls_rec->Tables["PSSM"].Rows.Add();
			//	}*/
			//	bcls_rec->Tables["PSSM"].Rows.Add(); // 创建一行
			//	bcls_rec->Tables["PSSM"].Rows[i]["FACTORY_DIV"] = v_factory_div;
			//	bcls_rec->Tables["PSSM"].Rows[i]["PLAN_BACKLOG_CODE"] = v_plan_backlog_code;
			//	bcls_rec->Tables["PSSM"].Rows[i]["MAT_NO"] = tmmsm36["MAT_NO"];
			//	bcls_rec->Tables["PSSM"].Rows[i]["PLAN_NO"] = v_plan_no;

			//	/*	doFlag = f_pssm81_trace(bcls_rec, bcls_ret, conn);
			//		if (doFlag < 0)
			//		{
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//		}*/

			//}
			//cmd_sql.Close();





			///*发送MMS电文:炼钢板坯精整实绩*/
			//#if defined(_SYS_PES)

			//blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
			//if (blkNum < 0)
			//{
			//	bcls_rec->Tables.Add("MMSMSND");
			//	bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			//	bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_DIV");
			//	bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAT_NO");
			//}

			////--------向MMS送电文函数----------------



			//bcls_rec->Tables["MMSMSND"].Rows.Clear();

			//if (bcls_rec->Tables["MMSMSND"].Rows.get_Count() <= 0)
			//	bcls_rec->Tables["MMSMSND"].Rows.Add();
			//bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM36";
			//bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"] = tmmsm36["MAT_NO"];
			//bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;   /*1:新增 2:修改 0:删除*/

			//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]]", v_proc_div);
			//Log::Trace("", __FUNCTION__, "tmmsm36["MAT_NO"] =[{0}]]", tmmsm36["MAT_NO"].ToString());

			//doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
			//if (doFlag < 0)
			//{
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}



			//#endif

		}

		//Log::Trace("", __FUNCTION__, "Count222222222()=[{0}]", bcls_rec->Tables["MM0099"].Rows.get_Count());

		/*if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}*/


		/*if (bcls_rec->Tables["PSSM"].Rows.get_Count() > 0)
		{
			doFlag = f_pssm81_trace(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}*/

		// 发送电文---
		Log::Trace("", "", "t82303={0}", "发送电文");
		EIClass bcls_rec_t82303;
		bcls_rec_t82303.Tables[0].set_TableName("t82303");
		bcls_rec_t82303.Tables[0].Columns.Add(tmmsm36);
		bcls_rec_t82303.Tables[0].Rows.Add();
		bcls_rec_t82303.Tables[0].Rows[0]["MAT_NO"] = tmmsm36["MAT_NO"];
		Log::Trace("", "", "t82303={0}", "INNER_1I");
		doFlag = f_mmsm_t82303_snd(&bcls_rec_t82303, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

#if defined(_SYS_PES)
		for (int i = 0; i < bcls_rec->Tables["PARA"].Rows.get_Count(); i++)
		{
			/*发送MMS电文:炼钢板坯精整实绩*/

			if (bcls_rec->Tables["PARA"].Columns.Contains("MAT_NO"))
				tmmsm36["MAT_NO"] = bcls_rec->Tables["PARA"].Rows[i]["MAT_NO"].ToString().TrimOrBlank().ToUpper();

			blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("MMSMSND");
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_DIV");
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAT_NO");
			}

			//--------向MMS送电文函数----------------

			bcls_rec->Tables["MMSMSND"].Rows.Clear();

			if (bcls_rec->Tables["MMSMSND"].Rows.get_Count() <= 0)
				bcls_rec->Tables["MMSMSND"].Rows.Add();
			bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM36";
			bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"] = tmmsm36["MAT_NO"];
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;   /*1:新增 2:修改 0:删除*/

			//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]]", v_proc_div);
			//Log::Trace("", __FUNCTION__, "tmmsm36["MAT_NO"] =[{0}]]", tmmsm36["MAT_NO"].ToString());

			//doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}


		}

#endif



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

