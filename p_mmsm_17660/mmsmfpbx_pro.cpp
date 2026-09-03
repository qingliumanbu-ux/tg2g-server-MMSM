/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include<regex>

/***** C++ 的业务头文件部分 *****/


/* ***** 静态函数申明 ***** */

// service入口
//修磨初磨外弧维护

BM2F_ENTERACE(mmsmfpbx_pro)


int f_mmsmfpbx_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

		CModel tmmsmfp_bx("TMMSMFP_BX");

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());

			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				tmmsmfp_bx.Reset();
				tmmsmfp_bx.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				tmmsmfp_bx.TrimOrBlank();

				if (tmmsmfp_bx.QueryCount("HEAT_NO")>0)
				{
					Log::Trace("", "", tmmsmfp_bx["HEAT_NO"].ToString().Trim());
					tmmsmfp_bx["REC_REVISOR"] = s.userid;
					tmmsmfp_bx["REC_REVISE_TIME"] = nowTime;
					tmmsmfp_bx["HEAT_NO"] = bcls_rec->Tables["UPD"].Rows[i]["HEAT_NO"].ToString().Trim();
					//////////
					tmmsmfp_bx["ST_NO"] = bcls_rec->Tables["UPD"].Rows[i]["ST_NO"].ToString().Trim();
					tmmsmfp_bx["OPERATOR"] = bcls_rec->Tables["UPD"].Rows[i]["OPERATOR"].ToString().Trim();
					tmmsmfp_bx["SLAB_CUT_TIME"] = bcls_rec->Tables["UPD"].Rows[i]["SLAB_CUT_TIME"].ToString().Trim();
					tmmsmfp_bx["MAT_ACT_LEN"] = bcls_rec->Tables["UPD"].Rows[i]["MAT_ACT_LEN"].ToDecimal();
					tmmsmfp_bx["MAT_ACT_WIDTH"] = bcls_rec->Tables["UPD"].Rows[i]["MAT_ACT_WIDTH"].ToDecimal();
					tmmsmfp_bx["MAT_ACT_THICK"] = bcls_rec->Tables["UPD"].Rows[i]["MAT_ACT_THICK"].ToDecimal();
					tmmsmfp_bx["MAT_ACT_WT"] = bcls_rec->Tables["UPD"].Rows[i]["MAT_ACT_WT"].ToDecimal();
					tmmsmfp_bx.TrimOrBlank();
					proc_sum += tmmsmfp_bx.Update("REC_REVISOR,REC_REVISE_TIME,ST_NO,OPERATOR,SLAB_CUT_TIME,MAT_ACT_LEN,MAT_ACT_WIDTH,MAT_ACT_THICK,MAT_ACT_WT", "HEAT_NO");
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
				tmmsmfp_bx.Reset();
				tmmsmfp_bx.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);

				if (!tmmsmfp_bx.Query("HEAT_NO")){
					sprintf(s.msg, "未找到坯号[{0}],无法删除!", (const char*)tmmsmfp_bx["HEAT_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//sqlstr = "DELETE FROM TMMSMOUT1WH";
				proc_sum += tmmsmfp_bx.Delete("HEAT_NO");
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