/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-11-25
Description: 模铸炉次实绩子表增删改
===========================================================</remark>*/


/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

 

//外部函数声明
int f_mmsm_count(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_confirm_flag(const CString& factory_div, const CString& heat_no, CString& heat_confirm_flag, CDbConnection * conn);
//int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm41a_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	/****** 定义函数名称 ***** */
CString FunctionEname = "f_mmsm41a_proc";                //定义函数英文名称  
CString FunctionCname = "模铸炉次实绩子表增删改";              //定义函数中文名称


CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
  

  //程序用变量
  int   doFlag = 0;
  int   fetchRowCount = 0;
  int   i = 0;
  int   n_count = 0;
  int   blkNum;

  CString sqlstr="";
  CString v_heat_confirm_flag = "";
	CString v_area_id  = "";
	CString v_proc_div = "";
	CString v_station_id = "";
	CString v_station_no = "";
	CString practCollMode = "";
	
	

  CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
   
  
  try
  {
		CPageInfo pageInfo;	

		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
	CModel tmmsm41a("TMMSM41A");
				
     //初始化实体类
		tmmsm41a.Reset();
		practCollMode = bcls_rec->Tables[0].Rows[i]["PRACT_COLL_MODE"];
		if (practCollMode == "1"){
			//获取输入参数
			tmmsm41a.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			if (bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
				v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();
		}
		else{
			//获取输入参数
			tmmsm41a.MergeFrom(bcls_rec->Tables[1].Rows[0]);

			if (bcls_rec->Tables[1].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
				v_proc_div = bcls_rec->Tables[1].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();
		}
		
		//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);

		//已炉次确定则返回
		//doFlag = f_mmsm_confirm_flag(tmmsm41a.FACTORY_DIV, tmmsm41a["HEAT_NO"].ToString(), v_heat_confirm_flag, conn);

		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (v_heat_confirm_flag != "0" && v_heat_confirm_flag != "")
		{
			strcpy(s.msg, "该制造命令号已经炉次确定!!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
		if ((practCollMode == "1") && (v_proc_div == "U")){
			tmmsm41a.Delete("HEAT_NO,PROC_NO");
		}
		

		/* 获取输入参数*/
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			CString purStartTime = bcls_rec->Tables[0].Rows[i]["PUR_START_TIME"];
			CDecimal purStartWt = bcls_rec->Tables[0].Rows[i]["PUR_START_WT"];
			if ((purStartTime.Trim() == "") && (purStartWt == 0)){
				break;
			}
			tmmsm41a.Reset();
			tmmsm41a.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (practCollMode == "1"){
				tmmsm41a.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			}
			else{
				tmmsm41a.MergeFrom(bcls_rec->Tables[1].Rows[0]);
			}
			tmmsm41a.TrimOrBlank();

			//Log::Info("", __FUNCTION__, "PRACT_COLL_MODE=[{0}]", practCollMode);
			//Log::Info("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
			//if (v_proc_div == "I")
			if (((practCollMode == "1") && (v_proc_div == "U")) || v_proc_div == "I")
			{
				tmmsm41a["REC_CREATE_TIME"] = dateNow;
				tmmsm41a["REC_CREATOR"] = s.userid;

			
				//取最大计数器
				if (tmmsm41a["PROC_COUNT"].ToDecimal() == 0)
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
					bcls_rec->Tables["PROCCOUNT"].Rows[0]["TABLE_TYPE"] = "TMMSM41A";
					bcls_rec->Tables["PROCCOUNT"].Rows[0]["HEAT_NO"] = tmmsm41a["HEAT_NO"];
					bcls_rec->Tables["PROCCOUNT"].Rows[0]["PROC_NO"] = tmmsm41a["PROC_NO"];


					doFlag = f_mmsm_count(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					tmmsm41a["PROC_COUNT"] = bcls_ret->Tables[0].Rows[0]["PROC_COUNT"];
					tmmsm41a.Insert();

				}

				//Log::Info("", __FUNCTION__, "tmmsm41a["PROC_COUNT"] =[{0}]", tmmsm41a["PROC_COUNT"].ToDecimal());
			}
			else if ((practCollMode == "0") && (v_proc_div == "U"))
			{
					//取原记录的创建时间和创建人
					sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
								 "		FROM	TMMSM41A "
								 "   WHERE  PROC_NO	= @proc_no"
								 "   AND    PROC_COUNT = @proc_count";

					//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
		
					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.Parameters.Clear();
					cmd_sql.Parameters.Set("proc_no",tmmsm41a["PROC_NO"].ToString()); 
					cmd_sql.Parameters.Set("proc_count",tmmsm41a["PROC_COUNT"].ToDecimal());
					cmd_sql.ExecuteReader();

					

					if(cmd_sql.Read())
					{
						tmmsm41a["REC_CREATE_TIME"] = cmd_sql.GetString(1);
						tmmsm41a["REC_CREATOR"] = cmd_sql.GetString(2);
					}
					cmd_sql.Close();

					tmmsm41a["REC_REVISE_TIME"] = dateNow;
					tmmsm41a["REC_REVISOR"] = s.userid;
			
					tmmsm41a.Delete(); 
					tmmsm41a.Insert();    
  			}
			else if (v_proc_div == "D")
			{
				if (tmmsm41a["PROC_COUNT"].ToDecimal() == 0)
				{
					tmmsm41a.Delete("PROC_NO"); 
				}
				else
				{
					tmmsm41a.Delete("PROC_NO,PROC_COUNT"); 
				}
					
			}
				
								
			#if defined(_SYS_PES)
			//调用发送电文
			blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
			if(blkNum < 0)
			{
					bcls_rec->Tables.Add("MMSMSND"); 
			}
		
			if(!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			}
	
			if(!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING,"PROC_DIV");
			}
		
					
			tmmsm41a.MergeTo(bcls_rec->Tables["MMSMSND"], false);
	
			bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM2B";
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;
	
							
			//doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
			if(doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			#endif
		}


			
			
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

