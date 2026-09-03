/*<remark>=========================================================
/// <summary>
/// 炉次确认实绩信息查询
/// <para>
/// 1.根据炉次确定时刻时间范围,厂别分区条件进行炉次实绩信息查询。
/// </para>
/// <para>数据库表：
///1.炼钢连铸作业实绩表 TMMSM31   
///2.炼钢转炉作业实绩表 TMMSM21
///3.炼钢合金消耗实绩表 TMMSM2A
///4.炼钢原辅料品名信息表 TMMSM50
///</para>
/// <para>主调用函数：        </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns>炉次实绩信息 </returns>
===========================================================</remark>*/

#include "stdafx.h"

BM2F_ENTERACE(mmsm01g41_inq)


int f_mmsm01g41_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	CString sqlstr = " ";
	try
	{
		CString sql_where = "",sqlcount=" ",sqlcnt_where="";
		CDbCommand com(conn),com_cnt(conn);
		CDbCommand com_lu(conn); //统计炼钢铁水量,机清量
		

		CDecimal lu_1 = 0;              //一二炼钢总炉数
		CDecimal iron_wt_1 = 0;     //一二炼钢铁水量
		CDecimal steel_wt_1 = 0 ;   //一二炼钢钢水量
		CDecimal mach_rct_1 = 0;   // 一二炼钢机清量

		CString v_factory_div[100];
		int v_factory_div_num = 0;

		int PageSize, RecordForm; //查询起始行，和页的大小
		CDecimal Total_count; //查询结果的总记录数

		
		CString v_div_st = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().Trim();
		CString v_heat_confm_time_f = bcls_rec->Tables[0].Rows[0]["start_time_f"].ToString().Trim();
		CString v_heat_confm_time_e = bcls_rec->Tables[0].Rows[0]["start_time_e"].ToString().Trim();

		PageSize = bcls_rec->Tables["PageInfo"].Rows[0]["PageSize"];
		RecordForm = bcls_rec->Tables["PageInfo"].Rows[0]["RecordFrom"];

		Log::Info("", __FUNCTION__, "炼钢区分div_st=[{0}]", v_div_st);
		Log::Info("", __FUNCTION__, "生产日期v_heat_confm_time_f=[{0}]", v_heat_confm_time_f);
		Log::Info("", __FUNCTION__, "---v_heat_confm_time_e=[{0}]", v_heat_confm_time_e);
		Log::Info("", __FUNCTION__, "---PageSize=[{0}]", PageSize);
		Log::Info("", __FUNCTION__, "---RecordForm=[{0}]", RecordForm);


		if (v_div_st.GetLength()>0){
			sql_where += " AND A.FACTORY_DIV LIKE @v_div_st ";
		}
		if (v_heat_confm_time_f.GetLength()>0)
		{
			if (v_heat_confm_time_f.GetLength() == 8){
				v_heat_confm_time_f += "000000";
				sql_where += " AND A.HEAT_CONFM_TIME >= @v_heat_confm_time_f ";
				sqlcnt_where += " AND A.HEAT_CONFM_TIME >= @v_heat_confm_time_f ";
			}
			else if (v_heat_confm_time_f.GetLength() == 14){
				sql_where += " AND A.HEAT_CONFM_TIME >= @v_heat_confm_time_f ";
				sqlcnt_where += " AND A.HEAT_CONFM_TIME >= @v_heat_confm_time_f ";
			}
			else
			{
				sprintf(s.msg, "获取起始时间为[%s],不是14或者8位", (const char*)v_heat_confm_time_f);
				throw CApplicationException(-1, s.msg, "mmsm01g41_inq");
			}
			
		}
		if (v_heat_confm_time_e.GetLength()>0)
		{
			if (v_heat_confm_time_e.GetLength() == 8){
				v_heat_confm_time_e += "235959";
				sql_where += " AND A.HEAT_CONFM_TIME <= @v_heat_confm_time_e ";
				sqlcnt_where += " AND A.HEAT_CONFM_TIME <= @v_heat_confm_time_e ";
			}
			else if (v_heat_confm_time_e.GetLength() == 14){
				sql_where += " AND A.HEAT_CONFM_TIME <= @v_heat_confm_time_e ";
				sqlcnt_where += " AND A.HEAT_CONFM_TIME <= @v_heat_confm_time_e ";
			}
			else
			{
				sprintf(s.msg, "获取起始时间为[%s],不是14或者8位", (const char*)v_heat_confm_time_e);
				throw CApplicationException(-1, s.msg, "mmsm01g41_inq");
			}
		}
		//
		sqlcount = " SELECT COUNT(*) FROM TMMSM31 A WHERE 1=1 " + sql_where;
		
		sqlstr = " SELECT A.PONO,A.HEAT_NO,B.TAP_START_TIME,A.PREC_ST_NO,A.FIN_ST_NO,A.HEAT_CONFM_TIME, "
			" A.SLAB_WT ,A.STEEL_WT,A.SLAB_DEST , NVL(B.MOLTIRON_WT,0)+NVL(C.SCRAP_STEEL_WT,0) AS LOAD_WT, "
			" NVL(B.MOLTIRON_WT,0) AS MOLTIRON_WT ,NVL(C.SCRAP_STEEL_WT,0) AS SCRAP_STEEL_WT   FROM ( ("
			" SELECT A.PONO,A.HEAT_NO,A.PREC_ST_NO,A.FIN_ST_NO,A.HEAT_CONFM_TIME, "
			" A.SLAB_WT ,A.SLAB_DEST ,SUM(B.STEEL_WT) AS STEEL_WT  FROM "
			" ("
			" SELECT DISTINCT A.PONO,A.HEAT_NO,A.PREC_ST_NO,A.FIN_ST_NO,A.HEAT_CONFM_TIME, "
			" A.SLAB_WT ,A.SLAB_DEST FROM TMMSM31 A WHERE 1=1 " + sql_where +
			" ) "
			" AS A LEFT JOIN TMMSM31 B ON A.HEAT_NO=B.HEAT_NO "
			" GROUP BY A.PONO,A.HEAT_NO,A.PREC_ST_NO,A.FIN_ST_NO,A.HEAT_CONFM_TIME,A.SLAB_WT ,A.SLAB_DEST "
			" )AS A LEFT JOIN TMMSM21 B ON A.HEAT_NO=B.HEAT_NO) "
			" LEFT JOIN ("
			" SELECT A.HEAT_NO,SUM(C.DEVO_WT) AS SCRAP_STEEL_WT "
			" FROM( SELECT DISTINCT HEAT_NO FROM TMMSM31 A WHERE 1=1 " + sql_where +
			" )AS A LEFT JOIN TMMSM2A C ON A.HEAT_NO=C.HEAT_NO "
			" WHERE C.MAT_CODE IN (SELECT MAT_CODE FROM TMMSM50 WHERE MAT_TYPE='2') "
			" GROUP BY A.HEAT_NO "
			" ) AS C ON A.HEAT_NO=C.HEAT_NO "
			" ORDER BY A.PONO ";

		Log::Info("", __FUNCTION__, "sql=[{0}]", sqlstr);
		if (v_div_st.GetLength() > 0){
			com.Parameters.Set("v_div_st", v_div_st); com_cnt.Parameters.Set("v_div_st", v_div_st);
		}
		if (v_heat_confm_time_f.GetLength() > 0){
			com.Parameters.Set("v_heat_confm_time_f", v_heat_confm_time_f); com_cnt.Parameters.Set("v_heat_confm_time_f", v_heat_confm_time_f);
			com_lu.Parameters.Set("v_heat_confm_time_f", v_heat_confm_time_f);
		}
		if (v_heat_confm_time_e.GetLength() > 0){
			com.Parameters.Set("v_heat_confm_time_e", v_heat_confm_time_e); com_cnt.Parameters.Set("v_heat_confm_time_e", v_heat_confm_time_e);
			com_lu.Parameters.Set("v_heat_confm_time_e", v_heat_confm_time_e);
		}

		com_cnt.SetCommandText(sqlcount);
		Total_count = com_cnt.ExecuteScalar();
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = Total_count;
		if (RecordForm > Total_count){ RecordForm = 0; }

		com.SetCommandText(sqlstr);
		com.ExecuteQuery(bcls_ret->Tables[0],RecordForm,PageSize);

		///获取炼钢广别
		sqlstr = "SELECT CODE FROM TEP0002 WHERE CODE_CLASS LIKE 'M00F' "
			" AND CODE_DESC_2_CONTENT LIKE 'SM' ";
		com.Parameters.Clear();
		com.SetCommandText(sqlstr);
		com.ExecuteReader();
		while (com.Read())
		{
			v_factory_div[v_factory_div_num] = com.GetString(1).Trim();
			v_factory_div_num++;
		}
		if (bcls_ret->Tables.Contains("MMSMTJ")){
			bcls_ret->Tables.Remove("MMSMTJ");
		}
		bcls_ret->Tables.Add("MMSMTJ");

		if (v_factory_div_num > 0){
			bcls_ret->Tables["MMSMTJ"].Rows.Add();
			for (int cnt = 0; cnt < v_factory_div_num; cnt++){
				sqlstr = "SELECT COUNT(*),SUM(STEEL_WT) FROM TMMSM31 A "
					" WHERE A.FACTORY_DIV LIKE  '" + v_factory_div[cnt]+"'"+ sqlcnt_where;
				com_lu.SetCommandText(sqlstr); 
				com_lu.ExecuteReader();
				if (com_lu.Read())
				{
					lu_1 = com_lu.GetDecimal(1);
					steel_wt_1 = com_lu.GetDecimal(2).ConvertToPrecScale(6, 3);
					Log::Info("", __FUNCTION__, "lu_1=[{0}] ;steel_wt_1 = [{1}]", lu_1, steel_wt_1);

				}
				com_lu.Close();
				sqlstr = " SELECT  A.MOLTIRON_WT, B.MACH FROM( "
					" SELECT SUM(MOLTIRON_WT)AS MOLTIRON_WT    FROM TMMSM21 WHERE HEAT_NO IN( "
					" SELECT A.HEAT_NO FROM TMMSM31 A "
					" WHERE A.FACTORY_DIV LIKE   '" + v_factory_div[cnt] + "'" + sqlcnt_where + " ) "
					" ) AS A, "
					" ( SELECT COUNT(PONO)AS MACH   FROM TMMSM34   WHERE HEAT_NO IN( "
					" SELECT A.HEAT_NO FROM TMMSM31 A "
					" WHERE A.FACTORY_DIV LIKE   '" + v_factory_div[cnt] + "'" + sqlcnt_where + " ) "
					" ) AS B ";
				
				com_lu.SetCommandText(sqlstr);
				com_lu.ExecuteReader();
				if (com_lu.Read())
				{
					iron_wt_1 = com_lu.GetDecimal(1).ConvertToPrecScale(6, 3);
					mach_rct_1 = com_lu.GetDecimal(2).ConvertToPrecScale(6, 0);
					Log::Info("", __FUNCTION__, "iron_wt_1=[{0}];mach_rct_1 = [{1}]", iron_wt_1, mach_rct_1);
				}
				com_lu.Close();
				CString temp = v_factory_div[cnt]+"_sum" ;
				bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, temp);
				bcls_ret->Tables["MMSMTJ"].Rows[0][temp] = lu_1;
				temp = v_factory_div[cnt] + "_steel_wt";
				bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, temp);
				bcls_ret->Tables["MMSMTJ"].Rows[0][temp] = steel_wt_1;
				temp = v_factory_div[cnt] + "_iron_wt";
				bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, temp);
				bcls_ret->Tables["MMSMTJ"].Rows[0][temp] = iron_wt_1;
				temp = v_factory_div[cnt] + "_mach_clear";
				bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, temp);
				bcls_ret->Tables["MMSMTJ"].Rows[0][temp] = mach_rct_1;

			}
			
		}
		

		com_cnt.Close();
		com.Close();
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error，sqlcode=[{0},{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		Log::Info("", __FUNCTION__, "erro=[{0}];", s.sysmsg);
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


