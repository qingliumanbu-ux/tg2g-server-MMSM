/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 分摊加实绩的总消耗
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_getprice(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_zxh02(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString backlog_code = "";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString start_time = " ";
	CString end_time_1 = " ";
	CString end_time = " ";
	CModel tmmsmzxhbb("TMMSMZXHBB");
	CDecimal cd_count = 0;
	CDecimal devo_wt = 0;
	CDecimal devo_wt1 = 0;
	CString biaoji = "0";
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	int i_idx = 0;
	int i_count = 0;

	CString sqlstr = "";

	CString stat_date = " ";
	CString stat_date_b = " ";

	CString origin_sys_code = "";
	CString heat_no = "";
	CDecimal prod_out_wt = 0;

	CDecimal ni_wt = 0; // 成分*同炉同物料汇总重量Ni
	CDecimal cr_wt = 0; // 成分*同炉同物料汇总重量Cr
	CDecimal ni_wt1 = 0; // ni平均数据
	CDecimal cr_wt1 = 0; // Cr平均数据
	CDecimal ni_xs = 0; // Ni系数
	CDecimal cr_xs = 0; // Cr系数
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CModel tmmsmzxh01("TMMSMZXH01");
	CString datetime = CDateTime::Now().AddHours(-3).ToString("yyyyMMddHHmmss");
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EIClass EItables;

	try
	{
		stat_date = datetime.SubstringNE(0, 6);
		if (bcls_rec->Tables[0].Columns.Contains("STAT_DATE"))	
		{
			stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 6);
			stat_date_b = CDateTime::Parse(stat_date+"01000001").AddYears(-1).ToString("yyyyMM");
		}
		Log::Trace("", __FUNCTION__, "stat_date = [{0}],stat_date_b=[{1}]", stat_date, stat_date_b); 

		//更新物料价格
		EIClass bcls_ret3;
		EIClass bcls_rec3;
		bcls_rec3.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");
		bcls_rec3.Tables[0].Columns.Add(DT_STRING, "PRICE_TYPE");
		bcls_rec3.Tables[0].Columns.Add(DT_DECIMAL, "CR_VALUE");
		bcls_rec3.Tables[0].Columns.Add(DT_DECIMAL, "NI_VALUE");
		bcls_rec3.Tables[0].Columns.Add(DT_DECIMAL, "MO_VALUE");
		bcls_rec3.Tables[0].Rows.Add();

		sqlstr = " select MAT_CODE_DR,INCLUDE_CR,INCLUDE_NI,INCLUDE_MO"
			" from TQMTSCB01a_MX"
			" where 1=1"
			" and MAT_TYPE_DESC not in ('步骤费','回收')"
			" and  DATE_C <= @stat_date"
			" and  DATE_C >= @stat_date_b"
			" group by MAT_CODE_DR,INCLUDE_CR,INCLUDE_NI,INCLUDE_MO"
			; 
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("stat_date_b", stat_date_b);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			bcls_rec3.Tables[0].Rows[0]["PRICE_TYPE"] = "BZ";
			bcls_rec3.Tables[0].Rows[0]["MAT_CODE"] = cmd_inq.GetString(1);
			bcls_rec3.Tables[0].Rows[0]["CR_VALUE"] = cmd_inq.GetDecimal(2);
			bcls_rec3.Tables[0].Rows[0]["NI_VALUE"] = cmd_inq.GetDecimal(3);
			bcls_rec3.Tables[0].Rows[0]["MO_VALUE"] = cmd_inq.GetDecimal(4);
			doFlag = f_mmsm_getprice(&bcls_rec3, &bcls_ret3, conn);	
			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用f_mmsm_getprice失败-------");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else
			{
				sqlstr = " update tqmtscb01a_mx set  CONVERSION_PRICE = @unit_price"
					" ,COST = Round(WEIGHT* @unit_price,2)"
					",COST_PERT	=decode(QUALIFIED_WT,0,0,round((@unit_price*WEIGHT)/QUALIFIED_WT,2))"
					",UNIT_PRICE_XY = case when substr(GRADE_ID,1,2) in ('1F','1M','4F','4M') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_CR"].ToString() + "  when substr(GRADE_ID,1,2) in ('1A','1D','4A','4D') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_NI"].ToString() + " else 0 end "
					",COST_XY = Round(WEIGHT* (case when substr(GRADE_ID,1,2) in ('1F','1M','4F','4M') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_CR"].ToString() + " when substr(GRADE_ID,1,2) in ('1A','1D','4A','4D') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_NI"].ToString() + " else 0 end ), 2)"
					",COST_PERT_XY = decode(QUALIFIED_WT,0,0,round(WEIGHT* (case when substr(GRADE_ID,1,2) in ('1F','1M','4F','4M') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_CR"].ToString() + " when substr(GRADE_ID,1,2) in ('1A','1D','4A','4D') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_NI"].ToString() + " else 0 end )/QUALIFIED_WT,2))"
					",REC_REVISOR =@user_id"
					",REC_REVISE_TIME =@dateNow"
					" where 1=1"
					" and MAT_TYPE_DESC not in ('步骤费','回收')"
					" and INCLUDE_CR =@cr_value"
					" and INCLUDE_NI =@ni_value"
					" and INCLUDE_MO =@mo_value"
					" and MAT_CODE_DR = @mat_code"
					" and  DATE_C <= @stat_date"
					" and  DATE_C >= @stat_date_b"
					;
				Log::Trace("", __FUNCTION__, "UNIT_PRICE = [{0}],UNIT_PRICE_cr=[{1}],UNIT_PRICE_cr=[{2}]", bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE"].ToDecimal(), bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_CR"].ToDecimal(), bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_NI"].ToDecimal());
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.Parameters.Set("stat_date", stat_date);
				cmd_inq1.Parameters.Set("stat_date_b", stat_date_b.SubstringNE(0, 4)); 
				cmd_inq1.Parameters.Set("user_id", s.userid);
				cmd_inq1.Parameters.Set("dateNow", dateNow);
				cmd_inq1.Parameters.Set("cr_value", cmd_inq.GetDecimal(2));
				cmd_inq1.Parameters.Set("ni_value", cmd_inq.GetDecimal(3));
				cmd_inq1.Parameters.Set("mo_value", cmd_inq.GetDecimal(4));
				cmd_inq1.Parameters.Set("mat_code", cmd_inq.GetString(1));
				cmd_inq1.Parameters.Set("unit_price", bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE"].ToDecimal());
				cmd_inq1.Parameters.Set("unit_price_cr", bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_CR"].ToDecimal());
				cmd_inq1.Parameters.Set("unit_price_ni", bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_NI"].ToDecimal());
				cmd_inq1.ExecuteNonQuery();
				cmd_inq1.Close();

				sqlstr = " update tqmtscb03a_mx set  CONVERSION_PRICE = @unit_price"
					" ,COST = Round(WEIGHT* @unit_price,2)"
					",COST_PERT	=decode(QUALIFIED_WT,0,0,round((@unit_price*WEIGHT)/QUALIFIED_WT,2))"
					",UNIT_PRICE_XY = case when substr(GRADE_ID,1,2) in ('1F','1M','4F','4M') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_CR"].ToString() + "  when substr(GRADE_ID,1,2) in ('1A','1D','4A','4D') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_NI"].ToString() + " else 0 end "
					",COST_XY = Round(WEIGHT* (case when substr(GRADE_ID,1,2) in ('1F','1M','4F','4M') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_CR"].ToString() + " when substr(GRADE_ID,1,2) in ('1A','1D','4A','4D') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_NI"].ToString() + " else 0 end ), 2)"
					",COST_PERT_XY = decode(QUALIFIED_WT,0,0,round(WEIGHT* (case when substr(GRADE_ID,1,2) in ('1F','1M','4F','4M') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_CR"].ToString() + " when substr(GRADE_ID,1,2) in ('1A','1D','4A','4D') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_NI"].ToString() + " else 0 end )/QUALIFIED_WT,2))"
					",REC_REVISOR =@user_id"
					",REC_REVISE_TIME =@dateNow"
					" where 1=1"
					" and MAT_TYPE_DESC not in ('步骤费','回收')"
					" and INCLUDE_CR =@cr_value"
					" and INCLUDE_NI =@ni_value"
					" and INCLUDE_MO =@mo_value"
					" and MAT_CODE_DR = @mat_code"
					" and  DATE_C <= @stat_date"
					" and  DATE_C >= @stat_date_b"
					;
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.Parameters.Set("user_id", s.userid);
				cmd_inq1.Parameters.Set("dateNow", dateNow);
				cmd_inq1.Parameters.Set("stat_date", stat_date);
				cmd_inq1.Parameters.Set("stat_date_b", stat_date_b.SubstringNE(0,4));
				cmd_inq1.Parameters.Set("cr_value", cmd_inq.GetDecimal(2));
				cmd_inq1.Parameters.Set("ni_value", cmd_inq.GetDecimal(3));
				cmd_inq1.Parameters.Set("mo_value", cmd_inq.GetDecimal(4));
				cmd_inq1.Parameters.Set("mat_code", cmd_inq.GetString(1));
				cmd_inq1.Parameters.Set("unit_price", bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE"].ToDecimal());
				cmd_inq1.Parameters.Set("unit_price_cr", bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_CR"].ToDecimal());
				cmd_inq1.Parameters.Set("unit_price_ni", bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_NI"].ToDecimal());
				cmd_inq1.ExecuteNonQuery();
				cmd_inq1.Close();
			}
		}
		cmd_inq.Close(); 
	

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
