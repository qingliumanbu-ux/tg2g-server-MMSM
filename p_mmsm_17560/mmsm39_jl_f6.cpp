/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2011-12-13
Version:1.0
Description: 炼钢板坯组批管理
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/




/* ***** 静态函数申明 ***** */

//修改板坯主档信息

/*<remark>=========================================================
/// <summary>
/// 炼钢板坯切废管理
/// <para>
/// 炼钢板坯切废管理
/// </para>
///		分切后的切废量先用分切前的坯号数据，并保留分切时人工录入的切废长度和切废重量
////
///
/////// 当坯子处于封锁状态时，用户仍可操作，但不对主档表更新，不发送电文给产销
/// 存入待办事项表中，等待封锁取消后，统一处理
///
/// </summary>
/// <param name="">炼钢板坯切废管理</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm39_jl_f6)


int f_mmsm39_jl_f6(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_count1 = "";
	int  n_count = 0;
	CString cutFinFlag = "";
	int mat_seq = 0;
	int mat_tube = 0;
	int fetchRowCount = 0;
	CString vcf_heat_no = "";//代表成分熔炼号
	CString v_remark = "";//备注
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_st_no = "";//出钢记号
	CString v_c_div = "";//碳锈区分  1  不锈钢  2碳钢
	CString new_heat_no = "";
	int count_heat_no = 0;
	CString v_operate = "";//操作区分	 I 新增    U 修改
	CString v_resume_seq_no = "";//序号
	CString v_sap_erp_matnr = "";//物料编码
	CString v_mat_no = "";
	CString v_prod_shift_no = "";
	CString v_prod_shift_group = "";
	CString v_recut_group = "";
	CDecimal v_cut_scrap_wt = 0;

	
	CModel tmmsm39("TMMSM39");
	
	CDbCommand cmd_inq(conn);


	try
	{
		tmmsm39.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm39["REC_REVISOR"] = s.userid;
		tmmsm39["REC_REVISE_TIME"] = datetime;
		tmmsm39.TrimOrBlank();
		tmmsm39.Update("REC_REVISOR,REC_REVISE_TIME,RECUT_DATE,CUT_BEFORE_LEN,CUT_AFTER_LEN,OTHER_CUT_LEN,CUT_BEFORE_WT,CUT_AFTER_WT,CUT_SCRAP_WT,CUTTING_TYPE,GRINDING_FLAG", "RESUME_SEQ_NO,MAT_NO");

		


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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


	return doFlag;

}
