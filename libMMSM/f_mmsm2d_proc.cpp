/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2017-8-25
Description: 通电实绩明细实绩增删改
===========================================================</remark>*/


/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

 

//外部函数声明
int f_mmsm_count(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_confirm_flag(const CString& factory_div, const CString& heat_no, CString& heat_confirm_flag, CDbConnection * conn);
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm2d_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm2d_proc";                //定义函数英文名称  
	CString FunctionCname = "通电实绩明细实绩增删改";              //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
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



	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		CPageInfo pageInfo;

		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
	CModel tmmsm2d("TMMSM2D");

		//初始化实体类
		tmmsm2d.Reset();

		//获取输入参数
		tmmsm2d.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (tmmsm2d["L2_PROC_NO"].ToString() == " "){
			tmmsm2d["L2_PROC_NO"] = tmmsm2d["PROC_NO"];
		}
		if (tmmsm2d["PROC_NO"].ToString() == " "){
			tmmsm2d["PROC_NO"] = tmmsm2d["L2_PROC_NO"];
		}

		if (bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
			v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("AREA_ID")) //区分冶炼区还是精炼区
			v_area_id = bcls_rec->Tables[0].Rows[0]["AREA_ID"].ToString().TrimOrBlank().ToUpper();

		//Log::Trace("", __FUNCTION__, "tmmsm2d["FACTORY_DIV"] =[{0}]", tmmsm2d["FACTORY_DIV"].ToString());
		//Log::Trace("", __FUNCTION__, "v_area_id=[{0}]", v_area_id);
		//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
		//Log::Trace("", __FUNCTION__, "heat_no=[{0}]", tmmsm2d["HEAT_NO"].ToString());
		//Log::Trace("", __FUNCTION__, "PROC_NO=[{0}]", tmmsm2d["PROC_NO"].ToString());

		//已炉次确定则返回
		doFlag = f_mmsm_confirm_flag(tmmsm2d["FACTORY_DIV"].ToString(), tmmsm2d["HEAT_NO"].ToString(), v_heat_confirm_flag, conn);

		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (v_heat_confirm_flag != "0" && v_heat_confirm_flag != "")
		{
			strcpy(s.msg, "该制造命令号已经炉次确定!!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/******检查输入参数合法性 ***** */
		if (v_area_id.Trim() == "4")
		{
			if (tmmsm2d["PROC_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "处理号不能为空!");
				throw CApplicationException(-1, s.msg, log.Location);

			}
		}

		if (v_proc_div == "I")
		{
			
			//Log::Info("", __FUNCTION__, "tmmsm2d["PROC_NO"] =[{0}]", tmmsm2d["PROC_NO"].ToString());

			sqlstr = "SELECT SUBSTR(DEV_CODE,1,1),SUBSTR(DEV_CODE,2,1) "
				"		FROM	 TPSSM12 "
				"		WHERE  FACTORY_DIV =  @factory_div"
				"		 AND   HEAT_NO	= @heat_no"
				"		 AND   PROC_NO	= @proc_no";

			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Clear();
			cmd_sql.Parameters.Set("factory_div", tmmsm2d["FACTORY_DIV"].ToString());
			cmd_sql.Parameters.Set("heat_no", tmmsm2d["HEAT_NO"].ToString());
			cmd_sql.Parameters.Set("proc_no", tmmsm2d["PROC_NO"].ToString());
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				v_station_id = cmd_sql.GetString(1);
				v_station_no = cmd_sql.GetString(2);
			}

			cmd_sql.Close();
		}
		if (v_proc_div == "U"){
			//电文是按条传送的 所以需要通过处理计数来删除
			if (bcls_rec->Tables[0].Columns.Contains("PRACT_COLL_MODE")){
				if (bcls_rec->Tables[0].Rows[0]["PRACT_COLL_MODE"].ToString() == "1"){
					tmmsm2d.Delete("HEAT_NO,PROC_NO,PROC_COUNT");
					//Log::Trace("", __FUNCTION__, ">>>>>>删除条件：HEAT_NO,PROC_NO,PROC_COUNT");
				}
				else{
					tmmsm2d.Delete("HEAT_NO,PROC_NO");
					//Log::Trace("", __FUNCTION__, ">>>>>>删除条件：HEAT_NO,PROC_NO");
				}
			}
			else{
				tmmsm2d.Delete("HEAT_NO,PROC_NO");
				//Log::Trace("", __FUNCTION__, ">>>>>>删除条件：HEAT_NO,PROC_NO");
			}
		}

		//Log::Trace("", __FUNCTION__, "1");
		/* 获取输入参数*/
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			CString measTempTime = bcls_rec->Tables[0].Rows[i]["MEAS_TEMP_TIME"];
			CDecimal steelTemp = bcls_rec->Tables[0].Rows[i]["STEEL_TEMP"];
			if ((measTempTime.Trim() == "") && (steelTemp == 0)){
				break;
			}
			//Log::Trace("", __FUNCTION__, "2");

			tmmsm2d.Reset();
			tmmsm2d.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			tmmsm2d.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm2d.TrimOrBlank();

			//Log::Trace("", __FUNCTION__, "3");

			if (v_proc_div == "I" || v_proc_div == "U")
			{
				tmmsm2d["REC_CREATE_TIME"] = dateNow;
				tmmsm2d["REC_CREATOR"] = s.userid;

				if (tmmsm2d["STATION_ID"].ToString().Trim() == "")
				{
					tmmsm2d["STATION_ID"] = v_station_id;
				}

				if (tmmsm2d["STATION_NO"].ToString().Trim() == "0" || tmmsm2d["STATION_NO"].ToString().Trim() == "")
				{
					tmmsm2d["STATION_NO"] = v_station_no;
				}

				//取最大计数器
				if (tmmsm2d["PROC_COUNT"].ToDecimal() == 0)
				{
					blkNum = bcls_rec->Tables.IndexOf("PROCCOUNT");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("PROCCOUNT");
					}

					if (!bcls_rec->Tables["PROCCOUNT"].Columns.Contains("TABLE_TYPE"))
					{
						bcls_rec->Tables["PROCCOUNT"].Columns.Add(DT_STRING, "TABLE_TYPE");
					}

					if (!bcls_rec->Tables["PROCCOUNT"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["PROCCOUNT"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					if (!bcls_rec->Tables["PROCCOUNT"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["PROCCOUNT"].Columns.Add(DT_STRING, "PROC_NO");
					}

					bcls_rec->Tables["PROCCOUNT"].Rows.Add();
					bcls_rec->Tables["PROCCOUNT"].Rows[0]["TABLE_TYPE"] = "TMMSM2D";
					bcls_rec->Tables["PROCCOUNT"].Rows[0]["HEAT_NO"] = tmmsm2d["HEAT_NO"];
					bcls_rec->Tables["PROCCOUNT"].Rows[0]["PROC_NO"] = tmmsm2d["PROC_NO"];


					doFlag = f_mmsm_count(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				//Log::Trace("", __FUNCTION__, "4");
				//ssp 2016-12-15 由于电文和后备通用，所以原来新增这一段放在 if里面，现在挪到外边
				tmmsm2d["PROC_COUNT"] = bcls_rec->Tables[0].Rows[0]["PROC_COUNT"];
				//Log::Info("", __FUNCTION__, "tmmsm2d["PROC_COUNT"] =[{0}]", tmmsm2d["PROC_COUNT"].ToDecimal());
				
				//Log::Trace("", __FUNCTION__, "5");

				tmmsm2d.Print();
				tmmsm2d.Insert();
			}
			else if (v_proc_div == "D"){
				//电文是按条传送的 所以需要通过处理计数来删除
				if (bcls_rec->Tables[0].Columns.Contains("PRACT_COLL_MODE")){
					if (bcls_rec->Tables[0].Rows[0]["PRACT_COLL_MODE"].ToString() == "1"){
						tmmsm2d.Delete("HEAT_NO,PROC_NO,PROC_COUNT");
						//Log::Trace("", __FUNCTION__, ">>>>>>删除条件：HEAT_NO,PROC_NO,PROC_COUNT");
					}
					else
					{
						if (tmmsm2d["PROC_COUNT"].ToDecimal() == 0)
						{
							tmmsm2d.Delete("HEAT_NO,PROC_NO");
							//Log::Trace("", __FUNCTION__, ">>>>>>删除条件：HEAT_NO,PROC_NO");
						}
						else
						{
							tmmsm2d.Delete("HEAT_NO,PROC_NO,PROC_COUNT");
							//Log::Trace("", __FUNCTION__, ">>>>>>删除条件：HEAT_NO,PROC_NO,PROC_COUNT");
						}
					}
				}
				else
				{
					if (tmmsm2d["PROC_COUNT"].ToDecimal() == 0)
					{
						tmmsm2d.Delete("HEAT_NO,PROC_NO");
						//Log::Trace("", __FUNCTION__, ">>>>>>删除条件：HEAT_NO,PROC_NO");
					}
					else
					{
						tmmsm2d.Delete("HEAT_NO,PROC_NO,PROC_COUNT");
						//Log::Trace("", __FUNCTION__, ">>>>>>删除条件：HEAT_NO,PROC_NO,PROC_COUNT");
					}
				}
			}

			//Log::Trace("", __FUNCTION__, "6");
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


			tmmsm2d.MergeTo(bcls_rec->Tables["MMSMSND"], false);

			bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM2D";
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;


			//doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
#endif
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

