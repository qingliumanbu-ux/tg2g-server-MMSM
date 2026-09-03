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

// service入口
BM2F_ENTERACE(mmsmfpmx_pro)

int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号


int f_mmsmfpmx_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_st_no = "";//出钢记号
	CString v_c_div = "";//碳锈区分  1  不锈钢  2碳钢
	CString v_operate = "";//操作区分	 I 新增    U 修改
	CString v_resume_seq_no = "";//序号

	CModel tmmsmfp("TMMSMFP");

	CDbCommand cmd_inq(conn);


	try
	{
		v_operate = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();
		v_c_div = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString().Trim();
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tmmsmfp.Reset();
			tmmsmfp.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (v_operate == "I")
			{
				doFlag = f_mm0011("TMMSM39_seq", 8, v_resume_seq_no, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsmfp["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
				tmmsmfp["REC_CREATOR"] = s.userid;
				tmmsmfp["REC_CREATE_TIME"] = datetime;
				tmmsmfp["C_DIV"] = v_c_div;
				tmmsmfp.Print();
				tmmsmfp.Insert();
			}
			else if (v_operate == "D")
			{
				tmmsmfp.Delete("RESUME_SEQ_NO");
			}
			else if (v_operate == "U")
			{
				tmmsmfp["REC_REVISOR"] = s.userid;
				tmmsmfp["REC_REVISE_TIME"] = datetime;
				tmmsmfp.Update("REC_REVISOR,REC_REVISE_TIME,PROD_TIME,PROD_SHIFT_GROUP,MAT_ACT_WIDTH,ST_NO,CUT_SCRAP_REASON,CUT_SCRAP_WT,MATERIAL_DESC,HEAT_NO1,HEAT_NO2,MAT_LEN_1,MAT_LEN_2,MAT_LEN,MATERIAL_DESC_FORE,MATERIAL_DESC_BACK,TOTAL_CYCLE,APP_CODE", "RESUME_SEQ_NO");
			}
		}

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
