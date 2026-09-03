/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-07-04
Description: 
更新60表库存及成份信息，致标记
如果该料仓为空仓，则给二级发送空仓信息
***********************************************************************/


/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"


/***** C++ 的业务头文件部分 *****/

BM2_FUNCTION_EXPORT
int f_mmsm_t8e2yx_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_t8e2yb_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_updlc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	CString bunker_no = "";
	CString quality_batch_no = "";
	CString mat_code = "";
	CString bunker_type = "";
	CString tcNO = "";
	CString table_name = "";
	EPEX epex;
	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_inq_s(conn);
	
	CModel tmmsm60("TMMSM60");
	CModel tmmsm85("TMMSM85");
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");	

	try
	{
		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMLCSND");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TABLE_NAME"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TABLE_NAME");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TABLE_NAME_CHILD"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TABLE_NAME_CHILD");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("ACTION"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "ACTION");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BUNKER_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BUNKER_NO");
		}
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			bunker_no = "";
			quality_batch_no = "";
			mat_code = "";

			bunker_no = bcls_rec->Tables[0].Rows[i]["BUNKER_NO"].ToString();
			if (bcls_rec->Tables[0].Columns.Contains("QUALITY_BATCH_NO"))
			{
				quality_batch_no = bcls_rec->Tables[0].Rows[i]["QUALITY_BATCH_NO"].ToString();
			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
			{
				mat_code = bcls_rec->Tables[0].Rows[i]["MAT_CODE"].ToString();
			}

			//判断是否传过来成份信息，传过来则进行删除成份

			//更新库存
			sqlstr = " update tmmsm60 set stock_wt = (select nvl(sum(stock_wt),0) from tmmsm85 where BUNKER_NO = @bunker_no)"
				" where BUNKER_NO = @bunker_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", bunker_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//更新预警
			sqlstr = " update tmmsm60 set RATE = round(STOCK_WT/UPPER_LIMIT_VALUE,3)"
				" where 1=1"
				" and UPPER_LIMIT_VALUE != 0"
				" and BUNKER_NO = @bunker_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", bunker_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//更新库存
			sqlstr = " update tmmsm60 set BACK_C3='0',BACK_C1='0',BACK_C2='0'"
				" where 1=1"
				" and BUNKER_NO = @bunker_no"
				" and stock_wt = 0"
				" and bunker_type in ('EAFBOX','BOFBOX','AODBOX')"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", bunker_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//如果高位料仓为0时需要发空仓需要给二级发送空仓信息
			sqlstr = " select stock_wt,BUNKER_TYPE,STK_NO"
				" from tmmsm60"
				" where 1=1"
				" and BUNKER_NO in ( select BUNKER_NO from tmmsm60 where FLAG1 = 'H' )"
				" and BUNKER_NO = @bunker_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", bunker_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tcNO = "";
				table_name = "";
				bunker_type = cmd_inq.GetString(2);
				if (cmd_inq.GetDecimal(1) == 0)
				{

					//20240316 给原料L2发送空仓信息T8E2Y1 / T8E2Y3 / T8E2Y5 / T8E2Y7 / T8E2Y8/ T8E2Y9/ T8E2YI（）
					if (bunker_type.SubstringNE(0, 1) == "A")
					{
						tcNO = "T8E2Y1";
						table_name = "INT_AOD_BIN";  					
					}

					else if (bunker_type.SubstringNE(0, 1) == "B")
					{
						tcNO = "T8E2Y3";
						table_name = "INT_BOF_BIN"; 
					}
					else if (bunker_type.SubstringNE(0, 1) == "E")
					{
						tcNO = "T8E2Y5";
						table_name = "INT_EAF_BIN";
					}
					else if (bunker_type.SubstringNE(0, 1) == "I")
					{
						tcNO = "T8E2Y8";
						table_name = "INT_IF_BIN";
					}
					else if (bunker_type.SubstringNE(0, 1) == "F")
					{
						tcNO = "T8E2Y9";
						table_name = "INT_LF_BIN";
					}
					else if (bunker_type.SubstringNE(0, 1) == "T")
					{
						tcNO = "T8E2YA";
						table_name = "INT_LTS_BIN";
					}
					else if (bunker_type.SubstringNE(0, 1) == "R")
					{
						tcNO = "T8E2YF";
						table_name = "INT_RH_BIN";
					}
					else if (bunker_type.SubstringNE(0, 1) == "V")
					{
						tcNO = "T8E2YI";
						table_name = "INT_VOD_BIN";
					}

					if (epex.Initialize(tcNO) < 0)
					{
						sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (epex.SetValue(table_name, "LOCATION", 0, cmd_inq.GetString(3)) < 0 ||
						epex.SetValue(table_name, "MATERIAL_CODE", 0, "EMPTY") < 0 ||
						epex.SetValue(table_name, "BATCH_NUMBER", 0, "EMPTY") < 0 ||
						epex.SetValue(table_name, "QUALITY_BATCH", 0, " ") < 0)
					{
						sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					if (epex.SendTele() < 0)
					{
						sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					// 释放
					epex.Uninitialize();

				}
			}
			cmd_inq.Close();

			//如果该成份的所有榜单都消耗完成，需要删除成份信息
			if (quality_batch_no.Trim() != "")
			{
				sqlstr = " select count(1) from tmmsm85"
					" where 1=1"
					" and quality_batch_no = @quality_batch_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("quality_batch_no", quality_batch_no);
				if (cmd_inq.ExecuteScalar().ToInt32() == 0)
				{
					//料仓成分
					bcls_rec->Tables["MMLCSND"].Rows.Clear();
					bcls_rec->Tables["MMLCSND"].Rows.Add();

					//取物料编码
					if (mat_code.Trim() == "")
					{
						sqlstr = " select mat_code from TMMSM81AL"
							" where 1=1"
							" and quality_batch_no = @quality_batch_no"
							;
						cmd_inq_s.SetCommandText(sqlstr);
						cmd_inq_s.Parameters.Set("quality_batch_no", quality_batch_no);
						cmd_inq_s.ExecuteReader();
						if (cmd_inq_s.Read())
						{
							mat_code = cmd_inq_s.GetString(1);
						}
					}
					cmd_inq_s.Close();

					//20240316 给原料L2发成份删除电文
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "MAT_CODE");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("QUALITY_BATCH_NO"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "QUALITY_BATCH_NO");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("STATION_NO"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "STATION_NO");
					}
					bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2YB";
					bcls_rec->Tables["MMLCSND"].Rows[0]["STATION_NO"] = "X0";
					bcls_rec->Tables["MMLCSND"].Rows[0]["ACTION"] = "D";
					bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"] = mat_code;
					bcls_rec->Tables["MMLCSND"].Rows[0]["QUALITY_BATCH_NO"] = quality_batch_no;
					doFlag = f_mmsm_t8e2yb_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t8e2yx_snd失败-------");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
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
