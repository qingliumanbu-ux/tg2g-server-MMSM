/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2015-08-13
Version:1.0
Description: 炼钢板坯分段实绩
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/





/* ***** 静态函数申明 ***** */

int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_210034_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
/*<remark>=========================================================
/// <summary>
/// 炼钢板坯分段实绩
/// <para>
/// 炼钢板坯分段实绩---大批量  苹果钢
/// </para>
/// </summary>
/// 当坯子处于封锁状态时，用户仍可操作，但不对主档表更新，不发送电文给产销
/// 存入待办事项表中，等待封锁取消后，统一处理
///
/// <param name="tmmsm35">分切实绩录入</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm35f7_cut)

int f_mmsm35f7_cut(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CDecimal cutNum = 0;
	CDecimal matTheoryWt = 0;
	int blkNum = 0;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_resume_seq_no = "";
	CString v_prod_shift_no = "";
	CString v_prod_shift_group = "";
	CModel tmmsm35mp("TMMSM35MP");//母坯表
	CModel tmmsm35("TMMSM35");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm01_slab("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm35_1("TMMSM35_1");
	CModel tmmsm3e("TMMSM3E");//反馈表
	CModel tmmsm33dbsx("TMMSM33DBSX");

	try
	{


		if (bcls_rec->Tables.Contains("MM0099") == false)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["MM0099"].Rows.Add();
		}

		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM15";
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm35f6_cut";


		EIClass bcls_rec_QM02;//材料表面判定
		bcls_rec_QM02.Tables[0].set_TableName("MM0099");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_CODE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_MAKER");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_TIME");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "DEFECT_CODE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SLAB_PLACE_CODE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SPARE_ITEM_0");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "MACH_CLEAR_FLAG");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SIZE_JUDGE_CODE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SIZE_DECIDE_CODE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SLAB_CHECK_RESULT");

		EIClass bcls_rec_QM18;//材料质量释放
		bcls_rec_QM18.Tables[0].set_TableName("MM0099");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "REL_REMARK");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "REL_MAKER");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "REL_TIME");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "DEFECT_CODE");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");

		EIClass bcls_rec_210034;//发送L4二切实绩电文
		bcls_rec_210034.Tables[0].set_TableName("210034");
		bcls_rec_210034.Tables[0].Columns.Add(tmmsm01);
		bcls_rec_210034.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");





		tmmsm35.MergeFrom(bcls_rec->Tables["TMMSM35MP"].Rows[0]);
		/* ***** 打印输入参数 ***** */
		EDLog(1, 1, "********************输出传入数据开始*******************");
		tmmsm35.Print();
		EDLog(1, 1, "********************输出传入数据结束*******************");

		tmmsm01["MAT_NO"] = tmmsm35["IN_MAT_NO"];
		tmmsm01.Query();
		tmmsm01_slab["MAT_NO"] = tmmsm35["IN_MAT_NO"];
		tmmsm01_slab.Query();
		tmmsm35mp.CopyFrom(tmmsm01);

		if (tmmsm35mp.QueryCount("MAT_NO"))
		{
			tmmsm35mp.Update("*", "MAT_NO");
		}
		else
		{
			tmmsm35mp.Insert();
		}

		/*if (tmmsm01["HOLD_FLAG"].ToString() != "0")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}处于封锁状态,不允许分段处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//N 未收货  W等待(等L4的反馈)  S收货成功   当未收货成功时，不允许二切  mfj   20231120  因接口问题，暂定W，等反馈状态就可进行分切   后续接口完善再更改  mfj  20240118
		/*if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}未收货,不允许分段处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

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
		/*if (tmmsm01["FINISH_FLAG"].ToString().Trim() == "1" &&tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已做改切，当前状态为等待制造管理系统反馈,暂不允许再进行该操作", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() != "1")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}未综判,不允许分段处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm01["LOGISTICS_STATUS"].ToString().Trim() != "0" && tmmsm01["LOGISTICS_STATUS"].ToString().Trim() != "4")
		{
			/*CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已装车，不允许分段处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);*/
		}
		if (tmmsm01["C_STATESIGN"].ToString().Trim() != "0"&& tmmsm01["C_STATESIGN"].ToString().Trim() != ""
			&& tmmsm01["C_STATESIGN"].ToString().Trim() != "6")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已不在现场，不允许分段处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm35.QueryCount("IN_MAT_NO") > 0)
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "该材料{0}已分切过，请先将分切后的子坯数据 分切撤销 后再进行分切操作！", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsm33dbsx["MAT_NO"] = tmmsm01["MAT_NO"];
		if (tmmsm33dbsx.QueryCount("MAT_NO") > 0)
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "该材料{0}有未完成的代办事项，请将代办事项处理完 后再进行分切操作！", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		matTheoryWt = tmmsm01["MAT_THEORY_WT"].ToDecimal().Round(3);


		//太钢定制，允许切合同材
		/*if (tmmsm01["ORDER_NO"].ToString().Trim() != "")
		{
		CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
		CMessageFormat::Format(s.msg, "材料{0}为合同材,不允许分段处理", arguments, 1);
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		cutNum = bcls_rec->Tables["TMMSM35MP"].Rows[0]["CUT_NUM"].ToDecimal();

		if (tmmsm35["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm35["PROD_SHIFT_GROUP"].ToString().Trim() == "")
		{
			f_epep_get_shift_group("SMCP", dateNow, v_prod_shift_no, v_prod_shift_group, conn);
			tmmsm35["PROD_SHIFT_NO"] = v_prod_shift_no;
			tmmsm35["PROD_SHIFT_GROUP"] = v_prod_shift_group;
		}

		tmmsm35["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmssmsff").SubstringNE(0, 18);
		tmmsm35["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tmmsm35["PROD_TIME"] = tmmsm35["REC_CREATE_TIME"];
		tmmsm35["PROD_MAKER"] = s.userid;
		tmmsm35["REC_CREATOR"] = s.userid;
		tmmsm35["HEAT_NO"] = tmmsm01["HEAT_NO"];
		tmmsm35["PONO"] = tmmsm01["PONO"];
		tmmsm35["SG_SIGN"] = tmmsm01["SG_SIGN"];
		tmmsm35["ST_NO"] = tmmsm01["ST_NO"];
		tmmsm35["MAT_THICK"] = tmmsm01["MAT_ACT_THICK"];
		tmmsm35["MAT_WIDTH"] = tmmsm01["MAT_ACT_WIDTH"];
		tmmsm35["IN_HEAT_NO"] = tmmsm35["HEAT_NO"];
		tmmsm35["IN_PONO"] = tmmsm35["PONO"];
		tmmsm35["IN_SG_SIGN"] = tmmsm35["SG_SIGN"];
		tmmsm35["IN_ST_NO"] = tmmsm35["ST_NO"];
		tmmsm35["IN_MAT_THICK"] = tmmsm35["MAT_THICK"];
		tmmsm35["IN_MAT_WIDTH"] = tmmsm35["MAT_WIDTH"];
		tmmsm35["IN_BATCH"] = tmmsm01["BATCH"];
		tmmsm35["IN_PRINT_NO"] = tmmsm35["PRINT_NO"];
		Log::Trace("", __FUNCTION__, "LINE", __LINE__);
		tmmsm35["OP_DIV"] = "1";  //1：铸坯分段；2：方坯拆批；3：方坯并批
		//f_epep_get_shift_group("SM", tmmsm35["REC_CREATE_TIME"].ToString(), tmmsm35["PROD_SHIFT_NO"].ToString(), tmmsm35["PROD_SHIFT_GROUP"].ToString(), conn);
		Log::Trace("", __FUNCTION__, "LINE", __LINE__);
		for (int i = 1; i <= cutNum; i++)
		{
			tmmsm35["MAT_NO"] = bcls_rec->Tables["TMMSM35"].Rows[i-1]["MAT_NO"].ToString();
			tmmsm35["MAT_LEN"] = bcls_rec->Tables["TMMSM35"].Rows[i-1]["MAT_LEN"].ToDecimal();
			tmmsm35["MAT_TUBE"] = bcls_rec->Tables["TMMSM35"].Rows[i - 1]["MAT_NUM"].ToDecimal();
			tmmsm35["BATCH"] = bcls_rec->Tables["TMMSM35"].Rows[i - 1]["BATCH"].ToString();
			tmmsm35["PRINT_NO"] = bcls_rec->Tables["TMMSM35"].Rows[i - 1]["PRINT_NO"].ToString().TrimOrBlank();
			tmmsm35["MAT_WT"] = bcls_rec->Tables["TMMSM35"].Rows[i - 1]["MAT_WT"].ToDecimal().Round(3);
			Log::Trace("", __FUNCTION__, "LINE", __LINE__);
			if (tmmsm35.QueryCount("BATCH") > 0)
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}批次号已被其他子坯使用，请重新操作一遍分切，重新获取最新批次号！", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm35.Insert();

			tmmsm35_1.CopyFrom(tmmsm35);
			tmmsm35_1.TrimOrBlank();
			tmmsm35_1.Insert();//记录最初始履历
			Log::Trace("", __FUNCTION__, "LINE", __LINE__);
			tmmsm01["MAT_NO"] = tmmsm35["MAT_NO"];
			tmmsm01["BATCH"] = tmmsm35["BATCH"];
			tmmsm01["PRINT_NO"] = tmmsm35["PRINT_NO"];
			tmmsm01["IN_MAT_NO"] = tmmsm35["IN_MAT_NO"];
			tmmsm01["MAT_ACT_LEN"] = tmmsm35["MAT_LEN"];
			tmmsm01["MAT_LEN"] = tmmsm35["MAT_LEN"];
			tmmsm01["MAT_NUM"] = tmmsm35["MAT_TUBE"];
			tmmsm01["MAT_TUBE"] = tmmsm35["MAT_TUBE"];
			tmmsm01["MAT_ACT_WT"] = tmmsm35["MAT_WT"].ToDecimal().Round(3);
			tmmsm01["MAT_WT"] = tmmsm35["MAT_WT"].ToDecimal().Round(3);
			tmmsm01["RCV_MAT_FLAG"] = "S";
			tmmsm01["RECEIVE_WEIGHT"] = 0;
			//tmmsm01["RECEIVE_WEIGHT"] = tmmsm35["MAT_WT"];//暂定 收货重量取切后重量，  后面待确认  mfj  20240131   收货重量为0，杨姐提出  mfj 20240319
			//tmmsm01["REAL_TIME_WT"] = tmmsm35["MAT_WT"];//实时重量
			tmmsm01["QUALIFIED_WT"] = tmmsm35["MAT_WT"].ToDecimal().Round(3);//合格产量
			tmmsm01["MAT_THEORY_WT"] = matTheoryWt / tmmsm35["IN_MAT_TUBE"].ToDecimal() / tmmsm35["IN_MAT_LEN"].ToDecimal() * tmmsm35["MAT_TUBE"].ToDecimal() * tmmsm35["MAT_LEN"];
			tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"].ToDecimal().Round(3);
			//tmmsm01.Insert();
			Log::Trace("", __FUNCTION__, "LINE", __LINE__);
			tmmsm01["PONO_SLAB"] = " ";
			for (int j = 1; j <= 12; j++)
			{
				tmmsm01["PONO_SLAB_" + CConvert::ToString(j)] =" ";
			}
			tmmsm01["RECEIVE_WEIGHT"] = 0;//收货重量
			//将修磨的数据不继承母坯
			tmmsm01["MEND_BEFORE_WEIGHT"] = 0; //磨前量
			tmmsm01["MEND_AFTER_WEIGHT"] = 0;//
			//tmmsm01["MEND_FLAG"] = "0";	//修磨标记  继承，子坯不做修磨  mfj  20240520
			tmmsm01["MEASURE_WT"] = 0;	//称重量
			tmmsm01["REAL_TIME_WT"] = 0;//实时重量
			tmmsm01["ORDER_NO"] = " ";//合同号
			tmmsm01["PRINT_NO"] = " ";//喷印号 
			tmmsm01["PONO_SLAB"] = " ";
			//将命令坯置空
			tmmsm01["LOGISTICS_STATUS"] = "0";
			tmmsm01["PRE_LOAD_FLAG"] = "0";
			tmmsm01["FACTORY_TO"] = " ";
			tmmsm01["DST_STOCK_CODE"] = " ";
			tmmsm01["UNLOAD_CODE"] = " ";
			Log::Trace("", __FUNCTION__, "LINE", __LINE__);

			tmmsm01["LSLAB_NO"] = " ";//长坯号-虚拟板坯号
			tmmsm01["FIX_SLAB_NUM"] = 0;
			//tmmsm01["SLAB_TYPE_OLD"] = "3";
			//20240821  LZ  上线后倒灌回上线前的板坯号不足20位
			tmmsm01["SLAB_NO"] = tmmsm01["SLAB_NO"].ToString().SubstringNE(0, 14) + tmmsm01["BATCH"].ToString().Substring(8, 2)
				+ tmmsm01["SLAB_NO"].ToString().SubstringNE(16);

			if (tmmsm35["BATCH"].ToString().Trim() == tmmsm35["IN_MAT_NO"].ToString().Trim())
			{
				tmmsm01["RECEIVE_WEIGHT"] = tmmsm01_slab["RECEIVE_WEIGHT"];
				tmmsm01["MEND_BEFORE_WEIGHT"] = tmmsm01_slab["MEND_BEFORE_WEIGHT"];
				tmmsm01["MEND_AFTER_WEIGHT"] = tmmsm01_slab["MEND_AFTER_WEIGHT"];
			}
			Log::Trace("", __FUNCTION__, "LINE", __LINE__);
			bcls_rec->Tables["MM0099"].Rows.Clear();
			tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM15";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "mmsm37_cut";
			tmmsm96["EVENT_DESC"] = "材料分切产出";
			tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);

			//未封锁时，正常处理。封锁时，不处理，只存母坯信息进待办事项表
			if (tmmsm01["HOLD_FLAG"].ToString() == "0")
			{

				Log::Trace("", __FUNCTION__, "LINE", __LINE__);
				

				//不处理，对物料做处理统一放在反馈里处理
				if (false)
				{
					doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else
			{

			}
			Log::Trace("", __FUNCTION__, "LINE", __LINE__);
			bcls_rec_QM02.Tables[0].Rows.Add();
			bcls_rec_QM02.Tables[0].Rows[i - 1]["EVENT_ID"] = "QM02";
			bcls_rec_QM02.Tables[0].Rows[i - 1]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_QM02.Tables[0].Rows[i - 1]["SYSTEM_ID"] = "MMSM";
			bcls_rec_QM02.Tables[0].Rows[i - 1]["FUNC_ID"] = "mmsm37_cut";
			bcls_rec_QM02.Tables[0].Rows[i - 1]["MAT_NO"] = tmmsm01["MAT_NO"];
			bcls_rec_QM02.Tables[0].Rows[i - 1]["SURFACE_DECIDE_CODE"] = "1";// 1:合格
			bcls_rec_QM02.Tables[0].Rows[i - 1]["SIZE_DECIDE_CODE"] = "1001";// 1:合格
			bcls_rec_QM02.Tables[0].Rows[i - 1]["SLAB_CHECK_RESULT"] = "1001";// 1:合格
			bcls_rec_QM02.Tables[0].Rows[i - 1]["SURFACE_DECIDE_MAKER"] = s.userid;
			bcls_rec_QM02.Tables[0].Rows[i - 1]["SURFACE_DECIDE_TIME"] = dateNow;
			bcls_rec_QM02.Tables[0].Rows[i - 1]["DEFECT_CODE"] = " ";
			bcls_rec_QM02.Tables[0].Rows[i - 1]["DEFECT_CLASS"] = " ";
			bcls_rec_QM02.Tables[0].Rows[i - 1]["SLAB_PLACE_CODE"] = tmmsm01["SLAB_PLACE_CODE"];
			bcls_rec_QM02.Tables[0].Rows[i - 1]["SPARE_ITEM_0"] = "炼钢分切，自动表判合格。";
			bcls_rec_QM02.Tables[0].Rows[i - 1]["MACH_CLEAR_FLAG"] = "";
			bcls_rec_QM02.Tables[0].Rows[i - 1]["SIZE_JUDGE_CODE"] = "1";//合格
			Log::Trace("", __FUNCTION__, "LINE", __LINE__);
			bcls_rec_QM18.Tables[0].Rows.Add();
			bcls_rec_QM18.Tables[0].Rows[i - 1]["EVENT_ID"] = "QM18";
			bcls_rec_QM18.Tables[0].Rows[i - 1]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_QM18.Tables[0].Rows[i - 1]["SYSTEM_ID"] = "MMSM";
			bcls_rec_QM18.Tables[0].Rows[i - 1]["FUNC_ID"] = "mmsm37_cut";
			bcls_rec_QM18.Tables[0].Rows[i - 1]["MAT_NO"] = tmmsm01["MAT_NO"];
			bcls_rec_QM18.Tables[0].Rows[i - 1]["REL_REMARK"] = "分切释放";
			bcls_rec_QM18.Tables[0].Rows[i - 1]["REL_MAKER"] = s.userid;
			bcls_rec_QM18.Tables[0].Rows[i - 1]["REL_TIME"] = dateNow;
			bcls_rec_QM18.Tables[0].Rows[i - 1]["DEFECT_CODE"] = " ";
			bcls_rec_QM18.Tables[0].Rows[i - 1]["DEFECT_CLASS"] = " ";

			Log::Trace("", __FUNCTION__, "LINE", __LINE__);
			//每块子坯单独发送电文   不然成本抛账会有问题   mfj  李振  20240528
			bcls_rec_210034.Tables[0].Rows.Add();
			bcls_rec_210034.Tables[0].Rows[i - 1].Merge(tmmsm01);
			bcls_rec_210034.Tables[0].Rows[i - 1]["MAT_NO"] = tmmsm01["MAT_NO"];
			bcls_rec_210034.Tables[0].Rows[i - 1]["DEAL_FLAG"] = "N";//新增
			

		}

		Log::Trace("", __FUNCTION__, "LINE", __LINE__);
		
		//未封锁时，正常处理。封锁时，不处理，只存母坯信息进待办事项表
		if (tmmsm01["HOLD_FLAG"].ToString() == "0")
		{

			//调用事件  只改标记 在反馈里集中处理
			bcls_rec->Tables["MM0099"].Rows.Clear();
			bcls_rec->Tables["MM0099"].Rows.Add();
			bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm01_slab);
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM3F";
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
			bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
			bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm35f7_cut";
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm35["IN_MAT_NO"];
			bcls_rec->Tables["MM0099"].Rows[0]["RCV_MAT_FLAG"] = "W";
			bcls_rec->Tables["MM0099"].Rows[0]["DIV_FLAG"] = "1";

			tmmsm3e.MergeFrom(bcls_rec->Tables["MM0099"].Rows[0]);
			tmmsm3e["RECEIVE_BACK_STATUS"] = "W";//等待反馈
			tmmsm3e["PROD_TIME"] = datetime;
			tmmsm3e["REMARK"] = "批量分切处理等待反馈";
			doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
			tmmsm3e.Insert();

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//不处理，对物料做处理统一放在反馈里处理
			if (false)
			{
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM16";
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
				bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm35f6_cut";
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm35["IN_MAT_NO"];

				doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "LINE", __LINE__);

				Log::Trace("", __FUNCTION__, "------------事件QM02[表面判定]--------------");
				doFlag = f_mmsm99(&bcls_rec_QM02, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				Log::Trace("", __FUNCTION__, "------------事件QM18[质量释放]--------------");
				doFlag = f_mmsm99(&bcls_rec_QM18, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//发送电文点放在上面的循环里，不在这里触发  mfj
				/********   太钢定制 发送L4电文 二切实绩   ***********/
				

			}

			if (bcls_rec_210034.Tables[0].Rows.get_Count()>0)
			{
				doFlag = f_mmsm_210034_snd(&bcls_rec_210034, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			

		}
		else
		{
			tmmsm33dbsx["MAT_NO"] = tmmsm35["IN_MAT_NO"];
			tmmsm33dbsx["EVENT_ID"] = "MM16";
			tmmsm33dbsx["SEQ_NO"] = tmmsm33dbsx.QueryCount("MAT_NO") + 1;
			tmmsm33dbsx.Insert();
		}





		/*发送MMS电文:炼钢板坯分切实绩*/
#if defined(_SYS_PES)

		blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSMSND");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "IN_MAT_NO");

		}

		//--------向MMS送电文函数----------------

		bcls_rec->Tables["MMSMSND"].Rows.Clear();

		if (bcls_rec->Tables["MMSMSND"].Rows.get_Count() <= 0)
			bcls_rec->Tables["MMSMSND"].Rows.Add();
		bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM35";
		bcls_rec->Tables["MMSMSND"].Rows[0]["IN_MAT_NO"] = tmmsm35["IN_MAT_NO"];

		//doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}



#endif




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
