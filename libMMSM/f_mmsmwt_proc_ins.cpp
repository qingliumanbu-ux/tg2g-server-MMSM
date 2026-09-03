/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   wzn
Version:    1.0
Date:     2018-01-31
Description: 铸坯单重参数录入
***********************************************************************/


#include "stdafx.h"
#include "epex.h"



int f_mmsm_unitwt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT


int f_mmsmwt_proc_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 变量定义 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString s_formname = "";
	CString v_fields_str = "";
	CString ingotCode = "";
	CString sgSign = "";
	CDecimal p_seq_id = 0;
	CString i_func_id = "";
	CString i_resume_seq_no= "";
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
			
			CDbCommand getSeq("SELECT MMSM_MATNO_SEQ.NEXTVAL  FROM DUAL", conn);
			CString newSeqNo = "0000" + getSeq.ExecuteScalar().ToString().Trim();
			newSeqNo = newSeqNo.Substring(newSeqNo.GetLength() - 4);
			tmmsmwt["RESUME_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss").Trim() + newSeqNo;
			//Log::Trace("", "", "tmmsmwt["RESUME_SEQ_NO"] =[{0}]", tmmsmwt["RESUME_SEQ_NO"].ToString());
			//Log::Trace("", "", "tmmsmwt["SEQ_ID"] =[{0}]", tmmsmwt["RESUME_SEQ_NO"].ToString());
			//Log::Trace("", "", "tmmsmwt["OPER_FLAG"] =[{0}]", tmmsmwt["OPER_FLAG"].ToString());
			i_resume_seq_no = tmmsmwt["RESUME_SEQ_NO"];
			if (tmmsmwt["OPER_FLAG"].ToString().Trim() == "I")
			{
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
				//tmmsmwt.Print();
				tmmsmwt.TrimOrBlank();

				/***********************这里需要判断是否有重复配置****************
				1、有配置传入，直接取配置
				2、通过画面名，查找配置代码
				3、如果是电文select t.*,t.rowid from tmm009a t where t.mat_kind = 'SM' AND EVENT_ID = 'MM9A';
				*****************************************************************/
				sqlstr = "SELECT PARA_NAME,PARA  FROM TMMSMPARA PROGRAM_NAME  = @s_formname and PARA_NAME = 'primary_key' ";
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
								//Log::Trace("", "", "执行状态：TMMSMWTB.Insert  ");
								sqlstr = "INSERT INTO TMMSMWTB WHERE OPER_FLAG = 'D' ";
								cmd_inq.SetCommandText(sqlstr);
								cmd_inq.ExecuteNonQuery();
								//Log::Trace("", "", "执行状态：tmmsmwt.Delete ");
								tmmsmwt_pre.Delete(v_fields_str);
								//Log::Trace("", "", "执行状态：tmmsmwt.Delete finish");
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
				//Log::Trace("", "", "执行状态：tmmsmwt.Insert [{0}]", tmmsmwt["SEQ_ID"].ToDecimal());
				tmmsmwt.Insert();
				//Log::Trace("", "", "执行状态：tmmsmwt.Insert Finish");
				tmmsmwtb.CopyFrom(tmmsmwt); //记录操作历史
				tmmsmwtb["OPER_FLAG"] = "I";
				tmmsmwtb["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tmmsmwtb["REC_CREATOR"] = s.userid;
				tmmsmwtb.Insert();
				//Log::Trace("", "", "执行状态：tmmsmwtb.Insert Finish");
			}
			else if (tmmsmwt["OPER_FLAG"].ToString().Trim() == "U")
			{
				tmmsmwt_pre["SEQ_ID"] = tmmsmwt["SEQ_ID"];
				tmmsmwt_pre.Query("SEQ_ID");

				tmmsmwtb.CopyFrom(tmmsmwt_pre); //记录操作历史
				tmmsmwtb["OPER_FLAG"] = "U";
				tmmsmwtb["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tmmsmwtb["REC_CREATOR"] = s.userid;
				tmmsmwtb.Insert();
				//Log::Trace("", "", "执行状态：tmmsmwtb.Insert Finish");

				tmmsmwt.Query("SEQ_ID");
				tmmsmwt.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tmmsmwt["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tmmsmwt["REC_REVISOR"] = s.userid;
				tmmsmwt["RESUME_SEQ_NO"] = i_resume_seq_no;
				tmmsmwt.Update("SEQ_ID");
			}
			else if (tmmsmwt["OPER_FLAG"].ToString().Trim() == "D")
			{
				tmmsmwt.Query("SEQ_ID");
				tmmsmwtb.CopyFrom(tmmsmwt); //记录操作历史
				tmmsmwtb["OPER_FLAG"] = "D";
				tmmsmwtb["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tmmsmwtb["REC_CREATOR"] = s.userid;
				tmmsmwtb.Insert();
				//Log::Trace("", "", "执行状态：tmmsmwtb.Insert Finish");
				tmmsmwt.Delete("SEQ_ID");
			}
			
		}
		
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


