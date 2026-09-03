
#include "stdafx.h"
#include "epex.h"



int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm009a_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT


int f_mmsmwt_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int 	doFlag = 0;
	int 	ret = 0;
	int 	fetchRowCount = 0;
	CString	lpsz_tc_no = "";
	int   blkNum = 0;
	CString v_time = "";
	int  v_count1 = 0;
	int  v_count2 = 0;
	int sqlid = 0;
	CString sqlstr = "";
	CString eventDesc = "";
	CString wtOrder = "";
	CDecimal totalNetWt = 0;
	CDecimal totalActWt = 0;

	CDbCommand cmd_inq(conn);
	CModel tmmsmwt("TMMSMWT");
	CModel tmmsmwta("TMMSMWTA");
	try
	{
		tmmsmwt.MergeFrom(bcls_rec->Tables["MMSMWT"].Rows[0]);
		tmmsmwta.MergeFrom(bcls_rec->Tables["MMSMWT"].Rows[0]);
		wtOrder = bcls_rec->Tables["MMSMWT"].Rows[0]["WT_ORDER"].ToString().Trim();
		if (bcls_rec->Tables["MMSMWT"].Columns.Contains("TOTAL_NET_WT"))
		{
			totalNetWt = bcls_rec->Tables["MMSMWT"].Rows[0]["TOTAL_NET_WT"].ToDecimal();
		}

		if (wtOrder == "2")
		{
			CDbCommand getSeq("SELECT MMSM_MATNO_SEQ.NEXTVAL  FROM DUAL", conn);
			CString newSeqNo = "0000" + getSeq.ExecuteScalar().ToString().Trim();
			newSeqNo = newSeqNo.Substring(newSeqNo.GetLength() - 4);
			tmmsmwt["RESUME_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss").Trim() + newSeqNo;
			//Log::Trace("", "", "tmmsmwt["RESUME_SEQ_NO"] =[{0}]", tmmsmwt["RESUME_SEQ_NO"].ToString());

			tmmsmwt["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsmwt["REC_CREATOR"] = s.userid;
			tmmsmwt.TrimOrBlank();
			tmmsmwt.Insert();
		}

		//Log::Trace("", "", "MM0099 EXIST = {0}", bcls_rec->Tables.Contains("MM0099"));
		if (!bcls_rec->Tables.Contains("MM0099"))
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_DESC");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_DECIMAL, "MAT_ACT_WT");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_DECIMAL, "MAT_THEORY_WT");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MEASURE_WT_FLAG");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "TC_BACKLOG");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_DECIMAL, "WT_PER_METER");
			bcls_rec->Tables["MM0099"].Rows.Add();
		}

		//Log::Trace("", "", "获取材料信息");
		CDataTable unWtMatTab;
		if (wtOrder == "1")
		{
			sqlstr = "SELECT DISTINCT MAT_NO,MAT_LEN MAT_ACT_LEN,MAT_TUBE,MAT_THEORY_WT,(SELECT SUM(MAT_THEORY_WT) FROM TYMSMV3 WHERE WT_ENTRUST = @tmmsmwt.WT_ENTRUST) TOTAL_THEORY_WT FROM TYMSMV3 WHERE WT_ENTRUST = @tmmsmwt.WT_ENTRUST ORDER BY MAT_NO";
			eventDesc = "根据实检获取重量";
		}
		else if (wtOrder == "2")
		{
			sqlstr = "SELECT DISTINCT MAT_NO,MAT_ACT_LEN,MAT_TUBE,MAT_THEORY_WT FROM "
				//注释当前车材料，业务要求抽检重量不对当前车起作用
				//"(SELECT MAT_NO,MAT_LEN MAT_ACT_LEN,MAT_TUBE,MAT_THEORY_WT FROM TYMSMV3 WHERE WT_ENTRUST = @tmmsmwt.WT_ENTRUST)"
				//"UNION ALL"
				"(SELECT MAT_NO,MAT_ACT_LEN,MAT_TUBE,MAT_THEORY_WT FROM TMMSM01 WHERE MEASURE_WT_FLAG = '0' AND UNIT_CODE = @tmmsmwt.CC_MACH_NO AND MAT_ACT_THICK = @tmmsmwt.SLAB_THICK AND MAT_ACT_WIDTH = @tmmsmwt.SLAB_WIDTH)"
				"UNION ALL"
				"(SELECT MAT_NO,MAT_ACT_LEN,MAT_TUBE,MAT_THEORY_WT FROM HMMSM01 WHERE MEASURE_WT_FLAG = '0' AND UNIT_CODE = @tmmsmwt.CC_MACH_NO AND MAT_ACT_THICK = @tmmsmwt.SLAB_THICK AND MAT_ACT_WIDTH = @tmmsmwt.SLAB_WIDTH)";
			eventDesc = "根据抽检获取重量";
		}
		CDbCommand getUnWtMat(sqlstr, conn);
		getUnWtMat.Parameters.Set("tmmsmwt.CC_MACH_NO", tmmsmwt["CC_MACH_NO"].ToString());
		getUnWtMat.Parameters.Set("tmmsmwt.SLAB_THICK", tmmsmwt["SLAB_THICK"].ToDecimal());
		getUnWtMat.Parameters.Set("tmmsmwt.SLAB_WIDTH", tmmsmwt["SLAB_WIDTH"].ToDecimal());
		getUnWtMat.Parameters.Set("tmmsmwt.WT_ENTRUST", tmmsmwta["WT_ENTRUST"].ToString());
		getUnWtMat.ExecuteQuery(unWtMatTab);

		//Log::Trace("", "", "修改实绩信息");
		CDbCommand updTmmsm33("UPDATE TMMSM33 SET MEASURE_WT_FLAG = @wtOrder,SLAB_WT = ROUND(@tmmsmwt.WT_PER_METER * (SLAB_LEN / 1000) * MAT_TUBE,3) WHERE MAT_NO = @tmmsm33.MAT_NO", conn);

		//Log::Trace("", "", "调用物料跟踪");
		for (int i = 0; i < unWtMatTab.Rows.get_Count(); i++)
		{
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM50";
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
			bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = eventDesc;
			bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = unWtMatTab.Rows[i]["MAT_NO"].ToString();
			if (wtOrder == "2")
			{
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WT"] = (tmmsmwt["WT_PER_METER"].ToDecimal() * (unWtMatTab.Rows[i]["MAT_ACT_LEN"].ToDecimal() / 1000) * unWtMatTab.Rows[i]["MAT_TUBE"].ToDecimal()).Round(3);

			}
			else if (wtOrder == "1")
			{
				if (i < (unWtMatTab.Rows.get_Count() - 1))
				{
					bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WT"] = (unWtMatTab.Rows[i]["MAT_THEORY_WT"].ToDecimal() / unWtMatTab.Rows[i]["TOTAL_THEORY_WT"].ToDecimal() * totalNetWt).Round(3);
					totalActWt = totalActWt + bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WT"].ToDecimal();
				}
				else
				{
					bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WT"] = totalNetWt - totalActWt;
				}
			}

			bcls_rec->Tables["MM0099"].Rows[0]["MAT_THEORY_WT"] = unWtMatTab.Rows[i]["MAT_THEORY_WT"].ToDecimal();
			bcls_rec->Tables["MM0099"].Rows[0]["MEASURE_WT_FLAG"] = wtOrder;
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (wtOrder == "2")
			{
				updTmmsm33.Parameters.Set("wtOrder", wtOrder);
				updTmmsm33.Parameters.Set("tmmsmwt.WT_PER_METER", tmmsmwt["WT_PER_METER"].ToDecimal());
				updTmmsm33.Parameters.Set("tmmsm33.MAT_NO", unWtMatTab.Rows[i]["MAT_NO"].ToString());
				updTmmsm33.ExecuteNonQuery();
			}


			bcls_rec->Tables["MM0099"].set_TableName("MMSMSND");
			bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "PAM1M2";
			bcls_rec->Tables["MMSMSND"].Rows[0]["WT_PER_METER"] = tmmsmwt["WT_PER_METER"];
			doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (wtOrder == "2")
			{
//	CModel tmmsmwta("TMMSMWTA");
				tmmsmwta["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tmmsmwta["REC_CREATOR"] = s.userid;
				tmmsmwta["RESUME_SEQ_NO"] = tmmsmwt["RESUME_SEQ_NO"];
				tmmsmwta["MAT_NO"] = unWtMatTab.Rows[i]["MAT_NO"].ToString();
				tmmsmwta["WT_PER_METER"] = tmmsmwt["WT_PER_METER"];
				tmmsmwta.Insert();
			}

			bcls_rec->Tables["MMSMSND"].set_TableName("MM0099");
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


