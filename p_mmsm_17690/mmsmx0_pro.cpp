/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wcm
Version:     1.0
Date:        2024-09-27
Description: 设备检修记录维护
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(mmsmx0_pro)

int f_mmsmx0_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sqlstr_inq = " ";
	CString msgstr = "提示信息:";	//提示信息
	CString s_userid("");
	CString	datetime("");
	CDateTime errorstarttime;
	CDateTime errorendtime;
	CDateTime planstarttime;
	CDateTime planendtime;
	CModel ttmsm201("TMMSMX0");
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);
	int proc_sum = 0;				//操作总数
	CString v_operate = "";
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		if (bcls_rec->Tables[0].Columns.Contains("PRO_DIV"))
		{
			v_operate = bcls_rec->Tables[0].Rows[0]["PRO_DIV"].ToString().Trim();
			Log::Trace("", "", "条件查询v_operate[{0}],", v_operate);
		}
		
		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD"].Rows.get_Count());

			Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);

			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				

				Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);
				ttmsm201.Reset();
				ttmsm201.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				
				ttmsm201["REC_CREATOR"] = s.userid;			//记录创建责任者
				
				ttmsm201["REC_CREATE_TIME"] = datetime;		//记录创建时刻
				ttmsm201["FINISH_TIME"] = datetime;		//记录创建时刻
				CDecimal st_seq_no = 1;
				sqlstr_inq = "SELECT nvl(MAX(SEQ_NO),0) FROM TMMSMX0";
				Log::Trace("", __FUNCTION__, "str_sql_inq		= [{0}]", sqlstr_inq);
				cmd.SetCommandText(sqlstr_inq);
				cmd.ExecuteReader();
				if (cmd.Read())
				{
					st_seq_no = cmd.GetDecimal(1) + 1;
				}
				ttmsm201["SEQ_NO"] = st_seq_no;		//序号
				proc_sum += ttmsm201.Insert();
			}

		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				
					ttmsm201.Reset();
					ttmsm201.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
					Log::Trace("", __FUNCTION__, "这里1", "");

					
					ttmsm201["REC_REVISOR"] = s.userid;			//记录修改责任者
					//ttmsm201["REPAIR_WRITER"] = s.username;     //录入人员
					ttmsm201["REC_REVISE_TIME"] = datetime;		//记录修改时刻
					ttmsm201["FINISH_TIME"] = datetime;		//记录创建时刻
					Log::Trace("", __FUNCTION__, "这里2", "");
					proc_sum += ttmsm201.Update("REC_REVISOR,REC_REVISE_TIME,FINISH_TIME,MEMO_DETAIL", "SEQ_NO");
					Log::Trace("", __FUNCTION__, "这里3", "");

			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				ttmsm201.Reset();
				ttmsm201.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);

				sqlstr = "DELETE FROM TMMSMX0";
			    proc_sum += ttmsm201.Delete();
				
			}
		}
		msgstr += msgstr.Format("%d条记录操作成功。", proc_sum);
		strncpy(s.msg, (const char*)msgstr, sizeof(s.msg) - 1);

	}


	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
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


