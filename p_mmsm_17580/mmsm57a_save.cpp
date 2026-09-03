
/// <summary>
/// 功能说明:北区废钢导入
/// </summary>
#include "stdafx.h"

// Service 入口
BM2F_ENTERACE(mmsm57a_save)
int f_mmsm57a_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int affectRows = 0;
	CDecimal seq_id = 0;
	CString sqlstr = " ";
	CString v_table = "tmmsm57a";
	CString stat_date = "";
	CString v_proc_div = "";
	int i = 0;
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");	
	//CModel tmmsm57("TMMSM57C");
	CDbCommand cmd_inq(conn);

	try
	{	
		v_table = bcls_rec->Tables["PARA"].Rows[0]["TABLE_FLAG"].ToString().ToUpper();
		CModel tmmsm57(v_table);

		stat_date = bcls_rec->Tables["PARA"].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 8);

		
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
				tmmsm57.Reset();
				tmmsm57.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tmmsm57["STAT_DATE"] = tmmsm57["STAT_DATE"].ToString().SubstringNE(0, 8);
				if (tmmsm57["STAT_DATE"].ToString().Trim() == "")
				{
					tmmsm57["STAT_DATE"] = stat_date;
				}

				if (v_proc_div == "I")
				{ 
					
					if (i == 0)
					{
						if (v_table == "TMMSM57C" || v_table == "TMMSM57D" || v_table == "TMMSM57A")	 //资源铁区只保留最新一份数据
						{
							sqlstr = " delete from " + v_table +
								" where   1=1"
								" and stat_date like @stat_date||'%'"
								;
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Set("stat_date", stat_date.SubstringNE(0, 6));
							cmd_inq.ExecuteNonQuery();
							cmd_inq.Close();
						}

						sqlstr = " select max(SEQ_ID) from " + v_table +
							" where   1=1"
							" and stat_date =@stat_date"
							;
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("stat_date", tmmsm57["STAT_DATE"].ToString());
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							seq_id = cmd_inq.GetDecimal(1);
						}
						cmd_inq.Close();
					}

					tmmsm57["SEQ_ID"] = seq_id + i + 1;
					tmmsm57["REC_CREATOR"] = s.userid;
					tmmsm57["REC_CREATE_TIME"] = dateNow;
					tmmsm57.TrimOrBlank();
					tmmsm57.Insert();
				}
				else if (v_proc_div == "U")
				{
					if (v_table=="TMMSM57B")
					{
						tmmsm57["REC_REVISOR"] = s.userid;
						tmmsm57["REC_REVISE_TIME"] = dateNow;
						tmmsm57.Update("REC_REVISOR,REC_REVISE_TIME,STATUS,STOCK_ADJ_WT,REMARK", "STAT_DATE,MAT_CODE,SEQ_ID");
					}
					else
					{
						tmmsm57["REC_REVISOR"] = s.userid;
						tmmsm57["REC_REVISE_TIME"] = dateNow;
						tmmsm57.Update("*", "STAT_DATE,MAT_CODE,SEQ_ID");
					} 					
				}
				else if (v_proc_div == "D")
				{
						tmmsm57.Delete("STAT_DATE,MAT_CODE,SEQ_ID");
					
				}
			}
		
		
		

		

	}
	catch (CDbException& ex)  //捕获数据库操作异常 
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.sysmsg, (const char*)ex.GetMsg(), sizeof(s.sysmsg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


