/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      XXX
Version:     1.0
Date:        2023-11-08
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 板坯称重数据电文接收，板坯处理数据详细信息表电文接收
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件



int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsmzdsh_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE_TELE(cm_e2t8m4_rcv)
//int f_mmsm_mat_no_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//生成材料号
//int f_mmsm_get_matwt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//获取重量


int f_cm_e2t8m4_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 业务变量 */
	//CString aggregate_name = "";//工位
	CString v_virtual_slab_id = "";//虚拟板坯号   二级存在错误情况，以三级匹配的为准，33表找个字段单独存放
	CDecimal v_weight = 0;
	CDecimal update_time = 0;//更新时间
	CDecimal v_code_b = 0;//B系数值

	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);

	CModel tmmsm3a("TMMSM3A");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm33czxs("TMMSM33CZXS");
	CModel tmmsm3b("TMMSM3B");


	try
	{

		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}

		//aggregate_name = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["AGGREGATE_NAME"].ToString();


		//将table【0】设置为table【TMMSM3A】,并将结构体3A表结构赋给table【TMMSM3A】

		EIClass bcls_rec_MMSM3A;
		bcls_rec_MMSM3A.Tables[0].set_TableName("TMMSM3A");
		bcls_rec_MMSM3A.Tables[0].Clear();
		bcls_rec_MMSM3A.Tables[0].Columns.Add(tmmsm3a);
		bcls_rec_MMSM3A.Tables[0].Rows.Add();

#pragma region  将接口字段与表字段对应

		//bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["MAT_NO"] = bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["SLAB_NUMBER"];//板坯号   目前传过来的是喷印号
		bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["VIRTUAL_SLAB_NO"] = bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["VIRTUAL_SLAB_ID"];//虚拟板坯号
		bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["PRINT_NO"] = bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["SLAB_NUMBER"];//喷印号  存在空的情况   MARKING_NUMBER
		bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["SLAB_THICK"] = bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["THICKNESS"].ToDecimal() * 1000;//厚度
		bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["SLAB_HEAD_WIDTH"] = bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["WIDTH_HEAD"].ToDecimal() * 1000; //头宽
		bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["SLAB_TAIL_WIDTH"] = bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["WIDTH_TAIL"].ToDecimal() * 1000; //尾宽
		bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["SLAB_WT"] = (bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["WEIGHT"].ToDecimal() / 1000).Round(3); //重量
		bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["SLAB_LEN"] = bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["LENGTH"].ToDecimal() * 1000; //长度

		bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["DEV_CODE"] = bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["AGGREGATE_NAME"]; //工位
		bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["OPER_TYPE"] = bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["TREATMENT_TYPE"]; //处理类型

		bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["STATION_ID"] = bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["AGGREGATE_NAME"].ToString().Substring(0, 1);
		bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["STATION_NO"] = bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["AGGREGATE_NAME"].ToString().Substring(1, 1);


		//材料号 = 批次号  生成是从喷印号中截取，
		if (bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["PRINT_NO"].ToString().Trim() != "")
		{
			bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["MAT_NO"] = bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["PRINT_NO"].ToString().SubstringNE(0, 8) +
				bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["PRINT_NO"].ToString().SubstringNE(15, 2);
		}
		Log::Trace("", "", "PRINT_NO = {0}", bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]["PRINT_NO"].ToString());
		Log::Trace("", "", "get_TableName = {0}", bcls_rec->Tables[0].get_TableName());


