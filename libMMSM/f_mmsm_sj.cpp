/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        
Description: 到达连铸后去判断是否有流通炉号数据错误
**************************************************/

#include "stdafx.h"
#include "epex.h"
int f_mmsm_gyins2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//原料函数
BM2_FUNCTION_EXPORT
int f_mmsm_sj(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sqlstr_2a = " ";
	CString sql_insert = " ";
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_sql_2a(conn);
	CDbCommand cmd_sql_2a_1(conn);
	CDbCommand cmd_sql_2ayl(conn);
	CDbCommand cmd_sql_2b(conn);
	CDbCommand cmd_sql_19(conn);
	CDbCommand cmd_sql_sj(conn);
	EIClass mmsmgy06;
	CString sm_plan_nol2 = " ";
	CString heat_no = " ";
	CString proc_no = " ";
	CString dev_code = " ";
	CString sm_plan_no = " ";
	CString proc_count = " ";
	CDecimal sj = 0;
	CModel tmmsm27("TMMSM27");
	CModel tmmsm21("TMMSM21");
	CModel tmmsm20("TMMSM20");
	CModel tmmsm24("TMMSM24");
	CModel tmmsm26("TMMSM26");
	CModel tmmsm23("TMMSM23");
	CModel tmmsm25("TMMSM25");
	CModel tmmsm31("TMMSM31");
	CModel tmmsm33("TMMSM33");
	CModel tmmsm14("TMMSM14");
	CModel tmmsm19("TMMSM19");
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm2a_yl("TMMSM2A_YL");
	CModel tmmsm2b("TMMSM2B");
	CString heat_no_sj = " ";
	CString proc_no_sj = " ";
	CString heat_no_sj1 = " ";
	CString proc_no_sj1 = " ";
	try
	{
		sm_plan_nol2=bcls_rec->Tables["MMSM31"].Rows[0]["SM_PLAN_NOL2"].ToString();
		Log::Trace("", __FUNCTION__, "sm_plan_nol2=[{0}]", sm_plan_nol2);
		cmd_sql.SetCommandText(" "
			" SELECT HEAT_NO, PROC_NO, DEV_CODE, SM_PLAN_NO, SM_PLAN_NOL2 "
			" FROM(select HEAT_NO, PROC_NO, DEV_CODE, SM_PLAN_NO, SM_PLAN_NOL2 "
			" from TPSSM12 "
			" UNION "
			" SELECT A.HEAT_NO, B.PROC_NO, B.DEV_CODE, A.SM_PLAN_NO, A.SM_PLAN_NOL2 "
			" FROM TPSSM41 A "
			" LEFT JOIN TPSSM42 B ON A.SM_PLAN_NO = B.SM_PLAN_NO AND A.SM_PLAN_NOL2 = B.SM_PLAN_NOL2) "
			" WHERE SM_PLAN_NOL2 = '" + sm_plan_nol2 + "' "
			" ");
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{
			heat_no = cmd_sql.GetString(1);
			proc_no = cmd_sql.GetString(2);
			dev_code = cmd_sql.GetString(3);
			sm_plan_no = cmd_sql.GetString(4);
			Log::Trace("", __FUNCTION__, "heat_no=[{0}]", heat_no);
			cmd_sql_sj.SetCommandText(" "
				" SELECT distinct HEAT_NO,SM_PLAN_NOL2 FROM (select HEAT_NO,PROC_NO,SM_PLAN_NOL2,SM_PLAN_NO from TMMSM19 "
				" UNION "
				" select HEAT_NO, PROC_NO, SM_PLAN_NOL2, SM_PLAN_NO from TMMSM20 "
				" UNION "
				" select HEAT_NO, PROC_NO, SM_PLAN_NOL2, SM_PLAN_NO from TMMSM21 "
				" UNION "
				" select HEAT_NO, PROC_NO, SM_PLAN_NOL2, SM_PLAN_NO from TMMSM23 "
				" UNION "
				" select HEAT_NO, PROC_NO, SM_PLAN_NOL2, SM_PLAN_NO from TMMSM24 "
				" UNION "
				" select HEAT_NO, PROC_NO, SM_PLAN_NOL2, SM_PLAN_NO from TMMSM25 "
				" UNION "
				" select HEAT_NO, PROC_NO, SM_PLAN_NOL2, SM_PLAN_NO from TMMSM26 "
				" UNION "
				" select HEAT_NO, PROC_NO, SM_PLAN_NOL2, SM_PLAN_NO from TMMSM27) "
				" WHERE SM_PLAN_NOL2 = '"+sm_plan_nol2+"' "
				" ");
			cmd_sql_sj.ExecuteReader();
			while (cmd_sql_sj.Read())
			{
				if (sj == 0){
					heat_no_sj = cmd_sql_sj.GetString(1);
					proc_no_sj = cmd_sql_sj.GetString(2);
					Log::Trace("", __FUNCTION__, "heat_no_sj=[{0}]", heat_no_sj);
				}
				if (sj==1){
					heat_no_sj1 = cmd_sql_sj.GetString(1);
					proc_no_sj1 = cmd_sql_sj.GetString(2);
				}
				sj = sj + 1;
			}
			cmd_sql_sj.Close();
			Log::Trace("", __FUNCTION__, "dev_code=[{0}]", dev_code);
			tmmsm19["SM_PLAN_NOL2"] = sm_plan_nol2;
			if (tmmsm19.QueryCount("SM_PLAN_NOL2")>0){
				cmd_sql.Fetch(tmmsm19);
				cmd_sql_2a.SetCommandText(" "
					" select  DISTINCT B.SM_PLAN_NOL2,B.HEAT_NO,B.PROC_NO,B.PONO,B.L2_PROC_NO "
					" from TMMSM2A A "
					" LEFT JOIN TMMSM19 B ON A.L2_PROC_NO = B.L2_PROC_NO "
					" WHERE A.REC_CREATOR != 'QC' "
					" AND A.DEV_CODE LIKE 'Z%' AND B.SM_PLAN_NOL2 != '11111111' AND B.SM_PLAN_NOL2 = '"+sm_plan_nol2+"' "
					" ");
				cmd_sql_2a.ExecuteReader();
				//sqlstr_2a
				while (cmd_sql_2a.Read())
				{
					cmd_sql_2a.Fetch(tmmsm2a);
					sqlstr_2a = "update TMMSM2A set SM_PLAN_NOL2='" + tmmsm2a["SM_PLAN_NOL2"].ToString() + "',HEAT_NO='" + tmmsm2a["HEAT_NO"].ToString() + "',PROC_NO='" + tmmsm2a["PROC_NO"].ToString() + "',PONO='" + tmmsm2a["PONO"].ToString() + "' where L2_PROC_NO='" + tmmsm2a["L2_PROC_NO"].ToString() + "' and STATION_ID='Z' ";
					cmd_sql_2a_1.SetCommandText(sqlstr_2a);
					cmd_sql_2a_1.ExecuteNonQuery();
					sqlstr = " update TMMSM2A_YL set SM_PLAN_NOL2='" + tmmsm2a["SM_PLAN_NOL2"].ToString() + "',HEAT_NO='" + tmmsm2a["HEAT_NO"].ToString() + "',PROC_NO='" + tmmsm2a["PROC_NO"].ToString() + "',PONO='" + tmmsm2a["PONO"].ToString() + "' where L2_PROC_NO='" + tmmsm2a["L2_PROC_NO"].ToString() + "' and STATION_ID='Z'  ";
					cmd_sql_2ayl.SetCommandText(sqlstr);
					cmd_sql_2ayl.ExecuteNonQuery();
					Log::Trace("", "", "sql_insert = [{0}]", sqlstr);
				}
				cmd_sql_2a_1.Close();
				cmd_sql_2ayl.Close();
				cmd_sql_2a.Close();
				cmd_sql_2b.SetCommandText(" select PROC_COUNT from tmmsm2b where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='Z' ");
				cmd_sql_2b.ExecuteReader();
				while (cmd_sql_2b.Read())
				{
					cmd_sql.Fetch(tmmsm2b);
					tmmsm2b["PROC_COUNT"] = cmd_sql_2b.GetString(1);
					tmmsm2b["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2b.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2b.Close();
			}
			if (dev_code.Substring(0, 1) == "E"){
				cmd_sql.Fetch(tmmsm20);
				tmmsm20.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "SM_PLAN_NOL2");
				cmd_sql_2a.SetCommandText(" select PROC_COUNT from tmmsm2a where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='E' ");
				cmd_sql_2a.ExecuteReader();
				while (cmd_sql_2a.Read())
				{
					cmd_sql.Fetch(tmmsm2a);
					tmmsm2a["PROC_COUNT"] = cmd_sql_2a.GetString(1);
					tmmsm2a["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2a.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
					//tmmsm2a_yl.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2a.Close();
				sqlstr = " update TMMSM2A_YL set HEAT_NO='" + tmmsm2a["HEAT_NO"].ToString() + "',PROC_NO='" + tmmsm2a["PROC_NO"].ToString() + "',SM_PLAN_NO='" + tmmsm2a["SM_PLAN_NO"].ToString() + "' where SM_PLAN_NOL2='" + tmmsm2a["SM_PLAN_NOL2"].ToString() + "' and STATION_ID='E'  ";
				cmd_sql_2ayl.SetCommandText(sqlstr);
				cmd_sql_2ayl.ExecuteNonQuery();
				cmd_sql_2ayl.Close();
				cmd_sql_2b.SetCommandText(" select PROC_COUNT from tmmsm2b where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='E' ");
				cmd_sql_2b.ExecuteReader();
				while (cmd_sql_2b.Read())
				{
					cmd_sql.Fetch(tmmsm2b);
					tmmsm2b["PROC_COUNT"] = cmd_sql_2b.GetString(1);
					tmmsm2b["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2b.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2b.Close();
			}
			if (dev_code.Substring(0, 1) == "B"){
				cmd_sql.Fetch(tmmsm21);
				Log::Trace("", __FUNCTION__, "tmmsm21=[{0}]", tmmsm21["SM_PLAN_NOL2"].ToString());
				tmmsm21.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "SM_PLAN_NOL2");
				Log::Trace("", __FUNCTION__, "like=[{0}]", __LINE__);
				cmd_sql_2a.SetCommandText(" select PROC_COUNT from tmmsm2a where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='B' ");
				cmd_sql_2a.ExecuteReader();
				while (cmd_sql_2a.Read())
				{
					Log::Trace("", __FUNCTION__, "like=[{0}]", __LINE__);
					cmd_sql.Fetch(tmmsm2a);
					Log::Trace("", __FUNCTION__, "like=[{0}]", __LINE__);
					tmmsm2a["PROC_COUNT"] = cmd_sql_2a.GetString(1);
					Log::Trace("", __FUNCTION__, "PROC_COUNT=[{0}]", tmmsm2a["PROC_COUNT"].ToString());
					tmmsm2a["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2a.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2a.Close();
				sqlstr = " update TMMSM2A_YL set HEAT_NO='" + tmmsm2a["HEAT_NO"].ToString() + "',PROC_NO='" + tmmsm2a["PROC_NO"].ToString() + "',SM_PLAN_NO='" + tmmsm2a["SM_PLAN_NO"].ToString() + "' where SM_PLAN_NOL2='" + tmmsm2a["SM_PLAN_NOL2"].ToString() + "' and STATION_ID='B'  ";
				cmd_sql_2ayl.SetCommandText(sqlstr);
				cmd_sql_2ayl.ExecuteNonQuery();
				cmd_sql_2ayl.Close();
				cmd_sql_2b.SetCommandText(" select PROC_COUNT from tmmsm2b where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='B' ");
				cmd_sql_2b.ExecuteReader();
				while (cmd_sql_2b.Read())
				{
					cmd_sql.Fetch(tmmsm2b);
					tmmsm2b["PROC_COUNT"] = cmd_sql_2b.GetString(1);
					tmmsm2b["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2b.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2b.Close();
			}
			if (dev_code.Substring(0, 1) == "R"){
				cmd_sql.Fetch(tmmsm23);
				tmmsm23.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "SM_PLAN_NOL2");
				cmd_sql_2a.SetCommandText(" select PROC_COUNT from tmmsm2a where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='R' ");
				cmd_sql_2a.ExecuteReader();
				while (cmd_sql_2a.Read())
				{
					cmd_sql.Fetch(tmmsm2a);
					tmmsm2a["PROC_COUNT"] = cmd_sql_2a.GetString(1);
					tmmsm2a["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2a.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2a.Close();
				sqlstr = " update TMMSM2A_YL set HEAT_NO='" + tmmsm2a["HEAT_NO"].ToString() + "',PROC_NO='" + tmmsm2a["PROC_NO"].ToString() + "',SM_PLAN_NO='" + tmmsm2a["SM_PLAN_NO"].ToString() + "' where SM_PLAN_NOL2='" + tmmsm2a["SM_PLAN_NOL2"].ToString() + "' and STATION_ID='R'  ";
				cmd_sql_2ayl.SetCommandText(sqlstr);
				cmd_sql_2ayl.ExecuteNonQuery();
				cmd_sql_2ayl.Close();
				cmd_sql_2b.SetCommandText(" select PROC_COUNT from tmmsm2b where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='R' ");
				cmd_sql_2b.ExecuteReader();
				while (cmd_sql_2b.Read())
				{
					cmd_sql.Fetch(tmmsm2b);
					tmmsm2b["PROC_COUNT"] = cmd_sql_2b.GetString(1);
					tmmsm2b["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2b.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2b.Close();
			}
			if (dev_code.Substring(0, 1) == "F"){
				cmd_sql.Fetch(tmmsm24);
				tmmsm24.Update(" PROC_NO,SM_PLAN_NO", "HEAT_NO");
				cmd_sql_2a.SetCommandText(" select PROC_COUNT from tmmsm2a where HEAT_NO='" + heat_no + "' and STATION_ID='F' ");
				cmd_sql_2a.ExecuteReader();
				while (cmd_sql_2a.Read())
				{
					cmd_sql.Fetch(tmmsm2a);
					tmmsm2a["PROC_COUNT"] = cmd_sql_2a.GetString(1);
					tmmsm2a["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2a.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2a.Close();
				sqlstr = " update TMMSM2A_YL set HEAT_NO='" + tmmsm2a["HEAT_NO"].ToString() + "',PROC_NO='" + tmmsm2a["PROC_NO"].ToString() + "',SM_PLAN_NO='" + tmmsm2a["SM_PLAN_NO"].ToString() + "' where SM_PLAN_NOL2='" + tmmsm2a["SM_PLAN_NOL2"].ToString() + "' and STATION_ID='F'  ";
				cmd_sql_2ayl.SetCommandText(sqlstr);
				cmd_sql_2ayl.ExecuteNonQuery();
				cmd_sql_2ayl.Close();
				cmd_sql_2b.SetCommandText(" select PROC_COUNT from tmmsm2b where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='F' ");
				cmd_sql_2b.ExecuteReader();
				while (cmd_sql_2b.Read())
				{
					cmd_sql.Fetch(tmmsm2b);
					tmmsm2b["PROC_COUNT"] = cmd_sql_2b.GetString(1);
					tmmsm2b["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2b.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2b.Close();
			}
			if (dev_code.Substring(0, 1) == "V"){
				cmd_sql.Fetch(tmmsm25);
				tmmsm25.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "SM_PLAN_NOL2");
				cmd_sql_2a.SetCommandText(" select PROC_COUNT from tmmsm2a where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='V' ");
				cmd_sql_2a.ExecuteReader();
				while (cmd_sql_2a.Read())
				{
					cmd_sql.Fetch(tmmsm2a);
					tmmsm2a["PROC_COUNT"] = cmd_sql_2a.GetString(1);
					tmmsm2a["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2a.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2a.Close();
				sqlstr = " update TMMSM2A_YL set HEAT_NO='" + tmmsm2a["HEAT_NO"].ToString() + "',PROC_NO='" + tmmsm2a["PROC_NO"].ToString() + "',SM_PLAN_NO='" + tmmsm2a["SM_PLAN_NO"].ToString() + "' where SM_PLAN_NOL2='" + tmmsm2a["SM_PLAN_NOL2"].ToString() + "' and STATION_ID='V'  ";
				cmd_sql_2ayl.SetCommandText(sqlstr);
				cmd_sql_2ayl.ExecuteNonQuery();
				cmd_sql_2ayl.Close();
				cmd_sql_2b.SetCommandText(" select PROC_COUNT from tmmsm2b where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='V' ");
				cmd_sql_2b.ExecuteReader();
				while (cmd_sql_2b.Read())
				{
					cmd_sql.Fetch(tmmsm2b);
					tmmsm2b["PROC_COUNT"] = cmd_sql_2b.GetString(1);
					tmmsm2b["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2b.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2b.Close();
			}
			if (dev_code.Substring(0, 1) == "S"){
				cmd_sql.Fetch(tmmsm26);
				tmmsm26.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "SM_PLAN_NOL2");
				cmd_sql_2a.SetCommandText(" select PROC_COUNT from tmmsm2a where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='S' ");
				cmd_sql_2a.ExecuteReader();
				while (cmd_sql_2a.Read())
				{
					cmd_sql.Fetch(tmmsm2a);
					tmmsm2a["PROC_COUNT"] = cmd_sql_2a.GetString(1);
					tmmsm2a["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2a.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2a.Close();
				sqlstr = " update TMMSM2A_YL set HEAT_NO='" + tmmsm2a["HEAT_NO"].ToString() + "',PROC_NO='" + tmmsm2a["PROC_NO"].ToString() + "',SM_PLAN_NO='" + tmmsm2a["SM_PLAN_NO"].ToString() + "' where SM_PLAN_NOL2='" + tmmsm2a["SM_PLAN_NOL2"].ToString() + "' and STATION_ID='S'  ";
				cmd_sql_2ayl.SetCommandText(sqlstr);
				cmd_sql_2ayl.ExecuteNonQuery();
				cmd_sql_2ayl.Close();
				cmd_sql_2b.SetCommandText(" select PROC_COUNT from tmmsm2b where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='S' ");
				cmd_sql_2b.ExecuteReader();
				while (cmd_sql_2b.Read())
				{
					cmd_sql.Fetch(tmmsm2b);
					tmmsm2b["PROC_COUNT"] = cmd_sql_2b.GetString(1);
					tmmsm2b["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2b.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2b.Close();
			}
			if (dev_code.Substring(0, 1) == "A"){
				cmd_sql.Fetch(tmmsm27);
				Log::Trace("", __FUNCTION__, "tmmsm27=[{0}]", tmmsm27["SM_PLAN_NOL2"].ToString());
				tmmsm27.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "SM_PLAN_NOL2");
				cmd_sql_2a.SetCommandText(" select PROC_COUNT from tmmsm2a where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='A' ");
				cmd_sql_2a.ExecuteReader();
				while (cmd_sql_2a.Read())
				{
					cmd_sql.Fetch(tmmsm2a);
					tmmsm2a["PROC_COUNT"] = cmd_sql_2a.GetString(1);
					tmmsm2a["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2a.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2a.Close();
				sqlstr = " update TMMSM2A_YL set HEAT_NO='" + tmmsm2a["HEAT_NO"].ToString() + "',PROC_NO='" + tmmsm2a["PROC_NO"].ToString() + "',SM_PLAN_NO='" + tmmsm2a["SM_PLAN_NO"].ToString() + "' where SM_PLAN_NOL2='" + tmmsm2a["SM_PLAN_NOL2"].ToString() + "' and STATION_ID='A'  ";
				cmd_sql_2ayl.SetCommandText(sqlstr);
				cmd_sql_2ayl.ExecuteNonQuery();
				cmd_sql_2ayl.Close();
				cmd_sql_2b.SetCommandText(" select PROC_COUNT from tmmsm2b where SM_PLAN_NOL2='" + sm_plan_nol2 + "' and STATION_ID='A' ");
				cmd_sql_2b.ExecuteReader();
				while (cmd_sql_2b.Read())
				{
					cmd_sql.Fetch(tmmsm2b);
					tmmsm2b["PROC_COUNT"] = cmd_sql_2b.GetString(1);
					tmmsm2b["SM_PLAN_NOL2"] = sm_plan_nol2;
					tmmsm2b.Update("HEAT_NO, PROC_NO,SM_PLAN_NO", "PROC_COUNT,SM_PLAN_NOL2");
				}
				cmd_sql_2b.Close();
			}
		}
		if (heat_no_sj != heat_no ){
			mmsmgy06.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			mmsmgy06.Tables[0].Columns.Add(DT_STRING, "SM_PLAN_NOL2");
			mmsmgy06.Tables[0].Rows.Add();
			mmsmgy06.Tables[0].Rows[0]["HEAT_NO"] = heat_no_sj;
			mmsmgy06.Tables[0].Rows[0]["SM_PLAN_NOL2"] = sm_plan_nol2;
			doFlag = f_mmsm_gyins2(&mmsmgy06, bcls_ret, conn);
			mmsmgy06.Tables[0].Rows[0]["HEAT_NO"] = heat_no;
			mmsmgy06.Tables[0].Rows[0]["SM_PLAN_NOL2"] = sm_plan_nol2;
			doFlag = f_mmsm_gyins2(&mmsmgy06, bcls_ret, conn);
		}
		if (heat_no_sj == heat_no){
			if (heat_no_sj1 != " "&&heat_no_sj1 != heat_no){
				mmsmgy06.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
				mmsmgy06.Tables[0].Columns.Add(DT_STRING, "SM_PLAN_NOL2");
				mmsmgy06.Tables[0].Rows.Add();
				mmsmgy06.Tables[0].Rows[0]["HEAT_NO"] = heat_no_sj1;
				mmsmgy06.Tables[0].Rows[0]["SM_PLAN_NOL2"] = sm_plan_nol2;
				doFlag = f_mmsm_gyins2(&mmsmgy06, bcls_ret, conn);
				mmsmgy06.Tables[0].Rows[0]["HEAT_NO"] = heat_no;
				mmsmgy06.Tables[0].Rows[0]["SM_PLAN_NOL2"] = sm_plan_nol2;
				doFlag = f_mmsm_gyins2(&mmsmgy06, bcls_ret, conn);
			}
		}
		cmd_sql.Close();
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