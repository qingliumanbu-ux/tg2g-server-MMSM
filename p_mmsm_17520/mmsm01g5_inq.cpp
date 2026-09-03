/*<remark>=========================================================
/// <summary>
/// 按产出日期和出钢记号查询特定炼钢的铁水实绩
/// <para>数据库表：
///1.受铁实绩表 TMMSM15
///</para>
/// <para>主调用函数：        </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns>满足查询条件的炼钢铁水实绩信息 </returns>
===========================================================</remark>*/
#include "stdafx.h"

BM2F_ENTERACE(mmsm01g5_inq)


int f_mmsm01g5_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	CString sqlstr = " ";
	try
	{
		CString v_steel_no = bcls_rec->Tables[0].Rows[0]["STEEL_NO"].ToString().Trim();//出钢钢号

		CString v_prod_time_from = bcls_rec->Tables[0].Rows[0]["PROD_TIME_FROM"].ToString().Trim();
		CString v_prod_time_to = bcls_rec->Tables[0].Rows[0]["PROD_TIME_TO"].ToString().Trim();
		CString sql_whre = "";
		CString sql_order_by = "";
		CString sql_count = "";

		CDecimal TotalRecond ;
		int	PageSize = bcls_rec->Tables["PageInfo"].Rows[0]["PageSize"]; /* 每页记录数 */
		int	start_row = bcls_rec->Tables["PageInfo"].Rows[0]["recordFrom"]; /* 将要压入outBlock的起始行 */

		Log::Info("", __FUNCTION__, "v_steel_no=[{0}]", v_steel_no);
		Log::Info("", __FUNCTION__, "v_prod_time_from=[{0}]", v_prod_time_from);
		Log::Info("", __FUNCTION__, "v_prod_time_to=[{0}]", v_prod_time_to);
		Log::Info("", __FUNCTION__, "PageSize=[{0}]", PageSize);
		Log::Info("", __FUNCTION__, "start_row=[{0}]", start_row);

		CDbCommand com(conn);

		if (v_steel_no.GetLength() > 0){
			sql_whre += " AND A.STEEL_NO LIKE  '%'||@v_steel_no ||'%' ";
		}
		if (v_prod_time_from.GetLength() > 0){
			if (v_prod_time_from.GetLength() == 8){
				v_prod_time_from += "000000";
				sql_whre += " AND A.REC_CREATE_TIME>= @v_prod_time_from ";
			}
			else if (v_prod_time_from.GetLength() == 14){
				sql_whre += " AND A.REC_CREATE_TIME>= @v_prod_time_from ";
			}
			else
			{
				sprintf(s.msg, "获取起始时间为[%s],不是14或者8位", (const char*)v_prod_time_from);
				throw CApplicationException(-1, s.msg, "mmsm01g5_inq");
			}
		}
		if (v_prod_time_to.GetLength() > 0){
			if (v_prod_time_to.GetLength() == 8){
				v_prod_time_to += "000000";
				sql_whre += " AND A.REC_CREATE_TIME<= @v_prod_time_to ";
			}
			else if (v_prod_time_to.GetLength() == 14){
				sql_whre += " AND A.REC_CREATE_TIME<= @v_prod_time_to ";
			}
			else
			{
				sprintf(s.msg, "获取起始时间为[%s],不是14或者8位", (const char*)v_prod_time_to);
				throw CApplicationException(-1, s.msg, "mmsm01g5_inq");
			}
		}
		sql_whre += " AND ( A.TC_PROC_FLAG='2')";
		sql_order_by = " ORDER BY HEAT_NO";
	   

		sqlstr = " SELECT DISTINCT REC_CREATE_TIME AS PRO_DATE,IRON_LADLE_NO,"
			" CAST(IRON_WT*0.1 AS DECIMAL(10,2)) AS IRON_WT,"
			" CAST(MEASURE_EMPTY_WT*0.1 AS DECIMAL(6,2)) AS MEASURE_EMPTY_WT,"
			" CAST(LIFTING_FILLED_WT*0.1 AS DECIMAL(6,2)) AS LIFTING_FILLED_WT, "
			" STEEL_NO ,FE_COUNT,TK_NO,CAST(IRON_EMPTY_WT*0.1 AS DECIMAL(6,2)) AS IRON_EMPTY_WT,"
			" CAST(MEASURE_FILLED_WT*0.1 AS DECIMAL(6,2)) AS MEASURE_FILLED_WT,"
			" CAST(IRON_FILLED_WT*0.1 AS DECIMAL(6,2)) AS IRON_FILLED_WT,SUBSTR(STEEL_NO,2,5) AS HEAT_NO "
			" FROM TMMSM15 A  WHERE 1=1 " + sql_whre +sql_order_by;

		com.Parameters.Clear();
		if (v_steel_no.GetLength() > 0){
			com.Parameters.Set("v_steel_no",v_steel_no);
		}
		if (v_prod_time_from.GetLength() > 0){
			com.Parameters.Set("v_prod_time_from", v_prod_time_from);
		}
		if (v_prod_time_to.GetLength() > 0){
			com.Parameters.Set("v_prod_time_to", v_prod_time_to);
		}
		sql_count = "SELECT COUNT(*) FROM (" + sqlstr + ")";
		Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
		Log::Info("", __FUNCTION__, "sql_count=[{0}]", sql_count);
		com.SetCommandText(sql_count);
	    TotalRecond=com.ExecuteScalar();
		/*if (start_row > TotalRecond){
			start_row = 0;
		}*/
		Log::Info("", __FUNCTION__, "TotalRecond=[{0}]", TotalRecond);
		com.SetCommandText(sqlstr);
		com.ExecuteQuery(bcls_ret->Tables[0], start_row, PageSize);
		com.Close();

		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0][0] = TotalRecond;


	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error，sqlcode=[{0},{1}]", arguments, 2);
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


