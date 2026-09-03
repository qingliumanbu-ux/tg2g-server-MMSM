/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:sw
Date:2023-11-13
Version:1.0
Description: 资源及时库存信息接收
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */


// service入口
BM2F_ENTERACE_TELE(cm_33t806_rcv)

int f_cm_33t806_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CDbCommand cmd_inq(conn);
	CModel tmmsm57e("TMMSM57E");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tfbsm57e("TFBSM57E");
	CDbCommand cmd_tfbsm(conn);

	try
	{
		tmmsm57e.MergeFrom(bcls_rec->Tables[0].Rows[0]);  		

		sqlstr = " select mat_name from tmmsm50 "
			" where mat_code = @mat_code"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("mat_code", tmmsm57e["MAT_CODE"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm57e["MAT_NAME"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close(); 		
		tmmsm57e["REC_CREATE_TIME"] = datetime;
		tmmsm57e["REC_CREATOR"] = s.userid;
		tmmsm57e.TrimOrBlank();
		tmmsm57e.Insert(); 		

		/************************ 新增功能开始 ************************/
		// 1. 清空TFBSM57E表所有数据 ID_SJ
		CString id_sj = bcls_rec->Tables[0].Rows[0]["id_sj"].ToString();
		sqlstr = " delete from TFBSM57E where id_sj!='"+id_sj+"' ";
		cmd_tfbsm.SetCommandText(sqlstr);
		cmd_tfbsm.ExecuteNonQuery();
		cmd_tfbsm.Close();
		//新增数据
		tfbsm57e.CopyFrom(tmmsm57e);
		tfbsm57e.Insert();
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
