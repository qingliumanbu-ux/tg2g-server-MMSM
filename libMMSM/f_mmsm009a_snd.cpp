/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2015-02-01
Description: 炼钢实绩总调电文发送
**************************************************/
//框架头文件
#include "stdafx.h" 
#include "epex.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢实绩总调电文发送
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件



//外部函数声明

BM2_FUNCTION_EXPORT
 int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	int j	= 0;

	/* 业务变量 */
	CString	datetime("");    
	int		fetchRowCount = 0;
	CString v_table_type = "";
	CString v_proc_div = "";
	CString v_heat_no = "";
	CString v_proc_no = "";
	CString v_event_id = "";
	CString cs_main_mat_no = "";
	CString v_tc_backlog = "";
	CDecimal v_proc_count = 0;


	/* 创建电文处理对象 */
	EPEX epex(&s,conn);

	/* 实体类定义 */
	CModel tmm009a("TMM009A");
	CModel tmmsm2b("TMMSM2B");
	CModel tmmsm12("TMMSM12");
	CModel tmmsm13("TMMSM13");
	CModel tmmsm14("TMMSM14");
	CModel tmmsm14b("TMMSMKR14");
	CModel tmmsm19("TMMSM19");
	
	CModel tmmsm20("TMMSM20");
	CModel tmmsm21("TMMSM21");
	CModel tmmsm22("TMMSM22");

	CModel tmmsm23("TMMSM23");
	CModel tmmsm24("TMMSM24");
	CModel tmmsm25("TMMSM25");
	CModel tmmsm26("TMMSM26");
	CModel tmmsm27("TMMSM27");

	CModel tmmsm31("TMMSM31");
	CModel tmmsm31a("TMMSM31A");
	CModel tmmsm33("TMMSM33");
	CModel tmmsm34("TMMSM34");
	CModel tmmsm35("TMMSM35");
	CModel tmmsm36("TMMSM36");
	CModel tmmsm2a("TMMSM2A_YL");
	
	CModel tmmsm50("TMMSM50");
	CModel tmmsm55("TMMSM55");
	CModel tmmsm56("TMMSM56");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm01_new("TMMSM01");
	CModel tmmsm061("TMMSM061");
	CModel tmmsmgy06("TMMSMGY06");
	

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	//连铸时间查询
	CDbCommand cmd_inq31(conn);
	//倒灌查询出铁开始结束时刻
	CDbCommand cmd_inq12(conn);

	try
	{	
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMSMSND 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
    
	
		/* 获取输入参数 */
	
		if(bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
			v_tc_backlog = bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"].ToString().Trim();
		if(bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_DIV"))
			v_proc_div = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"].ToString().Trim();

		if (bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_COUNT"))
			v_proc_count = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_COUNT"].ToDecimal();

		//Log::Trace("",__FUNCTION__,"传入参数,v_proc_count= [{0}]",v_proc_count);	  	
	
		tmm009a["MAT_KIND"] = "SM";
		tmm009a["EVENT_ID"] = "MM9A";
		tmm009a["EVENT_LINE_TYPE"] = "00";
		tmm009a["TC_BACKLOG"] = v_tc_backlog;
	

		/* 打印输入参数 */
			
		Log::Trace("",__FUNCTION__,"传入参数,v_tc_backlog				= [{0}]",v_tc_backlog);	  	

		/* 检查输入参数合法性 */
		if(tmm009a["TC_BACKLOG"].ToString().Trim() == "")
		{
			strcpy(s.msg,"电文工序值不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
		/* 查询电文号 */
		tmm009a.Query("MAT_KIND,EVENT_LINE_TYPE,EVENT_ID,TC_BACKLOG");
		tmm009a.TrimOrBlank();

		Log::Trace("",__FUNCTION__,"查询电文号 tmm009a.TC_NO = [{0}]",tmm009a["TC_NO"].ToString());	  	
	  	

		/* 实绩表配置电文号则发送电文 */
		if (tmm009a["TC_SEND_FLAG"].ToString().Trim() == "Y")	//Y-发送电文
		{
			if(tmm009a["TC_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"设置发送电文的配置，电文号不能为空!请在MM0097A1画面维护。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 发送电文开始 */
			//初始化
			if(epex.Initialize(tmm009a["TC_NO"].ToString()) < 0)
			{
				strcpy(s.msg,"初始化电文[" + tmm009a["TC_NO"].ToString() + "]失败，原因[" + epex.GetMsg()+"]。");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


			////铁水倒罐 
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM12")
			{
					tmmsm12["TPD_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["TPD_NO"].ToString().Trim();
					tmmsm12.Query("TPD_NO");
					tmmsm12.TrimOrBlank();
				
				//Log::Trace("",__FUNCTION__,"tmmsm12 BEGIN 发送电文开始 tmmsm12.TPD_NO	= [{0}]",tmmsm12["TPD_NO"].ToString());	  	

					if (epex.SetValue("tmmsm12",0, tmmsm12) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if(epex.SetValue("tmmsm12","PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg,epex.GetMsg()) ;
					throw CApplicationException(-1, s.msg, log.Location); 
				}
				//生产班组号 tmmsm12.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm12", "prod_group_no", 0, tmmsm12["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//班长 tmmsm12.group_leader NAME_MONITOR
				if (epex.SetValue("tmmsm12", "group_leader", 0, tmmsm12["NAME_MONITOR"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//操作工 tmmsm12.operator ASSISTANT
				if (epex.SetValue("tmmsm12", "operator", 0, tmmsm12["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理号 tmmsm12.treat_id TPD_NO
				if (epex.SetValue("tmmsm12", "treat_id", 0, tmmsm12["TPD_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包离开时间 tmmsm12.ladle_depart_time LADLE_LEAVE_TIME
				if (epex.SetValue("tmmsm12", "ladle_depart_time", 0, tmmsm12["LADLE_LEAVE_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//铁水重量 tmmsm12.iron_wt MOLTIRON_WT
				if (epex.SetValue("tmmsm12", "iron_wt", 0, tmmsm12["MOLTIRON_WT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//倒罐处理开始时刻 tmmsm12.tpd_start_time START_TIME
				if (epex.SetValue("tmmsm12", "tpd_start_time", 0, tmmsm12["START_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//倒罐处理结束时刻tmmsm12.tpd_end_time END_TIME
				if (epex.SetValue("tmmsm12", "tpd_end_time", 0, tmmsm12["END_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				// tmmsm12.tpc_count tpc_count 倒罐TPC数
				int  tpc_count = 0;
				if (tmmsm12["IRON_NO4"].ToString()!=" "){
					tpc_count = 4;
				}
				else if (tmmsm12["IRON_NO3"].ToString() != " "){
					tpc_count = 3;
				}
				else if (tmmsm12["IRON_NO2"].ToString() != " "){
					tpc_count = 2;
				}
				else if (tmmsm12["IRON_NO1"].ToString() != " "){
					tpc_count = 1;
				}
				else{
					tpc_count = 0;
				}
				if (epex.SetValue("tmmsm12", "tpc_count", 0, tpc_count) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				// tmmsm12.tpc_no1 TPC号1 IRON_NO1
				if (epex.SetValue("tmmsm12", "tpc_no1", 0, tmmsm12["IRON_NO1"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm12.tpc_pouring_start_time1 出铁开始时刻1,tmmsm12.tpc_pouring_end_time1 出铁开始时刻1
				//查询tpc_pouring_start_time1和 tpc_pouring_end_time1
				CString tpc_pouring_start_time1 = " ";
				CString tpc_pouring_end_time1 = " ";
				cmd_inq12.SetCommandText(" select TAP_IRON_START_TIME,TAP_IRON_END_TIME from TMMSM11B where IRON_NO=@IRON_NO ");
				cmd_inq12.Parameters.Clear();
				cmd_inq12.Parameters.Set("IRON_NO", tmmsm24["IRON_NO1"].ToString());
				cmd_inq12.ExecuteReader();
				if (cmd_inq12.Read())
				{
					tpc_pouring_start_time1 = cmd_inq12.GetString(1);
					tpc_pouring_end_time1 = cmd_inq12.GetString(2);

				}
				cmd_inq12.Close();
				if (epex.SetValue("tmmsm12", "tpc_pouring_start_time1", 0, tpc_pouring_start_time1) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (epex.SetValue("tmmsm12", "tpc_pouring_end_time1", 0, tpc_pouring_end_time1) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm12.moltiron_wt1 铁水重量1
				if (epex.SetValue("tmmsm12", "moltiron_wt1", 0, tmmsm12["MOLTIRON_WT1"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				// tmmsm12.tpc_no2 TPC号2 IRON_NO2
				if (epex.SetValue("tmmsm12", "tpc_no2", 0, tmmsm12["IRON_NO2"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm12.tpc_pouring_start_time2 出铁开始时刻2,tmmsm12.tpc_pouring_end_time2 出铁开始时刻2
				//查询tpc_pouring_start_time2和 tpc_pouring_end_time2
				CString tpc_pouring_start_time2 = " ";
				CString tpc_pouring_end_time2 = " ";
				cmd_inq12.SetCommandText(" select TAP_IRON_START_TIME,TAP_IRON_END_TIME from TMMSM11B where IRON_NO=@IRON_NO ");
				cmd_inq12.Parameters.Clear();
				cmd_inq12.Parameters.Set("IRON_NO", tmmsm24["IRON_NO2"].ToString());
				cmd_inq12.ExecuteReader();
				if (cmd_inq12.Read())
				{
					tpc_pouring_start_time2 = cmd_inq12.GetString(1);
					tpc_pouring_end_time2 = cmd_inq12.GetString(2);

				}
				cmd_inq12.Close();
				if (epex.SetValue("tmmsm12", "tpc_pouring_start_time2", 0, tpc_pouring_start_time2) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (epex.SetValue("tmmsm12", "tpc_pouring_end_time2", 0, tpc_pouring_end_time2) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm12.moltiron_wt2 铁水重量2
				if (epex.SetValue("tmmsm12", "moltiron_wt2", 0, tmmsm12["MOLTIRON_WT2"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				// tmmsm12.tpc_no3 TPC号3 IRON_NO3
				if (epex.SetValue("tmmsm12", "tpc_no3", 0, tmmsm12["IRON_NO3"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm12.tpc_pouring_start_time3 出铁开始时刻3,tmmsm12.tpc_pouring_end_time3 出铁开始时刻3
				//查询tpc_pouring_start_time3和 tpc_pouring_end_time3
				CString tpc_pouring_start_time3 = " ";
				CString tpc_pouring_end_time3 = " ";
				cmd_inq12.SetCommandText(" select TAP_IRON_START_TIME,TAP_IRON_END_TIME from TMMSM11B where IRON_NO=@IRON_NO ");
				cmd_inq12.Parameters.Clear();
				cmd_inq12.Parameters.Set("IRON_NO", tmmsm24["IRON_NO2"].ToString());
				cmd_inq12.ExecuteReader();
				if (cmd_inq12.Read())
				{
					tpc_pouring_start_time3 = cmd_inq12.GetString(1);
					tpc_pouring_end_time3 = cmd_inq12.GetString(2);

				}
				cmd_inq12.Close();
				if (epex.SetValue("tmmsm12", "tpc_pouring_start_time3", 0, tpc_pouring_start_time3) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (epex.SetValue("tmmsm12", "tpc_pouring_end_time3", 0, tpc_pouring_end_time3) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm12.moltiron_wt3 铁水重量3
				if (epex.SetValue("tmmsm12", "moltiron_wt3", 0, tmmsm12["MOLTIRON_WT3"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//Log::Trace("",__FUNCTION__,"tmmsm12 END 发送电文结束 tmmsm12.TPD_NO	= [{0}]",tmmsm12["TPD_NO"].ToString());	  		  	
			}

			//IEF实绩接收
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM19")
			{
				Log::Trace("", "MMSMSND.PROC_NO", "PROC_NO", bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"].ToString());
				tmmsm19["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString();
				Log::Trace("", "tmmsm19.PROC_NO", "PROC_NO", tmmsm19["PROC_NO"].ToString());
				tmmsm19["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString();
				tmmsm19.Query("L2_PROC_NO,HEAT_NO");
				tmmsm19.TrimOrBlank();

				//Log::Trace("",__FUNCTION__,"tmmsm19 BEGIN 发送电文开始 tmmsm19.TPD_NO	= [{0}]",tmmsm19["TPD_NO"].ToString());	  	

				if (epex.SetValue("tmmsm15", 0, tmmsm19) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (epex.SetValue("bapiheader", "msgtype", 0, "210016") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}

				Log::Trace("", __FUNCTION__, "tmmsm19 BEGIN 发送电文开始 tmmsm19.TPD_NO	= [{0}]");
				if (epex.SetValue("tmmsm15", "PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理号 tmmsm15.treat_id PROC_NO
				if (epex.SetValue("tmmsm15", "treat_id", 0, tmmsm19["PROC_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//生产班组号 tmmsm15.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm15", "prod_group_no", 0, tmmsm19["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//班长 tmmsm15.group_leader NAME_MONITOR
				if (epex.SetValue("tmmsm15", "group_leader", 0, tmmsm19["NAME_MONITOR"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//操作工 tmmsm15.operator ASSISTANT
				if (epex.SetValue("tmmsm15", "operator", 0, tmmsm19["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//炉龄 tmmsm15.ief_life FURNACE_AGE
				if (epex.SetValue("tmmsm15", "ief_life", 0, tmmsm19["FURNACE_AGE"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包号 tmmsm15.steel_ladle_no LADLE_NO
				if (epex.SetValue("tmmsm15", "steel_ladle_no", 0, tmmsm19["LADLE_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				// 出钢开始时刻 tmmsm15.tapping_start_time START_TIME
				if (epex.SetValue("tmmsm15", "tapping_start_time", 0, tmmsm19["START_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//出钢结束时刻tmmsm15.tapping_end_time END_TIME
				if (epex.SetValue("tmmsm15", "tapping_end_time", 0, tmmsm19["END_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包离开时间 tmmsm15.ladle_depart_time LADLE_LEAVE_TIME
				if (epex.SetValue("tmmsm15", "ladle_depart_time", 0, tmmsm19["LADLE_LEAVE_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm19["NEXT_DEV_CODE"].ToString() != " "){
					Log::Trace("", __FUNCTION__, "tmmsm19 NEXT_DEV_CODE= [{0}]", tmmsm19["NEXT_DEV_CODE"].ToString());
					if (tmmsm19["NEXT_DEV_CODE"].ToString().GetLength()>3){
						if (epex.SetValue("tmmsm15", "next_dev_code", 0, tmmsm19["NEXT_DEV_CODE"].ToString().Substring(0, 3)) < 0)
						{
							sprintf(s.msg, epex.GetMsg());
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					else{
						if (epex.SetValue("tmmsm15", "next_dev_code", 0, tmmsm19["NEXT_DEV_CODE"].ToString()) < 0)
						{
							sprintf(s.msg, epex.GetMsg());
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
				}
				//下道工序设备号 tmmsm15.next_dev_code
				if (tmmsm19["TAPTOTAP_DURATION"].ToString() != " "){
					CDecimal taptotap_duration = tmmsm19["TAPTOTAP_DURATION"].ToDecimal() / 60;
					taptotap_duration = taptotap_duration.Round(1);
					if (epex.SetValue("tmmsm15", "taptotap_duration", 0, taptotap_duration) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				Log::Trace("", __FUNCTION__, "tmmsm15 next_dev_code= [{0}]");
				//Log::Trace("",__FUNCTION__,"tmmsm19 END 发送电文结束 tmmsm19.TPD_NO	= [{0}]",tmmsm19["TPD_NO"].ToString());	  		  	
			}

			//EAF实绩接收 电炉
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM20")
			{

				tmmsm20["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString().Trim();
				tmmsm20["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString().Trim();
				tmmsm20.Query("L2_PROC_NO");
				tmmsm20.TrimOrBlank();
				Log::Trace("", __FUNCTION__, "tmmsm20 BEGIN 发送电文开始 tmmsm19.TPD_NO	= [{0}]", tmmsm20["L2_PROC_NO"].ToString());
				//Log::Trace("",__FUNCTION__,"tmmsm19 BEGIN 发送电文开始 tmmsm19.TPD_NO	= [{0}]",tmmsm19["TPD_NO"].ToString());	  	

				if (epex.SetValue("tmmsm16", 0, tmmsm20) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//电文号
				if (epex.SetValue("bapiheader", "msgtype", 0, "210017") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}

				if (epex.SetValue("tmmsm16", "PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//生产班组号 tmmsm16.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm16", "prod_group_no", 0, tmmsm20["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理号 tmmsm16.treat_id PROC_NO
				if (epex.SetValue("tmmsm16", "treat_id", 0, tmmsm20["L2_PROC_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//班长 tmmsm16.group_leader NAME_MONITOR
				if (epex.SetValue("tmmsm16", "group_leader", 0, tmmsm20["NAME_MONITOR"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
				}
				//操作工 tmmsm16.operator ASSISTANT
				if (epex.SetValue("tmmsm16", "operator", 0, tmmsm20["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//电炉熔炼号 tmmsm16.tmmsm16.eaf_heat_no HEAT_NO
				if (epex.SetValue("tmmsm16", "eaf_heat_no", 0, tmmsm20["HEAT_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//POWER_ON_DURATION1
				if (tmmsm20["POWER_ON_DURATION1"].ToDecimal() != 0){
					CDecimal power_on_duration1 = tmmsm20["POWER_ON_DURATION1"].ToDecimal() / 60;
					power_on_duration1 = power_on_duration1.Round(0);
					if (epex.SetValue("tmmsm16", "power_on_duration1", 0, power_on_duration1) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (tmmsm20["POWER_ON_DURATION3"].ToDecimal() != 0){
					CDecimal power_on_duration3 = tmmsm20["POWER_ON_DURATION3"].ToDecimal() / 60;
					power_on_duration3 = power_on_duration3.Round(0);
					if (epex.SetValue("tmmsm16", "power_on_duration3", 0, power_on_duration3) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (tmmsm20["POWER_ON_DURATION2"].ToDecimal() != 0){
					CDecimal power_on_duration2 = tmmsm20["POWER_ON_DURATION2"].ToDecimal() / 60;
					power_on_duration2 = power_on_duration2.Round(0);
					if (epex.SetValue("tmmsm16", "power_on_duration2", 0, power_on_duration2) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (tmmsm20["NEXT_DEV_CODE"].ToString() != " "){
					//下道工序设备号 tmmsm16.next_dev_code
					if (epex.SetValue("tmmsm16", "next_dev_code", 0, tmmsm20["NEXT_DEV_CODE"].ToString().Substring(0, 3)) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				//Log::Trace("",__FUNCTION__,"tmmsm19 END 发送电文结束 tmmsm19.TPD_NO	= [{0}]",tmmsm19["TPD_NO"].ToString());	  		  	
			}

			//铁水脱硫
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM14")
			{
				
				tmmsm14["PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"].ToString().Trim();
				tmmsm14.Query("PROC_NO");
				tmmsm14.TrimOrBlank();
			
				//Log::Trace("",__FUNCTION__,"tmmsm14 BEGIN 发送电文开始 tmmsm14.TPD_NO	= [{0}]",tmmsm14["TPD_NO"].ToString());	  	

				if (epex.SetValue("tmmsm14",0, tmmsm14) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				//电文号
				if (epex.SetValue("bapiheader", "msgtype", 0, "210013") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}


				if (epex.SetValue("tmmsm14", "PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg,epex.GetMsg()) ;
					throw CApplicationException(-1, s.msg, log.Location); 
				}
				//生产班组号 tmmsm14.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm14", "prod_group_no", 0, tmmsm14["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//班长 tmmsm14.group_leader NAME_MONITOR
				if (epex.SetValue("tmmsm14", "group_leader", 0, tmmsm14["NAME_MONITOR"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//操作工 tmmsm14.operator ASSISTANT
				if (epex.SetValue("tmmsm14", "operator", 0, tmmsm14["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理号 tmmsm14.treat_id PROC_NO
				if (epex.SetValue("tmmsm14", "treat_id", 0, tmmsm14["PROC_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//脱硫处理方式 tmmsm14.de_s_proc_mode DE_S_TYPE
				if (epex.SetValue("tmmsm14", "de_s_proc_mode", 0, tmmsm14["DE_S_TYPE"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理前铁水重量 tmmsm14.iron_wt_start PREV_WT
				if (epex.SetValue("tmmsm14", "iron_wt_start", 0, tmmsm14["PREV_WT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理后铁水重量 tmmsm14.iron_wt_end FIN_WT
				if (epex.SetValue("tmmsm14", "iron_wt_end", 0, tmmsm14["FIN_WT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//生产开始时刻 tmmsm14.prod_start_time START_TIME
				if (epex.SetValue("tmmsm14", "prod_start_time", 0, tmmsm14["START_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//生产结束时刻tmmsm14.prod_end_time END_TIMEEAF实绩
				if (epex.SetValue("tmmsm14", "prod_end_time", 0, tmmsm14["END_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//下道工序设备号 tmmsm14.next_dev_code
				/*if (epex.SetValue("tmmsm14", "next_dev_code", 0, tmmsm14["NEXT_DEV_CODE"].ToString().Substring(0, 3)) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//Log::Trace("",__FUNCTION__,"tmmsm14 END 发送电文结束 tmmsm14.TPD_NO	= [{0}]",tmmsm14["TPD_NO"].ToString());	  		  	
			}

			//铁水KR实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM14B")
			{

				tmmsm14b["DES_ID"] = bcls_rec->Tables["MMSMSND"].Rows[0]["DES_ID"].ToString().Trim();
				tmmsm14b.Query("DES_ID");
				tmmsm14b.TrimOrBlank();

				Log::Trace("",__FUNCTION__,"tmmsm14b BEGIN 发送电文开始 tmmsm14b.DES_ID	= [{0}]",tmmsm14b["DES_ID"].ToString());	  	

				if (epex.SetValue("tmmsm14b", 0, tmmsm14b) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("bapiheader", "msgtype", 0, "210015") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}

				if (epex.SetValue("tmmsm14b", "PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm14b.prod_date 生产日期 DES_SUMMARY.PRODUCE_DATE
				if (epex.SetValue("tmmsm14b", "prod_date", 0, tmmsm14b["DES_END"].ToString().SubstringNE(0,8)) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm14b.dev_code 设备代码 DES_SUMMARY.DES_STATION_NO
				if (epex.SetValue("tmmsm14b", "dev_code", 0, tmmsm14b["DES_STATION_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "LINKE= [{0}]",__LINE__);
				//tmmsm14b.prod_group_no 生产班组号 DES_SUMMARY.CREW_ID
				if (epex.SetValue("tmmsm14b", "prod_group_no", 0, tmmsm14b["CREW_ID"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm14b.prod_shift_no 生产班次号 DES_SUMMARY.SHIFT_ID
				if (epex.SetValue("tmmsm14b", "prod_shift_no", 0, tmmsm14b["SHIFT_ID"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "LINKE= [{0}]", __LINE__);
				//tmmsm14b.group_leader 班长
				/*if (epex.SetValue("tmmsm14b", "", 0, tmmsm14b[""].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//tmmsm14b.operator 操作者
				/*if (epex.SetValue("tmmsm14b", "", 0, tmmsm14b[""].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/ 
				//tmmsm14b.treat_id 处理号 DES_SUMMARY.DES_ID
				if (epex.SetValue("tmmsm14b", "treat_id", 0, tmmsm14b["DES_ID"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm14b.st_no 内部钢种 DES_SUMMARY.STEEL_GRADE
				if (epex.SetValue("tmmsm14b", "st_no", 0, tmmsm14b["STEEL_GRADE"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "LINKE= [{0}]", __LINE__);
				//tmmsm14b.iron_ladle_no 铁水包号 DES_SUMMARY.IRON_LADLE_ID
				if (epex.SetValue("tmmsm14b", "iron_ladle_no", 0, tmmsm14b["IRON_LADLE_ID"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm14b.de_s_proc_mode 脱硫处理模式
				/*if (epex.SetValue("tmmsm14b", "", 0, tmmsm14b[""].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//tmmsm14b.prod_start_time 生产开始时刻 DES_SUMMARY.DES_START
				if (epex.SetValue("tmmsm14b", "prod_start_time", 0, tmmsm14b["DES_START"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				} 
				Log::Trace("", __FUNCTION__, "LINKE= [{0}]", __LINE__);
				//tmmsm14b.iron_wt_start 处理前铁水重量 DES_SUMMARY.INI_WGT
				if (epex.SetValue("tmmsm14b", "iron_wt_start", 0, tmmsm14b["INI_WGT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm14b.des_agent_name 脱硫剂型号
				/*if (epex.SetValue("tmmsm14b", "", 0, tmmsm14b[""].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//tmmsm14b.des_agent_wt_pert 脱硫剂加入量 DES_SUMMARY.ADDWGT_ACT
				/*if (epex.SetValue("tmmsm14b", "des_agent_wt_pert", 0, tmmsm14b["ADDWGT_ACT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				Log::Trace("", __FUNCTION__, "LINKE= [{0}]", __LINE__);
				//tmmsm14b.avg_rotation_rate 平均搅拌速度 DES_SUMMARY.STIRRER_SPEED_AVG
				if (epex.SetValue("tmmsm14b", "avg_rotation_rate", 0, tmmsm14b["STIRRER_SPEED_AVG"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm14b.miron_cover_agent 铁水覆盖剂
				/*if (epex.SetValue("tmmsm14b", "miron_cover_agent", 0, tmmsm14b[""].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//tmmsm14b.miron_cover_agent_amount 铁水覆盖剂加入量 DES_SUMMARY.CALEFACIENT_USED
				if (epex.SetValue("tmmsm14b", "miron_cover_agent_amount", 0, tmmsm14b["CALEFACIENT_USED"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "LINKE= [{0}]", __LINE__);
				//tmmsm14b.miron_gs_agent 铁水扒渣剂
				/*if (epex.SetValue("tmmsm14b", "", 0, tmmsm14b[""].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//tmmsm14b.miron_gs_agent_amount 铁水扒渣剂加入量
				/*if (epex.SetValue("tmmsm14b", "", 0, tmmsm14b[""].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//tmmsm14b.iron_wt_end 处理后铁水重量  DES_SUMMARY.FIN_WGT
				if (epex.SetValue("tmmsm14b", "iron_wt_end", 0, tmmsm14b["FIN_WGT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm14b.gs_naked_square 扒渣后裸露面积
				/*if (epex.SetValue("tmmsm14b", "", 0, tmmsm14b[""].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//tmmsm14b.prod_end_time 生产结束时刻 DES_SUMMARY.DES_END
				if (epex.SetValue("tmmsm14b", "prod_end_time", 0, tmmsm14b["DES_END"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "LINKE= [{0}]", __LINE__);
				//tmmsm14b.next_dev_code 下道设备号
				/*if (epex.SetValue("tmmsm14b", "", 0, tmmsm14b[""].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				// tmmsm14b.remark 备注
				/*if (epex.SetValue("tmmsm14b", "", 0, tmmsm14b[""].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
			}

		

			//转炉实绩 【二炼北】BOF实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM21")
			{
				tmmsm21["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString();
				tmmsm21["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString();
				tmmsm21.Query("L2_PROC_NO,HEAT_NO");
				tmmsm21.TrimOrBlank();
			
				//Log::Trace("",__FUNCTION__,"tmmsm21 BEGIN 发送电文开始 tmmsm21.HEAT_NO	= [{0}]",tmmsm21["HEAT_NO"].ToString());	  	

				if (epex.SetValue("tmmsm21",0, tmmsm21) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}

				//bapiheader.msgtype

				if (epex.SetValue("bapiheader", "msgtype", 0, "210018") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if(epex.SetValue("tmmsm21","PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg,epex.GetMsg()) ;
					throw CApplicationException(-1, s.msg, log.Location); 
				}
				//生产班组号 tmmsm21.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm21", "prod_group_no", 0, tmmsm21["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理号 tmmsm21.treat_id PROC_NO
				if (epex.SetValue("tmmsm21", "treat_id", 0, tmmsm21["PROC_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//班长 tmmsm21.group_leader NAME_MONITOR
				if (epex.SetValue("tmmsm21", "group_leader", 0, tmmsm21["NAME_MONITOR"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
				}
				//操作工 tmmsm21.operator ASSISTANT
				if (epex.SetValue("tmmsm21", "operator", 0, tmmsm21["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包号 tmmsm21.steel_ladle_no LADLE_NO
				if (epex.SetValue("tmmsm21", "steel_ladle_no", 0, tmmsm21["LADLE_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//铁水重 tmmsm21.iron_wt MOLTIRON_WT
				if (epex.SetValue("tmmsm21", "iron_wt", 0, tmmsm21["MOLTIRON_WT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//回炉钢水重量 tmmsm21.return_steel_wt RET_STEEL_WT
				if (epex.SetValue("tmmsm21", "return_steel_wt", 0, tmmsm21["RET_STEEL_WT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//氧气消耗量 tmmsm21.total_o2_cons OXYGEN_FINAL
				/*if (epex.SetValue("tmmsm21", "total_o2_cons", 0, tmmsm21["O2_SUM_COMSUME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//包龄 tmmsm21.ladle_life LADLE_AGE
				if (epex.SetValue("tmmsm21", "ladle_life", 0, tmmsm21["LADLE_AGE"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包离开时间 tmmsm21.ladle_depart_time LADLE_LEAVE_TIME
				if (epex.SetValue("tmmsm21", "ladle_depart_time", 0, tmmsm21["LADLE_LEAVE_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm21["NEXT_DEV_CODE"].ToString() != " "){
					//下道工序设备号 tmmsm21.next_dev_code
					if (epex.SetValue("tmmsm21", "next_dev_code", 0, tmmsm21["NEXT_DEV_CODE"].ToString().Substring(0, 3)) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				//Log::Trace("",__FUNCTION__,"tmmsm21 END 发送电文结束tmmsm21.HEAT_NO	= [{0}]",tmmsm21["HEAT_NO"].ToString());	    		  	
			}

			//AOD实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM27")
			{
				tmmsm27["PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"].ToString().Trim();
				tmmsm27["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString().Trim();
				tmmsm27["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString().Trim();
				tmmsm27.Query("L2_PROC_NO,HEAT_NO");
				tmmsm27.TrimOrBlank();

				//Log::Trace("",__FUNCTION__,"tmmsm22 BEGIN 发送电文开始 tmmsm22.PROC_NO	= [{0}]",tmmsm22["PROC_NO"].ToString());	  	

				if (epex.SetValue("tmmsm22", 0, tmmsm27) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("bapiheader", "msgtype", 0, "210020") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}


				if (epex.SetValue("tmmsm22", "PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//生产班组号 tmmsm22.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm22", "prod_group_no", 0, tmmsm27["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//操作工 tmmsm22.operator ASSISTANT
				if (epex.SetValue("tmmsm22", "operator", 0, tmmsm27["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包号 tmmsm22.steel_ladle_no LADLE_NO
				if (epex.SetValue("tmmsm22", "steel_ladle_no", 0, tmmsm27["LADLE_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//包龄 tmmsm27.ladle_life LADLE_AGE
				if (epex.SetValue("tmmsm22", "ladle_life", 0, tmmsm27["LADLE_AGE"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//出钢开始时刻 tmmsm22.tapping_start_time TAP_START_TIME
				if (epex.SetValue("tmmsm22", "tapping_start_time", 0, tmmsm27["TAP_START_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
				}
				//出钢结束时刻 tmmsm22.tapping_end_time TAP_END_TIME
				if (epex.SetValue("tmmsm22", "tapping_end_time", 0, tmmsm27["TAP_END_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
				}
				//出钢温度 tmmsm22.tapping_temp OUT_STEEL_TEMP
				if (epex.SetValue("tmmsm22", "tapping_temp", 0, tmmsm27["OUT_STEEL_TEMP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
				}
				//钢包离开时间 tmmsm22.ladle_depart_time LADLE_LEAVE_TIME
				if (epex.SetValue("tmmsm22", "ladle_depart_time", 0, tmmsm27["LADLE_LEAVE_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//下道工序设备号 tmmsm22.next_dev_code
				if (tmmsm27["NEXT_DEV_CODE"].ToString() != " "){
					if (epex.SetValue("tmmsm22", "next_dev_code", 0, tmmsm27["NEXT_DEV_CODE"].ToString().Substring(0, 3)) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				//Log::Trace("",__FUNCTION__,"tmmsm22 END 发送电文结束tmmsm22.PROC_NO	= [{0}]",tmmsm22["PROC_NO"].ToString());	    		  	
			}


			//吹AR实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM22")
			{
				tmmsm22["PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"].ToString().Trim();
				tmmsm22["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString().Trim();
				tmmsm22.Query("HEAT_NO,PROC_NO");
				tmmsm22.TrimOrBlank();
				
				//Log::Trace("",__FUNCTION__,"tmmsm22 BEGIN 发送电文开始 tmmsm22.PROC_NO	= [{0}]",tmmsm22["PROC_NO"].ToString());	  	

				if (epex.SetValue("tmmsm22",0, tmmsm22) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if(epex.SetValue("tmmsm22","PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg,epex.GetMsg()) ;
					throw CApplicationException(-1, s.msg, log.Location); 
				}
				//生产班组号 tmmsm21.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm22", "prod_group_no", 0, tmmsm22["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//操作工 tmmsm22.operator ASSISTANT
				if (epex.SetValue("tmmsm22", "operator", 0, tmmsm22["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//Log::Trace("",__FUNCTION__,"tmmsm22 END 发送电文结束tmmsm22.PROC_NO	= [{0}]",tmmsm22["PROC_NO"].ToString());	    		  	
			}

			//LTS实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM26")
			{
				tmmsm26["PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"].ToString().Trim();
				tmmsm26["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString().Trim();
				tmmsm26["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString().Trim();
				tmmsm26.Query("HEAT_NO,PROC_NO");
				tmmsm26.TrimOrBlank();

				//Log::Trace("",__FUNCTION__,"tmmsm26 BEGIN 发送电文开始 tmmsm26.PROC_NO	= [{0}]",tmmsm26["PROC_NO"].ToString());	  	

				if (epex.SetValue("tmmsm26", 0, tmmsm26) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//电文号
				if (epex.SetValue("bapiheader", "msgtype", 0, "210025") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}
				if (epex.SetValue("tmmsm26", "PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//生产班组号 tmmsm26.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm26", "prod_group_no", 0, tmmsm26["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理号 tmmsm26.treat_id PROC_NO
				if (epex.SetValue("tmmsm26", "treat_id", 0, tmmsm26["PROC_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//班长 tmmsm26.group_leader NAME_MONITOR
				if (epex.SetValue("tmmsm26", "group_leader", 0, tmmsm26["NAME_MONITOR"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
				}
				//操作工 tmmsm26.operator ASSISTANT
				if (epex.SetValue("tmmsm26", "operator", 0, tmmsm26["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包号 tmmsm26.steel_ladle_no LADLE_NO
				if (epex.SetValue("tmmsm26", "steel_ladle_no", 0, tmmsm26["LADLE_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//包龄 tmmsm26.ladle_life LADLE_AGE
				if (epex.SetValue("tmmsm26", "ladle_life", 0, tmmsm26["LADLE_AGE"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//软吹时间 tmmsm26.soft_blow_duration SOFT_BLOWING_DURATION
				/*if (epex.SetValue("tmmsm26", "soft_blow_duration", 0, tmmsm26["SOFT_BLOWING_DURATION"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//钢包离开时间 tmmsm26.ladle_depart_time LADLE_LEAVE_TIME
				if (epex.SetValue("tmmsm26", "ladle_depart_time", 0, tmmsm26["LADLE_LEAVE_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "查询电文号 ladle_depart_time.TC_NO = [{0}]", tmmsm26["LADLE_LEAVE_TIME"].ToString());
				//下道工序设备号 tmmsm26.next_dev_code
				if (tmmsm26["NEXT_DEV_CODE"].ToString() != " "){
					if (epex.SetValue("tmmsm26", "next_dev_code", 0, tmmsm26["NEXT_DEV_CODE"].ToString().Substring(0, 3)) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				//Log::Trace("",__FUNCTION__,"tmmsm23 END 发送电文结束tmmsm23.PROC_NO	= [{0}]",tmmsm23["PROC_NO"].ToString());	    		  	
			}

			//RH实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM23")
			{
				tmmsm23["PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"].ToString().Trim();
				tmmsm23["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString().Trim();
				tmmsm23["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString().Trim();
				tmmsm23.Query("HEAT_NO,L2_PROC_NO");
				tmmsm23.TrimOrBlank();
				
				//Log::Trace("",__FUNCTION__,"tmmsm23 BEGIN 发送电文开始 tmmsm23.PROC_NO	= [{0}]",tmmsm23["PROC_NO"].ToString());	  	

				if (epex.SetValue("tmmsm23",0, tmmsm23) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}

				if (epex.SetValue("bapiheader", "msgtype", 0, "210024") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}


				if (epex.SetValue("tmmsm23", "PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg,epex.GetMsg()) ;
					throw CApplicationException(-1, s.msg, log.Location); 
				}
				//生产班组号 tmmsm23.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm23", "prod_group_no", 0, tmmsm23["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理号 tmmsm23.treat_id PROC_NO
				if (epex.SetValue("tmmsm23", "treat_id", 0, tmmsm23["PROC_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//班长 tmmsm23.group_leader NAME_MONITOR
				if (epex.SetValue("tmmsm23", "group_leader", 0, tmmsm23["NAME_MONITOR"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
				}
				//操作工 tmmsm23.operator ASSISTANT
				if (epex.SetValue("tmmsm23", "operator", 0, tmmsm23["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//软吹时间 tmmsm23.soft_blow_duration SOFT_BLOWING_DURATION
				if (epex.SetValue("tmmsm23", "soft_blow_duration", 0, tmmsm23["SOFT_BLOWING_DURATION"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包离开时间 tmmsm23.ladle_depart_time LADLE_LEAVE_TIME
				if (epex.SetValue("tmmsm23", "ladle_depart_time", 0, tmmsm23["LADLE_LEAVE_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm23["NEXT_DEV_CODE"].ToString()!=" "){
					//下道工序设备号 tmmsm23.next_dev_code
					if (epex.SetValue("tmmsm23", "next_dev_code", 0, tmmsm23["NEXT_DEV_CODE"].ToString().Substring(0, 3)) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				//Log::Trace("",__FUNCTION__,"tmmsm23 END 发送电文结束tmmsm23.PROC_NO	= [{0}]",tmmsm23["PROC_NO"].ToString());	    		  	
			}

			//不锈LF实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM24A")
			{
				tmmsm24["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString();
				tmmsm24["PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"].ToString();
				tmmsm24["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString();
				tmmsm24.Query("HEAT_NO,L2_PROC_NO");
				tmmsm24.TrimOrBlank();
				
				//Log::Trace("",__FUNCTION__,"tmmsm24 BEGIN 发送电文开始 tmmsm24.PROC_NO	= [{0}]",tmmsm24["PROC_NO"].ToString());	  	

				if (epex.SetValue("tmmsm24", 0, tmmsm24) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//电文号
				if (epex.SetValue("bapiheader", "msgtype", 0, "210021") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}

				if (epex.SetValue("tmmsm24", "PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg,epex.GetMsg()) ;
					throw CApplicationException(-1, s.msg, log.Location); 
				}

				//生产班组号 tmmsm24.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm24", "prod_group_no", 0, tmmsm24["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//操作工 tmmsm24.operator ASSISTANT
				if (epex.SetValue("tmmsm24", "operator", 0, tmmsm24["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理号 tmmsm24.treat_id PROC_NO
				if (epex.SetValue("tmmsm24", "treat_id", 0, tmmsm24["PROC_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//班长 tmmsm24.group_leader NAME_MONITOR
				if (epex.SetValue("tmmsm24", "group_leader", 0, tmmsm24["NAME_MONITOR"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
				}
				//钢包号 tmmsm24.steel_ladle_no LADLE_NO
				if (epex.SetValue("tmmsm24", "steel_ladle_no", 0, tmmsm24["LADLE_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//包龄 tmmsm24.ladle_life LADLE_AGE
				if (epex.SetValue("tmmsm24", "ladle_life", 0, tmmsm24["LADLE_AGE"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//软吹时间 tmmsm24.soft_blow_duration SOFT_BLOWING_DURATION
				if (epex.SetValue("tmmsm24", "soft_blow_duration", 0, tmmsm24["SOFT_BLOWING_DURATION"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包离开时间 tmmsm24.ladle_depart_time LADLE_LEAVE_TIME
				if (epex.SetValue("tmmsm24", "ladle_depart_time", 0, tmmsm24["LADLE_LEAVE_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//下道工序设备号 tmmsm24.next_dev_code
				if (tmmsm24["NEXT_DEV_CODE"].ToString() != " "){
					if (epex.SetValue("tmmsm24", "next_dev_code", 0, tmmsm24["NEXT_DEV_CODE"].ToString().Substring(0, 3)) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				//Log::Trace("",__FUNCTION__,"tmmsm24 END 发送电文结束tmmsm24.PROC_NO	= [{0}]",tmmsm24["PROC_NO"].ToString());	    		  	
			}

			//钛钢LF实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM24B")
			{
				tmmsm24["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString();
				tmmsm24["PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"].ToString();
				tmmsm24["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString();
				tmmsm24.Query("HEAT_NO,L2_PROC_NO");
				tmmsm24.TrimOrBlank();
				
				//Log::Trace("",__FUNCTION__,"tmmsm24 BEGIN 发送电文开始 tmmsm24.PROC_NO	= [{0}]",tmmsm24["PROC_NO"].ToString());	  	

				if (epex.SetValue("tmmsm24a", 0, tmmsm24) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//电文号
				if (epex.SetValue("bapiheader", "msgtype", 0, "210022") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}

				if (epex.SetValue("tmmsm24a", "PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg,epex.GetMsg()) ;
					throw CApplicationException(-1, s.msg, log.Location); 
				}

				//生产班组号 tmmsm24.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm24a", "prod_group_no", 0, tmmsm24["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理号 tmmsm24.treat_id PROC_NO
				if (epex.SetValue("tmmsm24a", "treat_id", 0, tmmsm24["PROC_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//班长 tmmsm24.group_leader NAME_MONITOR
				if (epex.SetValue("tmmsm24a", "group_leader", 0, tmmsm24["NAME_MONITOR"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
				}
				//操作工 tmmsm24a.operator ASSISTANT
				if (epex.SetValue("tmmsm24a", "operator", 0, tmmsm24["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包号 tmmsm24a.steel_ladle_no LADLE_NO
				if (epex.SetValue("tmmsm24a", "steel_ladle_no", 0, tmmsm24["LADLE_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//包龄 tmmsm24a.ladle_life LADLE_AGE
				if (epex.SetValue("tmmsm24a", "ladle_life", 0, tmmsm24["LADLE_AGE"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//软吹时间 tmmsm24a.soft_blow_duration SOFT_BLOWING_DURATION
				if (epex.SetValue("tmmsm24a", "soft_blow_duration", 0, tmmsm24["SOFT_BLOWING_DURATION"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包离开时间 tmmsm24a.ladle_depart_time LADLE_LEAVE_TIME
				if (epex.SetValue("tmmsm24a", "ladle_depart_time", 0, tmmsm24["LADLE_LEAVE_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//下道工序设备号 tmmsm24a.next_dev_code
				if (tmmsm24["NEXT_DEV_CODE"].ToString() != " "){
					if (epex.SetValue("tmmsm24a", "next_dev_code", 0, tmmsm24["NEXT_DEV_CODE"].ToString().Substring(0, 3)) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				//Log::Trace("",__FUNCTION__,"tmmsm24 END 发送电文结束tmmsm24.PROC_NO	= [{0}]",tmmsm24["PROC_NO"].ToString());	    		  	
			}


			//VOD实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM25")
			{
				tmmsm25["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString().Trim();
				tmmsm25["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString().Trim();
				//tmmsm25["PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"].ToString().Trim();
				tmmsm25.Query("HEAT_NO,L2_PROC_NO");
				tmmsm25.TrimOrBlank();

				//Log::Trace("",__FUNCTION__,"tmmsm24 BEGIN 发送电文开始 tmmsm24.PROC_NO	= [{0}]",tmmsm24["PROC_NO"].ToString());	  	

				if (epex.SetValue("tmmsm25", 0, tmmsm25) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//电文号
				if (epex.SetValue("bapiheader", "msgtype", 0, "210023") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}

				if (epex.SetValue("tmmsm25", "PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//生产班组号 tmmsm24.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm25", "prod_group_no", 0, tmmsm25["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理号 tmmsm25.treat_id PROC_NO
				if (epex.SetValue("tmmsm25", "treat_id", 0, tmmsm25["PROC_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//班长 tmmsm25.group_leader NAME_MONITOR
				if (epex.SetValue("tmmsm25", "group_leader", 0, tmmsm25["NAME_MONITOR"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
				}
				//操作工 tmmsm25.operator ASSISTANT
				if (epex.SetValue("tmmsm25", "operator", 0, tmmsm25["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包号 tmmsm25.steel_ladle_no LADLE_NO
				if (epex.SetValue("tmmsm25", "steel_ladle_no", 0, tmmsm25["LADLE_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//包龄 tmmsm25.ladle_life LADLE_AGE
				if (epex.SetValue("tmmsm25", "ladle_life", 0, tmmsm25["LADLE_AGE"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包离开时间 tmmsm25.ladle_depart_time LADLE_LEAVE_TIME
				if (epex.SetValue("tmmsm25", "ladle_depart_time", 0, tmmsm25["LADLE_LEAVE_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//下道工序设备号 tmmsm25.next_dev_code
				if (tmmsm25["NEXT_DEV_CODE"].ToString() != " "){
					if (epex.SetValue("tmmsm25", "next_dev_code", 0, tmmsm25["NEXT_DEV_CODE"].ToString().Substring(0, 3)) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				//Log::Trace("",__FUNCTION__,"tmmsm24 END 发送电文结束tmmsm24.PROC_NO	= [{0}]",tmmsm24["PROC_NO"].ToString());	    		  	
			}

			//【二炼北】转炉精炼实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM21S")
			{
				tmmsmgy06["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString().Trim();
				tmmsmgy06["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString().Trim();
				tmmsmgy06["DEV_CODE"] = bcls_rec->Tables["MMSMSND"].Rows[0]["DEV_CODE"].ToString().Trim();
				tmmsmgy06.Query("HEAT_NO,L2_PROC_NO,DEV_CODE");
				tmmsmgy06.TrimOrBlank();
				CString xf_min = " ";
				if (bcls_rec->Tables["MMSMSND"].Columns.Contains("XF_MIN")){
					xf_min = bcls_rec->Tables["MMSMSND"].Rows[0]["XF_MIN"].ToString();
				}
				Log::Trace("", "xf_min", "xf_min = {0}]", xf_min);
				if (epex.SetValue("ZCHO_TMMSMCT", 0, tmmsmgy06) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//电文号
				if (epex.SetValue("bapiheader", "msgtype", 0, "210049") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}
				//INPUT.T_NAME
				//if (epex.SetValue("INPUT", "T_NAME", 0, " ") < 0)
				//{
				//sprintf(s.msg, epex.GetMsg());
				//throw CApplicationException(-1, s.msg, log.Location);
				//}

				////OUTPUT.T_NAME
				//if (epex.SetValue("OUTPUT", "T_NAME", 0, " ") < 0)
				//{
				//	sprintf(s.msg, epex.GetMsg());
				//	throw CApplicationException(-1, s.msg, log.Location);
				//}

				//ZCHO_TMMSMCT.REMARK_3
				/*if (epex.SetValue("ZCHO_TMMSMCT", "PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/

				//生产日期 ZCHO_TMMSMCT.PROD_DATE
				if (epex.SetValue("ZCHO_TMMSMCT", "PROD_DATE", 0, tmmsmgy06["PROD_DATE"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//AP抛AI日期 ZCHO_TMMSMCT.APP_THROW_AI_DATE
				/*if (epex.SetValue("ZCHO_TMMSMCT", "", 0, tmmsm21[""].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//机组代码（工位） ZCHO_TMMSMCT.PROC_UNIT
				if (epex.SetValue("ZCHO_TMMSMCT", "PROC_UNIT", 0, tmmsmgy06["DEV_CODE"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
				}
				//炉次开始时刻 ZCHO_TMMSMCT.START_OF_HEAT
				if (epex.SetValue("ZCHO_TMMSMCT", "START_OF_HEAT", 0, tmmsmgy06["START_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//炉次结束时刻 ZCHO_TMMSMCT.END_OF_HEAT
				if (epex.SetValue("ZCHO_TMMSMCT", "END_OF_HEAT", 0, tmmsmgy06["END_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (xf_min =="1"){
					CDecimal duration_time = 0;
					duration_time = tmmsmgy06["DURATION_TIME"].ToDecimal() - tmmsmgy06["DURATION_TIME"].ToDecimal() - tmmsmgy06["DURATION_TIME"].ToDecimal();
					Log::Trace("", "duration_time", "duration_time = {0}]", duration_time);
					//持续时间1 ZCHO_TMMSMCT.DURATION_TIME
					if (epex.SetValue("ZCHO_TMMSMCT", "DURATION_TIME", 0, duration_time) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}else{
					if (tmmsmgy06["TOGETHER_TIME"].ToDecimal() != 0){
						CDecimal together_time = 0;
						together_time = tmmsmgy06["TOGETHER_TIME"].ToDecimal() - tmmsmgy06["TOGETHER_TIME"].ToDecimal() - tmmsmgy06["TOGETHER_TIME"].ToDecimal();
						//持续时间1 ZCHO_TMMSMCT.DURATION_TIME
						if (epex.SetValue("ZCHO_TMMSMCT", "DURATION_TIME", 0, together_time) < 0)
						{
							sprintf(s.msg, epex.GetMsg());
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					else{
						if (epex.SetValue("ZCHO_TMMSMCT", "DURATION_TIME", 0, tmmsmgy06["DURATION_TIME"].ToString()) < 0)
						{
							sprintf(s.msg, epex.GetMsg());
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
				}
				Log::Trace("",__FUNCTION__,"tmmsm21 END 发送电文结束tmmsm21.PROC_NO	= [{0}]",tmmsm21["HEAT_NO"].ToString());

			}

			

			//连铸实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM31")
			{
				tmmsm31["PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"].ToString().Trim();
				tmmsm31["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString().Trim();
				tmmsm31["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString().Trim();
				tmmsm31.Query("HEAT_NO,L2_PROC_NO");
				tmmsm31.TrimOrBlank();
				
				//Log::Trace("",__FUNCTION__,"tmmsm31 BEGIN 发送电文开始 tmmsm31.HEAT_NO	= [{0}]",tmmsm31["HEAT_NO"].ToString());	  	

				if (epex.SetValue("tmmsm31",0, tmmsm31) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//电文号
				if (epex.SetValue("bapiheader", "msgtype", 0, "210026") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}

				if (epex.SetValue("tmmsm31", "PROC_DIV", 0, v_proc_div) < 0)
				{
					sprintf(s.msg,epex.GetMsg()) ;
					throw CApplicationException(-1, s.msg, log.Location); 
				}
				//生产班组号 tmmsm31.prod_group_no PROD_SHIFT_GROUP
				if (epex.SetValue("tmmsm31", "prod_group_no", 0, tmmsm31["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理号 tmmsm31.treat_id PROC_NO
				if (epex.SetValue("tmmsm31", "treat_id", 0, tmmsm31["PROC_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//班长 tmmsm31.group_leader NAME_MONITOR
				if (epex.SetValue("tmmsm31", "group_leader", 0, tmmsm31["NAME_MONITOR"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
				}
				//操作工 tmmsm31.operator ASSISTANT
				if (epex.SetValue("tmmsm31", "operator", 0, tmmsm31["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//钢包号 tmmsm31.steel_ladle_no LADLE_NO
				if (epex.SetValue("tmmsm31", "steel_ladle_no", 0, tmmsm31["LADLE_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				// 中包覆盖剂1厂家 tmmsm31.td_powder_supplier1 TD_COVER_MAKER
				if (epex.SetValue("tmmsm31", "td_powder_supplier1", 0, tmmsm31["TD_COVER_MAKER"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//中包覆盖剂1厂家 tmmsm31.td_powder_supplier2 TD_COVER_MAKER1
				if (epex.SetValue("tmmsm31", "td_powder_supplier2", 0, tmmsm31["TD_COVER_MAKER1"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//等钢时间 tmmsm31.wait_for_steel_duration WAIT_STEEL_DURATION
				if (epex.SetValue("tmmsm31", "wait_for_steel_duration", 0, tmmsm31["WAIT_STEEL_DURATION"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm31.td_no
				if (epex.SetValue("tmmsm31", "td_no", 0, tmmsm31["TD_NO_1"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm31.heat_seq_td
				CString heat_seq_td = " ";
				cmd_inq.SetCommandText(" select ID from (select row_number() over (partition by t.CAST_DIV_NO,t.TD_NO_1 order by LADLE_LEAVE_TIME) ID ,t.* from TMMSM31 t "
					" ) where HEAT_NO = '" + tmmsm31["HEAT_NO"].ToString() + "' AND DEV_CODE = '" + tmmsm31["DEV_CODE"].ToString() + "' ");
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read()){
					heat_seq_td = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
				if (epex.SetValue("tmmsm31", "heat_seq_td", 0, heat_seq_td) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm31.td_begin_time
				CString td_begin_time = " ";
				cmd_inq31.SetCommandText(" SELECT ON_LINE_TIME FROM TTMSM22 where TD_NO='" + tmmsm31["TD_NO_1"].ToString() + "' and CC_MACH_NO='" + tmmsm31["DEV_CODE"].ToString() + "' ");
				cmd_inq31.ExecuteReader();
				if (cmd_inq31.Read()){
					td_begin_time = cmd_inq31.GetString(1);
				}
				cmd_inq31.Close();
				if (epex.SetValue("tmmsm31", "td_begin_time", 0, td_begin_time) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//Log::Trace("",__FUNCTION__,"tmmsm31 END 发送电文结束tmmsm31.HEAT_NO	= [{0}]",tmmsm31["HEAT_NO"].ToString());	    		  	
			}

			//连铸流实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM31A")
			{

				tmmsm31a["STRAND_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["STRAND_NO"].ToString().Trim();
				tmmsm31a["REC_CREATE_TIME"] = bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATE_TIME"].ToString().Trim();
				tmmsm31a.Query("STRAND_NO,REC_CREATE_TIME");
				tmmsm31a.TrimOrBlank();

				Log::Trace("",__FUNCTION__,"tmmsm31a BEGIN 发送电文开始 tmmsm31a.STRAND_NO	= [{0}]",tmmsm31a["STRAND_NO"].ToString());	  	

				if (epex.SetValue("tmmsm31s", 0, tmmsm31) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//电文号
				if (epex.SetValue("bapiheader", "msgtype", 0, "210027") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}
				//tmmsm31s.strand_no
				if (epex.SetValue("tmmsm31s", "strand_no", 0, tmmsm31a["STRAND_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//生产班组号 tmmsm31s.prod_group_no PROD_SHIFT_GROUP
				/*if (epex.SetValue("tmmsm31s", "prod_group_no", 0, tmmsm31a["PROD_SHIFT_GROUP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//操作工 tmmsm31s.operator ASSISTANT
				if (epex.SetValue("tmmsm31s", "operator", 0, tmmsm31a["ASSISTANT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//Log::Trace("",__FUNCTION__,"tmmsm31a END 发送电文结束tmmsm31a.HEAT_NO	= [{0}]",tmmsm31a["HEAT_NO"].ToString());	    		  	
			}

			//测温信息
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM2B")
			{
				Log::Trace("", __FUNCTION__, "tmmsm31a BEGIN 发送电文开始 tmmsm31a.HEAT_NO	= [{0}],PROC_COUNT=[{1}]");
				tmmsm2b["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString();
				Log::Trace("", __FUNCTION__, "tmmsm31a BEGIN 发送电文开始 tmmsm31a.HEAT_NO	= [{0}]", bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString());
				tmmsm2b["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString();
				tmmsm2b["PROC_COUNT"] = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_COUNT"].ToString();
				Log::Trace("", __FUNCTION__, " BEGIN 发送电文开始 PROC_COUNT	= [{0}]", bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_COUNT"].ToString());
				tmmsm2b.Query("HEAT_NO,PROC_COUNT,L2_PROC_NO");
				tmmsm2b.TrimOrBlank();

				Log::Trace("", __FUNCTION__, "tmmsm31a BEGIN 发送电文开始 tmmsm31a.HEAT_NO	= [{0}],PROC_COUNT=[{1}]", tmmsm2b["HEAT_NO"].ToString(), tmmsm2b["PROC_COUNT"].ToString());
				//tmmsm51.heat_no
				if (epex.SetValue("tmmsm51", 0, tmmsm2b) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//电文号
				if (epex.SetValue("bapiheader", "msgtype", 0, "210031") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_proc_div == "U" || v_proc_div == "I"){
					v_proc_div = "1";
				}
				else if (v_proc_div == "D"){
					v_proc_div = "2";
				}
				//温度 tmmsm51.temperature
				if (epex.SetValue("tmmsm51", "temperature", 0, tmmsm2b["STEEL_TEMP"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//处理号
				if (epex.SetValue("tmmsm51", "treat_id", 0, tmmsm2b["PROC_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm51.seq_no
				if (epex.SetValue("tmmsm51", "seq_no", 0, tmmsm2b["PROC_COUNT"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//tmmsm51.temp_measure_time
				if (epex.SetValue("tmmsm51", "temp_measure_time", 0, tmmsm2b["REC_CREATE_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//Log::Trace("",__FUNCTION__,"tmmsm31a END 发送电文结束tmmsm31a.HEAT_NO	= [{0}]",tmmsm31a["HEAT_NO"].ToString());	    		  	
			}

			//加料信息
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM2A")
			{
				tmmsm2a["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"].ToString().Trim();
				/*tmmsm2a["L2_PROC_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"].ToString().Trim();
				tmmsm2a["MAT_CODE"] = bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_CODE"].ToString().Trim();
				tmmsm2a["PROD_SEQ_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["PROD_SEQ_NO"].ToString().Trim();*/
				//tmmsm2a.MergeFrom(bcls_rec->Tables["MMSMSND"].Rows[0]);
				cmd_inq.SetCommandText(" select * from TMMSM2A_YL where HEAT_NO=@HEAT_NO ");
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("HEAT_NO", tmmsm2a["HEAT_NO"].ToString());
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm2a);
					tmmsm2a.TrimOrBlank();
					CString al = " ";
					CString c = " ";
					CString ca = " ";
					CString cr = " ";
					CString cu = " ";
					CString fe = " ";
					CString mn = " ";
					CString ni = " ";
					CString p = " ";
					CString pb = " ";
					CString s1 = " ";
					CString si = " ";
					CString ti = " ";
					CString v = " ";
					if (tmmsm2a["QUALITY_BATCH_NO"].ToString().Trim() != ""){
						cmd_inq12.SetCommandText(" SELECT MAT_CODE, "
							" SUM(DECODE(ELM_NAME, 'Al', ELM_VALUE, 0)) Al, "
							" SUM(DECODE(ELM_NAME, 'C', ELM_VALUE, 0))  C, "
							" SUM(DECODE(ELM_NAME, 'Ca', ELM_VALUE, 0)) Ca, "
							" SUM(DECODE(ELM_NAME, 'Cr', ELM_VALUE, 0)) Cr, "
							" SUM(DECODE(ELM_NAME, 'Cu', ELM_VALUE, 0)) Cu, "
							" SUM(DECODE(ELM_NAME, 'Fe', ELM_VALUE, 0)) Fe, "
							" SUM(DECODE(ELM_NAME, 'Mn', ELM_VALUE, 0)) Mn, "
							" SUM(DECODE(ELM_NAME, 'Ni', ELM_VALUE, 0)) Ni, "
							" SUM(DECODE(ELM_NAME, 'P', ELM_VALUE, 0))  P, "
							" SUM(DECODE(ELM_NAME, 'Pb', ELM_VALUE, 0)) Pb, "
							" SUM(DECODE(ELM_NAME, 'S', ELM_VALUE, 0))  S, "
							" SUM(DECODE(ELM_NAME, 'Si', ELM_VALUE, 0)) Si, "
							" SUM(DECODE(ELM_NAME, 'Ti', ELM_VALUE, 0)) Ti, "
							" SUM(DECODE(ELM_NAME, 'V', ELM_VALUE, 0))  V "
							" FROM TMMSM81AL "
							" where QUALITY_BATCH_NO = @QUALITY_BATCH_NO "
							" and MAT_CODE = @MAT_CODE "
							" GROUP BY MAT_CODE "
							" ");
						cmd_inq12.Parameters.Clear();
						cmd_inq12.Parameters.Set("QUALITY_BATCH_NO", tmmsm2a["QUALITY_BATCH_NO"].ToString());
						cmd_inq12.Parameters.Set("MAT_CODE", tmmsm2a["MAT_CODE"].ToString());
						cmd_inq12.ExecuteReader();
						if (cmd_inq12.Read())
						{
							al = cmd_inq12.GetString(2);
							c = cmd_inq12.GetString(3);
							ca = cmd_inq12.GetString(4);
							cr = cmd_inq12.GetString(5);
							cu = cmd_inq12.GetString(6);
							fe = cmd_inq12.GetString(7);
							mn = cmd_inq12.GetString(8);
							ni = cmd_inq12.GetString(9);
							p = cmd_inq12.GetString(10);
							pb = cmd_inq12.GetString(11);
							s1 = cmd_inq12.GetString(12);
							si = cmd_inq12.GetString(13);
							ti = cmd_inq12.GetString(14);
							v = cmd_inq12.GetString(15);
						}

					}
					if (epex.SetValue("tmmsm52", 0, tmmsm2a) < 0)
					{
						sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					//电文号
					if (epex.SetValue("bapiheader", "msgtype", 0, "210032") < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					Log::Trace("", __FUNCTION__, "111");
					if (v_proc_div == "U" || v_proc_div == "I"){
						v_proc_div = "1";
					}
					else if (v_proc_div == "D"){
						v_proc_div = "2";
					}
					Log::Trace("", __FUNCTION__, "222");
					/*if(epex.SetValue("PROC_DIV", 0, v_proc_div) < 0)
					{
					sprintf(s.msg,epex.GetMsg()) ;
					throw CApplicationException(-1, s.msg, log.Location);
					}*/
					//处理号
					if (epex.SetValue("tmmsm52", "treat_id", 0, tmmsm2a["L2_PROC_NO"].ToString()) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					Log::Trace("", __FUNCTION__, "333");
					//tmmsm52.heat_no
					if (epex.SetValue("tmmsm52", "heat_no", 0, tmmsm2a["HEAT_NO"].ToString()) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					//序号tmmsm52.seq_no
					if (epex.SetValue("tmmsm52", "seq_no", 0, tmmsm2a["PROC_COUNT"].ToString()) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					//加料时机 tmmsm52.charge_point
					/*if (epex.SetValue("tmmsm52", "charge_point", 0, tmmsm2a["DEVO_TIME"].ToString()) < 0)
					{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
					}
					*/
					//加料设备号 tmmsm52.charge_dev_code
					if (epex.SetValue("tmmsm52", "charge_dev_code", 0, tmmsm2a["DEV_CODE"].ToString()) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 加入料代码 tmmsm52.mat_code_charge
					if (epex.SetValue("tmmsm52", "mat_code_charge", 0, tmmsm2a["MAT_CODE"].ToString()) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					Log::Trace("", __FUNCTION__, "444");
					// 加入料名称 tmmsm52.mat_name
					if (epex.SetValue("tmmsm52", "mat_name", 0, tmmsm2a["MAT_NAME"].ToString()) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					//// 采购订单号 tmmsm52.sap_bstkd
					//if (epex.SetValue("tmmsm52", "sap_bstkd", 0, ) < 0)
					//{
					//	sprintf(s.msg, epex.GetMsg());
					//	throw CApplicationException(-1, s.msg, log.Location);
					//}

					// 计量单号 tmmsm52.weight_no
					if (epex.SetValue("tmmsm52", "weight_no", 0,tmmsm2a["WEIGH_NO"].ToString() ) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					//// 加入长度（喂线）tmmsm52.mat_add_len
					//if (epex.SetValue("tmmsm52", "", 0, ) < 0)tmmsm2a
					//{
					//	sprintf(s.msg, epex.GetMsg());
					//	throw CApplicationException(-1, s.msg, log.Location);
					//}

					CDecimal devo_wt = 0;
					devo_wt = tmmsm2a["DEVO_WT"].ToDecimal() / 1000;
					// 加入量 tmmsm52.mat_add_wt
					if (epex.SetValue("tmmsm52", "mat_add_wt", 0, devo_wt) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 加料时刻 tmmsm52.mat_add_time
					if (epex.SetValue("tmmsm52", "mat_add_time", 0, tmmsm2a["REC_CREATE_TIME"].ToString()) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 成分来源(原带/复检） tmmsm52.elm_source
					/*if (epex.SetValue("tmmsm52", "", 0, ) < 0)
					{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
					}*/

					//成分1名称 tmmsm52.elm_name1 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name1", 0,"Al" ) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值1 tmmsm52.elm_value1
					if (epex.SetValue("tmmsm52", "elm_value1", 0, al) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//成分2名称 tmmsm52.elm_name1 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name2", 0,"C" ) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值2 tmmsm52.elm_value1
					if (epex.SetValue("tmmsm52", "elm_value2", 0,c ) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//成分3名称 tmmsm52.elm_name1 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name3", 0, "Ca") < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值3 tmmsm52.elm_value1
					if (epex.SetValue("tmmsm52", "elm_value3", 0, ca) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//成分4名称 tmmsm52.elm_name1 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name4", 0, "Cr") < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值4 tmmsm52.elm_value1
					if (epex.SetValue("tmmsm52", "elm_value4", 0, cr) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//成分5名称 tmmsm52.elm_name1 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name5", 0, "Cu") < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值5 tmmsm52.elm_value1
					if (epex.SetValue("tmmsm52", "elm_value5", 0, cu) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//成分6名称 tmmsm52.elm_name1 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name6", 0, "Fe") < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值6 tmmsm52.elm_value1
					if (epex.SetValue("tmmsm52", "elm_value6", 0, fe) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//成分7名称 tmmsm52.elm_name1 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name7", 0,"Mn" ) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值7 tmmsm52.elm_value1
					if (epex.SetValue("tmmsm52", "elm_value7", 0, mn) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//成分8名称 tmmsm52.elm_name1 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name8", 0, "Ni") < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值8 tmmsm52.elm_value1
					if (epex.SetValue("tmmsm52", "elm_value8", 0, ni) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//成分9名称 tmmsm52.elm_name1 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name9", 0, "P") < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值9 tmmsm52.elm_value1
					if (epex.SetValue("tmmsm52", "elm_value9", 0, p) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//成分10名称 tmmsm52.elm_name1 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name10", 0, "Pb") < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值10 tmmsm52.elm_value1
					if (epex.SetValue("tmmsm52", "elm_value10", 0, pb) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//成分11名称 tmmsm52.elm_name1 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name11", 0, "S") < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值1 tmmsm52.elm_value11
					if (epex.SetValue("tmmsm52", "elm_value11", 0, s1) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);

					}
					//成分1名称 tmmsm52.elm_name12 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name12", 0, "Si") < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值1 tmmsm52.elm_value12
					if (epex.SetValue("tmmsm52", "elm_value12", 0, si) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);

					}
					//成分13名称 tmmsm52.elm_name13 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name13", 0,"Ti" ) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值13 tmmsm52.elm_value13
					if (epex.SetValue("tmmsm52", "elm_value13", 0, ti) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//成分14名称 tmmsm52.elm_name1 tmmsm52.elm_name1
					if (epex.SetValue("tmmsm52", "elm_name14", 0, "V") < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 元素值14 tmmsm52.elm_value1
					if (epex.SetValue("tmmsm52", "elm_value14", 0, v) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

				}
				cmd_inq.Close();
				/*tmmsm2a.Query("L2_PROC_NO,MAT_CODE,HEAT_NO,PROD_SEQ_NO");*/

				Log::Trace("",__FUNCTION__,"tmmsm2a BEGIN 发送电文开始 tmmsm2a.PROC_NO	= [{0}]",tmmsm2a["PROC_NO"].ToString());
				//Log::Trace("",__FUNCTION__,"tmmsm2a BEGIN 发送电文开始 tmmsm2a.HEAT_NO	= [{0}]",tmmsm2a["HEAT_NO"].ToString());	  	
				//Log::Trace("",__FUNCTION__,"tmmsm2a END 发送电文结束tmmsm2a.HEAT_NO	= [{0}]",tmmsm2a["HEAT_NO"].ToString());	

			}



			//切断实绩
			//if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM33")
			//{
			//	tmmsm33.Reset();
			//	tmmsm33["MAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"].ToString().Trim();
			//	v_proc_div = bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"].ToString().Trim(); 
			//	tmmsm33.Query();
			//	tmmsm33.TrimOrBlank();

			//	if (epex.SetValue(0, tmmsm33) < 0)
			//	{
			//		sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	}

			//	if (epex.SetValue("PROC_DIV", 0, v_proc_div) < 0)
			//	{
			//		sprintf(s.msg, epex.GetMsg());
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}

			//	/*if (v_proc_div == "N")
			//	{
			//		if (epex.SetValue("FIN_ST_NO", 0, bcls_rec->Tables["TMMSM33"].Rows[0]["FIN_ST_NO"].ToString()) < 0)
			//		{
			//			sprintf(s.msg, epex.GetMsg());
			//			throw CApplicationException(-1, s.msg, log.Location);
			//		}

			//	}
			//	*/

			//	//Log::Trace("", __FUNCTION__, "tmmsm33 END 发送电文结束tmmsm33.HEAT_NO111	= [{0}]", tmmsm33["HEAT_NO"].ToString());
			//	
			//}
			

			//精整实绩
			//if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM34")
			//{
			//	
			//	tmmsm34["MAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"].ToString().Trim();
   //             tmmsm34["PROD_SEQ_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["PROD_SEQ_NO"].ToString().Trim();	
			//	tmmsm34.Query("MAT_NO,PROD_SEQ_NO");
			//	tmmsm34.TrimOrBlank();
			//			
			//	//Log::Trace("",__FUNCTION__,"tmmsm34 BEGIN 发送电文开始 tmmsm34.HEAT_NO	= [{0}]",tmmsm34["MAT_NO"].ToString());	  	

			//	if (epex.SetValue(0, tmmsm34) < 0)
			//	{
			//		sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	} 

			//	if(epex.SetValue("PROC_DIV", 0, v_proc_div) < 0)
			//	{
			//		sprintf(s.msg,epex.GetMsg()) ;
			//		throw CApplicationException(-1, s.msg, log.Location); 
			//	}

			//	//Log::Trace("",__FUNCTION__,"tmmsm34 END 发送电文结束tmmsm34.HEAT_NO	= [{0}]",tmmsm34["MAT_NO"].ToString());	    		  	
			//}

			////缓冷实绩
			//if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM36")
			//{

			//	tmmsm36["MAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"].ToString().Trim();
			//	tmmsm36.Query("MAT_NO");
			//	tmmsm36.TrimOrBlank();

			//	//Log::Trace("", __FUNCTION__, "tmmsm36 BEGIN 发送电文开始 tmmsm36.MAT_NO	= [{0}]", tmmsm36["MAT_NO"].ToString());

			//	if (epex.SetValue(0, tmmsm36) < 0)
			//	{
			//		sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	}

			//	if (epex.SetValue("PROC_DIV", 0, v_proc_div) < 0)
			//	{
			//		sprintf(s.msg, epex.GetMsg());
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}

			//	//Log::Trace("", __FUNCTION__, "tmmsm36 END 发送电文结束tmmsm36.MAT_NO	= [{0}]", tmmsm34["MAT_NO"].ToString());
			//}


			//分切实绩
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM35") 
			{
				//因电文配置和后续压值不对应，导致程序报错   2023年9月15日  
					//tmmsm35["IN_MAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["IN_MAT_NO"].ToString().Trim();

					//sqlstr = " SELECT  * FROM TMMSM35"
					//	" WHERE IN_MAT_NO = @tmmsm35.IN_MAT_NO AND OP_DIV = '1'";
			
					//cmd_inq.SetCommandText(sqlstr);		
					//cmd_inq.Parameters.Set("tmmsm35.IN_MAT_NO", tmmsm35["IN_MAT_NO"].ToString());
					//cmd_inq.ExecuteReader();
					//while (cmd_inq.Read())
					//{
					//	cmd_inq.Fetch(tmmsm35);

					//	if (epex.SetValue(j, tmmsm35) < 0)
					//	{
					//		cmd_inq.Close();
					//		sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					//		//Log::Trace("", "", "第{0}行获取失败", j);
					//		throw CApplicationException(-1, s.msg, s.svc_name);
					//	}
			
					//	//入口材料号信息 + 第一个子材料信息
					//	if (j == 0)
					//	{
					//		if (epex.SetValue("EVENT_ID", j, "MM16")<0)
					//		{
					//			cmd_inq.Close();
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//		if (epex.SetValue("REC_CREATOR", j, tmmsm35["REC_CREATOR"].ToString())<0)
					//		{
					//			cmd_inq.Close();
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//		if (epex.SetValue("REC_CREATE_TIME", j, tmmsm35["REC_CREATE_TIME"].ToString())<0)
					//		{
					//			cmd_inq.Close();
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//		if (epex.SetValue("AIM_MAT_NO", j, tmmsm35["IN_MAT_NO"].ToString())<0)
					//		{
					//			cmd_inq.Close();
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//		if (epex.SetValue("AIM_HEAT_NO", j, tmmsm35["IN_HEAT_NO"].ToString())<0)
					//		{
					//			cmd_inq.Close();
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//		if (epex.SetValue("AIM_PONO", j, tmmsm35["IN_PONO"].ToString())<0)
					//		{
					//			cmd_inq.Close();
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//		if (epex.SetValue("AIM_SG_SIGN", j, tmmsm35["IN_SG_SIGN"].ToString())<0)
					//		{
					//			cmd_inq.Close();
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//		if (epex.SetValue("AIM_ST_NO", j, tmmsm35["IN_ST_NO"].ToString())<0)
					//		{
					//			cmd_inq.Close();
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//		if (epex.SetValue("AIM_THICK", j, tmmsm35["IN_MAT_THICK"].ToDecimal())<0)
					//		{
					//			cmd_inq.Close();
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//		if (epex.SetValue("AIM_WIDTH", j, tmmsm35["IN_MAT_WIDTH"].ToDecimal())<0)
					//		{
					//			cmd_inq.Close();
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//		if (epex.SetValue("AIM_LEN", j, tmmsm35["IN_MAT_LEN"].ToDecimal())<0)
					//		{
					//			cmd_inq.Close();
					//			throw CApplicationException(-1, s.msg, log.Location);
					//		}
					//	}
					//	j++;
					//}
					//cmd_inq.Close();
		
			}

			


			//原辅料代码
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM50")
			{

				tmmsm50["MAT_CODE"] = bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_CODE"].ToString().Trim();
				tmmsm50["MAT_NAME"] = bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NAME"].ToString().Trim();
				tmmsm50.Query("MAT_CODE,MAT_NAME");
				tmmsm50.TrimOrBlank();

				//Log::Trace("", __FUNCTION__, "tmmsm50 BEGIN 发送电文开始 tmmsm50.MAT_CODE	= [{0}]", tmmsm50["MAT_CODE"].ToString());

				if (epex.SetValue(0, tmmsm50) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("OPER_FLAG", 0, v_proc_div) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//Log::Trace("", __FUNCTION__, "tmmsm50 END 发送电文结束tmmsm50.MAT_CODE	= [{0}]", tmmsm50["MAT_CODE"].ToString());
			}

/*
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM3F") //材料后备
			{
				
				tmmsm01["MAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"].ToString().Trim();
				v_event_id = bcls_rec->Tables["MMSMSND"].Rows[0]["EVENT_ID"].ToString().Trim();

				//Log::Trace("", __FUNCTION__, "tmmsm01.MAT_NO		= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());
				//Log::Trace("", __FUNCTION__, "v_event_id		= [{0}]", (const char*)v_event_id);

				if (tmmsm01["MAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg, "材料号不能为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				tmmsm01.Query();
				if (tmmsm01["MAT_ID"].ToString().Trim() == "")
				{
					sprintf(s.msg, "材料号[%s]在钢坯主档表里不存在!", (const char*)tmmsm01["MAT_NO"].ToString());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmmsm01["PONO"].ToString().Trim() == "")
				{
					sprintf(s.msg, "材料号[%s]的PONO[%s]为空!", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["PONO"].ToString());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//Log::Trace("",__FUNCTION__,"tmmsm01 BEGIN 发送电文开始 tmmsm01.MAT_NO	= [{0}]",tmmsm01["MAT_NO"].ToString());	

				if (epex.SetValue(0, tmmsm01) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("EVENT_ID", 0, v_event_id) < 0)
				{
					Log::Debug("", __FUNCTION__, "SetValue EVENT_ID:{0}", epex.GetMsg());
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
						
				//Log::Trace("",__FUNCTION__,"tmmsm01 END 发送电文结束 tmmsm56.MAT_CODE	= [{0}]",tmmsm01["MAT_NO"].ToString());	  		  	
			}
*/

			//方坯拆批
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM3G") //材料拆批
			{
				//Log::Trace("", __FUNCTION__, "发送电文开始");
				if (epex.SetValue("event_id", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["EVENT_ID"].ToString()) < 0)
				{
					sprintf(s.msg,"发送电文失败，原因[%s]",epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				
				if (epex.SetValue("rec_creator", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATOR"].ToString()) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("rec_create_time", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATE_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("aim_mat_no", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_MAT_NO"].ToString()) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("mat_no", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"].ToString()) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("mat_tube", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_TUBE"].ToDecimal()) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				
				//Log::Trace("",__FUNCTION__,"发送电文结束");	  		  	
			}

			//方坯并批
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM3H") //材料并批
			{
				//Log::Trace("", __FUNCTION__, "发送电文开始");
				if (epex.SetValue("event_id", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["EVENT_ID"].ToString()) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("rec_creator", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATOR"].ToString()) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("rec_create_time", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATE_TIME"].ToString()) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("aim_mat_no", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_MAT_NO"].ToString()) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (bcls_rec->Tables["MMSMSND"].Columns.Contains("AIM_HEAT_NO"))
				{
					if (epex.SetValue("aim_heat_no", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_HEAT_NO"].ToString()) < 0)
					{
						sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}

				if (bcls_rec->Tables["MMSMSND"].Columns.Contains("AIM_PONO"))
				{
					if (epex.SetValue("aim_pono", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_PONO"].ToString()) < 0)
					{
						sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}

				for (int i = 0; i < bcls_rec->Tables["MMSMSND"].Rows.get_Count(); i++)
				{
					if (epex.SetValue("mat_no", i, bcls_rec->Tables["MMSMSND"].Rows[i]["MAT_NO"].ToString()) < 0)
					{
						CFormattable arguments[] = { tmm009a["TC_NO"].ToString() }; // 定义参数列表的数组
						CMessageFormat::Format(s.msg, _RES("MMHRS0000166")/*材料号{0}向MMS发送电文失败*/, arguments, 1);
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					if (bcls_rec->Tables["MMSMSND"].Columns.Contains("MAIN_MAT_FLAG"))
					{
						if (epex.SetValue("main_mat_flag", i, bcls_rec->Tables["MMSMSND"].Rows[i]["MAIN_MAT_FLAG"].ToString()) < 0)
						{
							CFormattable arguments[] = { tmm009a["TC_NO"].ToString() }; // 定义参数列表的数组
							CMessageFormat::Format(s.msg, _RES("MMHRS0000166")/*材料号{0}向MMS发送电文失败*/, arguments, 1);
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
				}
					
				//Log::Trace("",__FUNCTION__,"发送电文结束");	  		  	
			}

/*
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM3Y") //材料缺陷
			{
					for(int i = 0; i < bcls_rec->Tables["MMSMSND"].Rows.get_Count(); i++)
					{
							tmmsm061.MergeFrom(bcls_rec->Tables["MMSMSND"].Rows[i]);	
							tmmsm061.TrimOrBlank();

							//Log::Trace("",__FUNCTION__,"传入参数,tmmsm061.MAT_NO			= [{0}]",tmmsm061["MAT_NO"].ToString());	
							//Log::Trace("",__FUNCTION__,"发送电文 BEGIN,tmmsm061.REC_CREATOR		= [{0}]",tmmsm061["REC_CREATOR"].ToString());		

							if(epex.SetValue(i, tmmsm061) < 0)
							{
								sprintf(s.msg,"发送电文失败，原因[%s]",epex.GetMsg());
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
							//Log::Trace("",__FUNCTION__,"发送电文 END,tmmsm061.REC_CREATOR		= [{0}]",tmmsm061["REC_CREATOR"].ToString());	
						}
			
			}

		
			//原辅料入库
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM55")
			{
				if (tmm009a["TC_DATA_TRANS_MODE"].ToString().Trim() == "B")//从传入块中读取
				{
					tmmsm55.MergeFrom(bcls_rec->Tables["MMSMSND"].Rows[0]);
				}
				else if (tmm009a["TC_DATA_TRANS_MODE"].ToString().Trim() == "T")//从实绩表中读取
				{
					tmmsm55["IN_STOCK_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["IN_STOCK_NO"].ToString().Trim();
					
					tmmsm55["MAT_CODE"] = bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_CODE"].ToString().Trim();

					tmmsm55.Query("IN_STOCK_NO,MAT_CODE");
					tmmsm55.TrimOrBlank();
				}
				//Log::Trace("",__FUNCTION__,"tmmsm55 BEGIN 发送电文开始 tmmsm55.MAT_CODE	= [{0}]",tmmsm55["MAT_CODE"].ToString());	  	

				if (epex.SetValue(0, tmmsm55) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				//Log::Trace("",__FUNCTION__,"tmmsm55 END 发送电文结束 tmmsm55.MAT_CODE	= [{0}]",tmmsm55["MAT_CODE"].ToString());	  		  	
			}

			//原辅料出库
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "MMSM56")
			{
				
				tmmsm56["OUT_STOCK_NO"] = bcls_rec->Tables["MMSMSND"].Rows[0]["OUT_STOCK_NO"].ToString().Trim();
			  tmmsm56["MAT_CODE"] = bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_CODE"].ToString().Trim();
				tmmsm56.Query("OUT_STOCK_NO,MAT_CODE");
				tmmsm56.TrimOrBlank();
			
				//Log::Trace("",__FUNCTION__,"tmmsm56 BEGIN 发送电文开始 tmmsm56.MAT_CODE	= [{0}]",tmmsm56["MAT_CODE"].ToString());	  	

				if (epex.SetValue(0, tmmsm56) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				//Log::Trace("",__FUNCTION__,"tmmsm56 END 发送电文结束 tmmsm56.MAT_CODE	= [{0}]",tmmsm56["MAT_CODE"].ToString());	  		  	
			}
*/

			//铸坯信息同步
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "PAM1M2")
			{
				tmmsm96.MergeFrom(bcls_rec->Tables["MMSMSND"].Rows[0]);

				if (epex.SetValue(0, tmmsm96) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (bcls_rec->Tables["MMSMSND"].Columns.Contains("WT_PER_METER"))
				{
					if (epex.SetValue("WT_PER_METER", 0, bcls_rec->Tables["MMSMSND"].Rows[0]["WT_PER_METER"].ToDecimal()) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				//Log::Trace("", __FUNCTION__, "PAM1M2 END 发送电文结束");

			}

			//库位变更、辊道出坯
			if (tmm009a["TC_BACKLOG"].ToString().Trim() == "PAM1Y2")
			{
				tmmsm96.MergeFrom(bcls_rec->Tables["MMSMSND"].Rows[0]);

				if (epex.SetValue(0, tmmsm96) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//Log::Trace("", __FUNCTION__, "PAM1Y2 END 发送电文结束");

			}

			//称重委托
			//if (tmm009a["TC_BACKLOG"].ToString().Trim() == "PAW101")
			//{
			//	CPAW101 xpaw101(conn);
			//	xpaw101.MergeFrom(bcls_rec->Tables["MMSMSND"].Rows[0]);
			//	if (epex.SetValue(0, xpaw101) < 0)
			//	{
			//		sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	}

			//	//Log::Trace("", __FUNCTION__, "PAW101 END 发送电文结束");

			//}

			////车辆委托
			//if (tmm009a["TC_BACKLOG"].ToString().Trim() == "PAT101")
			//{
			//	CPAT101 xpat101(conn);
			//	xpat101.MergeFrom(bcls_rec->Tables["MMSMSND"].Rows[0]);
			//	if (epex.SetValue(0, xpat101) < 0)
			//	{
			//		sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	}

			//	for (int i = 0; i < bcls_rec->Tables["MMSMSND"].Rows.get_Count(); i++)
			//	{
			//		xpat101.MAT_NO = bcls_rec->Tables["MMSMSND"].Rows[i]["MAT_NO"].ToString();
			//		xpat101.MAT_GROSS_WT = bcls_rec->Tables["MMSMSND"].Rows[i]["MAT_GROSS_WT"].ToDecimal();
			//		xpat101.SPEC = bcls_rec->Tables["MMSMSND"].Rows[i]["SPEC"].ToString();
			//		//Log::Trace("", __FUNCTION__, "xpat101.MAT_NO={0},xpat101.MAT_GROSS_WT={1}", xpat101.MAT_NO, xpat101.MAT_GROSS_WT);
			//		if (epex.SetValue("MAT_NO", i, xpat101.MAT_NO) < 0)
			//		{
			//			sprintf(s.msg, epex.GetMsg());
			//			throw CApplicationException(-1, s.msg, log.Location);
			//		}

			//		if (epex.SetValue("MAT_GROSS_WT", i, xpat101.MAT_GROSS_WT) < 0)
			//		{
			//			sprintf(s.msg, epex.GetMsg());
			//			throw CApplicationException(-1, s.msg, log.Location);
			//		}

			//		if (epex.SetValue("SPEC", i, xpat101.SPEC) < 0)
			//		{
			//			sprintf(s.msg, epex.GetMsg());
			//			throw CApplicationException(-1, s.msg, log.Location);
			//		}

			//	}
			//	//Log::Trace("", __FUNCTION__, "PAT101 END 发送电文结束");

			//}

			//Log::Trace("", __FUNCTION__, "调用框架发送电文开始");
			if (epex.SendTele() < 0)
			{
				strcpy(s.msg,"电文发送失败。");
				throw CApplicationException(-1, s.msg, log.Location); 
			}
			//Log::Trace("", __FUNCTION__, "调用框架发送电文结束");
			/* 释放 */
			epex.Uninitialize();
			
			//Log::Trace("", __FUNCTION__, "调用框架释放电文结束");


		
		}
	
		

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
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
     
    cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}




