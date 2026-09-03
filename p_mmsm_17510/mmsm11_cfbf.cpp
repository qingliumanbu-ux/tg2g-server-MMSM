/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-06-03 17:13:56
Description: 炉次确定
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */

/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// 炉次确定
/// <para>
/// 1.炉次确定。
///
/// </para>
/// <para>数据库表：TPSSM11(出钢计划表)					</para>
/// <para>主调用函数：前台PSSM91画面F3(新增)按钮				</para>
/// </summary>
/// <param name=" ">     </param>
/// <param name=" ">                </param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口
int f_21b004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);		//发送铁区MES收料确认实绩

BM2F_ENTERACE(mmsm11_cfbf)

int f_mmsm11_cfbf(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CModel tmmsm11("TMMSM11");
	CString cs_ticode = "";
	CString cs_st_sample_no = "";
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_cf;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_cf_inq(conn);
	try
	{
		sqlstr = " 					 SELECT TICODE, REMARK from TMMSM11_COPY_BF WHERE REMARK<>' '  ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while(cmd_inq.Read())
		{
			cs_ticode = cmd_inq.GetString(1);
			cs_st_sample_no = cmd_inq.GetString(2);
			tmmsm11["TICODE"] = cs_ticode;
			tmmsm11.Query("TICODE");
			Log::Trace("", __FUNCTION__, "TAPNO = [{0}]", tmmsm11["TAPNO"].ToString());
			sqlstr = "SELECT 'I' DEAL_FLAG,"
				"@TAPNO TCP_NO,"
				"@TPC_ID TPC_SEQ,"
				"@TPC_YL_NO TPC_NO,"
				"ST_SAMPLE_NO SAMPLE_NO,"
				"'TS0000' MAT_CODE,"
				"'普通铁水' MAT_CNAME,"
				"DEV_CODE SAMPLE_POS_CODE,"
				"SAMPLE_TAKEN_TIME SAMPLE_TIME,"
				"ANALYSE_TIME,"
				"7 ANALYSE_ITEM_NUM,"
				"DECODE(ANALYSE_ITEM_NAME,"
				"'C',"
				"'Y001',"
				"'Si',"
				"'Y002',"
				"'Mn',"
				"'Y003',"
				"'P',"
				"'Y004',"
				"'S',"
				"'Y005',"
				"'Ti',"
				"'Y006',"
				"'V',"
				"'Y028',"
				"ANALYSE_ITEM_NAME) ANALYSE_ITEM_CODE,"
				"ANALYSE_ITEM_NAME,"
				"'Y' ANALYSE_DATA_TYPE,"
				"ANALYSE_ITEM_VALUE "
				"FROM(SELECT T.ST_SAMPLE_NO,"
				"T.DEV_CODE,"
				"T.SAMPLE_TAKEN_TIME,"
				"T.ANALYSE_TIME,"
				"T.ELM_001 \"C\","
				"T.ELM_002 \"Si\","
				"T.ELM_003 \"Mn\","
				"T.ELM_004 \"P\","
				"T.ELM_005 \"S\","
				"T.ELM_013 \"Ti\","
				"T.ELM_012 \"V\" "
				"FROM TQMTS24 T "
				"WHERE T.ST_SAMPLE_NO = @ST_SAMPLE_NO) UNPIVOT(\"ANALYSE_ITEM_VALUE\" FOR \"ANALYSE_ITEM_NAME\" IN(\"C\","
				"\"Si\","
				"\"Mn\","
				"\"P\","
				"\"S\","
				"\"V\","
				"\"Ti\"))";
			cmd_cf_inq.SetCommandText(sqlstr);
			cmd_cf_inq.Parameters.Set("TAPNO", tmmsm11["TAPNO"]);
			cmd_cf_inq.Parameters.Set("TPC_ID", tmmsm11["TPC_ID"]);
			cmd_cf_inq.Parameters.Set("TPC_YL_NO", tmmsm11["TPC_YL_NO"]);
			cmd_cf_inq.Parameters.Set("ST_SAMPLE_NO", cs_st_sample_no);
			bcls_ret->Tables[0].Clear();
			cmd_cf_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_cf_inq.Close();
			if (bcls_ret->Tables[0].Rows.get_Count() > 0){
				if (f_21b004_snd(bcls_rec, bcls_ret, conn)){
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}


		}
		cmd_inq.Close();
		

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
