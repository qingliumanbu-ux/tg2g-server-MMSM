/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsmwt_ins)
int f_mmsm_unitwt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_mmsmwt_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString s_formname = "";
	CString v_fields_str = "";
	CString ingotCode = "";
	CString sgSign = "";
	CDecimal p_seq_id = 0;
	CString i_func_id = "";
	CModel tmmsmwt("TMMSMWT");
	CModel tmmsmwt_pre("TMMSMWT");
	CModel tmmsmwtb("TMMSMWTB");
	CDbCommand cmd_inq(conn);
	try
	{
		doFlag = f_mmsm_unitwt(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		s_formname = s.formname;
		//Log::Trace("", "", "formname=[{0}]", s_formname);

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsmwt.MergeFrom(bcls_rec->Tables[0].Rows[i]);

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
			//Log::Trace("", "", "tmmsmwt["RESUME_SEQ_NO"] =[{0}]", tmmsmwt["RESUME_SEQ_NO"].ToString());
			

			/*****2018-1-24 wzn 变更主键为seq_id  start*******/
			CDbCommand getSeq1("SELECT max(seq_id)+1  FROM tmmsmwt", conn);
			p_seq_id = getSeq1.ExecuteScalar();
			if (p_seq_id.ToString().GetLength() > 18)p_seq_id = 0;
			tmmsmwt["SEQ_ID"] = p_seq_id;
			//Log::Trace("", "", "SEQ_ID=[{0}]", tmmsmwt["SEQ_ID"].ToDecimal());
			for (int s = 0; s++;)
			{
				if (tmmsmwt.QueryCount("SEQ_ID") > 0)
				{
					tmmsmwt["SEQ_ID"] = tmmsmwt["SEQ_ID"].ToDecimal() + 1;
				}
				else
				{
					//Log::Trace("", "", "fin_SEQ_ID=[{0}]", tmmsmwt["SEQ_ID"].ToDecimal());
				}
			}
			/*****2018-1-24 wzn 变更主键为seq_id  end*******/
			tmmsmwt["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsmwt["REC_CREATOR"] = s.userid;
			tmmsmwt.Print();
			tmmsmwt.TrimOrBlank();

			/***********************这里需要判断是否有重复配置****************
			1、有配置传入，直接取配置
			2、通过画面名，查找配置代码
			3、如果是电文select t.*,t.rowid from tmm009a t where t.mat_kind = 'SM' AND EVENT_ID = 'MM9A';
			*****************************************************************/
			sqlstr = "SELECT PARA_NAME,PARA  FROM TMMSMPARA WHERE PROGRAM_NAME  = @s_formname and PARA_NAME = 'primary_key' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("s_formname", s_formname);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				i_func_id = cmd_inq.GetString(2);
				//Log::Trace("", "", "获取配置参数 i_func_id=[{0}]", i_func_id);
			}
			cmd_inq.Close();

			if (bcls_rec->Tables[0].Columns.Contains("I_FUNC_ID"))
			{
				i_func_id = bcls_rec->Tables[0].Rows[0]["I_FUNC_ID"];
				//Log::Trace("", "", "获取传递参数 i_func_id=[{0}]", i_func_id);
			}

			if (i_func_id.Trim() > "")
			{
				sqlstr = "SELECT ITEM_ENAME FROM TED54 WHERE FUNC_ID = @i_func_id and item_key_flag = '1' ";
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
					int para_num = tmmsmwt.QueryCount(v_fields_str);

					if (para_num > 0)//说明有重复的
					{
						//Log::Trace("", "", "para_num=[{0}]", para_num);
						tmmsmwt_pre.CopyFrom(tmmsmwt);
						if (para_num > 1)
						{
							tmmsmwt_pre["OPER_FLAG"] = "D";
							tmmsmwt_pre.Update("OPER_FLAG", v_fields_str);
							sqlstr = "INSERT INTO TMMSMWTB WHERE OPER_FLAG = 'D' ";
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.ExecuteNonQuery();
							tmmsmwt_pre.Delete(v_fields_str);
						}
						else
						{
							tmmsmwt_pre.Query(v_fields_str);
							tmmsmwt["SEQ_ID"] = tmmsmwt_pre["SEQ_ID"];   //单记录重复，直接用原有ID
							tmmsmwtb.CopyFrom(tmmsmwt_pre); //记录操作历史
							tmmsmwtb["OPER_FLAG"] = "D";
							tmmsmwtb["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
							tmmsmwtb["REC_CREATOR"] = s.userid;
							//Log::Trace("", "", "执行状态：tmmsmwt.INSERT SEQ_ID [{0}]", tmmsmwtb["SEQ_ID"].ToDecimal());
							tmmsmwtb.Insert();
							//Log::Trace("", "", "执行状态：tmmsmwt.Delete SEQ_ID [{0}]", tmmsmwt_pre["SEQ_ID"].ToDecimal());
							tmmsmwt_pre.Delete("SEQ_ID");
							//Log::Trace("", "", "执行状态：Tmmsmwt.DELETE Finish");
						}

					}
				}
			}

			/***********************这里需要判断是否有重复配置 end***********************/


			//tmmsmwt.Delete("INGOT_CODE,SG_SIGN");

			tmmsmwt.Insert();
			tmmsmwtb.CopyFrom(tmmsmwt); //记录操作历史
			tmmsmwtb["OPER_FLAG"] = "I";
			tmmsmwtb["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsmwtb["REC_CREATOR"] = s.userid;
			tmmsmwtb.Insert();
			//Log::Trace("", "", "执行状态：tmmsmwtb.Insert Finish");


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
