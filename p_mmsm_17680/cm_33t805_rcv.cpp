/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:sw
Date:2023-11-13
Version:1.0
Description: 资源库存信息接收
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */


// service入口
BM2F_ENTERACE_TELE(cm_33t805_rcv)

int f_cm_33t805_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CDbCommand cmd_inq(conn);
	CModel tmmsm57c("TMMSM57C");
	CModel tmmsm57c_m("TMMSM57C");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString datetime_s = CDateTime::Now().ToString("yyyyMMddHH");
	CDbCommand cmd_tfbsm(conn);
	CModel tfbsm57c("TFBSM57C");
	try
	{
		tmmsm57c.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if ("6221" == tmmsm57c["STOCK_CODE"].ToString().Trim())
		{
			strcpy(s.msg, "库存代码为南区!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ("" == tmmsm57c["MAT_CODE"].ToString().Trim())
		{
			strcpy(s.msg, "物料代码不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ("" == tmmsm57c["STAT_DATE"].ToString().Trim())
		{
			strcpy(s.msg, "统计日期不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sqlstr = " select mat_name from tmmsm50 "
			" where mat_code = @mat_code"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("mat_code", tmmsm57c["MAT_CODE"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm57c["MAT_NAME"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		tmmsm57c.Delete("MAT_CODE,STAT_DATE,STOCK_CODE");
		tmmsm57c["REC_CREATE_TIME"] = datetime;
		tmmsm57c["REC_CREATOR"] = s.userid;
		tmmsm57c.TrimOrBlank();
		tmmsm57c.Insert();

		// 取最新的更新为月库存
		tmmsm57c_m.Reset();
		tmmsm57c_m.CopyFrom(tmmsm57c);
		tmmsm57c_m["STAT_DATE"] = tmmsm57c["STAT_DATE"].ToString().SubstringNE(0, 6);
		tmmsm57c_m.Delete("MAT_CODE,STAT_DATE,STOCK_CODE");
		tmmsm57c_m.TrimOrBlank();
		tmmsm57c_m.Insert();
		/************************ 新增功能开始 ************************/
		// 1. 清空TFBSM57c表所有数据 ID_SJ
		Log::Trace("", __FUNCTION__, "inDMST02.datetime_s.sqlstr = [{0}]", datetime_s);
		sqlstr = " delete from TFBSM57C where DATE_TIME!='" + datetime_s + "' ";
		cmd_tfbsm.SetCommandText(sqlstr);
		cmd_tfbsm.ExecuteNonQuery();
		cmd_tfbsm.Close();
		//新增数据
		tfbsm57c.CopyFrom(tmmsm57c);
		tfbsm57c["DATE_TIME"] = datetime_s;
		tfbsm57c.Insert();
		/************************ 新增功能结束 ************************/

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
