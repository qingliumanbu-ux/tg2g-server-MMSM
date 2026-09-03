/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsmwt_del)

int f_mmsmwt_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CDecimal Count = 0;



	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsmwt("TMMSMWT");
	CModel tmmsmwtb("TMMSMWTB");


	CDbCommand cmd_inq(conn);

	try
	{
		tmmsmwtb.MergeFrom(bcls_rec->Tables[0].Rows[0]); //记录操作历史
		tmmsmwtb["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tmmsmwtb["REC_CREATOR"] = s.userid;
		tmmsmwtb["OPER_FLAG"] = "D";

		tmmsmwt.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		//Log::Trace("", "", "tmmsmwt["SEQ_ID"] =[{0}]", tmmsmwt["SEQ_ID"].ToDecimal());
		tmmsmwt.Query("SEQ_ID");
		//if (tmmsmwt["RESUME_SEQ_NO"].ToString().Trim() == "")
		//{
		//	sprintf(s.msg, "履历序号不能为空");
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//	sqlstr = "SELECT COUNT(1) FROM TMMSMWT WHERE 1=1";
		//	break;
		//}
		//cmd_inq.SetCommandText(sqlstr);
		//
		//Count = cmd_inq.ExecuteScalar();

		//if (Count < 1)
		//{
		//	sprintf(s.msg, "该库区不存在，无需删除。");
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			sqlstr = "SELECT nextval for MMSM_MATNO_SEQ FROM TMMSM25 ";
			break;
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT MMSM_MATNO_SEQ.NEXTVAL  FROM DUAL";
			break;
		}
		CDbCommand getSeq(conn);
		getSeq.SetCommandText(sqlstr);
		CString newSeqNo = "0000" + getSeq.ExecuteScalar().ToString().Trim();
		newSeqNo = newSeqNo.Substring(newSeqNo.GetLength() - 4);
		tmmsmwt["RESUME_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss").Trim() + newSeqNo;
		//Log::Trace("", "", "RESUME_SEQ_NO=[{0}]", tmmsmwt["RESUME_SEQ_NO"].ToString());
		
		tmmsmwt.Delete("SEQ_ID");
		//Log::Trace("", "", "执行状态：tmmsmwt.Delete Finish");
		tmmsmwtb.CopyFrom(tmmsmwt); //记录操作历史
		tmmsmwtb["OPER_FLAG"] = "D";
		tmmsmwtb["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tmmsmwtb["REC_REVISOR"] = s.userid;
		tmmsmwtb.Insert();
		//Log::Trace("", "", "执行状态：tmmsmwtb.Insert Finish");

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
