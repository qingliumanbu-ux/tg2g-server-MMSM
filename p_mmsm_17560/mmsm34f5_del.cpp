/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2015-8-13
Version:1.0
Description: 炼钢板坯修磨实绩删除
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
 

/***** C++ 的业务头文件部分 *****/ 




/* ***** 静态函数申明 ***** */

//修改板坯主档信息
int f_mmsm3401_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
int f_wmsm_t8p301_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

/*<remark>=========================================================
/// <summary>
/// 炼钢板坯修磨实绩删除
/// <para>
/// 删除炼钢板坯修磨实绩
/// </para>
/// </summary>
/// <param name="MAT_NO">材料号</param>
/// <param name="HSF_END_TIME">修磨结束时间</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm34f5_del)


int f_mmsm34f5_del(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cutFinFlag = "";
  
	CModel tmmsm34_1("TMMSM34_1");
	CModel tmmsm01("TMMSM01");

	try
	{
		CString matNo = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
		CString psq = bcls_rec->Tables[0].Rows[0]["PROD_SEQ_NO"].ToString();

		tmmsm34_1["MAT_NO"] = matNo;
		tmmsm34_1["PROD_SEQ_NO"] = psq;
		tmmsm34_1.Query();
		/* ***** 打印输入参数 ***** */
		tmmsm01["MAT_NO"] = tmmsm34_1["MAT_NO"];
		tmmsm01.Query();
		EIClass inblock;
		inblock.Tables[0].Columns.Add(tmmsm01);
		inblock.Tables[0].Rows.Clear();
		tmmsm01.MergeTo(inblock.Tables[0], false);
		doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
		if (doFlag < 0) {
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		

		Log::Trace("", "", "{0}", "---------开始---------");
		CString flag = tmmsm34_1["MEND_FLAG"].ToString();
		Log::Trace("", "", "mend_flag={0}", flag);		
		

		/*若当前主档信息等于之前修磨后实绩信息
		，则认为材料在修磨实绩形成到修磨实绩删除这段时间内未有变化
		，可以相应调整主档信息*/
		if (tmmsm01["MAT_LINE_TYPE"].ToString().Trim() == "SM"
			&& tmmsm01["PLAN_NO"].ToString().Trim() == "" 
			&& tmmsm34_1["MAT_ACT_THICK"].ToDecimal() == tmmsm01["MAT_ACT_THICK"].ToDecimal()
			&& tmmsm34_1["MAT_ACT_WIDTH"].ToDecimal() == tmmsm34_1["MAT_ACT_WIDTH"].ToDecimal()
			&& tmmsm34_1["MAT_ACT_LEN"].ToDecimal() == tmmsm34_1["MAT_ACT_LEN"].ToDecimal())
		{
			if (!bcls_rec->Tables.Contains("MMSM34"))
			{
				bcls_rec->Tables.Add("MMSM34");
			}
			if (!bcls_rec->Tables["MMSM34"].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables["MMSM34"].Columns.Add(DT_STRING, "PROC_DIV");
			}

			tmmsm34_1.MergeTo(bcls_rec->Tables["MMSM34"], false);
			bcls_rec->Tables["MMSM34"].Rows[0]["PROC_DIV"] = "D";/*I:新增 U:修改 D:删除*/


			doFlag = f_mmsm3401_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

		
			inblock.Tables[0].Rows.Clear();
			tmmsm01.Query("MAT_NO");
			tmmsm01.MergeTo(inblock.Tables[0], false);
			doFlag = f_wmsm_t8p301_snd(&inblock, bcls_ret, conn);
			if (doFlag < 0) {
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		else
		{
			strcpy(s.msg, "对修磨实绩进行删除必须保证材料状态没有变化，请核对材料为无计划且在库，规格没有发生过变化");
			strcpy(s.sysmsg, s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
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
