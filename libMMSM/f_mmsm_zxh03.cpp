/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    
Version:    1.0
Date:     2024
Description: 标准核算
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_getprice(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_zxh03(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString stat_date = "";
	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);

	CString datenow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EIClass EItables;

	try
	{
		stat_date = datenow.SubstringNE(0, 6);
		if (bcls_rec->Tables[0].Columns.Contains("STAT_DATE"))	
		{
			stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 6);
		}
		Log::Trace("", __FUNCTION__, "stat_date = [{0}]", stat_date); 

		//插入钢种单耗
		sqlstr = " delete from tqmtscb11c"
			" where DATE_C=@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into tqmtscb11c(REC_CREATOR,REC_CREATE_TIME,DATE_C,ST_NO,CR_PDI,NI_PDI,MO_PDI,MAT_CODE,MAT_NAME,CR_VALUE,NI_VALUE,MO_VALUE,WT_UNIT,KM_CODE,KM_NAME)"
			" select @rec_creator,@rec_create_time,@stat_date,t.st_no,nvl(CR_PDI,0),nvl(NI_PDI,0),nvl(MO_PDI,0),MAT_CODE,MAT_NAME,CR_VALUE,NI_VALUE,MO_VALUE,WT_UNIT,KM_CODE,KM_NAME"
			" from tqmtscb11a_dr t left join tqmtscb11b_dr t2 on t.st_no=t2.st_no"	
			" where t.year_mon in (select max(year_mon) from tqmtscb11a_dr )"			
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", datenow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新物料价格
		EIClass bcls_ret3;
		EIClass bcls_rec3;
		bcls_rec3.Tables[0].Columns.Add(DT_STRING, "DATE_C");
		bcls_rec3.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");
		bcls_rec3.Tables[0].Columns.Add(DT_STRING, "PRICE_TYPE");
		bcls_rec3.Tables[0].Columns.Add(DT_DECIMAL, "CR_VALUE");
		bcls_rec3.Tables[0].Columns.Add(DT_DECIMAL, "NI_VALUE");
		bcls_rec3.Tables[0].Columns.Add(DT_DECIMAL, "MO_VALUE");
		bcls_rec3.Tables[0].Rows.Add();

		sqlstr = " select MAT_CODE,CR_VALUE,NI_VALUE,MO_VALUE"
			" from TQMTSCB11C"
			" where 1=1"
			" and mat_code not in (select mat_code_dr from TQMTSCB08_DR where MAT_TYPE_DESC = '回收')"
			" and  DATE_C = @stat_date"
			" group by MAT_CODE,CR_VALUE,NI_VALUE,MO_VALUE"
			; 
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			bcls_rec3.Tables[0].Rows[0]["DATE_C"] = stat_date;
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
				sqlstr = " update TQMTSCB11C set  UNIT_PRICE = @unit_price"					
					",UNIT_PRICE_XY = case when substr(ST_NO,1,2) in ('1F','1M','4F','4M') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_CR"].ToString() + "  when substr(ST_NO,1,2) in ('1A','1D','4A','4D') then  " + bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_NI"].ToString() + " else 0 end "
					",REC_REVISOR =@user_id"
					",REC_REVISE_TIME =@datenow"
					" where 1=1"
					" and CR_VALUE =@cr_value"
					" and NI_VALUE =@ni_value"
					" and MO_VALUE =@mo_value"
					" and MAT_CODE = @mat_code"
					" and  DATE_C = @stat_date"
					;
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.Parameters.Set("stat_date", stat_date);
				cmd_inq1.Parameters.Set("user_id", s.userid);
				cmd_inq1.Parameters.Set("datenow", datenow);
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


		//回收的则根据钢种取单价		 
		sqlstr = " update TQMTSCB11C t set REC_REVISOR =@user_id"
			",REC_REVISE_TIME =@datenow"
			",UNIT_PRICE = (select UNIT_PRICE FROM TQMTSCB00_DR t2 where t2.KM_CODE "
			"in (select MAT_CODE_dr from TQMTSCB05_DR where st_no = t.st_no) and DATE_C in (select max(date_c) from TQMTSCB00_DR where KM_CODE "
			"in (select MAT_CODE_dr from TQMTSCB05_DR where st_no = t.st_no)) and rownum = 1)"
			" where 1 = 1"
			" and exists(select UNIT_PRICE FROM TQMTSCB00_DR t2 where t2.KM_CODE"
			" in (select MAT_CODE_dr from TQMTSCB05_DR where st_no = t.st_no))"
			" and mat_code in(select mat_code_dr from TQMTSCB08_DR where MAT_TYPE_DESC = '回收')"
			" and  DATE_C = @stat_date"
			;
		cmd_inq1.SetCommandText(sqlstr);
		cmd_inq1.Parameters.Set("stat_date", stat_date);
		cmd_inq1.Parameters.Set("user_id", s.userid);
		cmd_inq1.Parameters.Set("datenow", datenow);
		cmd_inq1.ExecuteNonQuery();
		cmd_inq1.Close();	

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
