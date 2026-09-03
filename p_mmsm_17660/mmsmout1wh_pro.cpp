/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include<regex>

/***** C++ 的业务头文件部分 *****/


/* ***** 静态函数申明 ***** */

// service入口
//修磨初磨外弧维护

BM2F_ENTERACE(mmsmout1wh_pro)


int f_mmsmout1wh_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString c_sql_condition = "";

	try
	{
		//获取传入参数
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		Log::Trace("", "", "获取传入参数...");

		Log::Trace("", "", "当前时间：nowTime=[{0}]", nowTime);

		CModel tmmsmout1wh("TMMSMOUT1WH");

		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("PRO"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["PRO"].Rows.get_Count());
			
			for (int i = 0; i < bcls_rec->Tables["PRO"].Rows.get_Count(); i++)
			{
				tmmsmout1wh.Reset();
				tmmsmout1wh.MergeFrom(bcls_rec->Tables["PRO"].Rows[i]);
				tmmsmout1wh.TrimOrBlank();
				
				if (tmmsmout1wh["MAT_NO"].ToString().Trim().GetLength() == 0)
				{
					strcpy(s.msg, "材料号不能为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				else
				{
					Log::Trace("", "", tmmsmout1wh["MAT_NO"].ToString().Trim());
					if (tmmsmout1wh.Query("MAT_NO")){
						sprintf(s.msg, "材料号[{0}]已存在!", (const char*)tmmsmout1wh["MAT_NO"].ToString());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					else{
						Log::Trace("", "", tmmsmout1wh["MAT_NO"].ToString().Trim());
						tmmsmout1wh["REC_CREATOR"] = s.userid;
						tmmsmout1wh["REC_CREATE_TIME"] = nowTime;
						tmmsmout1wh["MAT_NO"] = bcls_rec->Tables["PRO"].Rows[i]["MAT_NO"].ToString().Trim();
						tmmsmout1wh["ST_NO"] = bcls_rec->Tables["PRO"].Rows[i]["ST_NO"].ToString().Trim();
						tmmsmout1wh["OPERATOR"] = bcls_rec->Tables["PRO"].Rows[i]["OPERATOR"].ToString().Trim();
						tmmsmout1wh["SLAB_CUT_TIME"] = bcls_rec->Tables["PRO"].Rows[i]["SLAB_CUT_TIME"].ToString().Trim();
						tmmsmout1wh["MAT_ACT_LEN"] = bcls_rec->Tables["PRO"].Rows[i]["MAT_ACT_LEN"].ToDecimal();
						tmmsmout1wh["MAT_ACT_WIDTH"] = bcls_rec->Tables["PRO"].Rows[i]["MAT_ACT_WIDTH"].ToDecimal();
						tmmsmout1wh["MAT_ACT_THICK"] = bcls_rec->Tables["PRO"].Rows[i]["MAT_ACT_THICK"].ToDecimal();
						tmmsmout1wh["MAT_ACT_WT"] = bcls_rec->Tables["PRO"].Rows[i]["MAT_ACT_WT"].ToDecimal();
						tmmsmout1wh.TrimOrBlank();
						proc_sum += tmmsmout1wh.Insert();
					}

				}

				Log::Trace("", "", "1获取传入参数...");

				//sqlstr = "INSERT INTO TMMSMOUT1WH";

			}
		}
		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());

			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				tmmsmout1wh.Reset();
				tmmsmout1wh.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				tmmsmout1wh.TrimOrBlank();

				if (tmmsmout1wh.QueryCount("MAT_NO")>0)
				{
					Log::Trace("", "", tmmsmout1wh["MAT_NO"].ToString().Trim());
					tmmsmout1wh["REC_REVISOR"] = s.userid;
					tmmsmout1wh["REC_REVISE_TIME"] = nowTime;
					tmmsmout1wh["MAT_NO"] = bcls_rec->Tables["UPD"].Rows[i]["MAT_NO"].ToString().Trim();
					tmmsmout1wh["ST_NO"] = bcls_rec->Tables["UPD"].Rows[i]["ST_NO"].ToString().Trim();
					tmmsmout1wh["OPERATOR"] = bcls_rec->Tables["UPD"].Rows[i]["OPERATOR"].ToString().Trim();
					tmmsmout1wh["SLAB_CUT_TIME"] = bcls_rec->Tables["UPD"].Rows[i]["SLAB_CUT_TIME"].ToString().Trim();
					tmmsmout1wh["MAT_ACT_LEN"] = bcls_rec->Tables["UPD"].Rows[i]["MAT_ACT_LEN"].ToDecimal();
					tmmsmout1wh["MAT_ACT_WIDTH"] = bcls_rec->Tables["UPD"].Rows[i]["MAT_ACT_WIDTH"].ToDecimal();
					tmmsmout1wh["MAT_ACT_THICK"] = bcls_rec->Tables["UPD"].Rows[i]["MAT_ACT_THICK"].ToDecimal();
					tmmsmout1wh["MAT_ACT_WT"] = bcls_rec->Tables["UPD"].Rows[i]["MAT_ACT_WT"].ToDecimal();
					tmmsmout1wh.TrimOrBlank();
					proc_sum += tmmsmout1wh.Update("REC_REVISOR,REC_REVISE_TIME,ST_NO,OPERATOR,SLAB_CUT_TIME,MAT_ACT_LEN,MAT_ACT_WIDTH,MAT_ACT_THICK,MAT_ACT_WT", "MAT_NO");
				}

				Log::Trace("", "", "2获取传入参数...");


			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				tmmsmout1wh.Reset();
				tmmsmout1wh.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);

				if (!tmmsmout1wh.Query("MAT_NO")){
					sprintf(s.msg, "未找到材料号[{0}],无法删除!", (const char*)tmmsmout1wh["MAT_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//sqlstr = "DELETE FROM TMMSMOUT1WH";
				proc_sum += tmmsmout1wh.Delete("MAT_NO");
			}
		}

		msgstr += msgstr.Format("%d条记录操作成功。", proc_sum);
		strncpy(s.msg, (const char*)msgstr, sizeof(s.msg) - 1);

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