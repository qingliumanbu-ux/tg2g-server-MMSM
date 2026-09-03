/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:    songwei
Version:    1.0
Date:       2024-01-04
Description: 给资源铁区发消耗
**************************************************/
//框架头文件
#include "stdafx.h" 

//业务头文件


//外部函数声明
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_t8e2yx_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_t8e2yb_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21c005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21b006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21c004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(mmsm2a_sndn)

int f_mmsm2a_sndn(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int i_idx = 0;
	int n_idx = 0;
	int d_idx = 0;
	CDecimal cd_stock_wt = 0;
	CDecimal cd_seq_no = 0;
	CDecimal nd_seq_no = 0;
	CString SeqNo = "";
	CString SeqNo1 = "";
	CString missing_no = "";
	CString serial_number = "";
	CString	datetime1 = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CString	datetime("");
	CString bunker_no("");
	CString excludeCode = "";
	CString matCode = "";
	CString bunkerType = "";
	CString old_missing_no = "";
	EIClass EITable;

	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm50("TMMSM50");
	CModel tmmsm60("TMMSM60");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm85_Z("TMMSM85");
	CModel tmmsm89("TMMSM89");
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sql = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获得传入参数 */
		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMLCSND");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PROC_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "HEAT_NO");
		}

		//测试接口 废钢料篮计量信息21C004
		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMLCSND");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WORK_SEQ_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WORK_SEQ_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("FACTORY_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "FACTORY_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DST_STOCK_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DST_STOCK_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WORK_DATE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WORK_DATE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BASKET_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BASKET_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "MAT_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CNAME"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "MAT_CNAME");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("SRC_STOCK_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "SRC_STOCK_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("SRC_STOCK_PLACE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "SRC_STOCK_PLACE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BUY_ORDER_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BUY_ORDER_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("NET_WGT"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "NET_WGT");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WEIGH_TIME"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WEIGH_TIME");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BUNKER_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BUNKER_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("LOT_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "LOT_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("QUALITY_BATCH_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "QUALITY_BATCH_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("STOCK_WT"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "STOCK_WT");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("STATION_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "STATION_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("ACTION"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "ACTION");
		}

		bunker_no = bcls_rec->Tables["MMSM835S2N"].Rows[0]["BUNKER_NO"];
		tmmsm85["BUNKER_NO"] = bunker_no;

		if (bunker_no.SubstringNE(0, 2) != "BL")
		{
			sprintf(s.msg, "该料槽不是BOF废钢料槽,无法操作！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sqlstr = "SELECT BACK_C2,BACK_C3,BACK_C1 "
			"   FROM tmmsm60 "
			"  WHERE 1=1 and BUNKER_TYPE IN ('EAFBOX','BOFBOX','AODBOX') "
			" AND  bunker_no = @bunker_no "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("bunker_no", bunker_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			if (cmd_inq.GetString(3) == "0")
			{
				sprintf(s.msg, "该料篮物料未使用无法发送二级!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (cmd_inq.GetString(1) == "1")
			{
				sprintf(s.msg, "该料篮物料已发送到镍板库无法发送二级!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (cmd_inq.GetString(2) == "1")
			{
				sprintf(s.msg, "该料篮物料炉前状态无法发送二级!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		cmd_inq.Close();


		//料槽料篮配料传资源时排除的物料编码
		sqlstr = " select mat_code from tmmsm50 where  MAT_CODE LIKE 'F%' AND substr(MAT_CODE, 1, 3) NOT in ('F06') and  mat_code not IN (SELECT CODE FROM TEP0002 WHERE CODE_CLASS = 'MMLC01') ";
		;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			matCode = cmd_inq.GetString(1);
			excludeCode += matCode + ",";
		}
		cmd_inq.Close();
		Log::Trace(" ", __FUNCTION__, "excludeCode=[{0}]", excludeCode);	

			sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_L2LS.NEXTVAL),9 ) FROM DUAL ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				SeqNo1 = cmd_inq.GetString(1).Trim();

			}
			cmd_inq.Close();
		
			tmmsm85["SEQ_NO_TM"] = SeqNo1;
			
				sql = "  SELECT LPAD(TO_CHAR(MMLC_SJ.NEXTVAL),3 ,'0') FROM DUAL ";
				cmd_inq.SetCommandText(sql);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					SeqNo = cmd_inq.GetString(1).Trim();
				}
				cmd_inq.Close();
				missing_no = "ZY" + datetime1 + SeqNo + "S";
				old_missing_no = tmmsm85["MISSING_NO"];
				tmmsm85["MISSING_NO"] = missing_no;
				tmmsm85.Update("SEQ_NO_TM,MISSING_NO", "BUNKER_NO");
			

			

			tmmsm60["BUNKER_NO"] = bunker_no;
			tmmsm60.Query("BUNKER_NO");
			bunkerType = tmmsm60["BUNKER_TYPE"];
			
			sqlstr = " SELECT * FROM TMMSM85 WHERE bunker_no = @bunker_no ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", bunker_no);
			cmd_inq.ExecuteQuery(EITable.Tables[0]);
			cmd_inq.Close();

			CString SeqNo1 = "";
			if (EITable.Tables[0].Rows.get_Count() > 0)
			{
				for (int i = 0; i < EITable.Tables[0].Rows.get_Count(); i++)
				{
					tmmsm85_Z.Reset();
					sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_LS.NEXTVAL),18 ) FROM DUAL ";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						SeqNo1 = cmd_inq.GetString(1).Trim();

					}
					cmd_inq.Close();
					serial_number = "EG" + SeqNo1;
					tmmsm85_Z["BUNKER_NO"] = EITable.Tables[0].Rows[i]["BUNKER_NO"].ToString();
					tmmsm85_Z["MAT_CODE"] = EITable.Tables[0].Rows[i]["MAT_CODE"].ToString();
					tmmsm85_Z["WEIGH_NO"] = EITable.Tables[0].Rows[i]["WEIGH_NO"].ToString();
					tmmsm85_Z["SEQ_NO"] = EITable.Tables[0].Rows[i]["SEQ_NO"];
					tmmsm85_Z["SERIAL_NUMBER"] = serial_number;
					tmmsm85_Z.Update("SERIAL_NUMBER", "BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");

					matCode = EITable.Tables[0].Rows[i]["MAT_CODE"];
					bcls_rec->Tables["MMLCSND"].Rows.Clear();
					bcls_rec->Tables["MMLCSND"].Rows.Add();
					if (excludeCode.Find(matCode) != string::npos)
					{
						bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "I";
						bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21C004";
						bcls_rec->Tables["MMLCSND"].Rows[0]["WORK_SEQ_NO"] = serial_number; //实绩流水号
						bcls_rec->Tables["MMLCSND"].Rows[0]["FACTORY_CODE"] = "6240"; //工厂代码
						bcls_rec->Tables["MMLCSND"].Rows[0]["DST_STOCK_CODE"] = "6241"; //目的库区代码	
						bcls_rec->Tables["MMLCSND"].Rows[0]["WORK_DATE"] = EITable.Tables[0].Rows[i]["RECEIVE_DATA_TIME"]; //作业日期
						bcls_rec->Tables["MMLCSND"].Rows[0]["BASKET_NO"] = EITable.Tables[0].Rows[i]["BUNKER_NO"]; //料篮号
						bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"] = EITable.Tables[0].Rows[i]["MAT_CODE"]; //物料代码
						bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CNAME"] = EITable.Tables[0].Rows[i]["MAT_NAME"]; //物料名称
						bcls_rec->Tables["MMLCSND"].Rows[0]["SRC_STOCK_CODE"] = "6062"; // 源库区代码
						bcls_rec->Tables["MMLCSND"].Rows[0]["SRC_STOCK_PLACE"] = EITable.Tables[0].Rows[i]["BUNKER_NO_ORIGINAL"]; //源库位代码
						bcls_rec->Tables["MMLCSND"].Rows[0]["BUY_ORDER_NO"] = EITable.Tables[0].Rows[i]["MISSING_NO"]; //采购订单号
						bcls_rec->Tables["MMLCSND"].Rows[0]["NET_WGT"] = EITable.Tables[0].Rows[i]["STOCK_WT"];  //净重
						bcls_rec->Tables["MMLCSND"].Rows[0]["WEIGH_TIME"] = EITable.Tables[0].Rows[i]["TIME_1"]; //称量时刻	
						doFlag = f_mmsm_21c004_snd(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c004_snd失败-------");
							throw CApplicationException(-1, s.msg, log.Location);
						}

					}
					//料仓成分 						
					bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2YB";
					bcls_rec->Tables["MMLCSND"].Rows[0]["ACTION"] = "I";
					bcls_rec->Tables["MMLCSND"].Rows[0]["STATION_NO"] = bunkerType.Substring(0, 1);
					bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"] = EITable.Tables[0].Rows[i]["MAT_CODE"].ToString();
					bcls_rec->Tables["MMLCSND"].Rows[0]["QUALITY_BATCH_NO"] = EITable.Tables[0].Rows[i]["QUALITY_BATCH_NO"].ToString();
					doFlag = f_mmsm_t8e2yb_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t8e2yb_snd失败-------");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					tmmsm89.Reset();
					tmmsm89.MergeFrom(EITable.Tables[0].Rows[i]);
					tmmsm89["EVENT_CODE"] = "SNDL2";
					tmmsm89["EVENT_DESC"] = "发送炉后废钢";
					tmmsm89["EVENT_NAME"] = "发送炉后废钢";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm89["REC_CREATE_TIME"] = datetime;
					tmmsm89["BUNKER_NO_ORIGINAL2"] = tmmsm89["BUNKER_NO_ORIGINAL"].ToString();
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm89["BUNKER_NO"].ToString();
					tmmsm89["BUNKER_NO"] = "BMBL";

					tmmsm89["SERIAL_NUMBER"] = serial_number;
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);

				}
				//将转炉料槽的信息直接转到炉后废钢加料虚拟料槽
				sqlstr = "update tmmsm85 set BUNKER_NO = 'BMBL'"
					",BUNKER_NO_ORIGINAL2 = BUNKER_NO_ORIGINAL"
					" ,BUNKER_NO_ORIGINAL = @bunker_no"
					" where 1=1"
					" and BUNKER_NO = @bunker_no"
					;
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.Parameters.Set("bunker_no", bunker_no);
				cmd_inq1.ExecuteNonQuery();
				cmd_inq1.Close();

				//4）料槽使用状态变为“未使用”，上传状态变为“未上传”
				sqlstr = "update tmmsm60 set BACK_C1 = '0'" //未使用
					",BACK_C3 = '0'" //未上传
					",stock_wt = 0 "
					" where 1=1"
					" and BUNKER_NO = @bunker_no"
					;
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.Parameters.Set("bunker_no", bunker_no);
				cmd_inq1.ExecuteNonQuery();
				cmd_inq1.Close();

				//更新库存
				sqlstr = " update tmmsm60 set stock_wt = (select nvl(sum(stock_wt),0) from tmmsm85 where BUNKER_NO = 'BMBL')"
					" where BUNKER_NO ='BMBL'"
					;
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.ExecuteNonQuery();
				cmd_inq1.Close();
			}

		if (bcls_rec_tmmsm89_log.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


