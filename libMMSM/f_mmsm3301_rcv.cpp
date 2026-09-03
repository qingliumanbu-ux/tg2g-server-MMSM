/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      孟凡杰
Version:     1.0
Date:        2024-01-17
Description: 接收二级板坯原始数据
**************************************************/

#include "stdafx.h"
int f_mmsm_mat_no_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//生成材料号
int f_mmsm33_check(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_mmsm_get_matwt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//获取重量

//int f_mmsm3301n_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//铸坯产出处理，建物料主档，抛材料申请
//int f_mmsm3301u_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//铸坯产出处理，建物料主档，抛材料申请
//int f_wmsm_slab_no(CString pono, CDecimal mat_len, CDecimal mat_width, CDecimal mat_thick, CString st_no, CString &slab_no, CString &no_slab_cause, CDbConnection* conn);

 
BM2_FUNCTION_EXPORT
int f_mmsm3301_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	int ret = 0;
	CString aggregate_name = "";//工位	
	CString v_virtual_slab_id = "";//虚拟板坯号   二级存在错误情况，以三级匹配的为准，33表找个字段单独存放
	CString v_no_slab_cause = "";//匹配不上命令坯原因
	CString v_slab_final = "";//尾坯标记
	CString v_treatment_counter = "";//同工位处理次数
	int slab_no_count = 0;//根据二级虚拟板坯号查找长批号对应的板坯号循环数
	CString v_prod_shift_no = "";//班次
	CString v_prod_shift_group = "";//班组
	CDecimal if_pipei = 0;//匹配成功与否   0  失败   1 成功
	CDecimal v_lslab_no_length = 0;//长坯长度   为 0 则表示该命令坯为短坯
	CString v_slab_dest = "";//去向  10-1549热轧，11-2250热轧，20-型材，30-不锈线材，40-不锈热轧，41-4300厚板，50-外卖，60-二钢南区
	CString v_cast_lot_no = "";//
	CDecimal v_len = 0;//获取的长
	CDecimal v_width = 0;//获取的宽
	CDecimal v_thick = 0;//获取的厚
	CString v_factory_next = "";//计划去向  6391等
	CString  v_ingot_code = "";//锭型代码
	CDecimal v_slab_width = 0;//宽度  用来与称重系数表里的宽度进行比对
	CString v_mat_no = "";//材料号
	CString v_pono_03 = " ";//03表的PONO
	CString v_st_no_diff = "0";//钢种不一致的状态   0是不一致   1是一致

	//电文号

	//电文变量

	/* 实体类定义 */
	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_ss;
	CString sql_insert;
	CString sqlstr_upd03;//更新TPSSM03表数据
	CString sqlstr_zcxs;//获取称重系数
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_ss(conn);
	CDbCommand cmd_inq_upd03(conn);//更新TPSSM03表数据
	CDbCommand cmd_inq_code(conn);
	CDbCommand cmd_inq_czxs(conn);//查询称重系数
	CDbCommand cmd_insert(conn);

	//CModel tmmsm33("TMMSM33");
	CModel tmmsm33_1("TMMSM33_1");
	CModel tpssm03("TPSSM03");
	CModel tpssm01("TPSSM01");
	CModel tpssm10("TPSSM10");
	//CModel tpssm11("TPSSM11");
	//CModel tpssm41("TPSSM41");
	CModel tpssm40("TPSSM40");
	CModel tmmsm33czxs("TMMSM33CZXS");//称重系数表
	CModel tmmsm33bpgg("TMMSM33BPGG");//板坯规格上下限表
	//CModel twmsmpz("TWMSMPZ");

	try
	{

		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		aggregate_name = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["AGGREGATE_NAME"].ToString();



		//将table【0】设置为table【TMMSM33_1】,并将结构体33表结构赋给table【TMMSM33_1】

		EIClass bcls_rec_MMSM33_1;
		bcls_rec_MMSM33_1.Tables[0].set_TableName("TMMSM33_1");
		bcls_rec_MMSM33_1.Tables[0].Clear();
		bcls_rec_MMSM33_1.Tables[0].Columns.Add(tmmsm33_1);
		bcls_rec_MMSM33_1.Tables[0].Rows.Add();


		/*	blkNum = bcls_rec_SLAB_REPORT.Tables.IndexOf("TMMSM33");
		if (blkNum < 0)
		{
		bcls_rec_SLAB_REPORT.Tables.Add("TMMSM33");
		bcls_rec->Tables["TMMSM33"].Columns.Add(tmmsm33);
		bcls_rec->Tables["TMMSM33"].Rows.Add();
		}*/


#pragma region  将接口字段与表字段对应

		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["HEAT_NO"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["HEAT_NUMBER"].ToString().Trim();//熔炼号
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["STRAND_NO"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["STRAND_NUMBER"].ToString().Trim();//流号
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PRINT_NO"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["MARKING_NUMBER"];// 喷印号  存在空的情况    MARKING_NUMBER
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["VIRTUAL_SLAB_NO"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["VIRTUAL_SLAB_ID"];//虚拟板坯号
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_NO"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["SLAB_NUMBER"];//板坯号   数据与喷印号一致(存在二级出错导致喷印号和板坯号不一致的情况)
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_CUT_TIME"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["SLAB_CUT_TIME"].ToString();//板坯切断时刻
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_THICK"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["THICKNESS"].ToDecimal() * 1000;//厚度
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_HEAD_WIDTH"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["WIDTH_HEAD"].ToDecimal() * 1000; //头宽
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_TAIL_WIDTH"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["WIDTH_TAIL"].ToDecimal() * 1000; //尾宽
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SPLIT_INDICATION"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["SPLIT_INDICATION"]; //分包号

		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["DEV_CODE"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["AGGREGATE_NAME"]; //工位
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["PLAN_NUMBER"]; //计划号
		v_treatment_counter = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["TREATMENT_COUNTER"]; //同工位处理次数

		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["IF_SAMPLE"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["SAMPLE_CUT_DONE"]; //是否取样
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["MAT_TARG_LEN"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["AIM_LENGTH"].ToDecimal() * 1000; //目标长度     -- MAT_LEN 
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_LEN"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["ACTUAL_LENGTH"].ToDecimal() * 1000; //实际长度  -- MAT_ACT_LEN
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["L2_THEORY_WT"] = (bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["WEIGHT_CALC"].ToDecimal() / 1000).Round(3); //计算重量  二级理论量  存入01表MAT_THEORY_WT
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["ID_SJ"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["ID"].ToString().Trim();
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PER_SLAB"] = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["PER_SLAB"];


		//材料号 批次号  生成是从喷印号中截取，
		if (bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_NO"].ToString().Trim() != "")
		{
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["MAT_NO"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_NO"].ToString().SubstringNE(0, 8) +
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_NO"].ToString().SubstringNE(15, 2);
		}
		v_mat_no = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["MAT_NO"].ToString();
		if (v_mat_no.Substring(8, 2) == "00" || v_mat_no.Substring(8, 2) == "AA")//根据材料号尾号来判断 头坯，尾坯
		{
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_PLACE_CODE"] = "B";
		}
		else if (v_mat_no.Substring(8, 2) == "99" || v_mat_no.Substring(8, 2) == "ZZ")
		{
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_PLACE_CODE"] = "T";
		}

		v_slab_final = bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["SLAB_FINAL"].ToString().Trim(); //尾坯标记
		if (v_slab_final == "1")//尾坯标记为1为尾坯   需确认   mfj  20231227
		{
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_PLACE_CODE"] = "T";
		}

		//当喷印号没有值时，先拿板坯号赋值，后来又喷印号时再修改   
		//喷印号若没有值就不拿板坯号赋值   若后续电文继续发来喷印号，则更新
		/*if (bcls_rec_MMSM33.Tables["TMMSM33"].Rows[0]["PRINT_NO"].ToString().Trim() == "")
		{
		bcls_rec_MMSM33.Tables["TMMSM33"].Rows[0]["PRINT_NO"] = bcls_rec_MMSM33.Tables["TMMSM33"].Rows[0]["SLAB_NO"];
		}*/

		Log::Trace("", "", "PRINT_NO = {0}", bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PRINT_NO"].ToString().SubstringNE(15, 2));
		Log::Trace("", "", "get_TableName = {0}", bcls_rec_MMSM33_1.Tables[0].get_TableName());
		Log::Trace("", "", "SLAB_CUT_TIME = {0}", bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_CUT_TIME"].ToString());

		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["BATCH"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["MAT_NO"];
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["STATION_ID"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["DEV_CODE"].ToString().Substring(0, 1);
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["STATION_NO"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["DEV_CODE"].ToString().Substring(1, 1);
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_WIDTH"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_HEAD_WIDTH"].ToDecimal()> bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_TAIL_WIDTH"].ToDecimal()
			? bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_TAIL_WIDTH"].ToDecimal() : bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_HEAD_WIDTH"].ToDecimal();
		v_slab_width = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_WIDTH"].ToDecimal();
		//return  doFlag;

		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SM_PLAN_NO"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SM_PLAN_NOL2"];

#pragma endregion

		/****** 根据熔炼号获取PONO和处理号 *******/

		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PRACT_COLL_MODE"] = "1";//实绩收集方式  1 - 电文接收

		//为了模拟过去，这里的出钢记号先获取实际的出钢记号，非信号电文里的
		CString time_temp = Db::QueryCString("select CODE from TWMSMZD02 where CODE_CLASS='MMSM' ");

		//if (datetime < time_temp)      //测试阶段
		//{
		//	sqlstr = "SELECT PONO,DECODE(ST_NO_1,' ',ST_NO,ST_NO_1)ST_NO_1,CAST_NO,CAST_DIV_NO,SM_PLAN_NO  FROM TPSSM11 WHERE HEAT_NO = '" +
		//		bcls_rec_MMSM33.Tables["TMMSM33"].Rows[0]["HEAT_NO"].ToString().Trim() + "'  AND  SM_PLAN_NOL2_TEST ='" +
		//		bcls_rec_MMSM33.Tables["TMMSM33"].Rows[0]["SM_PLAN_NOL2"].ToString().Trim() + "' ";
		//}
		//else {


		sqlstr = "SELECT PONO,ST_NO_1,CAST_NO,CAST_DIV_NO,ST_NO,SM_PLAN_NO  FROM TPSSM11 WHERE   SM_PLAN_NOL2 ='" +
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SM_PLAN_NOL2"].ToString().Trim() + "' ";

		cmd_inq.SetCommandText(sqlstr);
		Log::Trace("", "", "sqlstr = {0}", sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PONO"] = cmd_inq.GetString(1);
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["ST_NO"] = cmd_inq.GetString(2);
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["CAST_NO"] = cmd_inq.GetString(3);
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["CAST_DIV_NO"] = cmd_inq.GetString(4);
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PREC_ST_NO"] = cmd_inq.GetString(5);
		}
		cmd_inq.Close();

		if (bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["ST_NO"].ToString().Trim() == "")
		{
			sqlstr = "SELECT PONO,ST_NO_1,CAST_NO,CAST_DIV_NO,ST_NO,SM_PLAN_NO  FROM TPSSM41 WHERE   SM_PLAN_NOL2 ='" +
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SM_PLAN_NOL2"].ToString().Trim() + "' ";

			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", "", "sqlstr = {0}", sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PONO"] = cmd_inq.GetString(1);
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["ST_NO"] = cmd_inq.GetString(2);
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["CAST_NO"] = cmd_inq.GetString(3);
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["CAST_DIV_NO"] = cmd_inq.GetString(4);
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PREC_ST_NO"] = cmd_inq.GetString(5);
			}
			cmd_inq.Close();

			sqlstr_ss = " select ST_NO from tpssmss where HEAT_NO='" + bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["HEAT_NO"].ToString() + "' and DEV_CODE like 'C%' AND STATUS_NAME LIKE 'CUT%' AND ROWNUM=1 ";
			cmd_inq_ss.SetCommandText(sqlstr_ss);
			cmd_inq_ss.ExecuteReader();
			if (cmd_inq_ss.Read() && bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["ST_NO"].ToString().Trim() != "")
			{
				if (bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["ST_NO"].ToString() != cmd_inq_ss.GetString(1))
				{
					bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["ST_NO"] = cmd_inq_ss.GetString(1);
				}
			}
			cmd_inq_ss.Close();
		}

		if (bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["ST_NO"].ToString().Trim() == "")
		{
			sqlstr = "SELECT PONO,ST_NO FROM TPSSM_PLAN_ST  WHERE HEAT_NO = '" +
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["HEAT_NO"].ToString().Trim() + "'";

			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", "", "sqlstr_stNO = {0}", sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PONO"] = cmd_inq.GetString(1);
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["ST_NO"] = cmd_inq.GetString(2);

			}
			else
			{
				strcpy(s.msg, "未获取到信号点的内部钢种！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();
		}

		//cmd_inq.SetCommandText("SELECT BACK_N_1,BACK_N_2,BACK_C1,BACK_C2 FROM TWMSMPZ WHERE CODE_CLASS='JHKD' AND CODE_DESC_1_CONTENT='" + bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["STRAND_NO"].ToString() + "'");
		//cmd_inq.ExecuteReader();
		//if (cmd_inq.Read())
		//{
		//	if (cmd_inq.GetString(3) == "1")
		//	{
		//		if (cmd_inq.GetString(4).Trim() != "")//三级浇次号存在，判断三级浇次号和宽度偏差
		//		{
		//			if (bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_WIDTH"].ToDecimal() + cmd_inq.GetDecimal(2) == cmd_inq.GetDecimal(1) && bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["CAST_NO"].ToString() == cmd_inq.GetString(4))
		//			{
		//				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_WIDTH"] = cmd_inq.GetDecimal(1);
		//				//tmmsm01["MAT_ACT_WIDTH"] = cmd_inq.GetDecimal(1);
		//			}
		//			else {
		//				twmsmpz["CODE_CLASS"] = "JHKD";
		//				twmsmpz["CODE_DESC_1_CONTENT"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["STRAND_NO"].ToString();
		//				twmsmpz["BACK_C1"] = "0";
		//				twmsmpz["BACK_C2"] = " ";
		//				twmsmpz.Update("BACK_C1,BACK_C2", "CODE_CLASS,CODE_DESC_1_CONTENT");
		//			}
		//		}
		//		else {//三级浇次号不存在，只判断宽度偏差
		//			if (bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_WIDTH"].ToDecimal() + cmd_inq.GetDecimal(2) == cmd_inq.GetDecimal(1))
		//			{
		//				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_WIDTH"] = cmd_inq.GetDecimal(1);
		//				//tmmsm01["MAT_ACT_WIDTH"] = cmd_inq.GetDecimal(1);

		//				twmsmpz["CODE_CLASS"] = "JHKD";
		//				twmsmpz["CODE_DESC_1_CONTENT"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["STRAND_NO"].ToString();
		//				twmsmpz["BACK_C2"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["CAST_NO"];
		//				twmsmpz.Update("BACK_C2", "CODE_CLASS,CODE_DESC_1_CONTENT");
		//			}
		//			else {
		//				twmsmpz["CODE_CLASS"] = "JHKD";
		//				twmsmpz["CODE_DESC_1_CONTENT"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["STRAND_NO"].ToString();
		//				twmsmpz["BACK_C1"] = "0";
		//				twmsmpz["BACK_C2"] = " ";
		//				twmsmpz.Update("BACK_C1,BACK_C2", "CODE_CLASS,CODE_DESC_1_CONTENT");
		//			}
		//		}
		//	}
		//}
		//cmd_inq.Close();

		//若连铸出钢记号和计划出钢记号相同，则匹配命令坯 若不同，则不匹配命令坯 按余材收  mfj  初亮  20240520 10：56
		if (bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["ST_NO"].ToString().Trim() == bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PREC_ST_NO"].ToString().Trim())
		{
			v_st_no_diff = "1";
		}
		else
		{
			v_st_no_diff = "0";
		}

		Log::Trace("", "", "ST_NO = {0}", bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["ST_NO"].ToString().Trim());
		Log::Trace("", "", "PREC_ST_NO = {0}", bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PREC_ST_NO"].ToString().Trim());

		sqlstr = "SELECT PROC_NO FROM TPSSM12 WHERE HEAT_NO = '" + bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["HEAT_NO"].ToString() + "'  AND AREA_ID = '5'   AND DEV_CODE = '" + bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["DEV_CODE"].ToString().Trim() + "' ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PROC_NO"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_TYPE"] = "1";//钢坯类型  1  板坯 

		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["MEASURE_WT_FLAG"] = "0";//称重标记  0 未称重



		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["REC_CREATOR"] = s.userid;

		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["MANAGE_FLAG"] = "1";//电文每次接收一支
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["MAT_TUBE"] = 1;//材料根数
		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["FACTORY_DIV"] = "LG1";

		if (!bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Columns.Add(DT_STRING, "PROC_DIV");
			for (int i = 0; i < bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows.get_Count(); i++)
			{
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[i]["PROC_DIV"] = "N";
			}
		}




		tmmsm33_1.MergeFrom(bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]);


		//若已有该坯子数据，则不走匹配逻辑，直接进行修改逻辑，需确认主要修改哪些数据  mfj 20240117
		if (tmmsm33_1.QueryCount("MAT_NO"))
		{

		}
		else
		{
			//获取班次班组
			if (tmmsm33_1["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm33_1["PROD_SHIFT_GROUP"].ToString().Trim() == "")
			{
				f_epep_get_shift_group("SMCP", tmmsm33_1["SLAB_CUT_TIME"].ToString(), v_prod_shift_no, v_prod_shift_group, conn);
				tmmsm33_1["PROD_SHIFT_NO"] = v_prod_shift_no;
				tmmsm33_1["PROD_SHIFT_GROUP"] = v_prod_shift_group;
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PROD_SHIFT_NO"] = v_prod_shift_no;
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PROD_SHIFT_GROUP"] = v_prod_shift_group;
			}




			tpssm10["PONO"] = tmmsm33_1["PONO"].ToString();
			if (!tpssm10.Query("PONO"))//若10表没有，则查询40表
			{
				tpssm40["PONO"] = tmmsm33_1["PONO"].ToString();
				tpssm40.Query("PONO");
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["C_DIV"] = tpssm40["C_DIV"];
			}
			else
			{
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["C_DIV"] = tpssm10["C_DIV"];
			}

#pragma region   根据去向获取板坯上下限数据,再根据碳锈区分分别获取
			/*if (v_factory_next == "6391" || v_factory_next == "639S") //4300 去向的  若有639S，则优先匹配639S   4300是中板
			{
			tmmsm33bpgg["ST_NO"] = "6";
			tmmsm33bpgg["SLAB_TYPE"] = "Z";
			tmmsm33bpgg.Query("ST_NO,SLAB_TYPE");
			}
			else
			{
			tpssm10["PONO"] = tmmsm33["PONO"].ToString();
			if (!tpssm10.Query("PONO"))//若10表没有，则查询40表
			{
			tpssm40["PONO"] = tmmsm33["PONO"].ToString();
			tpssm40.Query("PONO");
			}


			//1  是不锈钢   2是碳钢  ，3是硅钢。6是4300的
			if (tpssm10["C_DIV"].ToString().Trim() == "1" || tpssm40["C_DIV"].ToString().Trim() == "1")
			{
			tmmsm33bpgg["ST_NO"] = "1";
			//10-1549热轧去向  11-2250热轧去向   卷板
			if (v_slab_dest == "10" || v_slab_dest == "11")
			{
			tmmsm33bpgg["SLAB_TYPE"] = "J";
			}
			if (v_slab_dest == "20")//型材去向
			{

			}
			if (v_slab_dest == "30" || v_slab_dest == "40")//30-不锈线材去向  40-不锈热轧去向
			{
			tmmsm33bpgg["SLAB_TYPE"] = "Z";
			}
			}
			else if (tpssm10["C_DIV"].ToString().Trim() == "2" || tpssm40["C_DIV"].ToString().Trim() == "2")
			{
			tmmsm33bpgg["ST_NO"] = "2";
			//10-1549热轧去向  11-2250热轧去向   卷板
			if (v_slab_dest == "10" || v_slab_dest == "11")
			{
			tmmsm33bpgg["SLAB_TYPE"] = "J";
			}
			if (v_slab_dest == "20")//型材去向
			{

			}
			if (v_slab_dest == "30" || v_slab_dest == "40")//30-不锈线材去向  40-不锈热轧去向
			{
			tmmsm33bpgg["SLAB_TYPE"] = "Z";
			}
			}
			else
			{
			strcpy(s.sysmsg, "碳锈区分未获取，请重新接收确认！");
			throw CApplicationException(-1, s.msg, log.Location);
			}

			tmmsm33bpgg.Query("ST_NO,SLAB_TYPE");
			}

			*/
#pragma endregion


#pragma region 根据流号，出钢记号，碳锈区分获取三级计算重量   二级理论量*系数A*系数B    20240226--增加宽度和宽度上下限范围判断
			tmmsm33czxs["STRAND_NO"] = tmmsm33_1["STRAND_NO"];
			tmmsm33czxs["ST_NO"] = tmmsm33_1["ST_NO"];
			tmmsm33czxs["C_DIV"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["C_DIV"].ToString();

			/*sqlstr_zcxs = " SELECT  *  FROM ( "
			"	SELECT abs(WIDTH - '" + v_slab_width.ToString() + "') as CHAZHI, T.* FROM TMMSM33CZXS T  "
			"	WHERE(WIDTH + MAXWIDTH) > '" + v_slab_width.ToString() + "' AND(WIDTH - MINWIDTH) < '" + v_slab_width.ToString() + "'  "
			"   AND STRAND_NO ='" + tmmsm33["STRAND_NO"].ToString() + "'  AND ST_NO ='" + tmmsm33["ST_NO"].ToString() + "' AND C_DIV ='" + tpssm10["C_DIV"].ToString() + "' )"
			"  ORDER BY CHAZHI asc ";

			Log::Trace("", "", "sqlstr_zcxs = {0}", sqlstr_zcxs);
			cmd_inq_czxs.SetCommandText(sqlstr_zcxs);
			cmd_inq_czxs.ExecuteReader();
			if (cmd_inq_czxs.Read())
			{
			cmd_inq_czxs.Fetch(tmmsm33czxs);
			}
			cmd_inq_czxs.Close();*/



			//若没有查到，则赋默认值   ----极端情况和前期数据不全的情况下会有该情况，故打个补丁  mfj  20240117
			if (!tmmsm33czxs.Query("ST_NO,STRAND_NO"))
			{
				tmmsm33czxs["COE_A"] = 1;
				tmmsm33czxs["COE_B"] = 1;
			}
			//将系数存入实绩表和主档表中
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["COE_A"] = tmmsm33czxs["COE_A"];
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["COE_B"] = tmmsm33czxs["COE_B"];
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["COE_B_UPPER_LIMIT"] = tmmsm33czxs["COE_B_UPPER_LIMIT"];
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["COE_B_LOWER_LIMIT"] = tmmsm33czxs["COE_B_LOWER_LIMIT"];

			//根据系数计算重量
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_WT"] = (tmmsm33_1["L2_THEORY_WT"].ToDecimal()* tmmsm33czxs["COE_A"].ToDecimal() * tmmsm33czxs["COE_B"].ToDecimal()).Round(3);
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_WT"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_WT"].ToDecimal().Round(3);
#pragma endregion


			////当有虚拟板坯号时，先根据虚拟板坯号获取部分默认值，防止虚拟板坯已经使用导致下面代码获取不到任何数据
			//if (tmmsm33_1["VIRTUAL_SLAB_NO"].ToString().Trim() != "")
			//{
			//	tpssm01["PONO"] = tmmsm33_1["PONO"].ToString();

			//	sqlstr = "SELECT  CAST_LOT_NO,FACTORY_NEXT,INGOT_CODE,SLAB_DEST,PONO FROM TPSSM03 WHERE LSLAB_NO ='" + tmmsm33_1["VIRTUAL_SLAB_NO"].ToString() + "' ";
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.ExecuteReader();
			//	if (cmd_inq.Read())
			//	{
			//		v_cast_lot_no = cmd_inq.GetString(1).Trim();
			//		v_factory_next = cmd_inq.GetString(2).Trim();
			//		v_ingot_code = cmd_inq.GetString(3).Trim();
			//		v_slab_dest = cmd_inq.GetString(4).Trim();
			//		v_pono_03 = cmd_inq.GetString(5).Trim();
			//	}
			//	cmd_inq.Close();
			//	if (tpssm01.Query("PONO") && v_cast_lot_no.Trim() != ""&&tpssm01["CAST_LOT_NO"].ToString() != v_cast_lot_no)//先判断虚拟板坯与制造命令是否是同一浇次
			//	{
			//		v_cast_lot_no = " ";
			//		v_factory_next = " ";
			//		v_ingot_code = " ";
			//		v_slab_dest = " ";
			//		if_pipei = 0;
			//		tmmsm33_1["VIRTUAL_SLAB_NO"] = " ";
			//	}
			//	else if (v_pono_03.Trim() != ""&&tmmsm33_1["ST_NO"].ToString() != Db::QueryCString("SELECT ST_NO FROM TPSSM01 WHERE PONO='" + v_pono_03 + "'"))//在判断虚拟板坯带的钢种与炉次钢种是否相同
			//	{
			//		v_cast_lot_no = " ";
			//		v_factory_next = " ";
			//		v_ingot_code = " ";
			//		v_slab_dest = " ";
			//		if_pipei = 0;
			//		tmmsm33_1["VIRTUAL_SLAB_NO"] = " ";
			//		bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["NO_SLAB_CAUSE"] = "钢种不合";
			//	}
			//}



#pragma region  匹配命令板坯  二级的虚拟板坯号有可能是长坯号，也可能是短坯号，所以，需要先拿着二级虚拟板坯号去匹配，先长坯后短坯，都没有再调函数
			//拿着二级虚拟板坯号，去查找规格，然后按照加减上下限去匹配二级规格，若符合则匹配成功
			//若二级虚拟板坯号规格不符，则查询浇次（4300按炉）查找虚拟板坯号，再根据虚拟板坯号找规格，去匹配对应，若有，则跳出循环
			//匹配上后要根据长批号再去更新03表的标记，表示该批号已使用
			//PONO  9开头的为调度室手动创建计划，不匹配命令坯
			//只有钢种一致的时候才自动匹配命令坯
			if (tmmsm33_1["VIRTUAL_SLAB_NO"].ToString().Trim() != "" && bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PONO"].ToString().Trim().SubstringNE(0, 1) != "9" && v_st_no_diff.Trim() == "1")
			{
				sqlstr = "SELECT DECODE(LSLAB_NO_LENGTH,0,SLAB_LEN,LSLAB_NO_LENGTH) LSLAB_NO_LENGTH,SLAB_WIDTH,SLAB_THICK,CAST_LOT_NO,FACTORY_NEXT,INGOT_CODE,SLAB_DEST,LSLAB_NO_LENGTH,LSLAB_NO  FROM TPSSM03 WHERE LSLAB_NO ='" + tmmsm33_1["VIRTUAL_SLAB_NO"].ToString() + "' AND   SLAB_PROD_FLAG <> '1' AND SLAB_PROD_FLAG <> '9'  and SUBSTR(PONO,0,1) <> '9' ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())//获取到同炉虚拟板坯
				{
					v_len = cmd_inq.GetDecimal(1);
					v_width = cmd_inq.GetDecimal(2);
					v_thick = cmd_inq.GetDecimal(3);
					v_cast_lot_no = cmd_inq.GetString(4).Trim();
					v_factory_next = cmd_inq.GetString(5).Trim();
					v_ingot_code = cmd_inq.GetString(6).Trim();
					v_slab_dest = cmd_inq.GetString(7).Trim();
					v_lslab_no_length = cmd_inq.GetDecimal(8);

#pragma region   根据锭型代码获取板坯上下限表数据 
					//1.先根据下游工厂判断是不是去4300的（6391和639S）
					//2.判定锭型是不是5位，是5位的直接取前两位，就是AA或者AB
					//3.锭型不是5位的，长坯直接AB，短坯看下游工厂是6391是AB，其余是AA
					//AA是卷板，AB是中板

					if (v_factory_next == "6391" || v_factory_next == "639S") //4300 去向的  若有639S，则优先匹配639S   4300是中板
					{
						tmmsm33bpgg["ST_NO"] = "6";
						tmmsm33bpgg["SLAB_TYPE"] = "Z";
						tmmsm33bpgg.Query("ST_NO,SLAB_TYPE");
					}
					else
					{
						//1  是不锈钢   2是碳钢  ，3是硅钢。6是4300的
						if (tpssm10["C_DIV"].ToString().Trim() == "1" || tpssm40["C_DIV"].ToString().Trim() == "1")
						{
							tmmsm33bpgg["ST_NO"] = "1";
						}
						else if (tpssm10["C_DIV"].ToString().Trim() == "2" || tpssm40["C_DIV"].ToString().Trim() == "2")
						{
							tmmsm33bpgg["ST_NO"] = "2";
						}
						else if (tpssm10["C_DIV"].ToString().Trim() == "3" || tpssm40["C_DIV"].ToString().Trim() == "3")
						{
							tmmsm33bpgg["ST_NO"] = "3";
						}
						else if (tpssm10["C_DIV"].ToString().Trim() == "6" || tpssm40["C_DIV"].ToString().Trim() == "6")
						{
							tmmsm33bpgg["ST_NO"] = "6";
						}

						//上面判断是否是不锈钢或是碳钢，这里判断是中板还是卷板
						if (v_ingot_code.GetLength() == 5)
						{
							if (v_ingot_code.Substring(1, 2) == "AA")//卷板
							{
								tmmsm33bpgg["SLAB_TYPE"] = "J";
							}
							else if (v_ingot_code.Substring(1, 2) == "AB")//中板
							{
								tmmsm33bpgg["SLAB_TYPE"] = "Z";
							}
						}
						else
						{
							//长坯长大于0，表示为长坯 ，默认AB
							if (v_lslab_no_length > 0)
							{
								tmmsm33bpgg["SLAB_TYPE"] = "Z";
							}
							else if (v_lslab_no_length == 0)//上面已排除下游工厂的6391，故这里默认其他（卷板）
							{
								tmmsm33bpgg["SLAB_TYPE"] = "J";
							}
						}

						tmmsm33bpgg.Query("ST_NO,SLAB_TYPE");
					}
#pragma endregion

					//二级长度小于获取计划长度下限值或是大于计划长度的上限值
					if (tmmsm33_1["SLAB_LEN"].ToDecimal() < (v_len - tmmsm33bpgg["LEN_MIN"].ToDecimal()) || tmmsm33_1["SLAB_LEN"].ToDecimal() > (v_len + tmmsm33bpgg["LEN_MAX"].ToDecimal()))
					{
						if_pipei = 0;
						bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["NO_SLAB_CAUSE"] = "长度不合";
					}
					else if (tmmsm33_1["SLAB_WIDTH"].ToDecimal() < (v_width - tmmsm33bpgg["MINWIDTH"].ToDecimal()) || tmmsm33_1["SLAB_WIDTH"].ToDecimal() > (v_width + tmmsm33bpgg["MAXWIDTH"].ToDecimal()))
					{
						if_pipei = 0;
						bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["NO_SLAB_CAUSE"] = "宽度不合";
					}
					else if (tmmsm33_1["SLAB_THICK"].ToDecimal() < (v_thick - tmmsm33bpgg["THICK_MIN"].ToDecimal()) || tmmsm33_1["SLAB_THICK"].ToDecimal() > (v_thick + tmmsm33bpgg["THICK_MAX"].ToDecimal()))
					{
						if_pipei = 0;
						bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["NO_SLAB_CAUSE"] = "厚度不合";
					}
					else {
						if_pipei = 1;
						bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["LSLAB_NO"] = cmd_inq.GetString(9);
					}
				}


				cmd_inq.Close();
			}

			//没有匹配成功的，先查找出同炉的所有没有匹配过的长批号，然后排序，按照顺序再根据长批号去查找规格，比对规格
			//因为4300的分为6391和639S，需要先匹配639S,故先根据去向倒序排序，再根据长坯号正序排序
			//PONO  9开头的为调度室手动创建计划，不匹配命令坯
			//只有钢种一致的时候才自动匹配命令坯
			if (if_pipei == 0 && bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PONO"].ToString().Trim().SubstringNE(0, 1) != "9" && v_st_no_diff.Trim() == "1")
			{
				sqlstr = " select LSLAB_NO, DECODE(lslab_no_length, 0, SLAB_LEN, lslab_no_length) lslab_no_length,SLAB_WIDTH, SLAB_THICK  "
					"  ,CAST_LOT_NO,FACTORY_NEXT,INGOT_CODE,SLAB_DEST,LSLAB_NO_LENGTH  "
					" FROM TPSSM03 WHERE  PONO = '" + tmmsm33_1["PONO"].ToString().Trim() + "'  AND    SLAB_PROD_FLAG <> '1' AND SLAB_PROD_FLAG <> '9'  and SUBSTR(PONO,0,1) <> '9'  order by FACTORY_NEXT desc, LSLAB_NO asc ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())//获取到同炉虚拟长坯号
				{
					//先将数据置零
					v_len = 0;
					v_width = 0;
					v_thick = 0;
					if_pipei = 0;

					v_len = cmd_inq.GetDecimal(2);
					v_width = cmd_inq.GetDecimal(3);
					v_thick = cmd_inq.GetDecimal(4);
					v_cast_lot_no = cmd_inq.GetString(5).Trim();
					v_factory_next = cmd_inq.GetString(6).Trim();
					v_ingot_code = cmd_inq.GetString(7).Trim();
					v_slab_dest = cmd_inq.GetString(8).Trim();
					v_lslab_no_length = cmd_inq.GetDecimal(9);


#pragma region   根据锭型代码获取板坯上下限表数据 
					//1.先根据下游工厂判断是不是去4300的（6391和639S）
					//2.判定锭型是不是5位，是5位的直接取前两位，就是AA或者AB
					//3.锭型不是5位的，长坯直接AB，短坯看下游工厂是6391是AB，其余是AA
					//AA是卷板，AB是中板

					if (v_factory_next == "6391" || v_factory_next == "639S") //4300 去向的  若有639S，则优先匹配639S   4300是中板
					{
						tmmsm33bpgg["ST_NO"] = "6";
						tmmsm33bpgg["SLAB_TYPE"] = "Z";
						tmmsm33bpgg.Query("ST_NO,SLAB_TYPE");
					}
					else
					{
						//1  是不锈钢   2是碳钢  ，3是硅钢。6是4300的
						if (tpssm10["C_DIV"].ToString().Trim() == "1" || tpssm40["C_DIV"].ToString().Trim() == "1")
						{
							tmmsm33bpgg["ST_NO"] = "1";
						}
						else if (tpssm10["C_DIV"].ToString().Trim() == "2" || tpssm40["C_DIV"].ToString().Trim() == "2")
						{
							tmmsm33bpgg["ST_NO"] = "2";
						}
						else if (tpssm10["C_DIV"].ToString().Trim() == "3" || tpssm40["C_DIV"].ToString().Trim() == "3")
						{
							tmmsm33bpgg["ST_NO"] = "3";
						}
						else if (tpssm10["C_DIV"].ToString().Trim() == "6" || tpssm40["C_DIV"].ToString().Trim() == "6")
						{
							tmmsm33bpgg["ST_NO"] = "6";
						}

						//上面判断是否是不锈钢或是碳钢，这里判断是中板还是卷板
						if (v_ingot_code.GetLength() == 5)
						{
							if (v_ingot_code.Substring(1, 2) == "AA")//卷板
							{
								tmmsm33bpgg["SLAB_TYPE"] = "J";
							}
							else if (v_ingot_code.Substring(1, 2) == "AB")//中板
							{
								tmmsm33bpgg["SLAB_TYPE"] = "Z";
							}
						}
						else
						{
							//长坯长大于0，表示为长坯 ，默认AB
							if (v_lslab_no_length > 0)
							{
								tmmsm33bpgg["SLAB_TYPE"] = "Z";
							}
							else if (v_lslab_no_length == 0)//上面已排除下游工厂的6391，故这里默认其他（卷板）
							{
								tmmsm33bpgg["SLAB_TYPE"] = "J";
							}
						}

						tmmsm33bpgg.Query("ST_NO,SLAB_TYPE");
					}
#pragma endregion

					if (tmmsm33_1["SLAB_LEN"].ToDecimal() < (v_len - tmmsm33bpgg["LEN_MIN"].ToDecimal()) || tmmsm33_1["SLAB_LEN"].ToDecimal() > (v_len + tmmsm33bpgg["LEN_MAX"].ToDecimal()))
					{
						continue;//跳过该循环，表示没有匹配上该长坯号的规格
					}
					else if (tmmsm33_1["SLAB_WIDTH"].ToDecimal() < (v_width - tmmsm33bpgg["MINWIDTH"].ToDecimal()) || tmmsm33_1["SLAB_WIDTH"].ToDecimal() > (v_width + tmmsm33bpgg["MAXWIDTH"].ToDecimal()))
					{
						continue;//跳过该循环，表示没有匹配上该长坯号的规格
					}
					else if (tmmsm33_1["SLAB_THICK"].ToDecimal() < (v_thick - tmmsm33bpgg["THICK_MIN"].ToDecimal()) || tmmsm33_1["SLAB_THICK"].ToDecimal() > (v_thick + tmmsm33bpgg["THICK_MAX"].ToDecimal()))
					{
						continue;//跳过该循环，表示没有匹配上该长坯号的规格
					}
					else {
						if_pipei = 1;
						bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["LSLAB_NO"] = cmd_inq.GetString(1);
						bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["NO_SLAB_CAUSE"] = " ";
						break;//匹配上后跳出循环
					}
				}
				cmd_inq.Close();
			}

			Log::Trace("", "", "匹配不上原因NO_SLAB_CAUSE={0}", bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["NO_SLAB_CAUSE"].ToString());
			//PONO  9开头的为调度室手动创建计划，不匹配命令坯
			//只有钢种一致的时候才自动匹配命令坯
			if (if_pipei == 0 && v_factory_next != "6391" &&v_factory_next != "639S"  && bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PONO"].ToString().Trim().SubstringNE(0, 1) != "9" && v_st_no_diff.Trim() == "1")//上面没有匹配到，并且已经匹配完炉了，现在要匹配浇次了，4300只匹配炉
			{
				sqlstr = "select LSLAB_NO, DECODE(lslab_no_length, 0, SLAB_LEN, lslab_no_length) lslab_no_length,SLAB_WIDTH, SLAB_THICK  "
					"  ,CAST_LOT_NO,FACTORY_NEXT,INGOT_CODE,SLAB_DEST,LSLAB_NO_LENGTH  "
					" FROM TPSSM03 WHERE  CAST_LOT_NO = '" + v_cast_lot_no + "'  AND    SLAB_PROD_FLAG <> '1' AND  SLAB_PROD_FLAG <> '9' and PONO  IN (SELECT PONO FROM TPSSM10 where ST_NO='" + tmmsm33_1["ST_NO"].ToString() + "')  and SUBSTR(PONO,0,1) <> '9'  order by FACTORY_NEXT desc, LSLAB_NO asc ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())//获取到同炉虚拟板坯
				{
					//先将数据置零
					v_len = 0;
					v_width = 0;
					v_thick = 0;
					if_pipei = 0;

					v_len = cmd_inq.GetDecimal(2);
					v_width = cmd_inq.GetDecimal(3);
					v_thick = cmd_inq.GetDecimal(4);
					v_cast_lot_no = cmd_inq.GetString(5).Trim();
					v_factory_next = cmd_inq.GetString(6).Trim();
					v_ingot_code = cmd_inq.GetString(7).Trim();
					v_slab_dest = cmd_inq.GetString(8).Trim();
					v_lslab_no_length = cmd_inq.GetDecimal(9);


#pragma region   根据锭型代码获取板坯上下限表数据 
					//1.先根据下游工厂判断是不是去4300的（6391和639S）
					//2.判定锭型是不是5位，是5位的直接取前两位，就是AA或者AB
					//3.锭型不是5位的，长坯直接AB，短坯看下游工厂是6391是AB，其余是AA
					//AA是卷板，AB是中板

					if (v_factory_next == "6391" || v_factory_next == "639S") //4300 去向的  若有639S，则优先匹配639S   4300是中板
					{
						tmmsm33bpgg["ST_NO"] = "6";
						tmmsm33bpgg["SLAB_TYPE"] = "Z";
						tmmsm33bpgg.Query("ST_NO,SLAB_TYPE");
					}
					else
					{
						//1  是不锈钢   2是碳钢  ，3是硅钢。6是4300的
						if (tpssm10["C_DIV"].ToString().Trim() == "1" || tpssm40["C_DIV"].ToString().Trim() == "1")
						{
							tmmsm33bpgg["ST_NO"] = "1";
						}
						else if (tpssm10["C_DIV"].ToString().Trim() == "2" || tpssm40["C_DIV"].ToString().Trim() == "2")
						{
							tmmsm33bpgg["ST_NO"] = "2";
						}
						else if (tpssm10["C_DIV"].ToString().Trim() == "3" || tpssm40["C_DIV"].ToString().Trim() == "3")
						{
							tmmsm33bpgg["ST_NO"] = "3";
						}
						else if (tpssm10["C_DIV"].ToString().Trim() == "6" || tpssm40["C_DIV"].ToString().Trim() == "6")
						{
							tmmsm33bpgg["ST_NO"] = "6";
						}

						//上面判断是否是不锈钢或是碳钢，这里判断是中板还是卷板
						if (v_ingot_code.GetLength() == 5)
						{
							if (v_ingot_code.Substring(1, 2) == "AA")//卷板
							{
								tmmsm33bpgg["SLAB_TYPE"] = "J";
							}
							else if (v_ingot_code.Substring(1, 2) == "AB")//中板
							{
								tmmsm33bpgg["SLAB_TYPE"] = "Z";
							}
						}
						else
						{
							//长坯长大于0，表示为长坯 ，默认AB
							if (v_lslab_no_length > 0)
							{
								tmmsm33bpgg["SLAB_TYPE"] = "Z";
							}
							else if (v_lslab_no_length == 0)//上面已排除下游工厂的6391，故这里默认其他（卷板）
							{
								tmmsm33bpgg["SLAB_TYPE"] = "J";
							}
						}

						tmmsm33bpgg.Query("ST_NO,SLAB_TYPE");
					}
#pragma endregion

					if (tmmsm33_1["SLAB_LEN"].ToDecimal() < (v_len - tmmsm33bpgg["LEN_MIN"].ToDecimal()) || tmmsm33_1["SLAB_LEN"].ToDecimal() > (v_len + tmmsm33bpgg["LEN_MAX"].ToDecimal()))
					{
						bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["NO_SLAB_CAUSE"] = "长度不合";
						continue;//跳过该循环，表示没有匹配上该长坯号的规格
					}
					else if (tmmsm33_1["SLAB_WIDTH"].ToDecimal() < (v_width - tmmsm33bpgg["MINWIDTH"].ToDecimal()) || tmmsm33_1["SLAB_WIDTH"].ToDecimal() > (v_width + tmmsm33bpgg["MAXWIDTH"].ToDecimal()))
					{
						bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["NO_SLAB_CAUSE"] = "宽度不合";
						continue;//跳过该循环，表示没有匹配上该长坯号的规格
					}
					else if (tmmsm33_1["SLAB_THICK"].ToDecimal() < (v_thick - tmmsm33bpgg["THICK_MIN"].ToDecimal()) || tmmsm33_1["SLAB_THICK"].ToDecimal() > (v_thick + tmmsm33bpgg["THICK_MAX"].ToDecimal()))
					{
						bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["NO_SLAB_CAUSE"] = "厚度不合";
						continue;//跳过该循环，表示没有匹配上该长坯号的规格
					}
					else {
						if_pipei = 1;
						bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["LSLAB_NO"] = cmd_inq.GetString(1);
						bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["NO_SLAB_CAUSE"] = " ";
						break;//匹配上后跳出循环
					}
				}
				cmd_inq.Close();
			}

			//匹配成功
			if (if_pipei == 1)
			{
				Log::Trace("", "", "LSLAB_NO = {0} ", bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["LSLAB_NO"].ToString().Trim());
				sqlstr = "SELECT SLAB_NO,LSLAB_NO_LENGTH FROM TPSSM03 WHERE LSLAB_NO ='" + bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["LSLAB_NO"].ToString().Trim() + "' AND   SLAB_PROD_FLAG <> '1' AND SLAB_PROD_FLAG <> '9'  and SUBSTR(PONO,0,1) <> '9'   ORDER BY  LSLAB_NO,slab_no";
				Log::Trace("", "", "sqlstr = {0} ", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())//获取到同炉虚拟板坯
				{
					++slab_no_count;
					Log::Trace("", "", "slab_no_count = {0} ", slab_no_count);
					bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PONO_SLAB_" + CConvert::ToString(slab_no_count)] = cmd_inq.GetString(1);
					v_lslab_no_length = cmd_inq.GetDecimal(2);
				}
				cmd_inq.Close();

				Log::Trace("", "", "slab_no_count = {0}  PONO_SLAB_   {1}  ", slab_no_count, bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PONO_SLAB_" + CConvert::ToString(slab_no_count)].ToString());
			}
			else if (if_pipei == 0)//匹配失败的 赋余材原因
			{

			}

			//制造命令号 9 开头的是调度手动创建计划
			if (bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PONO"].ToString().Trim().SubstringNE(0, 1) == "9")
			{
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["NO_SLAB_CAUSE"] = "调度手动创建计划";
			}




#pragma endregion
			if (v_factory_next.Trim() != "")
			{
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["FACTORY_NEXT"] = v_factory_next;
			}
			if (v_cast_lot_no.Trim() != "")
			{
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["CAST_LOT_NO"] = v_cast_lot_no;
			}

			tpssm03["SLAB_NO"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PONO_SLAB_1"].ToString();
			tpssm03.Query("SLAB_NO");
			//tpssm03.Print();
			if (tpssm03["SLAB_DEST"].ToString().Trim() != "")
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_PLAN_DEST"] = tpssm03["SLAB_DEST"];
			else
				bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_PLAN_DEST"] = v_slab_dest;

			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["INGOT_CODE"] = v_ingot_code;
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["FIX_SLAB_NUM"] = slab_no_count;//余材为0

			//没有匹配到且没获取到去向，则表示余材，去向取该炉最后一支去向
			if (if_pipei == 0 && v_slab_dest.Trim() == "")
			{
				sqlstr = "SELECT SLAB_DEST,INGOT_CODE FROM TPSSM03 WHERE PONO ='" + tmmsm33_1["PONO"].ToString().Trim() + "'  order by  SLAB_NO desc ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())//获取去向
				{
					bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_PLAN_DEST"] = cmd_inq.GetString(1);
					bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["INGOT_CODE"] = cmd_inq.GetString(2);
				}
				cmd_inq.Close();
			}
			if (bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_PLAN_DEST"].ToString().Trim() == "")
			{
				tpssm10["PONO"] = tmmsm33_1["PONO"].ToString();
				tpssm40["PONO"] = tmmsm33_1["PONO"].ToString();
				if (tpssm10.Query("PONO"))
				{
					bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_PLAN_DEST"] = tpssm10["SLAB_DEST"];
				}
				else
				{
					tpssm40.Query("PONO");
					bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_PLAN_DEST"] = tpssm40["SLAB_DEST"];
				}

			}
			Log::Trace("", "", "SLAB_PLAN_DEST = {0}", bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_PLAN_DEST"].ToString());


#if defined(_SYS_PES)
			//获取此时出钢记号判断结果
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:        // Oracle 数据库
			default:
				sqlstr = CString(
					" SELECT FIN_ST_NO,JUDGE_CODE FROM TQMTS23 "
					"  WHERE HEAT_NO = @heat_no "
					//"    AND JUDGE_CODE = '1' "
					);
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tmmsm33_1["HEAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm33_1["FIN_ST_NO"] = cmd_inq.GetString(1);
			}
			else
			{
				tmmsm33_1["FIN_ST_NO"] = " ";
			}
			cmd_inq.Close();
			Log::Trace("", "", "获取最终出钢记号={0}", tmmsm33_1["FIN_ST_NO"].ToString());
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["FIN_ST_NO"] = tmmsm33_1["FIN_ST_NO"];

#endif



			///若材料号为空，则生成   电文材料号一般以二级为准
			if (tmmsm33_1["MAT_NO"].ToString().Trim() == "")
			{
				ret = f_mmsm_mat_no_ins(&bcls_rec_MMSM33_1, bcls_ret, conn);
				if (ret < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}


			if (false)//若有需要，则打开。因二级有计算重量传上来，目前暂不调用
			{
				ret = f_mmsm_get_matwt(&bcls_rec_MMSM33_1, bcls_ret, conn);
				if (ret < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			/*ret = f_mmsm33_check(&bcls_rec_MMSM33_1, bcls_ret, conn);
			if (ret < 0)
			{
			throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", "", "匹配结果if_pipei={0}", if_pipei);

			if (if_pipei == 1)//因f_mmsm33_check对命令坯做校验，故在调用完后再更新状态
			{
			//将匹配成功的命令坯号使用掉（状态改变）
			sqlstr_upd03 = " UPDATE TPSSM03 SET  SLAB_PROD_FLAG = '1' WHERE LSLAB_NO ='" + bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["LSLAB_NO"].ToString().Trim() + "' "
			" and   SLAB_PROD_FLAG <> '1' ";
			cmd_inq_upd03.SetCommandText(sqlstr_upd03);
			cmd_inq_upd03.ExecuteNonQuery();
			cmd_inq_upd03.Close();
			}*/



		}

		if (!tmmsm33_1.QueryCount("SLAB_NO"))
		{
			tmmsm33_1.Reset();

			tmmsm33_1.MergeFrom(bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]);
			tmmsm33_1["START_TIME"] = tmmsm33_1["SLAB_CUT_TIME"];
			tmmsm33_1["END_TIME"] = tmmsm33_1["SLAB_CUT_TIME"];

			tmmsm33_1.TrimOrBlank();
			//tmmsm33.Print();
			tmmsm33_1["PROD_MAKER"] = tmmsm33_1["REC_CREATOR"];
			tmmsm33_1.Insert();

			////将数据插入集控大屏需要的表中
			//sql_insert = "INSERT INTO DA_CCM_SLAB_SUMMARY  "
			//	" (ID, HEATNUMBER, PLANID, SPLITINDICATION, TREATMENTCOUNTER, STRANDNUMBER, SLABNUMBER, VIRTUALSLABID, MARKINGNUMBER, SLABFINAL, SLABCUTTIME, SAMPLECUTDONE, AIMLENGTH, ACTUALLENGTH, THICKNESS, WIDTHHEAD, WIDTHTAIL, WEIGHTCALC, TIMESTAMP, PMREADTIME, PMREAD, QMREADTIME, QMREAD, AGGREGATENAME, DMREADTIME, DMREAD, NYREAD, NYREADTIME) "
			//	"VALUES (" + tmmsm33_1["ID_SJ"].ToString() + ", '" + tmmsm33_1["HEAT_NO"].ToString() + "', '"
			//	+ tmmsm33_1["SM_PLAN_NOL2"].ToString().Trim() + "', 0, 1, '" + tmmsm33_1["STRAND_NO"].ToString() + "', '" + tmmsm33_1["SLAB_NO"].ToString()
			//	+ "', '" + tmmsm33_1["VIRTUAL_SLAB_NO"].ToString() + "', '" + tmmsm33_1["PRINT_NO"].ToString() + "', '" + v_slab_final
			//	+ "', TO_DATE('" + tmmsm33_1["SLAB_CUT_TIME"].ToString() + "','YYYY-MM-DD HH24:MI:SS'), " + tmmsm33_1["IF_SAMPLE"].ToString() + "," + bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["AIM_LENGTH"].ToString()
			//	+ "," + bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["ACTUAL_LENGTH"].ToString() + ", " + bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["THICKNESS"].ToString() + ", " + bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["WIDTH_HEAD"].ToString() + ", " + bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["WIDTH_TAIL"].ToString()
			//	+ ", " + bcls_rec->Tables["INT_MES_SLAB_REPORT"].Rows[0]["WEIGHT_CALC"].ToString()
			//	+ ", TO_DATE( '" + datetime + "','YYYY-MM-DD HH24:MI:SS'), null, 'N', null, 'N', '" + tmmsm33_1["DEV_CODE"].ToString() + "', null, 'N', 'N', null)";

			//cmd_insert.SetCommandText(sql_insert);
			//Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
			//cmd_insert.ExecuteNonQuery();
			//Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
			//cmd_insert.Close();






			/*ret = f_mmsm3301n_proc(&bcls_rec_MMSM33_1, bcls_ret, conn);
			if (ret < 0)
			{
			throw CApplicationException(-1, s.msg, log.Location);
			}*/
		}
		else
		{
			if (bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Columns.Contains("PROC_DIV"))
			{
				for (int i = 0; i < bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows.get_Count(); i++)
				{
					bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[i]["PROC_DIV"] = "U";
				}
			}

			Log::Trace("", __FUNCTION__, "SLAB_NO ={0}", tmmsm33_1["SLAB_NO"].ToString().Trim());

			tmmsm33_1.Query("SLAB_NO");
			//tmmsm33.MergeFrom(bcls_rec_MMSM33.Tables["TMMSM33"].Rows[0]);//这里是否改成单字段赋值   这里改成单独赋值  只赋关键字段

			tmmsm33_1["HEAT_NO"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["HEAT_NO"];
			tmmsm33_1["STRAND_NO"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["STRAND_NO"];
			tmmsm33_1["PRINT_NO"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["PRINT_NO"];
			tmmsm33_1["VIRTUAL_SLAB_NO"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["VIRTUAL_SLAB_NO"];
			tmmsm33_1["MAT_NO"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["MAT_NO"];
			tmmsm33_1["SLAB_CUT_TIME"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_CUT_TIME"];
			tmmsm33_1["SLAB_THICK"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_THICK"];
			tmmsm33_1["SLAB_HEAD_WIDTH"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_HEAD_WIDTH"];
			tmmsm33_1["SLAB_TAIL_WIDTH"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_TAIL_WIDTH"];
			tmmsm33_1["SPLIT_INDICATION"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SPLIT_INDICATION"];
			tmmsm33_1["SLAB_WIDTH"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_HEAD_WIDTH"].ToDecimal() > bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_TAIL_WIDTH"].ToDecimal()
				? bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_TAIL_WIDTH"].ToDecimal() : bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_HEAD_WIDTH"].ToDecimal();
			tmmsm33_1["DEV_CODE"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["DEV_CODE"];
			tmmsm33_1["SM_PLAN_NOL2"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SM_PLAN_NOL2"];
			tmmsm33_1["SM_PLAN_NO"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SM_PLAN_NO"];

			tmmsm33_1["IF_SAMPLE"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["IF_SAMPLE"];
			tmmsm33_1["MAT_TARG_LEN"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["MAT_TARG_LEN"];
			tmmsm33_1["SLAB_LEN"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_LEN"];
			tmmsm33_1["L2_THEORY_WT"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["L2_THEORY_WT"];
			tmmsm33_1["ID_SJ"] = bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["ID_SJ"];

			tmmsm33_1["START_TIME"] = tmmsm33_1["SLAB_CUT_TIME"];
			tmmsm33_1["END_TIME"] = tmmsm33_1["SLAB_CUT_TIME"];

			tmmsm33_1["REC_REVISE_TIME"] = s.datetime;
			tmmsm33_1["REC_REVISOR"] = s.userid;
			Log::Trace("", "", "SLAB_PLAN_DEST = {0}", tmmsm33_1["SLAB_PLAN_DEST"].ToString());


			tmmsm33_1.TrimOrBlank();
			//tmmsm33.Print();
			tmmsm33_1["PROD_MAKER"] = tmmsm33_1["REC_CREATOR"];
			tmmsm33_1.Update("PRINT_NO,MAT_NO,SLAB_CUT_TIME,SLAB_THICK,SLAB_HEAD_WIDTH,SLAB_TAIL_WIDTH,SLAB_WIDTH,SPLIT_INDICATION,DEV_CODE,SM_PLAN_NOL2,IF_SAMPLE,MAT_TARG_LEN,SLAB_LEN,ID_SJ,START_TIME,END_TIME,REC_REVISE_TIME,REC_REVISOR,PROD_MAKER", "SLAB_NO");
			tpcommit(0);
			tpbegin(0, 0);
			bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows.Clear();
			tmmsm33_1.MergeTo(bcls_rec_MMSM33_1.Tables["TMMSM33_1"], false);
			if (bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Columns.Contains("PROC_DIV"))
			{
				for (int i = 0; i < bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows.get_Count(); i++)
				{
					bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[i]["PROC_DIV"] = "U";
				}
			}

			Log::Trace("", "", "SLAB_PLAN_DEST = {0}", bcls_rec_MMSM33_1.Tables["TMMSM33_1"].Rows[0]["SLAB_PLAN_DEST"].ToString());


			/*ret = f_mmsm3301u_proc(&bcls_rec_MMSM33_1, bcls_ret, conn);
			if (ret < 0)
			{
			throw CApplicationException(-1, s.msg, log.Location);
			}*/
		}



	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
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


