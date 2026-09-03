/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      吴新
Version:     1.0
Date:        2016-04-25
Description: 库位移动履历
**************************************************/

#include "stdafx.h"
//
//#include "twma1.h" 
//#include "tmmsm89.h"

BM2_FUNCTION_EXPORT
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int retCnt = 0;
	CString sqlstr = "";
	CString s_resume_seq_no = " ";

	CString s_shift_group = " ", s_shift_no = " ", s_operate_time = " ";

	CDbCommand cmd_inq(conn);
	CString SeqNo = "";
	CDbCommand	execute_sql(conn);
	//定义表实体对象
	CModel tmmsm89 = CModel("TMMSM89");
	//CTWMA1 twma1(conn);
	//Ctmmsm89 tmmsm89(conn);

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	Log::Info("", __FUNCTION__, "s.fore_ip =[{0}]", s.fore_ip);

	try
	{
		


		//获取班组班次信息/////////////////////////////////
		if (bcls_rec->Tables[0].Rows[0]["EVENT_NAME"].ToString() == "料槽料篮上料" || bcls_rec->Tables[0].Rows[0]["EVENT_NAME"].ToString() == "镍板库上料")
		{
			f_epep_get_shift_group("SMCP", datetime, s_shift_no, s_shift_group, conn);
		}
		else
		{
			f_epep_get_shift_group("SM", datetime, s_shift_no, s_shift_group, conn);
		}
	

		//传入参数检核
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			sqlstr = "SELECT  LPAD(TO_CHAR(RESUME_SEQ_NO.NEXTVAL), 8, '0') FROM DUAL ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{ 
				SeqNo = cmd_inq.GetString(1).Trim();
			}
			cmd_inq.Close(); 			
			tmmsm89.Reset();
			tmmsm89.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (tmmsm89["BUNKER_NO_ORIGINAL"].ToString().Trim() != "")
			{
				sqlstr = "SELECT  BUNKER_TYPE,BUNKER_NAME FROM tmmsm60 where BUNKER_NO = @BUNKER_NO ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("BUNKER_NO", tmmsm89["BUNKER_NO_ORIGINAL"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsm89["BUNKER_TYPE_ORIGINAL"] = cmd_inq.GetString(1);
					tmmsm89["BUNKER_NAME_ORIGINAL"] = cmd_inq.GetString(2);
				}
				cmd_inq.Close();
			}


			sqlstr = "SELECT  BUNKER_TYPE,BUNKER_NAME FROM tmmsm60 where BUNKER_NO = @BUNKER_NO ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("BUNKER_NO", tmmsm89["BUNKER_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm89["BUNKER_TYPE"] = cmd_inq.GetString(1);
				tmmsm89["BUNKER_NAME"] = cmd_inq.GetString(2);
			}
			cmd_inq.Close();

			tmmsm89["REC_CREATOR"] = s.userid;
			tmmsm89["REC_CREATE_TIME"] = datetime;
			tmmsm89["RESUME_SEQ_NO"] = datetime + SeqNo;
			tmmsm89["EVENT_TIME"] = datetime;
			tmmsm89["SHIFT_GROUP"] = s_shift_group;
			tmmsm89["SHIFT_NO"] = s_shift_no;
			tmmsm89["FUNC_ID"] = s.svc_name;
			tmmsm89["CLIENT_IP"] = s.fore_ip;
			tmmsm89.TrimOrBlank();
			tmmsm89.Insert();  ///写库位移动履历
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
	}
	catch (const CApplicationException& ex)
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

	return(doFlag);
}

