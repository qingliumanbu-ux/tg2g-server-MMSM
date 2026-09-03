/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2023-11-16
Description: 鱼雷罐倒铁实绩操作函数
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

int f_getSeqNextValue(CString SEQ_NAME, CString& SEQ_VALUE, CDbConnection * conn);
int InsertTmmsm12a(CModel &ttmsm11, CModel &tmmsm12, CModel &tmmsm12a, CDbConnection * conn);
int UpdateTmmsm12a(CModel &ttmsm11, CModel &tmmsm12, CModel &tmmsm12a, CDbConnection * conn);
int DeleteTmmsm12a(CModel &ttmsm11, CModel &tmmsm12, CModel &tmmsm12a, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm12a_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_proc_div = "";
	CString v_pract_coll_mode = "";
	CString v_factory_div = "";

	CModel tmmsm12a("TMMSM12A");
	CModel tmmsm12("TMMSM12");
	CModel ttmsm11("TTMSM11");

	CDbCommand cmd(conn);

	try{
		//获取操作标志
		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();

		//传入参数接收
		tmmsm12a.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm12.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//铁包信息判断
		if (ttmsm11.Query("IRON_LADLE_NO")){
			//判断铁包号状态
			if (ttmsm11["MIT_STATUS"].ToString().Trim() == "06")
			{
				strcpy(s.msg, "铁包号" + ttmsm11["IRON_LADLE_NO"].ToString() + "报废不能新增。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else{
			strcpy(s.msg, "铁包号" + ttmsm11["IRON_LADLE_NO"].ToString() + "在铁包工器具不存在。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (v_proc_div == "I"){
			if (InsertTmmsm12a(ttmsm11, tmmsm12, tmmsm12a, conn) != 0){
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}else if(v_proc_div == "U"){
			if (UpdateTmmsm12a(ttmsm11, tmmsm12, tmmsm12a, conn) != 0){
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else if (v_proc_div == "D"){
			if (DeleteTmmsm12a(ttmsm11, tmmsm12, tmmsm12a, conn) != 0){
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else if(v_proc_div == "DWQR"){
			if (tmmsm12.Query("TPD_NO")){
				tmmsm12["REC_REVISOR"] = s.userid;
				tmmsm12["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				switch (tmmsm12a["PROC_COUNT"].ToDecimal().ToInt32())
				{
				case 1:
					tmmsm12["POUR_FLAG1"] = "Y";
					break;
				case 2:
					tmmsm12["POUR_FLAG2"] = "Y";
					break;
				case 3:
					tmmsm12["POUR_FLAG3"] = "Y";
					break;
				case 4:
					tmmsm12["POUR_FLAG4"] = "Y";
					break;
				default:
					break;
				}
				tmmsm12.Update("REC_REVISOR, REC_REVISE_TIME, POUR_FLAG1, POUR_FLAG2, POUR_FLAG3, POUR_FLAG4", "TPD_NO");
			
				//发送铁区
			}
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg();
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

int InsertTmmsm12a(CModel& ttmsm11, CModel& tmmsm12, CModel& tmmsm12a, CDbConnection * conn){

	CTracer log(__FUNCTION__);

	int doFlag = 0;
	CString sqlstr = "";
	CDbCommand cmd(conn);

	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";

	try{
		//铁水受铁实绩查询
		tmmsm12["POUR_FLAG"] = "N";

		//鱼雷罐倒铁实绩创建人、创建时间信息
		tmmsm12a["REC_CREATOR"] = s.userid;
		tmmsm12a["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//铁水包修改人、修改时间信息
		ttmsm11["REC_REVISOR"] = s.userid;
		ttmsm11["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

		if (tmmsm12.Query("IRON_LADLE_NO, POUR_FLAG")){
			tmmsm12.Print();
			//铁水包受铁实绩修改人、修改时间信息
			tmmsm12["REC_REVISOR"] = s.userid;
			tmmsm12["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

			//判断铁水包倒罐次数
			if (tmmsm12["MOLTIRON_WT1"].ToDecimal() == 0){
				tmmsm12a["PROC_COUNT"] = "1";
				tmmsm12["IRON_NO1"] = tmmsm12a["IRON_NO"];
				tmmsm12["TPC_YL_NO1"] = tmmsm12a["TPC_YL_NO"];
				tmmsm12["MOLTIRON_WT1"] = tmmsm12a["MOLTIRON_WT"];
				tmmsm12["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT"].ToDecimal() + tmmsm12a["MOLTIRON_WT"].ToDecimal();
			}
			else if (tmmsm12["MOLTIRON_WT2"].ToDecimal() == 0){
				tmmsm12a["PROC_COUNT"] = "2";
				tmmsm12["IRON_NO2"] = tmmsm12a["IRON_NO"];
				tmmsm12["TPC_YL_NO2"] = tmmsm12a["TPC_YL_NO"];
				tmmsm12["MOLTIRON_WT2"] = tmmsm12a["MOLTIRON_WT"];
				tmmsm12["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT"].ToDecimal() + tmmsm12a["MOLTIRON_WT"].ToDecimal();
			}
			else if (tmmsm12["MOLTIRON_WT3"].ToDecimal() == 0){
				tmmsm12a["PROC_COUNT"] = "3";
				tmmsm12["IRON_NO3"] = tmmsm12a["IRON_NO"];
				tmmsm12["TPC_YL_NO3"] = tmmsm12a["TPC_YL_NO"];
				tmmsm12["MOLTIRON_WT3"] = tmmsm12a["MOLTIRON_WT"];
				tmmsm12["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT"].ToDecimal() + tmmsm12a["MOLTIRON_WT"].ToDecimal();
			}
			else if (tmmsm12["MOLTIRON_WT4"].ToDecimal() == 0){
				tmmsm12a["PROC_COUNT"] = "4";
				tmmsm12["IRON_NO4"] = tmmsm12a["IRON_NO"];
				tmmsm12["TPC_YL_NO4"] = tmmsm12a["TPC_YL_NO"];
				tmmsm12["MOLTIRON_WT4"] = tmmsm12a["MOLTIRON_WT"];
				tmmsm12["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT"].ToDecimal() + tmmsm12a["MOLTIRON_WT"].ToDecimal();
			}
			else{
				strcpy(s.msg, "该铁水包倒铁次数已达上限！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//更新铁水包受铁实绩TMMSM12
			tmmsm12.Update("REC_REVISOR, REC_REVISE_TIME, MOLTIRON_WT, IRON_NO1, TPC_YL_NO1, MOLTIRON_WT1,"
				"IRON_NO2, TPC_YL_NO2, MOLTIRON_WT2,"
				"IRON_NO3, TPC_YL_NO3, MOLTIRON_WT3,"
				"IRON_NO4, TPC_YL_NO4, MOLTIRON_WT4", "IRON_LADLE_NO, POUR_FLAG");
			//新增鱼雷罐倒铁实绩
			tmmsm12a.Insert();
		}
		else
		{
			tmmsm12["REC_CREATOR"] = s.userid;
			tmmsm12["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			CString seqValue = "";
			if (f_getSeqNextValue("TPD_NO_SEQ", seqValue, conn) != 0){
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm12["TPD_NO"] = CDateTime::Now().ToString("yyyyMMdd") + seqValue;

			//作业班次、作业班组计算
			if (tmmsm12["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm12["PROD_SHIFT_GROUP"].ToString().Trim() == "")
			{
				f_epep_get_shift_group("SM", tmmsm12["START_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
				tmmsm12["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
				tmmsm12["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;
			}

			//计算倒罐时间
			sqlstr = "SELECT (TO_DATE(@end_time, 'yyyy-mm-dd hh24:mi:ss') - "
				"TO_DATE(@start_time, 'yyyy-mm-dd hh24:mi:ss')) * 24 * 60 MINT "
				"FROM DUAL";
			cmd.Close();
			cmd.SetCommandText(sqlstr);
			cmd.Parameters.Set("start_time", tmmsm12["START_TIME"].ToString());
			cmd.Parameters.Set("end_time", tmmsm12["END_TIME"].ToString());

			cmd.ExecuteReader();
			if (cmd.Read()){
				tmmsm12["TPD_DURATION"] = cmd.GetDecimal(1);
			}
			tmmsm12["MOLTIRON_WT"] = tmmsm12a["MOLTIRON_WT"];
			tmmsm12["IRON_NO1"] = tmmsm12a["IRON_NO"];
			tmmsm12["TPC_YL_NO1"] = tmmsm12a["TPC_YL_NO"];
			tmmsm12["MOLTIRON_WT1"] = tmmsm12a["MOLTIRON_WT"];
			tmmsm12["POUR_FLAG1"] = "N";
			tmmsm12["POUR_FLAG2"] = "N";
			tmmsm12["POUR_FLAG3"] = "N";
			tmmsm12["POUR_FLAG4"] = "N";
			tmmsm12["POUR_FLAG"] = "N";
			//新增铁水包受铁实绩TMMSM12
			tmmsm12.Insert();

			//新增鱼雷罐倒铁实绩TMMSM12A
			tmmsm12a["PROC_COUNT"] = "1";
			tmmsm12a.Insert();
		}

		//更新铁水包信息
		ttmsm11["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT"];
		ttmsm11["MOLTIRON_WT1"] = tmmsm12["MOLTIRON_WT1"];
		ttmsm11["MOLTIRON_WT2"] = tmmsm12["MOLTIRON_WT2"];
		ttmsm11["MOLTIRON_WT3"] = tmmsm12["MOLTIRON_WT3"];
		ttmsm11["MOLTIRON_WT4"] = tmmsm12["MOLTIRON_WT4"];
		ttmsm11.Update("REC_REVISOR, REC_REVISE_TIME, MOLTIRON_WT, MOLTIRON_WT1, MOLTIRON_WT2, MOLTIRON_WT3, MOLTIRON_WT4", "IRON_LADLE_NO");
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}

int UpdateTmmsm12a(CModel& ttmsm11, CModel& tmmsm12, CModel& tmmsm12a, CDbConnection * conn){
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	CString sqlstr = "";
	CDbCommand cmd(conn);
	CModel tmmsm12_old("TMMSM12");
	CModel tmmsm12a_old("TMMSM12A");

	try{
		//数据库原始对象赋值
		tmmsm12_old.CopyFrom(tmmsm12);
		tmmsm12a_old.CopyFrom(tmmsm12a);

		if (tmmsm12a_old.Query("IRON_LADLE_NO, IRON_NO, TPC_YL_NO, PROC_COUNT"))
		{
			tmmsm12a_old["REC_REVISOR"] = s.userid;
			tmmsm12a_old["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm12a_old["MOLTIRON_WT"] = tmmsm12a["MOLTIRON_WT"];
			tmmsm12a_old.Update("REC_REVISOR, REC_REVISE_TIME, MOLTIRON_WT", "IRON_LADLE_NO, IRON_NO, TPC_YL_NO, PROC_COUNT");

			if (tmmsm12_old.Query("TPD_NO")){
				tmmsm12_old["REC_REVISOR"] = s.userid;
				tmmsm12_old["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

				switch (tmmsm12a["PROC_COUNT"].ToDecimal().ToInt32())
				{
				case 1:
					tmmsm12_old["MOLTIRON_WT"] = tmmsm12_old["MOLTIRON_WT"].ToDecimal() + (tmmsm12a["MOLTIRON_WT"].ToDecimal() - tmmsm12_old["MOLTIRON_WT1"].ToDecimal());
					tmmsm12_old["MOLTIRON_WT1"] = tmmsm12a["MOLTIRON_WT"];
					ttmsm11["MOLTIRON_WT1"] = tmmsm12a["MOLTIRON_WT"];
					break;
				case 2:
					tmmsm12_old["MOLTIRON_WT"] = tmmsm12_old["MOLTIRON_WT"].ToDecimal() + (tmmsm12a["MOLTIRON_WT"].ToDecimal() - tmmsm12_old["MOLTIRON_WT2"].ToDecimal());
					tmmsm12_old["MOLTIRON_WT2"] = tmmsm12a["MOLTIRON_WT"];
					ttmsm11["MOLTIRON_WT2"] = tmmsm12a["MOLTIRON_WT"];
					break;
				case 3:
					tmmsm12_old["MOLTIRON_WT"] = tmmsm12_old["MOLTIRON_WT"].ToDecimal() + (tmmsm12a["MOLTIRON_WT"].ToDecimal() - tmmsm12_old["MOLTIRON_WT3"].ToDecimal());
					tmmsm12_old["MOLTIRON_WT3"] = tmmsm12a["MOLTIRON_WT"];
					ttmsm11["MOLTIRON_WT3"] = tmmsm12a["MOLTIRON_WT"];
					break;
				case 4:
					tmmsm12_old["MOLTIRON_WT"] = tmmsm12_old["MOLTIRON_WT"].ToDecimal() + (tmmsm12a["MOLTIRON_WT"].ToDecimal() - tmmsm12_old["MOLTIRON_WT4"].ToDecimal());
					tmmsm12_old["MOLTIRON_WT4"] = tmmsm12a["MOLTIRON_WT"];
					ttmsm11["MOLTIRON_WT4"] = tmmsm12a["MOLTIRON_WT"];
					break;
				default:
					break;
				}

				tmmsm12_old.Update("REC_REVISOR, REC_REVISE_TIME, MOLTIRON_WT, MOLTIRON_WT1, MOLTIRON_WT2, MOLTIRON_WT3, MOLTIRON_WT4", "TPD_NO");
			
				ttmsm11["REC_REVISOR"] = s.userid;
				ttmsm11["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				ttmsm11["MOLTIRON_WT"] = tmmsm12_old["MOLTIRON_WT"];
				ttmsm11.Update("REC_REVISOR, REC_REVISE_TIME, MOLTIRON_WT, MOLTIRON_WT1, MOLTIRON_WT2, MOLTIRON_WT3, MOLTIRON_WT4", "IRON_LADLE_NO");
			}
			else{
				strcpy(s.msg, "找不到对应的铁水包受铁记录！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else{
			strcpy(s.msg, "找不到对应的鱼雷罐倒铁记录！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}

int DeleteTmmsm12a(CModel& ttmsm11, CModel& tmmsm12, CModel& tmmsm12a, CDbConnection * conn){
	CTracer log(__FUNCTION__);

	int doFlag = 0;

	try{
		tmmsm12a.Delete("IRON_LADLE_NO, IRON_NO, TPC_YL_NO, PROC_COUNT");

		if (tmmsm12.Query("TPD_NO")){
			tmmsm12["REC_REVISOR"] = s.userid;
			tmmsm12["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

			switch (tmmsm12a["PROC_COUNT"].ToDecimal().ToInt32())
			{
			case 1:
				tmmsm12["IRON_NO1"] = " ";
				tmmsm12["TPC_YL_NO1"] = " ";
				tmmsm12["MOLTIRON_WT1"] = 0;
				ttmsm11["MOLTIRON_WT1"] = 0;
				break;
			case 2:
				tmmsm12["IRON_NO2"] = " ";
				tmmsm12["TPC_YL_NO2"] = " ";
				tmmsm12["MOLTIRON_WT2"] = 0;
				ttmsm11["MOLTIRON_WT2"] = 0;
				break;
			case 3:
				tmmsm12["IRON_NO3"] = " ";
				tmmsm12["TPC_YL_NO3"] = " ";
				tmmsm12["MOLTIRON_WT3"] = 0;
				ttmsm11["MOLTIRON_WT3"] = 0;
				break;
			case 4:
				tmmsm12["IRON_NO4"] = " ";
				tmmsm12["TPC_YL_NO4"] = " ";
				tmmsm12["MOLTIRON_WT4"] = 0;
				ttmsm11["MOLTIRON_WT4"] = 0;
				break;
			default:
				break;
			}

			tmmsm12["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT"].ToDecimal() - tmmsm12a["MOLTIRON_WT"].ToDecimal();
			tmmsm12.Update("REC_REVISOR, REC_REVISE_TIME, MOLTIRON_WT, IRON_NO1, TPC_YL_NO1, MOLTIRON_WT1, IRON_NO2, TPC_YL_NO2, MOLTIRON_WT2, IRON_NO3, TPC_YL_NO3, MOLTIRON_WT3, IRON_NO4, TPC_YL_NO4, MOLTIRON_WT4", "TPD_NO");
		
			ttmsm11["REC_REVISOR"] = s.userid;
			ttmsm11["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			ttmsm11["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT"];
			ttmsm11.Update("REC_REVISOR, REC_REVISE_TIME, MOLTIRON_WT, MOLTIRON_WT1, MOLTIRON_WT2, MOLTIRON_WT3, MOLTIRON_WT4", "IRON_LADLE_NO");
		}
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}