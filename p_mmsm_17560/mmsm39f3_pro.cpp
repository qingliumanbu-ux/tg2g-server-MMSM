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

/*<remark>=========================================================
/// <summary>
/// 炼钢板坯切废管理
/// <para>
/// 炼钢板坯切废管理
/// </para>
///		分切后的切废量先用分切前的坯号数据，并保留分切时人工录入的切废长度和切废重量
////
///
/////// 当坯子处于封锁状态时，用户仍可操作，但不对主档表更新，不发送电文给产销
/// 存入待办事项表中，等待封锁取消后，统一处理
///
/// </summary>
/// <param name="">炼钢板坯切废管理</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm39f3_pro)
int f_mmsm39_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_210044_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
int f_mmsm35f9_del(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm_get_density(CString ST_NO, CDecimal& MAT_DENSITY, CDbConnection* conn);//通过钢种计算密度


int f_mmsm39f3_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_count1 = "";
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
	CString v_operate = "";//操作区分	 I 新增    U 修改
	CString v_resume_seq_no = "";//序号
	CString v_sap_erp_matnr = "";//物料编码
	CString v_mat_no = "";
	CString v_prod_shift_no = "";
	CString v_prod_shift_group = "";
	CString v_recut_group = "";
	CDecimal v_cut_scrap_wt = 0;

	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm96_1("TMMSM96");
	CModel tmmsm39("TMMSM39");
	CModel tmmsm39_1("TMMSM39_1");
	CModel tmmsm33dbsx("TMMSM33DBSX");
	CModel tmmsm35mp("TMMSM35MP");
	CModel tmmsm3e("TMMSM3E");
	CDbCommand cmd_inq(conn);


	try
	{


		EIClass bcls_rec_210044;//发送L4二切实绩电文
		bcls_rec_210044.Tables[0].set_TableName("210044");
		bcls_rec_210044.Tables[0].Columns.Add(tmmsm39);
		bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
		bcls_rec_210044.Tables[0].Rows.Add();

		EIClass bcls_MM0099;//发送L4二切实绩电文
		bcls_MM0099.Tables[0].set_TableName("MM0099");
		bcls_MM0099.Tables[0].Columns.Add(tmmsm96_1);
		bcls_MM0099.Tables[0].Rows.Add();


		if (bcls_rec->Tables[0].Columns.Contains("PRO_DIV"))
		{
			v_operate = bcls_rec->Tables[0].Rows[0]["PRO_DIV"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("SAP_ERP_MATNR"))
		{
			v_sap_erp_matnr = bcls_rec->Tables[0].Rows[0]["SAP_ERP_MATNR"].ToString().Trim();
		}

		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
			tmmsm01.Query();
			tmmsm39.CopyFrom(tmmsm01);
			Log::Info("", __FUNCTION__, "PRINT_NO=[{0}]", tmmsm01["PRINT_NO"].ToString());
		}




		if (v_operate == "D")
		{
			//此处添加校验，根据材料号查询TMMSM01，如果查询不到，表示为母坯信息，则报错，需要先将分切撤销

			if (tmmsm01.QueryCount("MAT_NO") <= 0)
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}已做中板改切，请先操作中板改切撤销，再操作改切撤销！", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}


		/*if (tmmsm01["LOGISTICS_STATUS"].ToString().Trim() != "0"&& tmmsm01["LOGISTICS_STATUS"].ToString().Trim() != "1"
			&& tmmsm01["LOGISTICS_STATUS"].ToString().Trim() != "4")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已不在现场，不允许改切处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		if (tmmsm01["C_STATESIGN"].ToString().Trim() != "0"&& tmmsm01["C_STATESIGN"].ToString().Trim() != ""
			&& tmmsm01["C_STATESIGN"].ToString().Trim() != "6")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已不在现场，不允许改切处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//当其他反馈标记为空且收货标记不为S时，表示当时未做其他操作，且未收货
		//当其他标记有值，且收货标记不为S，表示等待反馈中，不允许做该操作
		if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S" && tmmsm01["FINISH_FLAG"].ToString().Trim() == ""
			&& tmmsm01["MEND_FEEDBACK_FLAG"].ToString().Trim() == ""&& tmmsm01["DIV_FLAG"].ToString().Trim() == "")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}未收货,不允许分段处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm01["DIV_FLAG"].ToString().Trim() == "1" &&tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已做中板改切，当前状态为等待制造管理系统反馈,暂不允许再进行该操作", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm01["MEND_FEEDBACK_FLAG"].ToString().Trim() == "1" &&tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已做修磨，当前状态为等待制造管理系统反馈,暂不允许再进行该操作", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm01["FINISH_FLAG"].ToString().Trim() == "1" &&tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已做改切，当前状态为等待制造管理系统反馈,暂不允许再进行该操作", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/*if (tmmsm01["LGORT"].ToString().Trim() == "6246")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已做修磨,请先撤销修磨再进行改切操作！", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() != "1")
		{
		CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
		CMessageFormat::Format(s.msg, "材料{0}未综判,不允许改切处理", arguments, 1);
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		tmmsm39.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm39["PROD_SHIFT_GROUP"] = tmmsm01["PROD_SHIFT_GROUP"];
		Log::Info("", __FUNCTION__, "PROD_SHIFT_GROUP =[{0}]", tmmsm39["PROD_SHIFT_GROUP"].ToString());

#pragma region   添加校验条件,因01表会根据切后长宽厚更新规格和重量，故，该数据不可为空  mfj  20240118
		if (tmmsm39["CUT_BEFORE_LEN"].ToDecimal() <= 0 ||
			tmmsm39["CUT_BEFORE_THICK"].ToDecimal() <= 0 ||
			tmmsm39["CUT_BEFORE_WIDTH"].ToDecimal() <= 0 || tmmsm39["CUT_BEFORE_WT"].ToDecimal() <= 0)
		{
			strcpy(s.sysmsg, "切前规格和重量不可为0！");
			strcpy(s.msg, s.sysmsg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm39["CUT_AFTER_LEN"].ToDecimal() <= 0 ||
			tmmsm39["CUT_AFTER_THICK"].ToDecimal() <= 0 ||
			tmmsm39["CUT_AFTER_WIDTH"].ToDecimal() <= 0 || tmmsm39["CUT_AFTER_WT"].ToDecimal() <= 0)
		{
			strcpy(s.sysmsg, "切后规格和重量不可为0！");
			strcpy(s.msg, s.sysmsg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//切前量与切后量不一致，且切废量没有值时，认为忘记录入切废量，这里计算一下
		if ((tmmsm39["CUT_AFTER_WT"].ToDecimal() != tmmsm39["CUT_BEFORE_WT"].ToDecimal()) && tmmsm39["CUT_SCRAP_WT"].ToDecimal() == 0)
		{
			tmmsm39["CUT_SCRAP_WT"] = tmmsm39["CUT_BEFORE_WT"].ToDecimal() - tmmsm39["CUT_AFTER_WT"].ToDecimal();
		}

		

		Log::Info("", __FUNCTION__, "CUT_AFTER_WT=[{0}]", tmmsm39["CUT_AFTER_WT"].ToDecimal());
		//	将重量保留三位小数
		tmmsm39["CUT_AFTER_WT"] = tmmsm39["CUT_AFTER_WT"].ToDecimal().Round(3);
		tmmsm39["CUT_SCRAP_WT"] = tmmsm39["CUT_SCRAP_WT"].ToDecimal().Round(3);


#pragma endregion 

		//名义规格与实际规格保持一致
		tmmsm39["MAT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
		tmmsm39["MAT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
		tmmsm39["MAT_THICK"] = tmmsm39["CUT_AFTER_THICK"];

		//画面切废，规格取切后长宽厚
		tmmsm39["MAT_ACT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
		tmmsm39["MAT_ACT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
		tmmsm39["MAT_ACT_THICK"] = tmmsm39["CUT_AFTER_THICK"];
		tmmsm39["MAT_WT"] = tmmsm39["CUT_AFTER_WT"];//切后重量
		tmmsm39["MAT_ACT_WT"] = tmmsm39["CUT_AFTER_WT"];//系统重量即实际重量
		//tmmsm39["REAL_TIME_WT"] = tmmsm39["CUT_AFTER_WT"];//实时重量
		tmmsm39["QUALIFIED_WT"] = tmmsm39["CUT_AFTER_WT"];//合格产量

		tmmsm39["SAP_ERP_MATNR"] = tmmsm39["CUTTING_TYPE"];//切割类型 存入 物料编码中
		v_cut_scrap_wt = tmmsm39["CUT_SCRAP_WT"];//保存切废撤销前的切废量


		if (v_operate == "I")
		{
			//判断一下切前量与当前的01实际重量是否一致，如果不一致，表示已经做了改切或是修磨等其他操作。
			if (tmmsm01["MAT_ACT_WT"].ToDecimal() != tmmsm39["CUT_BEFORE_WT"].ToDecimal())
			{
				strcpy(s.sysmsg, "切前的系统重量与录入的切前重量数据不一致，请查看系统重量在此时是否更改了！");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm39["CUT_AFTER_LEN"].ToDecimal() == 0 )
			{
				strcpy(s.sysmsg, "切后长度不能为0 ！");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//if (tmmsm39["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm39["PROD_SHIFT_GROUP"].ToString().Trim() == "")
			if (tmmsm39["RECUT_GROUP"].ToString().Trim() == "")
			{
				f_epep_get_shift_group("SMCP", datetime, v_prod_shift_no, v_prod_shift_group, conn);
				//tmmsm39["PROD_SHIFT_NO"] = v_prod_shift_no;
				tmmsm39["RECUT_GROUP"] = v_recut_group;
			}

			doFlag = f_mm0011("TMMSM39_seq", 8, v_resume_seq_no, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm39["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
			tmmsm39["REC_CREATOR"] = s.userid;
			tmmsm39["REC_CREATE_TIME"] = datetime;
			//if ((v_mat_no.Substring(8, 2) == "00" || v_mat_no.Substring(8, 2) == "99" || v_mat_no.Substring(8, 2) == "AA" || v_mat_no.Substring(8, 2) == "ZZ")
			//	&& (tmmsm39["CUTTING_TYPE"].ToString().Trim() == "8" || tmmsm39["CUTTING_TYPE"].ToString().Trim() == "9"))
			//切废量为0，finish_flag为9    2024.07.12
			if (((v_mat_no.Substring(8, 2) == "00" || v_mat_no.Substring(8, 2) == "99" || v_mat_no.Substring(8, 2) == "AA" || v_mat_no.Substring(8, 2) == "ZZ")
				&& (tmmsm39["CUTTING_TYPE"].ToString().Trim() == "8" || tmmsm39["CUTTING_TYPE"].ToString().Trim() == "9") && (tmmsm01["MEND_FLAG"].ToString().Trim() == "" || tmmsm01["MEND_FLAG"].ToString().Trim() == "0")) || tmmsm39["CUT_SCRAP_WT"].ToDecimal() == 0)
			{
				tmmsm39["FINISH_FLAG"] = "9";//1 待反馈   3 删除待反馈 9处理成功   头尾坯且切头切尾类型的走物料同步，不走反馈
			}
			else
			{
				tmmsm39["FINISH_FLAG"] = "1";//1 待反馈   3 删除待反馈 9处理成功
			}

			tmmsm39.TrimOrBlank();
			tmmsm39.Insert();

			//切废量为0，不记录实绩    2024.07.12
			if (tmmsm39["CUT_SCRAP_WT"].ToDecimal() != 0)
			{
				tmmsm39_1.CopyFrom(tmmsm39);//记录履历
				tmmsm39_1.TrimOrBlank();
				tmmsm39_1.Insert();
			}
		}
		else if (v_operate == "U")
		{
			tmmsm39["REC_REVISOR"] = s.userid;
			tmmsm39["REC_REVISE_TIME"] = datetime;
			tmmsm39.TrimOrBlank();
			tmmsm39.Update("REC_REVISOR,REC_REVISE_TIME,RECUT_DATE,CUT_BEFORE_LEN,CUT_AFTER_LEN,OTHER_CUT_LEN,CUT_BEFORE_WT,CUT_AFTER_WT,CUT_SCRAP_WT,CUTTING_TYPE,GRINDING_FLAG", "RESUME_SEQ_NO,MAT_NO");
		}
		else if (v_operate == "D")
		{
			if ((v_mat_no.Substring(8, 2) == "00" || v_mat_no.Substring(8, 2) == "99" || v_mat_no.Substring(8, 2) == "AA" || v_mat_no.Substring(8, 2) == "ZZ")
				&& (tmmsm39["CUTTING_TYPE"].ToString().Trim() == "8" || tmmsm39["CUTTING_TYPE"].ToString().Trim() == "9") && (tmmsm01["MEND_FLAG"].ToString().Trim() == "" || tmmsm01["MEND_FLAG"].ToString().Trim() == "0"))
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}改切为头尾坯且切割类型为切头切尾，*/不可进行撤销操作！", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else
			{
				//更改 删除时，只更新状态，等到产销反馈再处理具体
				if (tmmsm01["MAT_ACT_WT"].ToDecimal() == tmmsm39["CUT_AFTER_WT"].ToDecimal())
				{
					//切废封锁取消	2024.06.25
					/*if (tmmsm01["HOLD_FLAG"].ToString() != "0")
					{
						CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
						CMessageFormat::Format(s.msg, "材料{0}处于封锁状态,不允许删除处理", arguments, 1);
						throw CApplicationException(-1, s.msg, log.Location);
					}*/
					if (tmmsm39["CUT_SCRAP_WT"].ToDecimal() == 0 && tmmsm39["FINISH_FLAG"].ToString().Trim() == "9")
					{
						tmmsm39.Delete("RESUME_SEQ_NO,MAT_NO");//切废为0，删除39表记录    2024.07.12
					}
					else
					{
						tmmsm39.TrimOrBlank();
						tmmsm39["FINISH_FLAG"] = "3";


						//因为同时删除两笔，会导致反馈时获取到两笔，故这里需要卡一下
						if (tmmsm39.QueryCount("FINISH_FLAG,MAT_NO") > 0)
						{
							strcpy(s.sysmsg, "该材料已经有一笔改切记录处于删除等待反馈状态，请等待处理成功后再操作！");
							strcpy(s.msg, s.sysmsg);
							throw CApplicationException(-1, s.msg, log.Location);
						}
						tmmsm39.Update("FINISH_FLAG", "RESUME_SEQ_NO,MAT_NO");
						//tmmsm39.Delete("RESUME_SEQ_NO,MAT_NO");  //不直接删除，发送电文后等待产销反馈后再删

						tmmsm39_1.CopyFrom(tmmsm39);//记录履历
						tmmsm39_1.TrimOrBlank();
						tmmsm39_1["FINISH_FLAG"] = "3";
						tmmsm39_1.Update("FINISH_FLAG", "RESUME_SEQ_NO,MAT_NO");
						//tmmsm39_1.Delete("RESUME_SEQ_NO,MAT_NO");
					}
					
				}
				else
				{
					strcpy(s.sysmsg, "切后重量与系统重量不一致！");
					strcpy(s.msg, s.sysmsg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

		}
		tmmsm96.CopyFrom(tmmsm01);
		tmmsm96_1.CopyFrom(tmmsm01);
		//添加改切记录履历
		if (v_operate == "I" || v_operate == "U")
		{
			tmmsm96_1["EVENT_ID"] = "MM3K";
			tmmsm96_1["EVENT_LINE_TYPE"] = "SM";
			tmmsm96_1["FUNC_ID"] = "f_mmsm39_proc";
			tmmsm96_1["SYSTEM_ID"] = "MMSM";
			tmmsm96_1["EVENT_DESC"] = "改切记录数据录入";
		}
		else  if (v_operate == "D")
		{
			tmmsm96_1["EVENT_ID"] = "MM3K";
			tmmsm96_1["EVENT_LINE_TYPE"] = "SM";
			tmmsm96_1["FUNC_ID"] = "f_mmsm39_proc";
			tmmsm96_1["SYSTEM_ID"] = "MMSM";
			tmmsm96_1["EVENT_DESC"] = "改切记录删除";
		}
		
		bcls_MM0099.Tables[0].Rows[0].Merge(tmmsm96_1);
		if (bcls_MM0099.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&bcls_MM0099, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		
		Log::Trace("", __FUNCTION__, "RECEIVE_WEIGHT[{0}] v_operate[{1}] ", tmmsm01["RECEIVE_WEIGHT"].ToString(), v_operate);
		if (v_operate == "I" || v_operate == "U")
		{
			//名义规格与实际规格保持一致
			tmmsm96["MAT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
			tmmsm96["MAT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
			tmmsm96["MAT_THICK"] = tmmsm39["CUT_AFTER_THICK"];

			//画面切废，规格取切后长宽厚
			tmmsm96["MAT_ACT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
			tmmsm96["MAT_ACT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
			tmmsm96["MAT_ACT_THICK"] = tmmsm39["CUT_AFTER_THICK"];
			tmmsm96["MAT_WT"] = tmmsm39["CUT_AFTER_WT"];//切后重量
			tmmsm96["MAT_ACT_WT"] = tmmsm39["CUT_AFTER_WT"];//系统重量即实际重量
			//tmmsm96["REAL_TIME_WT"] = tmmsm39["CUT_AFTER_WT"];//实时重量
			tmmsm96["QUALIFIED_WT"] = tmmsm39["CUT_AFTER_WT"];//合格产量

			tmmsm96["EVENT_ID"] = "MM37";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "f_mmsm39_proc";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "钢坯切废实绩";

		}
		else if (v_operate == "D")//当为删除时，需将切前长宽厚和重量赋给01表重量   删除校验添加在了前面
		{
			//名义规格与实际规格保持一致
			tmmsm96["MAT_WIDTH"] = tmmsm39["CUT_BEFORE_WIDTH"];
			tmmsm96["MAT_LEN"] = tmmsm39["CUT_BEFORE_LEN"];
			tmmsm96["MAT_THICK"] = tmmsm39["CUT_BEFORE_THICK"];

			//画面切废，规格取切后长宽厚
			tmmsm96["MAT_ACT_WIDTH"] = tmmsm39["CUT_BEFORE_WIDTH"];
			tmmsm96["MAT_ACT_LEN"] = tmmsm39["CUT_BEFORE_LEN"];
			tmmsm96["MAT_ACT_THICK"] = tmmsm39["CUT_BEFORE_THICK"];
			tmmsm96["MAT_WT"] = tmmsm39["CUT_BEFORE_WT"];//切前重量
			tmmsm96["MAT_ACT_WT"] = tmmsm39["CUT_BEFORE_WT"];//系统重量即实际重量
			//tmmsm96["REAL_TIME_WT"] = tmmsm39["CUT_BEFORE_WT"];//实时重量
			tmmsm96["QUALIFIED_WT"] = tmmsm39["CUT_BEFORE_WT"];//合格产量

			tmmsm96["EVENT_ID"] = "MM39";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "f_mmsm39_proc";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "钢坯切废实绩删除";
		}

		bcls_rec->Tables.Add("MM0099");
		bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		if (tmmsm39["CUT_SCRAP_WT"].ToDecimal() != 0)
		{
			bcls_rec->Tables["MM0099"].Rows.Add();

			bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
		}
		Log::Info("", __FUNCTION__, "AEGQERGERG=[{0}]", tmmsm39["CUT_SCRAP_WT"].ToDecimal(), bcls_rec->Tables["MM0099"].Rows.get_Count());

		//bcls_rec->Tables["MM0099"].Rows.Add();
		//bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);

		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
		{
			//当未封锁时正常处理，当处于封锁时，暂时不处理，存入待办事项表，封锁释放时统一处理
			//切废封锁取消,不存入待办事项	2024.06.25
			//if (tmmsm01["HOLD_FLAG"].ToString() == "0")
			//{
				//头尾坯切头尾时走物料同步
				if ((v_mat_no.Substring(8, 2) == "00" || v_mat_no.Substring(8, 2) == "99" || v_mat_no.Substring(8, 2) == "AA" || v_mat_no.Substring(8, 2) == "ZZ")
					&& (tmmsm39["CUTTING_TYPE"].ToString().Trim() == "8" || tmmsm39["CUTTING_TYPE"].ToString().Trim() == "9")&&(tmmsm01["MEND_FLAG"].ToString().Trim()==""|| tmmsm01["MEND_FLAG"].ToString().Trim() == "0"))
				{
					bcls_rec->Tables["MM0099"].Rows.Clear();
					
					//名义规格与实际规格保持一致
					tmmsm96["MAT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
					tmmsm96["MAT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
					tmmsm96["MAT_THICK"] = tmmsm39["CUT_AFTER_THICK"];

					//画面切废，规格取切后长宽厚
					tmmsm96["MAT_ACT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
					tmmsm96["MAT_ACT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
					tmmsm96["MAT_ACT_THICK"] = tmmsm39["CUT_AFTER_THICK"];

					if (true)
					{
						CDecimal v_code_wt = 0;//计算重量的系数

						/*if (tmmsm96["ST_NO"].ToString().Trim().Substring(0, 3) == "1A6")
						{
							v_code_wt = 7.95;
						}
						else if (tmmsm96["ST_NO"].ToString().Trim().Substring(0, 3) == "1A9")
						{
							v_code_wt = 7.95;
						}
						else if (tmmsm96["ST_NO"].ToString().Trim().Substring(0, 2) == "1D")
						{
							v_code_wt = 7.8;
						}
						else if (tmmsm96["ST_NO"].ToString().Trim().Substring(0, 1) == "1")
						{
							v_code_wt = 7.9;
						}
						else if (tmmsm96["ST_NO"].ToString().Trim().Substring(0, 1) == "2")
						{
							v_code_wt = 7.85;
						}
						else if (tmmsm96["ST_NO"].ToString().Trim().Substring(0, 1) == "3")
						{
							v_code_wt = 7.85;
						}*/

						doFlag = f_mmsm_get_density(tmmsm96["ST_NO"].ToString(), v_code_wt,conn);

						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}

						tmmsm96["PRODUTE_CAL_WT"] = ((tmmsm96["MAT_ACT_WIDTH"].ToDecimal() / 1000) * (tmmsm96["MAT_ACT_LEN"].ToDecimal() / 1000) * (tmmsm96["MAT_ACT_THICK"].ToDecimal() / 1000) * v_code_wt).Round(3);

					}

					
					Log::Trace("", __FUNCTION__, "tmmsm01[MAT_NO].ToString()	= [{0}]", tmmsm01["RECV_MAT_TIME"].ToString());
					if (tmmsm01["RECV_MAT_TIME"].ToString().Trim() != ""
						&& tmmsm01["RECV_MAT_TIME"].ToString().SubstringNE(0, 6) < datetime.SubstringNE(0, 6)
						&& tmmsm01["RECEIVE_WEIGHT"].ToDecimal() != tmmsm39["CUT_AFTER_WT"].ToDecimal())
					{
						strcpy(s.msg, "跨月收货的头尾坯，‘切头/切尾’不可修改切后重量(t)!");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				

					tmmsm96["MAT_WT"] = tmmsm39["CUT_AFTER_WT"];//切后重量
					tmmsm96["MAT_ACT_WT"] = tmmsm39["CUT_AFTER_WT"];//系统重量即实际重量
					tmmsm96["QUALIFIED_WT"] = tmmsm39["CUT_AFTER_WT"];//合格产量
					tmmsm96["MAT_NO"] = tmmsm39["MAT_NO"];
					tmmsm96["RECEIVE_WEIGHT"] = tmmsm96["MAT_ACT_WT"];
					tmmsm96["EVENT_ID"] = "MM03";
					tmmsm96["EVENT_LINE_TYPE"] = "00";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["FUNC_ID"] = "mmsm39f3_pro";
					tmmsm96["EVENT_DESC"] = "改切头尾坯且为切头切尾时走物料同步";

					bcls_rec->Tables["MM0099"].Rows.Add();
					bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);



				}
				else if (v_operate == "I")
				{

					bcls_rec->Tables["MM0099"].Rows.Clear();
					tmmsm96.Reset();
					tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["MAT_NO"] = tmmsm39["MAT_NO"];
					tmmsm96["RCV_MAT_FLAG"] = "W";
					tmmsm96["FINISH_FLAG"] = "1";
					tmmsm96["EVENT_ID"] = "MM3F";
					tmmsm96["EVENT_LINE_TYPE"] = "SM";
					tmmsm96["FUNC_ID"] = "mmsm39f3_pro";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["EVENT_DESC"] = "改切新增处理等待反馈";

					bcls_rec->Tables["MM0099"].Rows.Add();
					bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);

					tmmsm3e.CopyFrom(tmmsm96);
					doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;

					tmmsm3e["PROD_TIME"] = datetime;
					tmmsm3e["REMARK"] = "改切新增处理等待反馈";
					tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
					tmmsm3e.Insert();

				}
				else if (v_operate == "D")
				{

					bcls_rec->Tables["MM0099"].Rows.Clear();
					tmmsm96.Reset();
					tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["MAT_NO"] = tmmsm39["MAT_NO"];
					tmmsm96["RCV_MAT_FLAG"] = "W";
					tmmsm96["FINISH_FLAG"] = "3";
					tmmsm96["EVENT_ID"] = "MM3F";
					tmmsm96["EVENT_LINE_TYPE"] = "SM";
					tmmsm96["FUNC_ID"] = "mmsm39f3_pro";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["EVENT_DESC"] = "改切删除处理等待反馈";

					bcls_rec->Tables["MM0099"].Rows.Add();
					bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);

					tmmsm3e.CopyFrom(tmmsm96);
					doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;

					tmmsm3e["PROD_TIME"] = datetime;
					tmmsm3e["REMARK"] = "改切删除处理等待反馈";
					tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
					tmmsm3e.Insert();
				}

				doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			//}
			//else
			//{
			//	tmmsm33dbsx["MAT_NO"] = tmmsm39["MAT_NO"];
			//	tmmsm33dbsx["EVENT_ID"] = "MM37";
			//	tmmsm33dbsx["RESUME_SEQ_NO"] = tmmsm39["RESUME_SEQ_NO"];
			//	int dbsx_count = tmmsm33dbsx.QueryCount("MAT_NO,EVENT_ID");
			//	if (dbsx_count != 0)//为防止现场在封锁状态下多次操作改切，故改切在封锁下只允许操作一次
			//	{
			//		strcpy(s.sysmsg, "该材料已做改切，目前状态为等待处理，等待解除封锁后再操作改切！");
			//		strcpy(s.msg, s.sysmsg);
			//		throw CApplicationException(-1, s.msg, log.Location);

			//	}
			//	tmmsm33dbsx["SEQ_NO"] = dbsx_count + 1;
			//	tmmsm33dbsx.Insert();
			//}
		}


		bcls_rec_210044.Tables[0].Rows[0].Merge(tmmsm39);

		if (v_operate == "I")
		{
			bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "N";//新增
		}
		else if (v_operate == "D")
		{
			bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "D";//删除
			bcls_rec_210044.Tables[0].Rows[0]["CUT_SCRAP_WT"] = v_cut_scrap_wt;//发电文时传切废撤销前的切废量
		}
		//改切的头尾坯(切割类型为切头切尾)可以做改切记录，但是不发改切实绩电文
		//切废重量为0时，也不发改切实绩电文		2024.06.20
		//不管切不切废都发实绩电文    20224.9.9
		//切废封锁取消	2024.06.25
		/*if (tmmsm01["HOLD_FLAG"].ToString() == "0")
		{*/
			if ((v_mat_no.Substring(8, 2) == "00" || v_mat_no.Substring(8, 2) == "99" || v_mat_no.Substring(8, 2) == "AA" || v_mat_no.Substring(8, 2) == "ZZ")
				&& (tmmsm39["CUTTING_TYPE"].ToString().Trim() == "8" || tmmsm39["CUTTING_TYPE"].ToString().Trim() == "9") && (tmmsm01["MEND_FLAG"].ToString().Trim() == "" || tmmsm01["MEND_FLAG"].ToString().Trim() == "0"))
			{

			}
			else
			{
				if (tmmsm39["CUT_SCRAP_WT"].ToDecimal() != 0)
				{
					/********   太钢定制 发送L4电文 钢坯切废实绩   ***********/
					doFlag = f_mmsm_210044_snd(&bcls_rec_210044, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				
			}

		//}




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
