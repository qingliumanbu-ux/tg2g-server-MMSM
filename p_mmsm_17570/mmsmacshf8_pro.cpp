/************************/
/*** 2023-11-13 ********/
/****   mfj **************/
/**** 后备功能-修改收货标记  当产销拒收后反馈为E失败的标记时，通过该功能将标记修正，以便现场可以继续操作 ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件

int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsmacshf8_pro)

int f_mmsmacshf8_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString v_rcv_mat_flag = "";//收货标记
	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");

	CDbCommand cmd_inq(conn);

	try
	{

		if (bcls_rec->Tables.Contains("MM0099") == false)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm01.Reset();
			tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
			v_rcv_mat_flag = bcls_rec->Tables[0].Rows[i]["RCV_MAT_FLAG"].ToString().Trim();

			if (v_rcv_mat_flag == "")
			{
				sprintf(s.msg, "收货标记不可为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tmmsm01.Query();//获取01表所有数据


			tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM3I";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "mmsmacshf8_pro";
			tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
			//tmmsm96["RCV_NO_STATUS"] = "S";收货取消状态   S 为不自动收货  N 为可自动收货
			
			tmmsm96["RCV_NO_STATUS"] = "S";
			tmmsm96["RCV_MAT_FLAG"] = v_rcv_mat_flag;
			tmmsm96["EVENT_DESC"] = "收货标记手动修正！";

			
			if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= i)
			{
				bcls_rec->Tables["MM0099"].Rows.Add();
			}
			bcls_rec->Tables["MM0099"].Rows[i].Merge(tmmsm96);

		}

		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
		{

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
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
