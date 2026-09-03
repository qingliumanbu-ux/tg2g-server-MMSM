/***********************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: IC实绩增删改
*************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件


//外部函数声明
int f_mmsm_sj_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
int f_pssm12_mm_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_acyfl_tlcc(EIClass * bcls_rec, EIClass * bcls_ret, CString& v_acjc_relation_id, CDbConnection * conn);
int f_mmsm_acyfl_seq(CString& v_acjc_relation_id, CDbConnection * conn);


BM2_FUNCTION_EXPORT
int f_mmsm41_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	/****** 定义函数名称 ***** */
CString FunctionEname = "f_mmsm41_proc";                //定义函数英文名称  
CString FunctionCname = "CC信息增删改";          //定义函数中文名称


CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
  

  //程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   blkNum;

	CString sqlstr="";
	CString v_proc_div= "";
	CString v_pract_rcv_flag= "";
	CDecimal  v_cut_slab = 0;
	CDecimal  v_cut_charge = 0;
	CString v_factory_div = "";
	CString v_station_id = "";
	CString v_acjc_relation_id = "";

    CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
			
	EIClass inBlock1;
	EIClass outBlock1;

     
  try
  {
		CPageInfo pageInfo;	

		/*数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
	CModel tmmsm41("TMMSM41");
	CModel tmmsm41a("TMMSM41A");
	CModel tmmsm00("TMMSM00");
			
		//初始化实体类
	
		tmmsm41.Reset();
		tmmsm41a.Reset();
		tmmsm00.Reset();

	
		//调用校验及公共处理函数,针对有共性的字段进行赋值

		if(!bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING,"TABLE_TYPE"); 
		}

		if (!bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "STATION_ID");
		}
		bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"] = "TMMSM41";

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
			v_station_id = "I";

		//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
		//Log::Trace("", __FUNCTION__, "v_factory_div=[{0}]", v_factory_div);
		//Log::Trace("", __FUNCTION__, "v_station_id=[{0}]", v_station_id);
		
		tmmsm41.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm41.TrimOrBlank();
		tmmsm41.CopyFrom(tmmsm00);

		tmmsm41a["HEAT_NO"] = tmmsm41["HEAT_NO"];

		//Log::Trace("", __FUNCTION__, "v_heatnoaaa_id=[{0}]", tmmsm41a["HEAT_NO"].ToString());

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
		bcls_rec->Tables["MMSMAC"].Rows[0]["TABLE_NAME"] = "TMMSM41";
		bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		bcls_rec->Tables["MMSMAC"].Rows[0]["FLAG"] = "0"; //0抛负数 1抛正数
		bcls_rec->Tables["MMSMAC"].Rows[0]["ACJC_RELATION_ID"] = dateNow + "0000";
		bcls_rec->Tables["MMSMAC"].Rows[0]["PROC_DIV"] = v_proc_div;
		//Log::Trace("", __FUNCTION__, "bcls_rec->MMSMAC->HEAT_NO=[{0}]", bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"].ToString().Trim());
		doFlag = f_mmsm_acyfl_tlcc(bcls_rec, bcls_ret, v_acjc_relation_id, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//抛帐End
					
		if(v_proc_div == "I")
		{
			tmmsm41.Insert();    
			v_pract_rcv_flag = "1";

		}
		else if (v_proc_div == "U")
		{
			tmmsm41.Delete(); 
			tmmsm41.Insert();    
			v_pract_rcv_flag = "1";

  		}
		else if (v_proc_div == "D")
		{
			tmmsm41.Delete(); 
			tmmsm41a.Delete("HEAT_NO");
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
		}
		bcls_rec->Tables["MMSMAC"].Rows[0]["TABLE_NAME"] = "TMMSM41";
		bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		bcls_rec->Tables["MMSMAC"].Rows[0]["FLAG"] = "1"; //0抛负数 1抛正数
		bcls_rec->Tables["MMSMAC"].Rows[0]["ACJC_RELATION_ID"] = dateNow + "0000";
		bcls_rec->Tables["MMSMAC"].Rows[0]["PROC_DIV"] = v_proc_div;
		//Log::Trace("", __FUNCTION__, "bcls_rec->MMSMAC->HEAT_NO=[{0}]", bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"].ToString().Trim());
		doFlag = f_mmsm_acyfl_tlcc(bcls_rec, bcls_ret, v_acjc_relation_id, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//抛帐End

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


		tmmsm00.MergeTo(bcls_rec->Tables["PSSM12"], false);
		bcls_rec->Tables["PSSM12"].Rows[0]["PRACT_RCV_FLAG"] = v_pract_rcv_flag;
		if (!bcls_rec->Tables["PSSM12"].Columns.Contains("STATION_ID"))
		{
			bcls_rec->Tables["PSSM12"].Columns.Add(DT_STRING, "STATION_ID");
		}
		bcls_rec->Tables["PSSM12"].Rows[0]["STATION_ID"] = v_station_id;

		doFlag = f_pssm12_mm_rcv(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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

