/***********************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 模铸脱模实绩增删改
*************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

 
 
#include <queue>


//外部函数声明


int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2_FUNCTION_EXPORT
int f_mmsm4501_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm4501_proc";                //定义函数英文名称  
	CString FunctionCname = "模铸脱模信息增删改";          //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   blkNum;

	CString sqlstr = "";
	CString v_proc_div = "";
	CString v_pract_rcv_flag = "";
	CDecimal  v_cut_slab = 0;
	CDecimal  v_cut_charge = 0;
	CString v_factory_div = "";
	CString v_station_id = "";
	CString v_ejection_flag = "";
	
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass inBlock1;
	EIClass outBlock1;


	try
	{
		CPageInfo pageInfo;

		/*数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
	CModel tmmsm45("TMMSM45");
	CModel tmmsm01("TMMSM96");
	CModel tmmsm33("TMMSM33");
	CModel tmmsm96("TMMSM96");

		//初始化实体类

		tmmsm45.Reset();

	
		/*如果是电文调用需要在电文接收service里对厂别和设备类型进行赋值*/
		if (bcls_rec->Tables["PARA"].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
			v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables["PARA"].Columns.Contains("FACTORY_DIV"))  //厂别
			v_factory_div = bcls_rec->Tables["PARA"].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables["PARA"].Columns.Contains("STATION_ID"))  //设备类型
			v_station_id = bcls_rec->Tables["PARA"].Rows[0]["STATION_ID"].ToString().TrimOrBlank().ToUpper();

		//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
		//Log::Trace("", __FUNCTION__, "v_factory_div=[{0}]", v_factory_div);
		//Log::Trace("", __FUNCTION__, "v_station_id=[{0}]", v_station_id);

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++){
			tmmsm45.Reset();
			tmmsm45.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm45.TrimOrBlank();

			tmmsm01["MAT_NO"] = tmmsm45["MAT_NO"];

			//取产出部分信息
			sqlstr = "SELECT STRAND_NO,INGOT_CODE "
				"  FROM TMMSM33 "
				"  WHERE MAT_NO 	=   @mat_no ";

			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Clear();
			cmd_sql.Parameters.Set("mat_no", tmmsm45["MAT_NO"].ToString());
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				tmmsm33["STRAND_NO"] = cmd_sql.GetString(1);
				tmmsm33["INGOT_CODE"] = cmd_sql.GetString(2);

			}
			cmd_sql.Close();
			//Log::Trace("", __FUNCTION__, "tmmsm01["MAT_NUM"] =[{0}]", tmmsm33["STRAND_NO"].ToString());
			//Log::Trace("", __FUNCTION__, "tmmsm01["UNIT_CODE"] =[{0}]", tmmsm33["INGOT_CODE"].ToString());

			tmmsm45["MOULD_NUM"] = tmmsm33["STRAND_NO"];
			tmmsm45["PLATE_NUM"] = tmmsm33["INGOT_CODE"];
			tmmsm45.Print();


			if (v_proc_div == "I")
			{
				tmmsm45.Insert();
				v_pract_rcv_flag = "1";
				v_ejection_flag = "9";
			}
			else if (v_proc_div == "U")
			{
				tmmsm45.Delete();
				tmmsm45.Insert();
				v_pract_rcv_flag = "1";
				v_ejection_flag = "9";

			}
			else if (v_proc_div == "D")
			{
				tmmsm45.Delete();
				v_pract_rcv_flag = " ";
				v_ejection_flag = "1";
			}

		

			//Log::Trace("", __FUNCTION__, ">>>>>>MM0099新增数据行");
			if (bcls_rec->Tables.Contains("MM0099") == false)
			{
				bcls_rec->Tables.Add("MM0099");
				bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
			}

			tmmsm96["MAT_NO"] = tmmsm45["MAT_NO"];
			tmmsm96["MAT_ACT_WT"] = tmmsm45["EJECTION_WT"];
			tmmsm96["MAT_WT"] = tmmsm45["EJECTION_WT"];
			tmmsm96["EJECTION_FLAG"] = v_ejection_flag;
			tmmsm96["EJECTION_TIME"] = tmmsm45["EJECTION_TIME"];
			tmmsm96["MEASURE_WT_FLAG"] = "1";
			tmmsm96["EVENT_ID"] = "MM61";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "f_mmsm4501_proc";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "模铸脱模实绩产出";

			if (i == 0 && bcls_rec->Tables["MM0099"].Rows.get_Count() > 0){
				bcls_rec->Tables["MM0099"].Rows.Clear();
			}
			int mm0099_count = bcls_rec->Tables["MM0099"].Rows.get_Count();
			bcls_rec->Tables["MM0099"].Rows.Add();
			bcls_rec->Tables["MM0099"].Rows[mm0099_count].Merge(tmmsm96);
		}


     
		//Log::Trace("", __FUNCTION__, "MM0099_ROW_COUNT =[{0}]", bcls_rec->Tables["MM0099"].Rows.get_Count());
		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
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

