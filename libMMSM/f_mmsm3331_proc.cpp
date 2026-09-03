/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-07-04
Description: 把33的产量同步到31表
***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件


/***** C++ 的业务头文件部分 *****/ 





//连铸铸坯产出处理函数

int f_mmsm3331_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


//外部函数声明

BM2_FUNCTION_EXPORT
 int f_mmsm3331_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm3331_proc";                //定义函数英文名称  
	CString FunctionCname = "炉次主档_信息修改";              //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
	  

	/* ***** 自定义变量 ***** */
	int  doFlag = 0;
	int  ret = 0;
	int  n_count = 0;
	int  blkNum = 0;

	CString c_factory_div = ""; //厂别区分
	CString c_datetime = "";          //当前时间
	CString c_heat_confm_flag = "";   //炉次确定标志
	CString updateColumns = "";

	CString sqlstr = "";
	CModel tmmsm31("TMMSM31");
	CModel hmmsm31("TMMSM31");
	CModel tmmsm01("TMMSM01");

	CDbCommand cmd_inq(conn);

	try
	{
		blkNum = bcls_rec->Tables.IndexOf("TMMSM31");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 TMMSM31 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		for (int i = 0; i < bcls_rec->Tables["TMMSM31"].Rows.get_Count(); i++)
		{
			tmmsm31.Reset();
			//EDLog(1, 1, "********************获取传入数据开始*******************");
			tmmsm31.MergeFrom(bcls_rec->Tables["TMMSM31"].Rows[i]);
			//EDLog(1, 1, "********************获取传入数据结束*******************");
			/* ***** 打印输入参数 ***** */
			//Log::Trace("", "", "HEAT_NO = [{0}]", tmmsm31["HEAT_NO"].ToString());
			//Log::Trace("", "", "PONO = [{0}]", tmmsm31["PONO"].ToString());
			if (tmmsm31["HEAT_NO"].ToString() == "")
			{
				CFormattable arguments[] = { (const char*)tmmsm01["MAT_NO"].ToString() };// 定义参数列表的数组
				strcpy(s.msg, "传入炉号为空");
				strcpy(s.sysmsg, s.msg);
				EDLog(1, 1, "传入炉号为空");
				return doFlag;
				//strcpy(s.sysmsg, s.msg);
				//throw CApplicationException(-1, s.msg, log.Location);
			}
			hmmsm31["HEAT_NO"] = tmmsm31["HEAT_NO"];
		
			if (!hmmsm31.Query("HEAT_NO"))
			{
				EDLog(1, 1, "炉号在主档中不存在");
				return doFlag;
				//CFormattable arguments[] = { (const char*)tmmsm01["MAT_NO"].ToString() };// 定义参数列表的数组
				//CMessageFormat::Format(s.msg, "炉号{0}在主档中不存在", arguments, 1);//格式化字符串
				//strcpy(s.sysmsg, s.msg);
				//throw CApplicationException(-1, s.msg, log.Location);
			}
			/* ***** 打印输入参数 ***** */
			/*EDLog(1, 1, "********************输出传入数据开始*******************");
			tmmsm33.Print();
			EDLog(1, 1, "********************输出传入数据结束*******************");*/

			cmd_inq.SetCommandText("SELECT count(1),sum(slab_wt),sum(mat_theory_wt),max(fin_st_no) FROM TMMSM33 WHERE HEAT_NO = @tmmsm31.HEAT_NO group by heat_no ");
			cmd_inq.Parameters.Set("tmmsm31.HEAT_NO", tmmsm31["HEAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm31["CUT_SLAB_NUM"] = cmd_inq.GetDecimal(1);
				tmmsm31["SLAB_WT"] = cmd_inq.GetDecimal(2);
				tmmsm31["MAT_THEORY_WT"] = cmd_inq.GetDecimal(3);
				tmmsm31["FIN_ST_NO"] = cmd_inq.GetString(4);
			}
			cmd_inq.Close();
			//Log::Trace("", "", "CUT_SLAB_NUM = [{0}]", tmmsm31["CUT_SLAB_NUM"].ToDecimal());
			//Log::Trace("", "", "SLAB_WT = [{0}]", tmmsm31["SLAB_WT"].ToDecimal());
			//Log::Trace("", "", "MAT_THEORY_WT = [{0}]", tmmsm31["MAT_THEORY_WT"].ToDecimal());
			//Log::Trace("", "", "FIN_ST_NO = [{0}]", tmmsm31["FIN_ST_NO"].ToString());
			
			updateColumns = "CUT_SLAB_NUM,SLAB_WT,MAT_THEORY_WT,FIN_ST_NO";

			//Log::Trace("", "", "updateColumns = [{0}]", updateColumns);

			n_count = tmmsm31.Update(updateColumns, "HEAT_NO");

			//Log::Trace("", "", "n_count = [{0}]", n_count);

			if (n_count <= 0)
			{
				sprintf(s.sysmsg,"炉号【%s】修改产出信息失败",tmmsm31["HEAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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


	return doFlag;

}
