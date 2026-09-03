/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    
Version:    1.0
Date:       2016-07-21
Description: 铸坯组批取消
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/
   
  
  

//外部函数声明
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
#if defined _SYS_PES	//PES
//int f_cm_7000m6_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif
int f_mmsm38f4_unmerge(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

// service入口
BM2F_ENTERACE(mmsm38f4_unmerge)

int f_mmsm38f4_unmerge(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int atFlag = 0;
	int i;
	int blkNum;
	int fetchRowCount;
	CString sqlstr = "";
	CString resumeSeqNo = "";
	CString remark = "";

	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm38("TMMSM38");
	CModel hmmsm38("TMMSM38");

	CDbCommand tmmsm38_inq(conn);

	try
	{
		/* ***** 数据块定义区 start ***** */
		if (!bcls_rec->Tables.Contains("MM0099"))
		{
			bcls_rec->Tables.Add("MM0099");
		}
		bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);

		if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
		{
			bcls_rec->Tables["MM0099"].Rows.Add();
		}
#if defined _SYS_PES	//PES
		if (bcls_rec->Tables["7000M6"].Rows.get_Count() <= 0)
		{
			bcls_rec->Tables["7000M6"].Rows.Add();
		}
#endif
		resumeSeqNo = CDateTime::Now().ToString("yyyyMMddHHmmss");

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = "SELECT * "
				"  FROM TMMSM38 "
				" WHERE MAT_NO = @matNo "
				"   AND RESUME_SEQ_NO = "
				"     (SELECT MAX(RESUME_SEQ_NO) "
				"        FROM TMMSM38 "
				"       WHERE OP_DIV = '1' "
				"         AND MAT_NO = @matNo "
				"         AND HEAT_NO = @heatNo) ";
			break;
		}
		tmmsm38_inq.SetCommandText(sqlstr);

		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm01.Reset();
			tmmsm38.Reset();
			hmmsm38.Reset();
			tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString();
			tmmsm01.Query("MAT_NO");
			if (tmmsm01["MERGE_FLAG"].ToString().Trim() != "M")
			{
				CFormattable arguments[] = { (const char*)tmmsm01["MAT_NO"].ToString() };// 定义参数列表的数组
				CMessageFormat::Format(s.msg, "材料[{0}]未经过组批，不能组批取消", arguments, 1);//格式化字符串
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tmmsm38_inq.Parameters.Set("matNo", tmmsm01["MAT_NO"].ToString());
			tmmsm38_inq.Parameters.Set("heatNo", tmmsm01["HEAT_NO"].ToString());
			tmmsm38_inq.ExecuteReader();
			if (tmmsm38_inq.Read())
			{
				tmmsm38_inq.Fetch(hmmsm38);
				tmmsm38_inq.Close();
			}
			else
			{
				tmmsm38_inq.Close();
				CFormattable arguments[] = { (const char*)tmmsm01["MAT_NO"].ToString() };// 定义参数列表的数组
				CMessageFormat::Format(s.msg, "材料[{0}]未找到组批记录，不能组批取消", arguments, 1);//格式化字符串
				//s.msg = "材料" + tmmsm01["MAT_NO"].ToString() + "未找到组批记录，不能组批取消";
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tmmsm38["REC_CREATOR"] = s.userid;
			tmmsm38["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm38["MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm38["PONO"] = hmmsm38["PONO_OLD"];
			tmmsm38["PONO_OLD"] = tmmsm01["PONO"];
			tmmsm38["HEAT_NO"] = hmmsm38["HEAT_NO_OLD"];
			tmmsm38["HEAT_NO_OLD"] = tmmsm01["HEAT_NO"];
			tmmsm38["ST_NO"] = hmmsm38["ST_NO_OLD"];
			tmmsm38["ST_NO_OLD"] = tmmsm01["ST_NO"];
			tmmsm38["OP_DIV"] = "2";//"1":组；"2":撤
			tmmsm38["RESUME_SEQ_NO"] = resumeSeqNo;

			tmmsm38.Insert();

			tmmsm96["EVENT_ID"] = "MM47";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
			tmmsm96["FUNC_ID"] = "mmsm38f4_unmerge";
			tmmsm96["EVENT_DESC"] = "铸坯组批取消";
			tmmsm96["MAT_NO"] = tmmsm38["MAT_NO"];
			tmmsm96["HEAT_NO"] = tmmsm38["HEAT_NO"];
			tmmsm96["PONO"] = tmmsm38["PONO"];
			tmmsm96["ST_NO"] = tmmsm38["ST_NO"];
			//Log::Trace("", "", "tmmsm96["HEAT_NO"] = [{0}]", tmmsm96["HEAT_NO"].ToString());
			//Log::Trace("", "", "tmmsm01["OLD_HEAT_NO"] = [{0}]", tmmsm01["OLD_HEAT_NO"].ToString());
			if (tmmsm96["HEAT_NO"].ToString() == tmmsm01["OLD_HEAT_NO"].ToString())
			{
				tmmsm96["MERGE_FLAG"] = "0";
			}
			else
			{
				tmmsm96["MERGE_FLAG"] = tmmsm01["MERGE_FLAG"];
			}
			bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				//获取返回系统变量
				throw CApplicationException(-1, s.msg, log.Location);

			}
#if defined _SYS_PES	//PES
			tmmsm96.MergeTo(bcls_rec->Tables["7000M6"], false);
#endif
		}
#if defined _SYS_PES	//PES
		/*调用组批取消 发送电文*/
		//doFlag = f_cm_7000m6_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			//获取返回系统变量
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
