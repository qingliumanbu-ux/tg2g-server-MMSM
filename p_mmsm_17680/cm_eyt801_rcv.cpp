/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:songwei
Date:2023-11-28
Version:1.0
Description: 接收上料
质量数据**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */


// service入口
BM2F_ENTERACE_TELE(cm_eyt801_rcv)

int f_cm_eyt801_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CString div_flag = "";
	CString matPlmsCode = "";
	CString matCode = "";
	CString SeqNo = "";
	CModel tmmsm83("TMMSM83");
	CModel tmmsm60("TMMSM60");
	CDbCommand cmd_inq(conn);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{

		tmmsm83.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm83["MSG_ID"] = bcls_rec->Tables[0].Rows[0]["ID"].ToString();
		tmmsm83["BUNKER_NO_ORIGINAL"] = bcls_rec->Tables[0].Rows[0]["L_BIN_NO"].ToString();
		tmmsm83["BUNKER_NO"] = bcls_rec->Tables[0].Rows[0]["BIN_A"].ToString();
		tmmsm83["PROD_SHIFT_GROUP"] = bcls_rec->Tables[0].Rows[0]["CLASS"].ToString();
		tmmsm83["TIME_STAMPS"] = bcls_rec->Tables[0].Rows[0]["TIMESTAMPS"].ToString();

		if (tmmsm83["BUNKER_NO_ORIGINAL"].ToString().Trim()!="")
		{
			tmmsm60.Reset();
			tmmsm60["BUNKER_NO"] = tmmsm83["BUNKER_NO_ORIGINAL"];
			tmmsm60.Query("BUNKER_NO");
			tmmsm83["BUNKER_NAME_ORIGINAL"] = tmmsm60["BUNKER_NAME"];
		}

		if (tmmsm83["BUNKER_NO"].ToString().Trim() != "")
		{
			tmmsm60.Reset();
			tmmsm60["BUNKER_NO"] = tmmsm83["BUNKER_NO"];
			tmmsm60.Query("BUNKER_NO");
			tmmsm83["BUNKER_NAME"] = tmmsm60["BUNKER_NAME"];
		}
		

		//确认关键字是否为接口上传主键，
		if ("" == tmmsm83["MSG_ID"].ToString().Trim())
		{
			strcpy(s.msg, "ID关键字为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ("" == tmmsm83["BUNKER_NO_ORIGINAL"].ToString().Trim() || "" == tmmsm83["BUNKER_NO"].ToString().Trim())
		{
			strcpy(s.msg, "低位或高位料仓为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm83["PROD_SHIFT_GROUP"].ToString() == "甲")tmmsm83["PROD_SHIFT_GROUP"] = "A";
		else if (tmmsm83["PROD_SHIFT_GROUP"].ToString() == "乙")tmmsm83["PROD_SHIFT_GROUP"] = "B";
		else if (tmmsm83["PROD_SHIFT_GROUP"].ToString() == "丙")tmmsm83["PROD_SHIFT_GROUP"] = "C";
		else if (tmmsm83["PROD_SHIFT_GROUP"].ToString() == "丁")tmmsm83["PROD_SHIFT_GROUP"] = "D";

		//根据地位料仓取当时的物料编码和物料名称
		sqlstr = " select mat_code,mat_name,WEIGH_NO,SEQ_NO,QUALITY_BATCH_NO from tmmsm85 "
			" where 1=1 "
			" and  BUNKER_NO =@bunker_no_original and QUALITY_BATCH_NO != ' ' "
			" order by SEQ_NO  "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("bunker_no_original", tmmsm83["BUNKER_NO_ORIGINAL"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm83["MAT_CODE"] = cmd_inq.GetString(1);
			tmmsm83["MAT_NAME"] = cmd_inq.GetString(2);
			tmmsm83["WEIGH_NO"] = cmd_inq.GetString(3);
			tmmsm83["SEQ_NO"] = cmd_inq.GetDecimal(4);
			tmmsm83["QUALITY_BATCH_NO"] = cmd_inq.GetString(5);
		}
		cmd_inq.Close();

		sqlstr = "  SELECT TRIM(LPAD(TO_CHAR(MMLC_T83.NEXTVAL),20 ))  FROM DUAL ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			SeqNo = cmd_inq.GetString(1).Trim();

		}
		cmd_inq.Close();

		tmmsm83["SEQ_CODE"] = SeqNo;
		if (tmmsm83["MAT_CODE"].ToString().Trim() == "")    //无则取最后一次上料的料仓的物料编码
		{
			sqlstr = " select mat_code,mat_name from tmmsm60 "
				" where 1=1 "
				" and  BUNKER_NO =@bunker_no_original"
			
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no_original", tmmsm83["BUNKER_NO_ORIGINAL"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm83["MAT_CODE"] = cmd_inq.GetString(1);
				tmmsm83["MAT_NAME"] = cmd_inq.GetString(2);
			}
			cmd_inq.Close();
		}
		
		tmmsm83["FACTORY_DIV"] = "LG1";
		tmmsm83["REC_CREATE_TIME"] = datetime;
		tmmsm83["REC_CREATOR"] = s.userid;
		tmmsm83.Insert();

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
