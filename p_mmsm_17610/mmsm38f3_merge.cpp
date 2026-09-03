/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    
Version:    1.0
Date:       2016-07-21
Description: 铸坯组批确认
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/
   



//外部函数声明
int f_qmts_cf_copy(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
#if defined _SYS_PES	//PES
//int f_cm_7000m6_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
int f_mmsm38f3_merge(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

// service入口
BM2F_ENTERACE(mmsm38f3_merge)

int f_mmsm38f3_merge(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int atFlag = 0;
	int i;
	int blkNum;
	int fetchRowCount;
	CString sqlstr = "";
	CString resume_seq_no = "";
	CString remark = "";
	CString factory_div = "";
	CString whole_backlog_code = "";
	CString max_heat_no = "";
	CString new_heat_no = "";
	CString pono = "";

	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm38("TMMSM38");
	CModel tqmts29("TQMTQQ0");

	CDbCommand cmd_inq(conn);

	EIClass bcls_rec_qmts;
	bcls_rec_qmts.Tables[0].set_TableName("QMTS");
	bcls_rec_qmts.Tables[0].Columns.Add(DT_STRING, "HEAT_NO_OLD");
	bcls_rec_qmts.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_qmts.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_qmts.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_qmts.Tables[0].Rows.Add();

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
		if (!bcls_rec->Tables.Contains("7000M6"))
		{
			bcls_rec->Tables.Add("7000M6");
		}
#endif

		resume_seq_no = CDateTime::Now().ToString("yyyyMMddHHmmss");

		remark = bcls_rec->Tables[0].Rows[0]["REMARK"].ToString();

		//根据规则获取炉号-----------------------------------------------------------------
		factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		whole_backlog_code = bcls_rec->Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"].ToString().Trim();
		new_heat_no = resume_seq_no.Substring(3, 2) + factory_div.Substring(1, 1) + whole_backlog_code;
		new_heat_no = new_heat_no.Trim();
		//Log::Trace("", "", "new_heat_no = [{0}]", new_heat_no);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = "SELECT TO_CHAR((NVL(SUBSTR(MAX(HEAT_NO), LENGTH(MAX(HEAT_NO)) - 4, 5), 0)+1), '00000') "
				"  FROM (SELECT HEAT_NO  "
				"          FROM TMMSM01 "
				"         WHERE MERGE_FLAG = 'M' "
				"         UNION ALL "
				"        SELECT HEAT_NO "
				"          FROM HMMSM01 "
				"         WHERE MERGE_FLAG = 'M') "
				"         WHERE HEAT_NO LIKE @new_heat_no||'%' ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("new_heat_no", new_heat_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			max_heat_no = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		//Log::Trace("", "", "max_heat_no = [{0}]", max_heat_no);

		max_heat_no = new_heat_no.Trim() + max_heat_no.Trim();

		//Log::Trace("", "", "max_heat_no = [{0}]", max_heat_no);
		//--------------------------------------------------------------------------------------

		//copy炉次成分信息----------------------------------------------------------------------
		tqmts29.MergeFrom(bcls_rec->Tables[1].Rows[0]);

		//Log::Trace("", "", "HEAT_NO_OLD = [{0}]", tqmts29["HEAT_NO"].ToString());

		bcls_rec_qmts.Tables[0].Rows[0]["HEAT_NO_OLD"] = tqmts29["HEAT_NO"];
		bcls_rec_qmts.Tables[0].Rows[0]["HEAT_NO"] = max_heat_no;
		bcls_rec_qmts.Tables[0].Rows[0]["PONO"] = max_heat_no;
		bcls_rec_qmts.Tables[0].Rows[0]["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();

		doFlag = f_qmts_cf_copy(&bcls_rec_qmts, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm01.Reset();
			tmmsm38.Reset();
			tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString();
			tmmsm01.Query();

			tmmsm38["REC_CREATOR"] = s.userid;
			tmmsm38["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm38["MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm38["PONO"] = max_heat_no;
			tmmsm38["PONO_OLD"] = tmmsm01["PONO"];
			tmmsm38["HEAT_NO"] = max_heat_no;
			tmmsm38["HEAT_NO_OLD"] = tmmsm01["HEAT_NO"];
			tmmsm38["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
			tmmsm38["ST_NO_OLD"] = tmmsm01["ST_NO"];
			tmmsm38["OP_DIV"] = "1";//"1":组；"2":撤
			tmmsm38["REMARK"] = remark;
			tmmsm38["RESUME_SEQ_NO"] = resume_seq_no;
			tmmsm38["FACTORY_DIV"] = factory_div;
			tmmsm38.TrimOrBlank();
			tmmsm38.Insert();

			tmmsm96["EVENT_ID"] = "MM46";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
			tmmsm96["FUNC_ID"] = "mmsm38f3_merge";
			tmmsm96["EVENT_DESC"] = "铸坯组批";
			tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm96["HEAT_NO"] = max_heat_no;
			tmmsm96["PONO"] = max_heat_no;
			tmmsm96["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
			if (tmmsm01["MERGE_FLAG"].ToString().Trim() == "M")    //判断之前是否组过，主档HEAT_NO_OLD保存最原始炉号
			{
				tmmsm96["OLD_HEAT_NO"] = tmmsm01["OLD_HEAT_NO"];
			}
			else
			{
				tmmsm96["OLD_HEAT_NO"] = tmmsm01["HEAT_NO"];
			}
			tmmsm96["MERGE_FLAG"] = "M";

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
		/*调用组批发送电文*/
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
