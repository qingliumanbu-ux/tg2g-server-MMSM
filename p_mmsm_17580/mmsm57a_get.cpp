/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 获取北区废钢库存,调用存储过程

**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include <regex>
//#include "h_common_aid.h"

// service入口
BM2F_ENTERACE(mmsm57a_get)
int f_mmsm57a_get(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString stat_date = "";
	CString sqlstr_where = "";
	CString heat_no = "";
	CString v_table = "tmmsm57a";
	CString cx_date = "";
	
	CModel tmmsm57a("TMMSM57A");
	CModel tmmsm57c("TMMSM57C");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	try
	{ 	

		/*Log::Info("", __FUNCTION__, "NAME =[{0}]", bcls_rec->Tables[0].get_TableName());
		Log::Info("", __FUNCTION__, "NAME =[{0}]", bcls_rec->Tables[1].get_TableName());
		Log::Info("", __FUNCTION__, "COUNT =[{0}]", bcls_rec->Tables["PARA"].Rows.get_Count());*/
		v_table = bcls_rec->Tables["PARA"].Rows[0]["TABLE_FLAG"].ToString().ToLower();
		stat_date = bcls_rec->Tables["PARA"].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 8);

		//tmmsm57a.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Info("", __FUNCTION__, "stat_date =[{0}],v_table=[{1}]", stat_date, v_table);

		if (v_table == "tmmsm57b")
		{
			if (stat_date.Trim() == "")
			{
				sprintf(s.msg, "必须传入的时间不能为空。");
				//strcpy(s.sysmsg,s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//调用存储过程
			/*
			delete from tmmsm57b where stat_date LIKE v_stat_date||'%' ;

			insert into tmmsm57b(REC_CREATE_TIME,stat_date,seq_id,mat_code,mat_name,STOCK_WT_QC,IN_STOCK_WT1,STOCK_WT_QC1,IN_STOCK_WT,IN_STOCK_WT2,OUT_STOCK_WT1
			,OUT_STOCK_WT2,OUT_STOCK_WT3,STOCK_WT1,STOCK_WT2,DIF_WT1,DIF_WT2,DIF_WT3,DIF_WT4,FT_FLAG,RATE1
			,RATE2,FT_WT1,FT_WT2,OUT_STOCK_WT,RATE3,FT_WT,REMARK,REC_TIME)
			SELECT to_char(SYSDATE, 'YYYYMMDDHHmmss'),week_day,id,mat_id,nvl(mat_desc,' '),storage_start_erp,quantity_no_upload,STORAGE_START_MANUAL_MONTH,QUANTITY_IN_MES,QUANTITY_IN_ERP,QUANTITY_CONSUME_MES
			,QUANTITY_ADJUST_MONTH,QUANTITY_ADJUST_MONTH,STORAGE_MES_TO_EIGHT,STORAGE_MANUAL_TO_EIGHT,DIFFER_STORAGE_START,DIFFER_LAST_CHECK,DIFFER_TODAY_CHECK,DIFFER_STORAGE_MONTH,ADJUST_OR_NOT,DIFFER_TODAY_CHECK_PERCENT
			,DIFFER_STORAGE_MONTH_PERCENT,QUANTITY_ADJUST_LAST_MONTH,QUANTITY_CONSUME_SUM,QUANTITY_ADJUST_SUM,QUANTITY_ADJUST_SUM_PERCENT,QUANTITY_ADJUST_LAST,nvl(REMARK,' '),to_char(TIMESTAMPS,'yyyyMMddHHmmss')
			FROM  DA_WEEK_REPORT_ARCHIVE@smp2ndb
			WHERE WEEK_DAY like v_stat_date||'%'
			*/
			CTransactionManager::Commit(0);
			sqlstr = "begin TGT8Z1.MMLC_TMMSM57B_INS(@stat_date);  end;"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date.Substring(0, 6));
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
			CTransactionManager::Begin(0, 0);  			
		}

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
