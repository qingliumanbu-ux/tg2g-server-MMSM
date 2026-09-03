/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2013
Author:      顾云峰
Version:     1.0
Date:        2015-1-30
Description: 生成目的板坯号
**************************************************/
/*<remark>============================================================================
/// <summary>
/// 生成目的板坯号逻辑(需定制修改!!!)
/// <para>
/// 1.
/// 2.
/// </para>
/// <para>数据库表：                </para>
/// </summary>
/// <param name="">                                                           </param>
/// <returns>                                                               </returns>
============================================================================</remark>*/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"

/* ***** 程序表结构引用 ***** */
#include "tmmsm03.h"
#include "hmmsm03.h"

//外部函数声明
BM2_FUNCTION_EXPORT
int f_mmsm0008_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int atFlag = 0;
	int i;
	int blkNum;
	int fetchRowCount;
	int i_count = 0;
	CString  datetime = "";

	CString matNo = " ";
	CDecimal i_cut_seq = 0;
	CDecimal i_cut_num = 0;

	CString c_mat_no_a = "";
	CString c_mat_no_b = "";
	int i_asc = 0;

	int i_div_num = 0;

	CString error_seq = " ";
	CString matDivCode = "";

	int  v_count = 0;


	//使用的表结构变量
	CModel tmmsm03("TMMSM03");
	CModel hmmsm03("HMMSM03");
	//CTMMSM03 tmmsm03(conn);
	//CHMMSM03 hmmsm03(conn);

	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("MMSM0008");
		if (blkNum < 0)
		{
			strcpy(s.msg, "系统出现异常，请联系系统维护人员。");
			strcpy(s.sysmsg, "传入数据块 MMSM0008 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//获取传入参数
		matNo = bcls_rec->Tables["MMSM0008"].Rows[0]["MAT_NO"].ToString();
		i_cut_num = bcls_rec->Tables["MMSM0008"].Rows[0]["CUT_NUM"];
		i_cut_seq = bcls_rec->Tables["MMSM0008"].Rows[0]["CUT_SEQ"];
		Log::Trace("", __FUNCTION__, "MAT_NO = [{0}]", matNo);
		Log::Trace("", __FUNCTION__, "CUT_NUM = [{0}]", i_cut_num);
		Log::Trace("", __FUNCTION__, "CUT_SEQ = [{0}]", i_cut_seq);

		/*校验传入参数*/
		if (matNo.Trim() == "")
		{
			strcpy(s.msg, _RES("GCRSS0000035")/*材料号不能为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//安刚定制生成目的板坯号 2021年12月17日 LINGGU 
		//规则：最后两位流水号
		//当厂坯末两位为01~09，则按10 20分切；当是10时,则转换为A0; 11->B0 ;12->C0  ; 13->D0 14->E0进行分切 
		//暂时不考虑板转坯情况；不考虑多次分切情况
		CString aim_mat_no_tmp = "";
		/*if (matNo.Substring(matNo.GetLength() - 2, 2) == "01")
		{
			aim_mat_no_tmp = "10";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "02")
		{
			aim_mat_no_tmp = "20";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "03")
		{
			aim_mat_no_tmp = "30";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "04")
		{
			aim_mat_no_tmp = "40";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "05")
		{
			aim_mat_no_tmp = "50";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "06")
		{
			aim_mat_no_tmp = "60";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "07")
		{
			aim_mat_no_tmp = "70";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "08")
		{
			aim_mat_no_tmp = "80";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "09")
		{
			aim_mat_no_tmp = "90";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "0A")
		{
			aim_mat_no_tmp = "A0";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "0C")
		{
			aim_mat_no_tmp = "C0";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "0D")
		{
			aim_mat_no_tmp = "D0";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "0E")
		{
			aim_mat_no_tmp = "E0";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "0F")
		{
			aim_mat_no_tmp = "F0";
		}
		else if (matNo.Substring(matNo.GetLength() - 2, 2) == "0G")
		{
			aim_mat_no_tmp = "G0";
		}
		else
		{
			strcpy(s.msg, "长坯号异常，暂时无法分割，请联系开发人员！");
			throw CApplicationException(-1, s.msg, log.Location);
		}*/


		//太钢定制   材料号10位+2位流水

		//c_mat_no_a = aim_mat_no_tmp.Substring(0, 1);  //第一位用变更后的第一位
		//c_mat_no_b = i_cut_seq.ToString();  //用切割顺序号替代

		//matNo = matNo.Substring(0, matNo.GetLength() - 2) + c_mat_no_a + c_mat_no_b;
		matNo = matNo + i_cut_seq.ToString() + "0";//普通倍尺分切只有9支  mfj  20240226

		Log::Trace("", __FUNCTION__, "ERROR matNo = [{0}]", matNo);
		bcls_rec->Tables["MMSM0008"].Rows[0]["MAT_NO"] = matNo;

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