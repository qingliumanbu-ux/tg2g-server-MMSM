
/// <summary>
/// 功能说明:新增工艺路径和物料消耗信息
/// </summary>


#include "stdafx.h"

// Service 入口
BM2F_ENTERACE(mmsm82c_comm)
int f_mmsm_gyins2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82c_comm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int affectRows = 0;
	CString sqlstr = " ";
	CString sm_plan_nol2 = " ";
	CString heatno_premelt = "";
	CDecimal seq_id = 0;
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tmmsm2a_yl("TMMSM2A_YL");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsmgy05("TMMSMGY05");
	//CModel da_heat_relation_syn("DA_HEAT_RELATION_SYN");
	//CModel da_heat_relation("DA_HEAT_RELATION");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);
	CDbCommand cmd_inq_1(conn);

	EIClass bcls_ret1;
	EIClass bcls_rec1;
	bcls_rec1.Tables[0].Columns.Add(tmmsmgy05);
	bcls_rec1.Tables[0].Rows.Clear();
	bcls_rec1.Tables[0].Rows.Add();
	try
	{
		EIClass bcls_rec_xh;
		EIClass bcls_ret_xh;
		bcls_rec_xh.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_rec_xh.Tables[0].Rows.Add();

		if (bcls_rec->Tables.Contains("heat_no"))
		{
			Log::Info("", __FUNCTION__, "i = [{0}]", bcls_rec->Tables["heat_no"].Rows.get_Count());
			

			for (int i = 0; i < bcls_rec->Tables["heat_no"].Rows.get_Count(); i++)
			{
				tmmsmgy05.MergeFrom(bcls_rec->Tables["heat_no"].Rows[i]);
				Log::Info("", __FUNCTION__, "HEAT_NO =[{0}]", tmmsmgy05["HEAT_NO"].ToString());
				tmmsmgy05["LOCK_FLAG"] = "Y";
				tmmsmgy05["AFFIRM_FLAG"] = "1";
				tmmsmgy05["AFFIRM_TIME"] = dateNow;

				/*tmmsmgy05.MergeTo(bcls_rec1.Tables[0]);
				doFlag = f_mmsm_gyins2(&bcls_rec1, &bcls_ret1, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					} 
					*/
				tmmsmgy05.Update("AFFIRM_FLAG,AFFIRM_TIME,LOCK_FLAG", "HEAT_NO");	

				tmmsmgy06["AFFIRM_FLAG"] = "1";
				tmmsmgy06["AFFIRM_TIME"] = dateNow;
				tmmsmgy06["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();

				tmmsmgy06.Update("AFFIRM_FLAG,AFFIRM_TIME", "HEAT_NO");

					bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
					doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
			}
		}

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsmgy06.Reset();
			tmmsmgy06.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (i == 0)
			{
				tmmsmgy05["HEAT_NO"] = tmmsmgy06["HEAT_NO"].ToString();
				tmmsmgy05["LOCK_FLAG"] = "Y";
				tmmsmgy05["AFFIRM_FLAG"] = "1";
				tmmsmgy05["AFFIRM_TIME"] = dateNow;
				tmmsmgy05.Update("LOCK_FLAG,AFFIRM_FLAG,AFFIRM_TIME", "HEAT_NO");

				bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
				doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			
			tmmsmgy06["AFFIRM_FLAG"] = "1";
			tmmsmgy06["AFFIRM_TIME"] = dateNow; 
			tmmsmgy06.Update("AFFIRM_FLAG,AFFIRM_TIME", "HEAT_NO,L2_PROC_NO");
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


