
#include "CUtils.h"

//int f_mmsm39_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString event_id = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmm0005("TMM0005");

	try
	{
		/* 判断是否存在指定块 */
		if (!bcls_rec->Tables.Contains("MM0099"))
		{
			strcpy(s.msg, "未传入数据块MM0099！");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (bcls_rec->Tables["MM0099"].Rows.get_Count() == 0)
		{
			strcpy(s.msg, "没有传入物料跟踪数据！");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		const CFieldInfoCollection& fic1 = tmmsm01.GetFields();
		if (!bcls_rec->Tables.Contains("MAT_DATA"))
		{
			bcls_rec->Tables.Add("MAT_DATA");
		}
		bcls_rec->Tables["MAT_DATA"] = GetTableColName(fic1);

		const CFieldInfoCollection& fic2 = tmmsm96.GetFields();
		if (!bcls_rec->Tables.Contains("MAT_TRACE"))
		{
			bcls_rec->Tables.Add("MAT_TRACE");
		}
		bcls_rec->Tables["MAT_TRACE"] = GetTableColName(fic2);

		const CFieldInfoCollection& fic3 = tmm0005.GetFields();
		if (!bcls_rec->Tables.Contains("MAT_TREE"))
		{
			bcls_rec->Tables.Add("MAT_TREE");
		}
		bcls_rec->Tables["MAT_TREE"] = GetTableColName(fic3);

		doFlag = f_mmtp_mat_track(bcls_rec, bcls_ret, conn, "SM");
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//此处不调用，放在002104电文接收程序里调用  mfj  20240529
		/*event_id = bcls_rec->Tables["NEWMM_TABLE"].Rows[0]["EVENT_ID"].ToString().Trim();
		if (event_id == "QM05" || event_id == "QM06")
		{
			doFlag = f_mmsm39_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}*/

		
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

BM2_FUNCTION_EXPORT
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn, CModel tmmsm96)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString event_id = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		if (tmmsm96["EVENT_ID"].ToString().Trim() == "")
		{
			strcpy(s.msg, "没有传入事件号!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		tmmsm96["MAT_KIND"] = "SM";

		if (bcls_rec->Tables.IndexOf("MM0099") < 0)
		{
			bcls_rec->Tables.Add("MM0099");
		}

		bcls_rec->Tables["MM0099"].Rows.Clear();
		tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);

		doFlag = f_mmtp_mat_track(bcls_rec, bcls_ret, conn, "SM");
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		tmmsm96.MergeFrom(bcls_rec->Tables["NEWMM_TABLE"].Rows[0]);

		//此处不调用，放在002104电文接收程序里调用  mfj  20240529
		/*event_id = bcls_rec->Tables["NEWMM_TABLE"].Rows[0]["EVENT_ID"].ToString().Trim();
		if (event_id == "QM05" || event_id == "QM06")
		{
			doFlag = f_mmsm39_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{

				throw CApplicationException(-1, s.msg, log.Location);
			}
		}*/

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
