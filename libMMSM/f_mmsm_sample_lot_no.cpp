/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2016-12-09
Description: 获取试批号
***********************************************************************/
#include "stdafx.h"


//int f_qmtq_sample_get(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
//int f_qmtq_sample_am(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
BM2_FUNCTION_EXPORT


int f_mmsm_sample_lot_no(EIClass *bcls_rec, CModel & tmmsm01_in, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sample_lot_no = " ";

	CModel tmmsm01("TMMSM01");

	try
	{
		//Log::Trace("", __FUNCTION__, "tmmsm01_in.ORDER_NO=[{0}] ", tmmsm01_in.ORDER_NO);
		//Log::Trace("", __FUNCTION__, "tmmsm01_in.WHOLE_BACKLOG_NO=[{0}] ", tmmsm01_in.WHOLE_BACKLOG_NO);
		//Log::Trace("", __FUNCTION__, "tmmsm01_in.WHOLE_BACKLOG_SEQ=[{0}] ", tmmsm01_in.WHOLE_BACKLOG_SEQ);
		//Log::Trace("", __FUNCTION__, "tmmsm01_in.HEAT_NO=[{0}] ", tmmsm01_in.HEAT_NO);

		//在TQMTOT3中按ORDER_NO, WHOLE_BACKLOG_NO, WHOLE_BACKLOG_SEQ查是否需取样
		CDbCommand cmd_inq("SELECT * FROM TQMTOT3 WHERE ORDER_NO = @order_no AND WHOLE_BACKLOG_NO = @whole_backlog_no AND WHOLE_BACKLOG_SEQ = @whole_backlog_seq ", conn);
		cmd_inq.Parameters.Set("order_no", tmmsm01_in["ORDER_NO"].ToString());
		cmd_inq.Parameters.Set("whole_backlog_no", tmmsm01_in["WHOLE_BACKLOG_NO"].ToString());
		cmd_inq.Parameters.Set("whole_backlog_seq", tmmsm01_in["WHOLE_BACKLOG_SEQ"].ToString());
		cmd_inq.ExecuteReader(); 
		if (cmd_inq.Read())
		{
			CDbCommand cmd_inq("SELECT SAMPLE_LOT_NO FROM TQMTOT3 WHERE ORDER_NO = @order_no AND PROD_NO_REP = @heat_no ", conn);
			cmd_inq.Parameters.Set("order_no", tmmsm01_in["ORDER_NO"].ToString());
			cmd_inq.Parameters.Set("heat_no", tmmsm01_in["HEAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				sample_lot_no = cmd_inq.GetString(1);
			}
			else{
				bcls_rec->Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"] = tmmsm01_in["WHOLE_BACKLOG_NO"].ToString();
				//f_qmtq_sample_get(bcls_rec, bcls_ret,  conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				sample_lot_no = bcls_rec->Tables[0].Rows[0]["SAMPLE_LOT_NO"].ToString().Trim();

				bcls_rec->Tables[0].Rows[0]["SAMPLE_LOT_NO"] = sample_lot_no;
				bcls_rec->Tables[0].Rows[0]["PROD_NO_REP"] = tmmsm01_in["HEAT_NO"].ToString();
				//f_qmtq_sample_am(bcls_rec, bcls_ret,  conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			//Log::Trace("", __FUNCTION__, "sample_lot_no=[{0}] ", sample_lot_no);
			tmmsm01_in["SAMPLE_LOT_NO"] = sample_lot_no;
			tmmsm01_in["SAMPLE_LOT_NO_1"] = sample_lot_no;
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


