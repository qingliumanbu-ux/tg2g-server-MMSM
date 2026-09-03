/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/


/* ***** 静态函数申明 ***** */

// service入口

//int f_mmsm50_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn); 

BM2F_ENTERACE(mmsmnx2_pro)


int f_mmsmnx2_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   n_count = 0;
	int   blkNum;
	int  proc_sum = 0;				//操作总数
	CDbCommand cmd_sql(conn); //与DB 建立连接。
	CString msgstr = "提示信息:";	//提示信息。
	CString sqlstr = "";
	CString sqlstr0 = "";
	CString sqlstr1 = "";
	CString c_sql_condition = "";
	CDecimal seq_id = 0;

	try
	{
		//获取传入参数
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		Log::Trace("", "", "获取传入参数...");

		Log::Trace("", "", "当前时间：nowTime=[{0}]", nowTime);

		CModel tmmsmnx("TMMSMNX02");

		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("PRO"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["PRO"].Rows.get_Count());
			sqlstr0 = " select nvl(max(t.rec_id),0) as ini_id from tmmsmnx02 t";
			cmd_sql.SetCommandText(sqlstr0);
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				seq_id = cmd_sql.GetDecimal(1);
			}
			for (int i = 0; i < bcls_rec->Tables["PRO"].Rows.get_Count(); i++)
			{
				tmmsmnx.Reset();
				tmmsmnx.MergeFrom(bcls_rec->Tables["PRO"].Rows[i]);
				tmmsmnx.TrimOrBlank();

				

				if (tmmsmnx["MAT_CODE"].ToString() == "AT000385" || tmmsmnx["MAT_CODE"].ToString() == "AB069987")
				{
					if (tmmsmnx["FEED_WT"].ToString()>0)
					{
						strcpy(s.msg, "库存不能为空!");
						if (tmmsmnx["NI_ACTUAL"].ToString().Trim().GetLength() == 0)
						{
							strcpy(s.msg, "原带镍不能为空!");
						}
						if (tmmsmnx["MANUFAC_NAME"].ToString().Trim().GetLength() == 0)
						{
							strcpy(s.msg, "厂家不能超过为空!");
						}
					}
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsmnx["REC_ID"].ToString().Trim().GetLength() == 0)
				
				{
					tmmsmnx["REC_ID"] = seq_id + i + 1;
					tmmsmnx["REC_CREATOR"] = s.userid;
					tmmsmnx["REC_CREATE_TIME"] = nowTime;
					tmmsmnx["NI_ACTUAL"] = bcls_rec->Tables["PRO"].Rows[i]["NI_ACTUAL"].ToDecimal();
					tmmsmnx["MANUFAC_NAME"] = bcls_rec->Tables["PRO"].Rows[i]["MANUFAC_NAME"].ToString().Trim();
					tmmsmnx["FEED_WT"] = bcls_rec->Tables["PRO"].Rows[i]["FEED_WT"].ToDecimal();
					tmmsmnx.TrimOrBlank();
					proc_sum += tmmsmnx.Insert();
				}
				else
				{
					if (tmmsmnx.Query("REC_ID"))
					{
						tmmsmnx["REC_ID"] = seq_id + i + 1;
						tmmsmnx["REC_REVISOR"] = s.userid;
						tmmsmnx["REC_REVISE_TIME"] = nowTime;
						tmmsmnx["NI_ACTUAL"] = bcls_rec->Tables["PRO"].Rows[i]["NI_ACTUAL"].ToDecimal();
						tmmsmnx["MANUFAC_NAME"] = bcls_rec->Tables["PRO"].Rows[i]["MANUFAC_NAME"].ToString().Trim();
						tmmsmnx["FEED_WT"] = bcls_rec->Tables["PRO"].Rows[i]["FEED_WT"].ToDecimal();
						tmmsmnx.TrimOrBlank();
						proc_sum += tmmsmnx.Insert();

					}
				}
				Log::Trace("", "", "3获取传入参数...");

				sqlstr = "INSERT INTO TMMSMNX02";
				

			}


		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			sqlstr0 = " select nvl(max(t.rec_id),0) as ini_id from tmmsmnx02 t";
			cmd_sql.SetCommandText(sqlstr0);
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				seq_id = cmd_sql.GetDecimal(1);
			}
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				tmmsmnx.Reset();
				tmmsmnx.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", bcls_rec->Tables["UPD"].Rows[i]["MAT_CODE"].ToString());
				tmmsmnx.TrimOrBlank();
				if (tmmsmnx["MAT_CODE"].ToString() == "AT000385" || tmmsmnx["MAT_CODE"].ToString() == "AB069987")
				{
					if (tmmsmnx["FEED_WT"].ToString()>0)
					{
						strcpy(s.msg, "原带镍不能为空!");
						if (tmmsmnx["NI_ACTUAL"].ToString().Trim().GetLength() == 0)
						{
							strcpy(s.msg, "原带镍不能为空!");
						}
						if (tmmsmnx["MANUFAC_NAME"].ToString().Trim().GetLength() == 0)
						{
							strcpy(s.msg, "厂家不能超过为空!");
						}
					}
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (tmmsmnx["REC_ID"].ToString().Trim().GetLength()>0)
				{
					tmmsmnx["REC_CREATOR"] = s.userid;
					tmmsmnx["REC_CREATE_TIME"] = nowTime;
					tmmsmnx["REC_ID"] = bcls_rec->Tables["UPD"].Rows[i]["REC_ID"].ToString().Trim();
					tmmsmnx["NI_ACTUAL"] = bcls_rec->Tables["UPD"].Rows[i]["NI_ACTUAL"].ToDecimal();
					tmmsmnx["MANUFAC_NAME"] = bcls_rec->Tables["UPD"].Rows[i]["MANUFAC_NAME"].ToString().Trim();
					tmmsmnx["FEED_WT"] = bcls_rec->Tables["UPD"].Rows[i]["FEED_WT"].ToDecimal();
					tmmsmnx.TrimOrBlank();
					proc_sum += tmmsmnx.Update("REC_REVISOR,REC_REVISE_TIME,NI_ACTUAL,MANUFAC_NAME,FEED_WT", "REC_ID");
				}
				else
				{
					tmmsmnx["REC_ID"] = seq_id + i + 1;
					tmmsmnx["REC_REVISOR"] = s.userid;
					tmmsmnx["REC_REVISE_TIME"] = nowTime;
					tmmsmnx["NI_ACTUAL"] = bcls_rec->Tables["UPD"].Rows[i]["NI_ACTUAL"].ToDecimal();
					tmmsmnx["MANUFAC_NAME"] = bcls_rec->Tables["UPD"].Rows[i]["MANUFAC_NAME"].ToString().Trim();
					tmmsmnx["FEED_WT"] = bcls_rec->Tables["UPD"].Rows[i]["FEED_WT"].ToDecimal();
					tmmsmnx.TrimOrBlank();
					
					proc_sum += tmmsmnx.Insert();
				}

			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				tmmsmnx.Reset();
				tmmsmnx.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);

				//if (tmmsm50.QueryCount(condition) != 1){
				if (!tmmsmnx.Query("REC_ID")){
					////Log::Debug("", __FUNCTION__, "未找到第{0}条记录，无法删除。", i + 1);
					msgstr += msgstr.Format("未找到第%d条记录，无法删除。", i + 1);
					continue;
				}
				sqlstr = "DELETE FROM TMMSMNX02";
				proc_sum += tmmsmnx.Delete("REC_ID");
			}
		}

		msgstr += msgstr.Format("%d条记录操作成功。", proc_sum);
		strncpy(s.msg, (const char*)msgstr, sizeof(s.msg) - 1);

		/*doFlag = f_mmsm50_proc(bcls_rec, bcls_ret,conn);
		if (doFlag < 0)
		{
		throw CApplicationException(-1, s.msg, s.svc_name);
		}*/

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