#pragma endregion

		tmmsm3a.MergeFrom(bcls_rec_MMSM3A.Tables["TMMSM3A"].Rows[0]);
		tmmsm3a["WT_DATE_TIME"] = datetime;
		tmmsm3a["PROC_COUNT"] = tmmsm3a.QueryCount("MAT_NO") + 1;
		tmmsm3a["REC_CREATE_TIME"] = datetime;
		tmmsm3a.Insert();

		tmmsm01["MAT_NO"] = tmmsm3a["MAT_NO"];

		Log::Trace("", "", "222");
		if (tmmsm01.QueryCount("MAT_NO") > 0){

			tmmsm01.Query("MAT_NO");
			tmmsm96.CopyFrom(tmmsm01);
			//重量
			v_weight = (bcls_rec->Tables["INT_MES_SLAB_TREAT"].Rows[0]["WEIGHT"].ToDecimal() / 1000).Round(3);

			tmmsm96["REAL_TIME_WT"] = v_weight;	//实时重量  
			//tmmsm96["QUALIFIED_WT"] = v_weight;	//合格产量
			//tmmsm96["MAT_ACT_WT"] = v_weight;	//实际重量，系统重量   系统重量称重时是否变？待定  孟凡杰20240305
			tmmsm96["MEASURE_WT"] = v_weight;	//称重重量
			if (tmmsm01["RECEIVE_WEIGHT"].ToDecimal() == 0 || tmmsm01["RECEIVE_WEIGHT"].ToDecimal() < 0 || tmmsm01["RCV_MAT_FLAG"].ToString() == "N")
			{
				tmmsm96["MAT_WT"] = v_weight;		//材料重量   此处更改为不修改，会影响盘库的修改 以及收货后数据
				//20240425  mfj 更新  当收货重量为0时才更新，否则不更新MAT_WT





			}
			else
			{
				tmmsm96["MAT_WT"] = tmmsm01["MAT_WT"];
			}

			if (v_weight > 0)
			{
				tmmsm96["MEASURE_WT_FLAG"] = "1";
			}
			else
			{
				tmmsm96["MEASURE_WT_FLAG"] = "0";
			}
			tmmsm33czxs["C_DIV"] = tmmsm01["C_DIV"];
			tmmsm33czxs["STRAND_NO"] = tmmsm01["STRAND_NO"];
			tmmsm33czxs["ST_NO"] = tmmsm01["ST_NO"];

			v_code_b = (v_weight / tmmsm01["L2_THEORY_WT"].ToDecimal()).Round(4);
			if (tmmsm33czxs.Query("STRAND_NO,ST_NO"))
			{
				tmmsm33czxs["COE_B"] = v_code_b;
				//tmmsm33czxs["COE_B_UPPER_LIMIT"] = 1.05;
				//tmmsm33czxs["COE_B_LOWER_LIMIT"] = 0.95;
				//更新时间 = DATE_TIME - SLAB_CUT_TIME
				update_time = (CDateTime::Parse(datetime) - CDateTime::Parse(tmmsm01["SLAB_CUT_TIME"].ToString())).TotalMinutes();
				update_time = update_time.Round(2);
				//更新系数条件，  在切断时间和接收时间的这个时间段小于设置的更新时间段   且大于最小值，小于最大值才更新
				if (update_time <= tmmsm33czxs["UPDATE_TIME_LIMIT"].ToDecimal() &&
					v_code_b <= tmmsm33czxs["COE_B_UPPER_LIMIT"].ToDecimal() && v_code_b >= tmmsm33czxs["COE_B_LOWER_LIMIT"].ToDecimal())
				{
					tmmsm33czxs.Update("COE_B", "STRAND_NO,ST_NO");
				}
			}
			else
			{
				tmmsm33czxs["COE_A"] = 1;
				tmmsm33czxs["COE_B"] = v_code_b;
				tmmsm33czxs["COE_B_UPPER_LIMIT"] = 1.05000;
				tmmsm33czxs["COE_B_LOWER_LIMIT"] = 0.95000;
				tmmsm33czxs["UPDATE_TIME_LIMIT"] = 30.00;
				tmmsm33czxs.TrimOrBlank();
				tmmsm33czxs.Insert();
			}
			tmmsm96["COE_A"] = tmmsm33czxs["COE_A"];
			tmmsm96["COE_B"] = tmmsm33czxs["COE_B"];

			tmmsm96["WT_DATE_TIME"] = datetime;
			tmmsm96["EVENT_ID"] = "MM35";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "cm_e2t8m4_rcv";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "板坯称重";

			bcls_rec->Tables["MM0099"].Rows.Add();
			bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);

			if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
			{
				doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}


			//先走完上面 
			//当收货称重电文时，若没有收货 则进行收货，否则置更新重量
			if ((tmmsm01["RECEIVE_WEIGHT"].ToDecimal() == 0 || tmmsm01["RECEIVE_WEIGHT"].ToDecimal() < 0 || tmmsm01["RCV_MAT_FLAG"].ToString() == "N")&& v_weight >12)
			{
				EIClass bcls_rec_ZDSH;
				bcls_rec_ZDSH.Tables[0].set_TableName("TMMSM01");
				bcls_rec_ZDSH.Tables[0].Clear();
				bcls_rec_ZDSH.Tables[0].Columns.Add(tmmsm01);
				bcls_rec_ZDSH.Tables[0].Rows.Add();


				bcls_rec_ZDSH.Tables[0].Rows[0].Merge(tmmsm01);


				doFlag = f_mmsmzdsh_proc(&bcls_rec_ZDSH, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}


			}


		}





		////将table【0】设置为table【TMMSM3B】,并将结构体3B表结构赋给table【TMMSM3B】

		//EIClass bcls_rec_MMSM3B;
		//bcls_rec_MMSM3B.Tables[0].set_TableName("TMMSM3B");
		//bcls_rec_MMSM3B.Tables[0].Clear();
		//bcls_rec_MMSM3B.Tables[0].Columns.Add(tmmsm3b); 
		//bcls_rec_MMSM3B.Tables[0].Rows.Add();

#pragma region  将接口字段与表字段对应
		if (bcls_rec->Tables.Contains("INT_MES_SLAB_TREAT_DET"))
		{
			//!bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Columns.Contains("TABLE_NAME") && 
			if (!bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Columns.Contains("SLAB_SIDE"))
			{
				Log::Trace("", "", "该表没有字段");
			}
			else{
				if (bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows.get_Count())
				{
					tmmsm3b["SLAB_SURFACE_QUALITY"] = bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows[0]["SLAB_SIDE"].ToString();
					tmmsm3b["TREATMENT_COUNTER"] = bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows[0]["TREATMENT_NUMBER"].ToString();
					tmmsm3b["GRIND_MODE"] = bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows[0]["SPOT_PATTERN"].ToString();
					tmmsm3b["SPOT_X1"] = bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows[0]["SPOT_X1"];
					tmmsm3b["SPOT_X2"] = bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows[0]["SPOT_X2"];
					tmmsm3b["SPOT_X3"] = bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows[0]["SPOT_X3"];
					tmmsm3b["SPOT_X4"] = bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows[0]["SPOT_X4"];
					tmmsm3b["SPOT_Y1"] = bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows[0]["SPOT_Y1"];
					tmmsm3b["SPOT_Y2"] = bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows[0]["SPOT_Y2"];
					tmmsm3b["SPOT_Y3"] = bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows[0]["SPOT_Y3"];
					tmmsm3b["SPOT_Y4"] = bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows[0]["SPOT_Y4"];
					tmmsm3b["TREATMENT_DEPTH"] = bcls_rec->Tables["INT_MES_SLAB_TREAT_DET"].Rows[0]["TREATMENT_DEPTH"];
					tmmsm3b.Insert();
				}

			}
		}
#pragma endregion



		//tmmsm3b.MergeFrom(tmmsm3b);
		//tmmsm3b["PROC_COUNT"] = tmmsm3b.QueryCount("MAT_NO") + 1;

	}

	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


