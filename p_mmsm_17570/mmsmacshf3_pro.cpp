/************************/
/*** 2023-11-13 ********/
/****   mfj **************/
/**** 关闭自动收货 ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件

//int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsmacshf3_pro)

int f_mmsmacshf3_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;
	CString v_update = "";
	CString s_formname = "";
	CString i_func_id = "";
	CString i_func = "";
	CString v_fields_str = "";
	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSMW96");
	CModel tmmsm33zdsh("TMMSM33ZDSH");
	CDbCommand cmd_inq(conn);

	try
	{

		
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm33zdsh.Reset();//数据清空

			tmmsm33zdsh.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm33zdsh.Print();
			tmmsm33zdsh["REC_REVISOR"] = s.userid;
			tmmsm33zdsh["REC_REVISE_TIME"] = datetime;
			tmmsm33zdsh.Update("*", "STRAND_NO");

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
