/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 临时录入计量单号进行收货,更新料仓号的上料信息和更新库存 ,发送
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm81xz1_ins)
int f_mmsm_21a001_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21a010_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21c011_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21c006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21b005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm81xz1_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString datetime1("");
	CString datetime("");
	datetime1 = CDateTime::Today().ToString("yyyyMMdd");
	datetime1 = datetime1.Substring(2, 6);
	int   blkNum;

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm81_s("TMMSM81_S");
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm89("TMMSM89");
	CModel tmmsm50("TMMSM50");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm60("TMMSM60");

	CDbCommand cmd_inq(conn);

	try
	{ 		

		tmmsm81_s.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");	
			//CString  dh = "S" + datetime + EPGetNextSeq("SQ_JLYLID", conn);
			tmmsm81_s["STOCK_WT"] = tmmsm81_s["NET_WT"].ToDecimal();
			tmmsm81_s["RECEIVE_DATA_TIME"] = datetime;
			tmmsm81_s["REC_CREATOR"] = s.userid;   //记录创建责任者
			tmmsm81_s["REC_CREATE_TIME"] = datetime;   //记录创建时刻
			tmmsm81_s["BUNKER_NO"] = bcls_rec->Tables[1].Rows[0]["BUNKER_NO"].ToString();  //料仓号
			tmmsm81_s["MAT_RCV_TIME"] = datetime;  //料仓号
			// 标记确认时那个画面新增的数据 便于查询 5 自循环物料收货画面 6原料进厂临时画面由前台传入 ，7废钢进厂
			//tmmsm81_s["MARK_POS_CODE"] = "6";
			tmmsm50["MAT_CODE"] = tmmsm81_s["MAT_CODE"];
			CString mat_code_lot_no = "";
			if (tmmsm50.QueryCount("MAT_CODE") == 1)
			{
				tmmsm50.Query("MAT_CODE");

				mat_code_lot_no = tmmsm50["MAT_CODE_L2"].ToString() + "@" + tmmsm50["LOT_NO"].ToString();
				//20250206wcm
				 CString mat_code = "";
				 mat_code = tmmsm50["MAT_CODE"];

				 CString mat_type = Db::QueryCString("select mat_type from tmmsm50 where mat_code='" + mat_code + "'");
				 Log::Trace(" ", __FUNCTION__, "mat_type = [{0}]", mat_type);
				if (mat_type.Trim() == "")
					{
						strcpy(s.msg, "物料编码" + mat_code + "的物料类型不能为空,请先配置!");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
			}
			
			tmmsm81_s["MAT_NAME"] = tmmsm50["MAT_NAME"].ToString() ;
			tmmsm81_s["MAT_TYPE"] = tmmsm50["MAT_TYPE"].ToString();	
			tmmsm81_s["MATERIAL_NAME"] = tmmsm50["MATERIAL_NAME"].ToString();
			
			tmmsm81_s.TrimOrBlank();
			tmmsm81_s["FORM_EDIT_FLAG"] = "0";
			tmmsm81_s.Insert();
			tmmsm60["BUNKER_NO"] = tmmsm81_s["BUNKER_NO"];
			tmmsm60.Query("BUNKER_NO");
			tmmsm89["BUNKER_TYPE"] = tmmsm60["BUNKER_TYPE"].ToString();
			tmmsm89["BUNKER_NAME"] = tmmsm60["BUNKER_NAME"].ToString();
			tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60["BUNKER_NO"].ToString();
			tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60["BUNKER_TYPE"].ToString();
			tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60["BUNKER_NAME"].ToString();

			EIClass bcls_rec_tmmsm89_log;
			bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);
			tmmsm89.CopyFrom(tmmsm81_s);
			tmmsm89["EVENT_CODE"] = "IN";
			tmmsm89["EVENT_DESC"] = "一般入库";
			if (tmmsm81_s["MARK_POS_CODE"].ToString() == "6")
			{
				tmmsm89["EVENT_NAME"] = "原料临时进厂";
			}
			else
			{
				tmmsm89["EVENT_NAME"] = "废钢临时进厂";
			}

			
			
			bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
			bcls_rec_tmmsm89_log.Tables[0].Rows[0].Merge(tmmsm89);

			//插入上料信息
			tmmsm85.CopyFrom(tmmsm81_s);
			tmmsm85["REC_CREATE_TIME"] = s.datetime;
			tmmsm85["TIME_INSTOCK"] = datetime;
			tmmsm85["REC_CREATOR"] = s.userid;
			tmmsm85["MAT_CODE_LOT_NO"] = mat_code_lot_no;
			tmmsm85["STOCK_WT"] = tmmsm81_s["NET_WT"].ToDecimal();
			tmmsm85["TIME_INSTOCK"] = tmmsm81_s["MAT_RCV_TIME"].ToString();
			tmmsm85["BUNKER_TYPE"] = tmmsm60["BUNKER_TYPE"].ToString();
			tmmsm85["BUNKER_NAME"] = tmmsm60["BUNKER_NAME"].ToString();
			//获取流水号
			sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSM85     WHERE 1=1   AND BUNKER_NO	= @bunker_no";
			cmd_inq.Parameters.Set("bunker_no", tmmsm85["BUNKER_NO"].ToString());		
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm85["SEQ_NO"] = cmd_inq.GetDecimal(1) + 1;
			}
			cmd_inq.Close();
			tmmsm85.TrimOrBlank();
			tmmsm85.Insert(); 			

			//更新料仓库存表
			
			tmmsm60["STOCK_WT"] = tmmsm60["STOCK_WT"].ToDecimal() + tmmsm85["STOCK_WT"].ToDecimal();
			if (tmmsm60["UPPER_LIMIT_VALUE"].ToDecimal() != 0)
			{
				tmmsm60["RATE"] = tmmsm60["STOCK_WT"].ToDecimal() / tmmsm60["UPPER_LIMIT_VALUE"].ToDecimal();
			}
			tmmsm60.Update("RATE,STOCK_WT", "BUNKER_NO"); 
			
			  //发送履历
			doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/*
			//发送电文
			//给物流/资源/铁区-汽运/火车采购进厂卸货确认 
			//1、直供物料确认卸货时给物流系统发送电文21A001 - 汽运采购进厂卸货确认，同时按照物料编码区分给铁区系统发送电文21B005 - 卸车确认 或者 资源系统发送电文21C011 - 采购进厂卸货确认信息
			//2、配送物料及调拨物料确认卸货时，给物流系统发送电文21A010 - 汽运、铁路卸车确认，由物流系统给资源系统或者铁区系统转发卸车电文
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
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PRIMARY_KEY"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PRIMARY_KEY");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PRIMARY_DATA"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PRIMARY_DATA");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WEIGH_NO"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WEIGH_NO");
			}
			bcls_rec->Tables["MMLCSND"].Rows.Add();
			bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "I";
			bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "TMMSM81";
			bcls_rec->Tables["MMLCSND"].Rows[0]["PRIMARY_KEY"] = "WEIGH_NO";
			bcls_rec->Tables["MMLCSND"].Rows[0]["PRIMARY_DATA"] = tmmsm81_s["WEIGH_NO"];

			//AUART 类型  配送或调拨-C 直供-B
			if (tmmsm81_s["AUART"].ToString() == "B")
			{
				bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21C006";
				doFlag = f_mmsm_21c006_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21a001_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else
			{
				bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21A010";
				doFlag = f_mmsm_21a010_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21a010_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			//SYSTEM_ID_MAT 物料来源系统   资源-C 铁区-B 			
			if (tmmsm50["SYSTEM_ID_MAT"].ToString() == "B")
			{
				bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21B005";
				doFlag = f_mmsm_21b005_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21b005_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else
			{
				bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21C011";
				doFlag = f_mmsm_21c011_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21C011_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			  */
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
