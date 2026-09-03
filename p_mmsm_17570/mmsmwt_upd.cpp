//框架头文件
#include "stdafx.h"
//程序用头文件


// service入口
BM2F_ENTERACE(mmsmwt_upd)
int f_mmsm_unitwt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_mmsmwt_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	CModel tmmsmwt("TMMSMWT");
	CModel tmmsmwtb("TMMSMWTB");


	CDbCommand cmd_inq(conn);

	try
	{
		doFlag = f_mmsm_unitwt(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		tmmsmwt.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		//if (tmmsmwt["RESUME_SEQ_NO"].ToString().Trim() == "")
		//{
		//	sprintf(s.msg, "履历序号不能为空");
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		//tmmsmwt["WT_PER_METER"] = (tmmsmwt.TOTAL_NET_WT / tmmsmwt.TOTAL_TUBE / (tmmsmwt["SLAB_LEN"].ToDecimal() / 1000)).Round(3);

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
		//Log::Trace("", "", "tmmsmwt["SEQ_ID"] =[{0}]", tmmsmwt["SEQ_ID"].ToDecimal());


		s_formname = s.formname;
		//Log::Trace("", "", "formname=[{0}]", s_formname);

		//根据是否弹出，获取修改参数
		sqlstr = "SELECT PARA_NAME,PARA  FROM TMMSMPARA PROGRAM_NAME  = @s_formname and PARA_NAME = 'pop_flag' ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("s_formname", s_formname);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			i_func = cmd_inq.GetString(2);
			//Log::Trace("", "", "获取配置参数 i_func=[{0}]", i_func);
		}
		cmd_inq.Close();
		//获取修改参数配置名
		if (i_func.Trim() == "Y")
		{
			sqlstr = "SELECT PARA_NAME,PARA  FROM TMMSMPARA PROGRAM_NAME  = @s_formname and PARA_NAME = 'func_id_p' ";
		}
		else
		{
			sqlstr = "SELECT PARA_NAME,PARA  FROM TMMSMPARA PROGRAM_NAME  = @s_formname and PARA_NAME = 'func_id' ";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("s_formname", s_formname);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			i_func_id = cmd_inq.GetString(2);
			//Log::Trace("", "", "获取配置参数 i_func_id=[{0}]", i_func_id);
		}
		cmd_inq.Close();

		//获取修改列
		sqlstr = "SELECT ITEM_ENAME FROM TED54 WHERE FUNC_ID = @i_func_id and FORM_EDIT_FLAG = '1' AND ITEM_ENAME!='SEQ_ID' ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("i_func_id", i_func_id);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			v_fields_str += cmd_inq.GetString(1);
			v_fields_str += ",";
		}
		cmd_inq.Close();
		if (v_fields_str.Trim()>"")v_fields_str = v_fields_str.SubstringNE(0, v_fields_str.GetLength() - 1);
		//Log::Trace("", "", "v_fields_str=[{0}]", v_fields_str);

		if (v_fields_str.Trim() > "")
		{
			tmmsmwt["REC_REVISE_TIME"] = datetime;
			tmmsmwt["REC_REVISOR"] = s.userid;
			//v_update = "CC_MACH_NO,SLAB_TYPE,WT_PER_METER,"
			//	"SLAB_THICK,SLAB_WIDTH,SLAB_LEN,TOTAL_NET_WT,TOTAL_TUBE,REC_REVISE_TIME,REC_REVISOR";
			//tmmsmwt.Update(v_update, "RESUME_SEQ_NO");
			tmmsmwt.Update(v_fields_str, "SEQ_ID");
			//Log::Trace("", "", "执行状态：tmmsmwt.Update Finish");
			tmmsmwtb.CopyFrom(tmmsmwt); //记录操作历史
			tmmsmwtb["OPER_FLAG"] = "U";
			tmmsmwtb["REC_REVISE_TIME"] = datetime;
			tmmsmwtb["REC_REVISOR"] = s.userid;
			tmmsmwtb.Insert();
			//Log::Trace("", "", "执行状态：tmmsmwtb.Insert Finish");
		}
	
		//Log::Trace("", "", "执行状态：tmmsmwtb.Insert continue");

		

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
