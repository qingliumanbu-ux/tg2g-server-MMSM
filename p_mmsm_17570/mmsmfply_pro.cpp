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
BM2F_ENTERACE(mmsmfply_pro)

int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号


int f_mmsmfply_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	//CString v_st_no = "";//出钢记号
	//CString v_c_div = "";//碳锈区分  1  不锈钢  2碳钢
	CString v_operate = "";//操作区分	 I 新增    U 修改
	CString v_resume_seq_no = "";//序号

	CModel tmmsmfply("TMMSMFPLY");

	CDbCommand cmd_inq(conn);


	try
	{
		v_operate = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();
		Log::Info("", __FUNCTION__, "v_operate =[{0}]", v_operate);

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tmmsmfply.Reset();
			tmmsmfply.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (v_operate == "I")
			{
				doFlag = f_mm0011("TMMSM39_seq", 8, v_resume_seq_no, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsmfply["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
				tmmsmfply["REC_CREATOR"] = s.userid;
				tmmsmfply["REC_CREATE_TIME"] = datetime;
				tmmsmfply["REC_REVISOR"] = " " ;
				tmmsmfply["REC_REVISE_TIME"] = " ";
				tmmsmfply.Print();
				tmmsmfply.Insert();
			}
			else if (v_operate == "D")
			{
				tmmsmfply.Delete("RESUME_SEQ_NO");
				Log::Info("", __FUNCTION__, "v_operate111 =[{0}]", v_operate);
			}
			else if (v_operate == "U")
			{
				Log::Info("", __FUNCTION__, "v_operate222 =[{0}]", v_operate);

				tmmsmfply["REC_REVISOR"] = s.userid;
				tmmsmfply["REC_REVISE_TIME"] = datetime;
				tmmsmfply.Update("REC_REVISOR,REC_REVISE_TIME,MAT_ACT_WIDTH,MAT_ACT_LEN,MAT_ACT_THICK,MAT_ACT_WT,SCRAP_ORIGIN", "RESUME_SEQ_NO");
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
