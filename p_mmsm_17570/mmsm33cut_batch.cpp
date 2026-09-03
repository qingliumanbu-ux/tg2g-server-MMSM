/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2011-12-13
Version:1.0
Description: 炼钢板坯组批管理
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/




/* ***** 静态函数申明 ***** */

//修改板坯主档信息
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_e2t8m1_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

/*<remark>=========================================================
/// <summary>
/// 炼钢板坯组批管理
/// <para>
/// 炼钢板坯组批管理
/// </para>
///   选择废钢进行新增，修改，删除
////  人工录入材料号，批次号和钢种
///
////
///
/// </summary>
/// <param name="">炼钢板坯组批管理</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm33cut_batch)

int f_mmsm33cut_batch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	int  n_count = 0;
	CString cutFinFlag = "";
	int mat_seq = 0;
	int mat_tube = 0;
	int fetchRowCount = 0;
	CString vcf_heat_no = "";//代表成分熔炼号
	CString v_remark = "";//备注
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString v_st_no = "";//出钢记号
	CString v_c_div = "";//碳锈区分  1  不锈钢  2碳钢
	CString new_heat_no = "";
	int count_heat_no = 0;
	CString v_proc_div = "";//操作标记  I  新增  U 修改  D  删除
	CString sm_plan_nol2 = "";
	CString dev_code = "";
	CString heat_no = "";
	CDecimal split_indication =0;
	CString v_strand_no = "";//流号
	CString v_mat_no = "";//材料号
	CString cut_no = "";//切割顺序号
	CString v_print_no = "";//喷印号-板坯号
	
	CString v_cast_div_no = "";//浇次顺序号
	CDecimal v_tpssm03_count = 0;//根据pono查询获取条数
	CDecimal v_slab_final = 0;//尾坯标记
	

	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm38("TMMSM38");
	CModel tmmsm39("TMMSM39");
	CModel tpssm03("TPSSM03");
	CModel tpssm03_JY("TPSSM03");
	CModel tpssm10("TPSSM10");
	CModel tpssm11("TPSSM11");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_mat_no(conn);


	try
	{

		EIClass bcls_rec_SLAB_REPORT;
		if (!bcls_rec_SLAB_REPORT.Tables.Contains("INT_MES_SLAB_REPORT"))
		{
			bcls_rec_SLAB_REPORT.Tables.Add("INT_MES_SLAB_REPORT");
		}

		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_STRING, "AGGREGATE_NAME");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_STRING, "HEAT_NUMBER");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_STRING, "PLAN_NUMBER");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_STRING, "STRAND_NUMBER");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_STRING, "SLAB_NUMBER");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_STRING, "VIRTUAL_SLAB_ID");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_STRING, "MARKING_NUMBER");

		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_DECIMAL, "SPLIT_INDICATION");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_DECIMAL, "TREATMENT_COUNTER");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_DECIMAL, "SLAB_FINAL");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_DECIMAL, "SLAB_CUT_TIME");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_DECIMAL, "SAMPLE_CUT_DONE");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_DECIMAL, "AIM_LENGTH");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_DECIMAL, "ACTUAL_LENGTH");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_DECIMAL, "THICKNESS");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_DECIMAL, "WIDTH_HEAD");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_DECIMAL, "WIDTH_TAIL");
		bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Columns.Add(DT_DECIMAL, "WEIGHT_CALC");

		EIClass bcls_rec_tpssm03;
		bcls_rec_tpssm03.Tables[0].set_TableName("TPSSM03");
		bcls_rec_tpssm03.Tables[0].Columns.Add(tpssm03);

		tpssm03["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"].ToString();
		v_tpssm03_count = tpssm03.QueryCount("PONO");

		sqlstr = "SELECT decode(LSLAB_NO_LENGTH,0,slab_len,LSLAB_NO_LENGTH) slab_len,T.* FROM TPSSM03 T WHERE PONO = '" + tpssm03["PONO"].ToString() + "'  AND  SLAB_PROD_FLAG = '0'  ORDER BY SLAB_NO ASC";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_rec_tpssm03.Tables[0]);
		cmd_inq.Close();


		sqlstr = "SELECT SM_PLAN_NOL2,HEAT_NO,SPLIT_INDICATION,ST_NO,CAST_DIV_NO FROM TPSSM11 WHERE PONO = '" + tpssm03["PONO"].ToString() + "'";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			sm_plan_nol2 = cmd_inq.GetString(1);
			heat_no = cmd_inq.GetString(2);
			split_indication = cmd_inq.GetDecimal(3);
			v_st_no = cmd_inq.GetString(4);
			v_cast_div_no = cmd_inq.GetString(5);
		}
		cmd_inq.Close();

		//当熔炼号没有获取到时代表11表已归档，此时查41表  正常情况归档不允许新增，此处上线前用于测试  mfj  20240226
		if (heat_no.Trim() == "")
		{
			sqlstr = "SELECT SM_PLAN_NOL2,HEAT_NO,SPLIT_INDICATION,ST_NO,CAST_DIV_NO FROM TPSSM41 WHERE PONO = '" + tpssm03["PONO"].ToString() + "'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				sm_plan_nol2 = cmd_inq.GetString(1);
				heat_no = cmd_inq.GetString(2);
				split_indication = cmd_inq.GetDecimal(3);
				v_st_no = cmd_inq.GetString(4);
				v_cast_div_no = cmd_inq.GetString(5);
			}
			cmd_inq.Close();
		}

		sqlstr = "SELECT DEV_CODE FROM TPSSM12 WHERE HEAT_NO = '" + heat_no + "' and AREA_ID = '5'";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			dev_code = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		//当dev_code没有获取到时，表示12表已归档，此时要查42表   正常情况归档不允许新增，此处上线前用于测试  mfj  20240226
		if (dev_code.Trim() == "")
		{
			sqlstr = "SELECT DEV_CODE FROM TPSSM42 WHERE HEAT_NO = '" + heat_no + "' and AREA_ID = '5'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				dev_code = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}



		Log::Trace("", __FUNCTION__, "5555[{0}]", bcls_rec_tpssm03.Tables[0].Rows.get_Count());

		for (int i = 0; i < bcls_rec_tpssm03.Tables[0].Rows.get_Count(); i++)
		{
			tpssm03.Reset();
			v_strand_no = "";
			tpssm03.MergeFrom(bcls_rec_tpssm03.Tables[0].Rows[i]);
			Log::Trace("", __FUNCTION__, "SLAB_LEN[{0}]", tpssm03["SLAB_LEN"].ToString());

			tpssm03_JY.MergeFrom(bcls_rec_tpssm03.Tables[0].Rows[i]);
			tpssm03_JY.Query();

			//当该命令坯已产出时，不再重复处理     防止倍尺坯出现重复处理情况   mfj  20240226
			if (tpssm03_JY["SLAB_PROD_FLAG"].ToString().Trim() == "1")
			{
				continue;
			}

			if (dev_code.Substring(1, 1) == "0")
				v_strand_no = "Z";
			else if (dev_code.Substring(1, 1) == "1")
				v_strand_no = "A";
			else if (dev_code.Substring(1, 1) == "2")
				v_strand_no = "B";
			else if (dev_code.Substring(1, 1) == "3" && (i%2)==0)
				v_strand_no = "C";
			else if (dev_code.Substring(1, 1) == "3" && (i % 2) != 0)
				v_strand_no = "E";
			else if (dev_code.Substring(1, 1) == "4" && (i % 2) == 0)
				v_strand_no = "E";
			else if (dev_code.Substring(1, 1) == "4" && (i % 2) != 0)
				v_strand_no = "F";

			cut_no = "";//置空
			v_mat_no = "";//置空

			#pragma region  生成材料号
			v_mat_no = heat_no;
			Log::Trace("", __FUNCTION__, "v_strand_no[{0}] v_tpssm03_count [{1}]  get_Count [{2}]", v_strand_no, v_tpssm03_count, bcls_rec_tpssm03.Tables[0].Rows.get_Count());

			

			//材料号生成 取自喷印号  头坯尾坯时会有00,99，AA,ZZ的情况  太钢定制  mfj   20231228  
			//如果总条数相同,当前为最后一只，则为尾坯，如果为第一支，且总条数相同，则为头坯
			if ((i == 0 || i == bcls_rec_tpssm03.Tables[0].Rows.get_Count()-1) && v_tpssm03_count == bcls_rec_tpssm03.Tables[0].Rows.get_Count())//头坯或尾坯
			{
				//CString v_strand_no = bcls_rec_SLAB_REPORT.Tables[0].Rows[i]["STRAND_NO"].ToString().Trim();//流号
				

				if (i ==0)//头坯
				{
					//当为头坯单流时，顺序号为00 ,双流为AA
					if (v_strand_no == "Z" || v_strand_no == "A" || v_strand_no == "B")
					{
						cut_no = "00";
					}
					else if (v_strand_no == "C" || v_strand_no == "D" || v_strand_no == "E" || v_strand_no == "F")
					{
						cut_no = "AA";
					}
				}
				else if (i == bcls_rec_tpssm03.Tables[0].Rows.get_Count() - 1)//尾坯
				{
					//当为尾坯单流时，顺序号为99 ,双流为ZZ
					if (v_strand_no == "Z" || v_strand_no == "A" || v_strand_no == "B")
					{
						cut_no = "99";
					}
					else if (v_strand_no == "C" || v_strand_no == "D" || v_strand_no == "E" || v_strand_no == "F")
					{
						cut_no = "ZZ";
					}
				}
				v_mat_no = v_mat_no + cut_no;
			}
			else
			{
				cmd_inq.SetCommandText("SELECT NVL(MAX(SUBSTR( MAT_NO, LENGTH(MAT_NO) - 1 , 2 )),0) FROM TMMSM33 WHERE HEAT_NO = @heatNo  AND STRAND_NO ='" + v_strand_no + "'"
					"  AND SUBSTR(MAT_NO,9,2) <> 'AA' AND SUBSTR(MAT_NO,9,2) <> '00' AND SUBSTR(MAT_NO,9,2) <> '99' AND SUBSTR(MAT_NO,9,2) <> 'ZZ' ");
				cmd_inq.Parameters.Set("heatNo", heat_no);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					if (cmd_inq.GetString(1).Trim() != "")
					{
						//因第一支坯子为AA时，该处查询结果为0,故认为第二支材料号  为00时亦然
						if ((cmd_inq.GetString(1).Trim() == "0") && v_strand_no != "D" && v_strand_no != "F")
						{
							v_mat_no = v_mat_no + "01";
						}
						else if (v_strand_no == "C" || v_strand_no == "E" || v_strand_no == "D" || v_strand_no == "F" )
						{
							if ((CDecimal::Parse(cmd_inq.GetString(1).Trim()) + 2).ToString().Trim().GetLength() == 1)
							{
								Log::Trace("", __FUNCTION__, "qqq	= [{0}]", (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + 2).ToString().Trim());
								Log::Trace("", __FUNCTION__, "i	= [{0}]", i);
								v_mat_no = v_mat_no + "0" + (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + 2).ToString().Trim();
							}
							else
							{
								v_mat_no = v_mat_no + (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + 2).ToString().Trim();
							}
						}
						else if ((CDecimal::Parse(cmd_inq.GetString(1).Trim())  + 1).ToString().Trim().GetLength() == 1)
						{
							Log::Trace("", __FUNCTION__, "qqq	= [{0}]", (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + 1).ToString().Trim());
							Log::Trace("", __FUNCTION__, "i	= [{0}]", i);
							v_mat_no = v_mat_no + "0" + (CDecimal::Parse(cmd_inq.GetString(1).Trim()) + 1).ToString().Trim();
						}
						else
						{
							v_mat_no = v_mat_no + (CDecimal::Parse(cmd_inq.GetString(1).Trim())  + 1).ToString().Trim();
						}
					}
				}
				cmd_inq.Close();
				//材料号产出时为10位，分切后为12位。
				v_mat_no = v_mat_no;
			}
			#pragma endregion
			
			int len_no = CDecimal::Parse(v_cast_div_no).Round(0).ToString().GetLength();
			
			CString cast_div_num = "";
			if (cut_no.Trim() != "")//当为头尾坯时
			{
				if (len_no == 1)
				{
					cast_div_num = "00";
				}
				else if (len_no == 2)
				{
					cast_div_num = "0";
				}
				else
				{
					cast_div_num = "";
				}
				v_print_no = heat_no + v_st_no +
					v_strand_no + cut_no + cast_div_num + CDecimal::Parse(v_cast_div_no).Round(0).ToString();
			}
			else//非头尾坯时
			{
				if (len_no == 1)
				{
					cast_div_num = "00";
				}
				else if (len_no == 2)
				{
					cast_div_num = "0";
				}
				else
				{
					cast_div_num = "";
				}

				v_print_no = heat_no + v_st_no +
					v_strand_no + v_mat_no.Substring(8, 2) + cast_div_num +
					+CDecimal::Parse(v_cast_div_no).Round(0).ToString();
			}

			Log::Trace("", __FUNCTION__, "len_no[{0}]  v_mat_no[{1}]  v_print_no = [{2}]", len_no, v_mat_no, v_print_no);
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows.Clear();
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows.Add();
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["AGGREGATE_NAME"] = dev_code;//工位
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["HEAT_NUMBER"] = heat_no;	//熔炼号
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["PLAN_NUMBER"] = sm_plan_nol2;//计划号
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["SPLIT_INDICATION"] = split_indication;//分包号
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["TREATMENT_COUNTER"] = 1;//同工位处理次数
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["STRAND_NUMBER"] = v_strand_no;//流号
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["SLAB_NUMBER"] = v_print_no;//板坯号
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["VIRTUAL_SLAB_ID"] = tpssm03["LSLAB_NO"].ToString();//虚拟板坯号
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["MARKING_NUMBER"] = v_print_no;//喷印号
			if (i == bcls_rec_tpssm03.Tables[0].Rows.get_Count() - 1)
				bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["SLAB_FINAL"] = 1;//尾坯标记
			else
				bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["SLAB_FINAL"] = 0;//尾坯标记

			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["SLAB_CUT_TIME"] = datetime;//板坯切断时刻
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["SAMPLE_CUT_DONE"] = 0;//是否取样
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["AIM_LENGTH"] = tpssm03["SLAB_LEN"].ToDecimal() /1000;//目标长度
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["ACTUAL_LENGTH"] = tpssm03["SLAB_LEN"].ToDecimal() / 1000;//实际长度
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["THICKNESS"] = tpssm03["SLAB_THICK"].ToDecimal() / 1000;//厚度
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["WIDTH_HEAD"] = tpssm03["SLAB_WIDTH"].ToDecimal() / 1000;//头宽
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["WIDTH_TAIL"] = tpssm03["SLAB_WIDTH"].ToDecimal() / 1000;//尾宽
			bcls_rec_SLAB_REPORT.Tables["INT_MES_SLAB_REPORT"].Rows[0]["WEIGHT_CALC"] = tpssm03["SLAB_WT"].ToDecimal() * 1000;//计算重量

			doFlag = f_e2t8m1_proc(&bcls_rec_SLAB_REPORT, bcls_ret, conn);
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
