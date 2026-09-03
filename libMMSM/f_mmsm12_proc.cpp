/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2023-11-13
Description: 炼钢倒罐实绩处理
***********************************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** 函数引用 *****/
int CheckInsertOrPour(CModel &tmmsm12, CTracer log);
int InsertOrPour(CString PROC_DIV, CModel &tmmsm12, CDbConnection *conn, CTracer log);
int Delete(CModel &tmmsm12, CTracer log);
int f_getSeqNextValue(CString SEQ_NAME, CString & SEQ_VALUE, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm12_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	CString v_proc_div = "";

	CModel tmmsm12("TMMSM12");
	CModel ttmsm11("TTMSM11");

	try{
		//参数接收
		v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString();
		Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);

		//循环传入参数集合
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm12.Reset();
			tmmsm12.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm12.TrimOrBlank();
			Log::Trace("", __FUNCTION__, "v_proc_div = {0}", v_proc_div);

			//新增
			if (v_proc_div == "I")
			{
				if (InsertOrPour(v_proc_div, tmmsm12, conn, log) != 0){
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			//删除
			else if (v_proc_div == "D"){
				if (Delete(tmmsm12, log) != 0){
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			//再倒
			else if (v_proc_div == "ZD"){
				if (InsertOrPour(v_proc_div, tmmsm12, conn, log) != 0){
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			//倒完确认
			else if (v_proc_div == "DWQR")
			{
				if (tmmsm12.Query("TPD_NO,TIME_STAMPS")){
					tmmsm12["REC_REVISOR"] = s.userid;
					tmmsm12["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tmmsm12["POUR_FLAG"] = "Y";
					tmmsm12.Update("REC_REVISOR, REC_REVISE_TIME, POUR_FLAG", "TPD_NO,TIME_STAMPS");
				}
			}
			//倒完取消
			else if (v_proc_div == "DWQX"){
				if (tmmsm12.Query("TPD_NO,TIME_STAMPS")){
					tmmsm12["REC_REVISOR"] = s.userid;
					tmmsm12["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tmmsm12["POUR_FLAG"] = "N";
					tmmsm12.Update("REC_REVISOR, REC_REVISE_TIME, POUR_FLAG", "TPD_NO,TIME_STAMPS");
				}
			}
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}

/** 新增、再倒判断
**  tmmsm12：鱼雷罐倒罐实绩(横表)
**	log：日志对象
**/
int CheckInsertOrPour(CModel &tmmsm12, CTracer log)
{
	Log::Trace("", __FUNCTION__, "判断开始！");
	int doFlag = 0;						//返回值
	CModel tmmsm12a("TMMSM12A");		//鱼雷罐倒铁实绩(竖表)

	try{
		//判断倒铁量不为空时，铁次号，罐号不允许为空，倒铁实绩同步到TMMSM12A表
		if (tmmsm12["MOLTIRON_WT1"].ToDecimal() != 0)
		{
			if (tmmsm12["IRON_NO1"].ToString().Trim() == "" || tmmsm12["TPC_YL_NO1"].ToString() == "")
			{
				strcpy(s.msg, "铁水重量1不为0时，铁次号1、鱼雷罐号1不允许为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else
			{
				tmmsm12a["IRON_NO"] = tmmsm12["IRON_NO1"];
				tmmsm12a["TPC_YL_NO"] = tmmsm12["TPC_YL_NO1"];
				tmmsm12a["IRON_LADLE_NO"] = tmmsm12["IRON_LADLE_NO"];
				tmmsm12a["PROC_COUNT"] = 1;

				//鱼雷罐倒铁实绩(竖表)记录存在，则删除之
				if (tmmsm12a.Query("IRON_NO, TPC_YL_NO, IRON_LADLE_NO, PROC_COUNT")){
					tmmsm12a.Delete();
					tmmsm12a["REC_REVISOR"] = s.userid;
					tmmsm12a["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				}
				else{
					tmmsm12a["REC_CREATOR"] = s.userid;
					tmmsm12a["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				}
				tmmsm12a["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT1"];
				tmmsm12a.Insert();
			}
		}
		if (tmmsm12["MOLTIRON_WT2"].ToDecimal() != 0)
		{
			if (tmmsm12["IRON_NO2"].ToString().Trim() == "" || tmmsm12["TPC_YL_NO2"].ToString() == "")
			{
				strcpy(s.msg, "铁水重量2不为0时，铁次号2、鱼雷罐号2不允许为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else
			{
				tmmsm12a["IRON_NO"] = tmmsm12["IRON_NO2"];
				tmmsm12a["TPC_YL_NO"] = tmmsm12["TPC_YL_NO2"];
				tmmsm12a["IRON_LADLE_NO"] = tmmsm12["IRON_LADLE_NO"];
				tmmsm12a["PROC_COUNT"] = 2;

				//鱼雷罐倒铁实绩(竖表)记录存在，则删除之
				if (tmmsm12a.Query("IRON_NO, TPC_YL_NO, IRON_LADLE_NO, PROC_COUNT")){
					tmmsm12a.Delete();
					tmmsm12a["REC_REVISOR"] = s.userid;
					tmmsm12a["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				}
				else{
					tmmsm12a["REC_CREATOR"] = s.userid;
					tmmsm12a["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				}
				tmmsm12a["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT2"];
				tmmsm12a.Insert();
			}
		}
		if (tmmsm12["MOLTIRON_WT3"].ToDecimal() != 0){
			if (tmmsm12["IRON_NO3"].ToString().Trim() == "" || tmmsm12["TPC_YL_NO3"].ToString() == "")
			{
				strcpy(s.msg, "铁水重量3不为0时，铁次号3、鱼雷罐号3不允许为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else
			{
				tmmsm12a["IRON_NO"] = tmmsm12["IRON_NO3"];
				tmmsm12a["TPC_YL_NO"] = tmmsm12["TPC_YL_NO3"];
				tmmsm12a["IRON_LADLE_NO"] = tmmsm12["IRON_LADLE_NO"];
				tmmsm12a["PROC_COUNT"] = 3;

				//鱼雷罐倒铁实绩(竖表)记录存在，则删除之
				if (tmmsm12a.Query("IRON_NO, TPC_YL_NO, IRON_LADLE_NO, PROC_COUNT")){
					tmmsm12a.Delete();
					tmmsm12a["REC_REVISOR"] = s.userid;
					tmmsm12a["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				}
				else{
					tmmsm12a["REC_CREATOR"] = s.userid;
					tmmsm12a["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				}
				tmmsm12a["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT3"];
				tmmsm12a.Insert();
			}
		}
		if (tmmsm12["MOLTIRON_WT4"].ToDecimal() != 0)
		{
			if (tmmsm12["IRON_NO4"].ToString().Trim() == "" || tmmsm12["TPC_YL_NO4"].ToString() == "")
			{
				strcpy(s.msg, "铁水重量4不为0时，铁次号4、鱼雷罐号4不允许为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else
			{
				tmmsm12a["IRON_NO"] = tmmsm12["IRON_NO4"];
				tmmsm12a["TPC_YL_NO"] = tmmsm12["TPC_YL_NO4"];
				tmmsm12a["IRON_LADLE_NO"] = tmmsm12["IRON_LADLE_NO"];
				tmmsm12a["PROC_COUNT"] = 4;

				//鱼雷罐倒铁实绩(竖表)记录存在，则删除之
				if (tmmsm12a.Query("IRON_NO, TPC_YL_NO, IRON_LADLE_NO, PROC_COUNT")){
					tmmsm12a.Delete();
					tmmsm12a["REC_REVISOR"] = s.userid;
					tmmsm12a["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				}
				else{
					tmmsm12a["REC_CREATOR"] = s.userid;
					tmmsm12a["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				}
				tmmsm12a["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT4"];
				tmmsm12a.Insert();
			}
		}

		Log::Trace("", __FUNCTION__, "判断结束！");
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}

/**
** DateTime：2023/11/13 16:55:00
** Author:李晓明
** Description：倒铁实绩新增、再倒操作
**/
int InsertOrPour(CString PROC_DIV, CModel &tmmsm12, CDbConnection *conn, CTracer log){
	int doFlag = 0;						//返回值
	CString sqlstr = "";
	CString  datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	

	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	CModel ttmsm11("TTMSM11");
	CModel tmmsm12_old("TMMSM12");

	try
	{
		if (PROC_DIV == "ZD"){
			tmmsm12_old.CopyFrom(tmmsm12);
			tmmsm12_old.Query();
		}

		//判断铁包号是否有效
		ttmsm11["IRON_LADLE_NO"] = tmmsm12["IRON_LADLE_NO"];
		if (!ttmsm11.Query("IRON_LADLE_NO"))
		{
			strcpy(s.msg, "铁包号" + ttmsm11["IRON_LADLE_NO"].ToString() + "在铁包工器具不存在。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//判断铁包号状态
		if (ttmsm11["MIT_STATUS"].ToString().Trim() == "06")
		{
			strcpy(s.msg, "铁包号" + ttmsm11["IRON_LADLE_NO"].ToString() + "报废不能新增。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (CheckInsertOrPour(tmmsm12, log) != 0){
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//铁水重量汇总
		tmmsm12["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT1"].ToDecimal() + tmmsm12["MOLTIRON_WT2"].ToDecimal() + tmmsm12["MOLTIRON_WT3"].ToDecimal() + tmmsm12["MOLTIRON_WT4"].ToDecimal();

		//倒铁开始时间、倒铁结束时间修正
		if (tmmsm12["START_TIME"].ToString().GetLength() != 14)
		{
			tmmsm12["START_TIME"] = datetime;
		}
		if (tmmsm12["END_TIME"].ToString().GetLength() != 14)
		{
			tmmsm12["END_TIME"] = datetime;
		}

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
		cmd_inq.Close();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("start_time", tmmsm12["START_TIME"].ToString());
		cmd_inq.Parameters.Set("end_time", tmmsm12["END_TIME"].ToString());

		cmd_inq.ExecuteReader();
		if (cmd_inq.Read()){
			tmmsm12["TPD_DURATION"] = cmd_inq.GetDecimal(1);
		}

		if (PROC_DIV == "I"){
			tmmsm12["REC_CREATOR"] = s.userid;
			tmmsm12["REC_CREATE_TIME"] = datetime;

			CString TPD_NO = "";
			if (f_getSeqNextValue("TPD_NO_SEQ", TPD_NO, conn) != 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			tmmsm12["TPD_NO"] = "OP" + CDateTime::Now().ToString("yyyyMMdd") + TPD_NO;
			tmmsm12["POUR_FLAG1"] = "N";
			tmmsm12["POUR_FLAG2"] = "N";
			tmmsm12["POUR_FLAG3"] = "N";
			tmmsm12["POUR_FLAG4"] = "N";
			tmmsm12["POUR_FLAG"] = "N";
			tmmsm12["TIME_STAMPS"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm12.Insert();
		}
		else if(PROC_DIV == "ZD")
		{
			tmmsm12_old["REC_REVISOR"] = s.userid;
			tmmsm12_old["REC_REVISE_TIME"] = datetime;
			tmmsm12_old["TPD_DURATION"] = tmmsm12["TPD_DURATION"];
			tmmsm12_old["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT"];
			tmmsm12_old["IRON_NO1"] = tmmsm12["IRON_NO1"];
			tmmsm12_old["TPC_YL_NO1"] = tmmsm12["TPC_YL_NO1"];
			tmmsm12_old["MOLTIRON_WT1"] = tmmsm12["MOLTIRON_WT1"];
			tmmsm12_old["IRON_NO2"] = tmmsm12["IRON_NO2"];
			tmmsm12_old["TPC_YL_NO2"] = tmmsm12["TPC_YL_NO2"];
			tmmsm12_old["MOLTIRON_WT2"] = tmmsm12["MOLTIRON_WT2"];
			tmmsm12_old["IRON_NO3"] = tmmsm12["IRON_NO3"];
			tmmsm12_old["TPC_YL_NO3"] = tmmsm12["TPC_YL_NO3"];
			tmmsm12_old["MOLTIRON_WT3"] = tmmsm12["MOLTIRON_WT3"];
			tmmsm12_old["IRON_NO4"] = tmmsm12["IRON_NO4"];
			tmmsm12_old["TPC_YL_NO4"] = tmmsm12["TPC_YL_NO4"];
			tmmsm12_old["MOLTIRON_WT4"] = tmmsm12["MOLTIRON_WT4"];
			tmmsm12_old.Update("REC_REVISOR, REC_REVISE_TIME, TPD_DURATION, MOLTIRON_WT, IRON_NO1, TPC_YL_NO1, MOLTIRON_WT1, "
				"IRON_NO2, TPC_YL_NO2, MOLTIRON_WT2, IRON_NO3, TPC_YL_NO3, MOLTIRON_WT3, IRON_NO4, TPC_YL_NO4, MOLTIRON_WT4");
		}

		//更新炼钢铁水信息
		ttmsm11["MOLTIRON_WT"] = tmmsm12["MOLTIRON_WT"].ToDecimal();
		ttmsm11["MOLTIRON_WT1"] = tmmsm12["MOLTIRON_WT1"].ToDecimal();
		ttmsm11["MOLTIRON_WT2"] = tmmsm12["MOLTIRON_WT2"].ToDecimal();
		ttmsm11["MOLTIRON_WT3"] = tmmsm12["MOLTIRON_WT3"].ToDecimal();
		ttmsm11["MOLTIRON_WT4"] = tmmsm12["MOLTIRON_WT4"].ToDecimal();
		ttmsm11.Update("MOLTIRON_WT, MOLTIRON_WT1, MOLTIRON_WT2, MOLTIRON_WT3, MOLTIRON_WT4", "IRON_LADLE_NO");
	
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

/** 
** DateTime：2023/11/13 16:55:00
** Author:李晓明
** Description：删除倒铁实绩
**/
int Delete(CModel &tmmsm12, CTracer log){
	int doFlag = 0;
	CModel ttmsm11("TTMSM11");
	CModel tmmsm12a("TMMSM12A");

	try{
		tmmsm12.Delete();

		if (ttmsm11["MIT_STATUS"].ToString().Trim() == "06")
		{
			strcpy(s.msg, "铁包号" + ttmsm11["IRON_LADLE_NO"].ToString() + "报废不能删除。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//更新炼钢铁水信息
		ttmsm11["MOLTIRON_WT"] = 0;
		ttmsm11.Update("MOLTIRON_WT", "IRON_LADLE_NO");

		//删除倒罐实绩(竖表)信息
		tmmsm12a.Reset();
		tmmsm12a["IRON_NO"] = tmmsm12["IRON_NO1"];
		tmmsm12a["TPC_YL_NO"] = tmmsm12["TPC_YL_NO1"];
		tmmsm12a["IRON_LADLE_NO"] = tmmsm12["IRON_LADLE_NO"];
		tmmsm12a["PROC_COUNT"] = 1;
		tmmsm12a.Print();
		if (tmmsm12a.Query()){
			tmmsm12a.Delete();
		}
		tmmsm12a.Reset();
		tmmsm12a["IRON_NO"] = tmmsm12["IRON_NO2"];
		tmmsm12a["TPC_YL_NO"] = tmmsm12["TPC_YL_NO2"];
		tmmsm12a["IRON_LADLE_NO"] = tmmsm12["IRON_LADLE_NO"];
		tmmsm12a["PROC_COUNT"] = 2;
		tmmsm12a.Print();
		if (tmmsm12a.Query()){
			tmmsm12a.Delete();
		}
		tmmsm12a.Reset();
		tmmsm12a["IRON_NO"] = tmmsm12["IRON_NO3"];
		tmmsm12a["TPC_YL_NO"] = tmmsm12["TPC_YL_NO3"];
		tmmsm12a["IRON_LADLE_NO"] = tmmsm12["IRON_LADLE_NO"];
		tmmsm12a["PROC_COUNT"] = 3;
		tmmsm12a.Print();
		if (tmmsm12a.Query()){
			tmmsm12a.Delete();
		}
		tmmsm12a.Reset();
		tmmsm12a["IRON_NO"] = tmmsm12["IRON_NO4"];
		tmmsm12a["TPC_YL_NO"] = tmmsm12["TPC_YL_NO4"];
		tmmsm12a["IRON_LADLE_NO"] = tmmsm12["IRON_LADLE_NO"];
		tmmsm12a["PROC_COUNT"] = 4;
		tmmsm12a.Print();
		if (tmmsm12a.Query()){
			tmmsm12a.Delete();
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