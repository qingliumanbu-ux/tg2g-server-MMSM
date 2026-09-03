/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2015-08-13
Version:1.0
Description: 炼钢板坯分段实绩
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

 
 


/* ***** 静态函数申明 ***** */

int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
/// <summary>
/// 炼钢板坯分段实绩
/// <para>
/// 炼钢板坯分段实绩
/// </para>
/// </summary>
/// <param name="tmmsm35">修改板坯精整实绩信息</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm34f6_cut)

int f_mmsm34f6_cut(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CDecimal cutNum = 0;
	CDecimal matTheoryWt = 0;
	int blkNum = 0;
	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";
	CModel tmmsm35("TMMSM35");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");

	try
	{
		

		if (bcls_rec->Tables.Contains("MM0099") == false)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"FUNC_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MAT_NO");
			bcls_rec->Tables["MM0099"].Rows.Add();
		}
		
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM15";
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm34f6_cut";
		

		tmmsm35.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		/* ***** 打印输入参数 ***** */
		EDLog(1, 1, "********************输出传入数据开始*******************");
		tmmsm35.Print();
		EDLog(1, 1, "********************输出传入数据结束*******************");

		tmmsm01["MAT_NO"] = tmmsm35["IN_MAT_NO"];
		tmmsm01.Query();
		matTheoryWt = tmmsm01["MAT_THEORY_WT"];


		if (tmmsm01["ORDER_NO"].ToString().Trim() != "")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}为合同材,不允许分段处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		cutNum = bcls_rec->Tables[0].Rows[0]["CUT_NUM"].ToDecimal();

		tmmsm35["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmssmsff").SubstringNE(0, 18);
		tmmsm35["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tmmsm35["PROD_TIME"] = tmmsm35["REC_CREATE_TIME"];
		tmmsm35["PROD_MAKER"] = s.userid;
		tmmsm35["REC_CREATOR"] = s.userid;
		tmmsm35["HEAT_NO"] = tmmsm01["HEAT_NO"];
		tmmsm35["PONO"] = tmmsm01["PONO"];
		tmmsm35["SG_SIGN"] = tmmsm01["SG_SIGN"];
		tmmsm35["ST_NO"] = tmmsm01["ST_NO"];
		tmmsm35["MAT_THICK"] = tmmsm01["MAT_ACT_THICK"];
		tmmsm35["MAT_WIDTH"] = tmmsm01["MAT_ACT_WIDTH"];
		tmmsm35["IN_HEAT_NO"] = tmmsm35["HEAT_NO"];
		tmmsm35["IN_PONO"] = tmmsm35["PONO"];
		tmmsm35["IN_SG_SIGN"] = tmmsm35["SG_SIGN"];
		tmmsm35["IN_ST_NO"] = tmmsm35["ST_NO"];
		tmmsm35["IN_MAT_THICK"] = tmmsm35["MAT_THICK"];
		tmmsm35["IN_MAT_WIDTH"] = tmmsm35["MAT_WIDTH"];
		
		tmmsm35["OP_DIV"] = "1";  //1：铸坯分段；2：方坯拆批；3：方坯并批
		//f_epep_get_shift_group("SM", tmmsm35["REC_CREATE_TIME"].ToString(), tmmsm35["PROD_SHIFT_NO"].ToString(), tmmsm35["PROD_SHIFT_GROUP"].ToString(), conn);
		f_epep_get_shift_group("SM", tmmsm35["REC_CREATE_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
		tmmsm35["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
		tmmsm35["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;
		for (int i = 1; i <= cutNum; i++)
		{
			tmmsm35["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO_" + CConvert::ToString(i)].ToString();
			tmmsm35["MAT_LEN"] = bcls_rec->Tables[0].Rows[0]["MAT_LEN_" + CConvert::ToString(i)].ToDecimal();
			tmmsm35["MAT_TUBE"] = bcls_rec->Tables[0].Rows[0]["MAT_TUBE_" + CConvert::ToString(i)].ToDecimal();
			tmmsm35["MAT_WT"] = bcls_rec->Tables[0].Rows[0]["MAT_WT_" + CConvert::ToString(i)].ToDecimal();
			tmmsm35.Insert();

			tmmsm01["MAT_NO"] = tmmsm35["MAT_NO"];
			tmmsm01["IN_MAT_NO"] = tmmsm35["IN_MAT_NO"];
			tmmsm01["MAT_ACT_LEN"] = tmmsm35["MAT_LEN"];
			tmmsm01["MAT_LEN"] = tmmsm35["MAT_LEN"];
			tmmsm01["MAT_NUM"] = tmmsm35["MAT_TUBE"];
			tmmsm01["MAT_ACT_WT"] = tmmsm35["MAT_WT"];
			tmmsm01["MAT_THEORY_WT"] = matTheoryWt / tmmsm35["IN_MAT_TUBE"].ToDecimal() / tmmsm35["IN_MAT_LEN"].ToDecimal() * tmmsm35["MAT_TUBE"].ToDecimal() * tmmsm35["MAT_LEN"];
			tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"].ToDecimal().Round(3);
			tmmsm01.Insert();

			bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
		}

		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM16";
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm34f6_cut";
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm35["IN_MAT_NO"];
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}


		/*发送MMS电文:炼钢板坯分切实绩*/
#if defined(_SYS_PES)

		blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
		if(blkNum < 0)
		{
				bcls_rec->Tables.Add("MMSMSND"); 
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TABLE_TYPE");
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING,"IN_MAT_NO");
			
		}
		
	//--------向MMS送电文函数----------------
						
		bcls_rec->Tables["MMSMSND"].Rows.Clear();

		if (bcls_rec->Tables["MMSMSND"].Rows.get_Count() <= 0)
			bcls_rec->Tables["MMSMSND"].Rows.Add();
		bcls_rec->Tables["MMSMSND"].Rows[0]["TABLE_TYPE"] = "MMSM35";			
		bcls_rec->Tables["MMSMSND"].Rows[0]["IN_MAT_NO"] = tmmsm35["IN_MAT_NO"];
	
		//doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}


	
#endif


		

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
