/************************/
/*** 2023-11-13 ********/
/****   mfj **************/
/**** 是否修磨和修磨原因录入   ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件

int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

// service入口
BM2F_ENTERACE(mmsmacshf7_pro)

int f_mmsmacshf7_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	int blkNum = 0;
	//系统的分页类信息。
	CPageInfo pageInfo;


	CModel tmmsm01("TMMSM01");
	CModel tmmsm01_query("TMMSM01");
	CModel tmmsm96("TMMSM96");

	CDbCommand cmd_inq(conn);

	try
	{
		Log::Trace("", __FUNCTION__, "get_Count[{0}]  ", bcls_rec->Tables[0].Rows.get_Count());

		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}




		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm01.Reset();
			tmmsm96.Reset();
			tmmsm01_query.Reset();

			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			/*tmmsm01_query.MergeFrom(bcls_rec->Tables[0].Rows[i]);*/
			if (!tmmsm01.QueryCount("MAT_NO"))//TMMSM01表未获取数据
			{
				Log::Trace("", __FUNCTION__, "MAT_NO[{0}]  ", tmmsm01["MAT_NO"].ToString());
				strcpy(s.sysmsg, "在线档中不存在数据，请确认数据是否已归档!");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			/*tmmsm01_query.Query("MAT_NO");

			if (tmmsm01_query["LSLAB_NO"].ToString().Trim() != "")
			{
				strcpy(s.sysmsg, "该材料不是余材，不能填写余材原因!");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}*/

			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
			//tmmsm96["REMAINDER_REASON"] = tmmsm01["REMAINDER_REASON"];
			tmmsm96["EVENT_ID"] = "MM41";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "mmsmacshf7_pro";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "铸坯是否修磨和修磨原因录入";

			bcls_rec->Tables["MM0099"].Rows.Add();
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
