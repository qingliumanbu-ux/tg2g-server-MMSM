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
int f_mmsm_210048_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_get_density(CString ST_NO, CDecimal& MAT_DENSITY, CDbConnection* conn);//通过钢种计算密度

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
BM2F_ENTERACE(mmsm33c_pro)

int f_mmsm33c_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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



	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm38("TMMSM38");
	CModel tmmsm39("TMMSM39");

	CDbCommand cmd_inq(conn);


	try
	{
		if (bcls_rec->Tables.Contains("MM0099") == false)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}

		EIClass bcls_rec_210048;//发送L4二切实绩电文
		bcls_rec_210048.Tables[0].set_TableName("210048");
		bcls_rec_210048.Tables[0].Columns.Add(tmmsm01);
		bcls_rec_210048.Tables[0].Rows.Add();


		//获取代表成分熔炼号和备注
		if (bcls_rec->Tables["TMMSM38"].Columns.Contains("PROC_DIV"))
			v_proc_div = bcls_rec->Tables["TMMSM38"].Rows[0]["PROC_DIV"].ToString();


		tmmsm01.MergeFrom(bcls_rec->Tables["PARA"].Rows[0]);
		tmmsm01.Query("MAT_NO");

		if (bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_NO"].ToString().GetLength() < 9){
			sprintf(s.msg, "材料号输入长度不正确");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_NO"].ToString().Trim() != "")
			tmmsm01["MAT_NO"] = bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_NO"].ToString().Trim();
		if (bcls_rec->Tables["TMMSM38"].Rows[0]["BATCH"].ToString().Trim() != "")
			tmmsm01["BATCH"] = bcls_rec->Tables["TMMSM38"].Rows[0]["BATCH"].ToString().Trim();
		if (bcls_rec->Tables["TMMSM38"].Rows[0]["PRINT_NO"].ToString().Trim() != "")
			tmmsm01["PRINT_NO"] = bcls_rec->Tables["TMMSM38"].Rows[0]["PRINT_NO"].ToString().Trim();
		if (bcls_rec->Tables["TMMSM38"].Rows[0]["SM_PLAN_NO"].ToString().Trim() != "")
			tmmsm01["SM_PLAN_NO"] = bcls_rec->Tables["TMMSM38"].Rows[0]["SM_PLAN_NO"].ToString().Trim();
		if (bcls_rec->Tables["TMMSM38"].Rows[0]["HEAT_NO"].ToString().Trim() != "")
			tmmsm01["HEAT_NO"] = bcls_rec->Tables["TMMSM38"].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables["TMMSM38"].Rows[0]["ST_NO"].ToString().Trim() != "")
			tmmsm01["ST_NO"] = bcls_rec->Tables["TMMSM38"].Rows[0]["ST_NO"].ToString().Trim();
		if (bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_THICK"].ToDecimal() != 0)
			tmmsm01["MAT_THICK"] = bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_THICK"].ToDecimal();
		if (bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_WIDTH"].ToDecimal() != 0)
			tmmsm01["MAT_WIDTH"] = bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_WIDTH"].ToDecimal();
		if (bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_LEN"].ToDecimal() != 0)
			tmmsm01["MAT_LEN"] = bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_LEN"].ToDecimal();
		if (bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_ACT_WT"].ToDecimal() != 0)
			tmmsm01["MAT_WT"] = bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_ACT_WT"].ToDecimal();
		if (bcls_rec->Tables["TMMSM38"].Rows[0]["SLAB_HEAD_WIDTH"].ToDecimal() != 0)
			tmmsm01["SLAB_HEAD_WIDTH"] = bcls_rec->Tables["TMMSM38"].Rows[0]["SLAB_HEAD_WIDTH"].ToDecimal();
		if (bcls_rec->Tables["TMMSM38"].Rows[0]["SLAB_TAIL_WIDTH"].ToDecimal() != 0)
			tmmsm01["SLAB_TAIL_WIDTH"] = bcls_rec->Tables["TMMSM38"].Rows[0]["SLAB_TAIL_WIDTH"].ToDecimal();

		tmmsm01["MAT_ACT_THICK"] = tmmsm01["MAT_THICK"];
		tmmsm01["MAT_ACT_WIDTH"] = tmmsm01["MAT_WIDTH"];
		tmmsm01["MAT_ACT_LEN"] = tmmsm01["MAT_LEN"];
		tmmsm01["MAT_ACT_WT"] = tmmsm01["MAT_WT"];//实时重量
		tmmsm01["REAL_TIME_WT"] = tmmsm01["MAT_WT"];//实时重量
		tmmsm01["QUALIFIED_WT"] = tmmsm01["MAT_WT"];//合格产量
		tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_WT"];//理论重量

		//获取计算重量    
		//首先判断钢种前两位   系数  1A 7.86  1D 7.76   1F  7.83   1M 7.83
		//若以上判断获取不到，则判断钢种第一位   1 7.85  2 7.82  3 7.82
		if (true)
		{
			CDecimal v_code_wt = 0;//计算重量的系数

			/*if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 3) == "1A6")
			{
				v_code_wt = 7.95;
			}
			else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 3) == "1A9")
			{
				v_code_wt = 7.95;
			}
			else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 2) == "1D")
			{
				v_code_wt = 7.8;
			}
			else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 1) == "1")
			{
				v_code_wt = 7.9;
			}
			else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 1) == "2")
			{
				v_code_wt = 7.85;
			}
			else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 1) == "3")
			{
				v_code_wt = 7.85;
			}*/

			doFlag = f_mmsm_get_density(tmmsm01["ST_NO"].ToString(), v_code_wt,conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tmmsm01["PRODUTE_CAL_WT"] = ((tmmsm01["MAT_ACT_THICK"].ToDecimal() / 1000) * (tmmsm01["MAT_ACT_WIDTH"].ToDecimal() / 1000) * (tmmsm01["MAT_ACT_LEN"].ToDecimal() / 1000) * v_code_wt).Round(3);

		}

		tmmsm39.CopyFrom(tmmsm01);
		tmmsm38.CopyFrom(tmmsm01);
		tmmsm39["RESUME_SEQ_NO"] = bcls_rec->Tables["PARA"].Rows[0]["RESUME_SEQ_NO"].ToString().Trim();

#pragma region 
		tmmsm01["PREC_SLAB_NO"] = "";           /*预定板坯号*/
		tmmsm01["PONO_SLAB"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_1"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_2"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_3"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_4"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_5"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_6"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_7"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_8"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_9"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_10"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_11"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_12"] = " ";           /*命令板坯号*/
		Log::Trace("", __FUNCTION__, "11");
		tmmsm01["FIX_SLAB_NUM"] = 0;
		tmmsm01["LSLAB_NO"] = " ";
		tmmsm01["ORDER_NO"] = " ";
		tmmsm01["INITIAL_ORDER_NO"] = tmmsm01["ORDER_NO"];//初始合同号
		tmmsm01["REPAIR_FLAG"] = "0";                  /*返修标记C1*/
		tmmsm01["HOLD_FLAG"] = "0";                  /*封锁标记C1*/
		tmmsm01["SURFACE_DECIDE_CODE"] = "1";              /*表面判定代码C1*/
		tmmsm01["SURFACE_DECIDE_MAKER"] = " ";             /*表面判定责任者*/
		tmmsm01["PCH_JUDGE_CODE"] = "0";              /*性能判定代码C1*/
		tmmsm01["COMPLEX_DECIDE_CODE"] = "0";              /*综合判定代码C1*/
		tmmsm01["SLABTOP_FLAG"] = "0";              /*板坯TOP点确认标志*/

		tmmsm01["IN_FLAG"] = "0";              /*入库标记*/
		tmmsm01["TRANSFER_FLAG"] = "0";              /*转库计划标记*/
		tmmsm01["PRODUCT_PACK_FLAG"] = "0";              /*成品包装标志*/
		tmmsm01["CONFM_FLAG"] = "0";              /*准发确认标记*/
		tmmsm01["APP_DECIDE_FLAG"] = "0";              /*现货申报标记*/
		tmmsm01["STOCK_PLACE_NO"] = "GD";
		tmmsm01["COE_A"] = 0;
		tmmsm01["COE_B"] = 0;
		tmmsm01["RCV_MAT_FLAG"] = "W";//收货标记
		tmmsm01["RECV_MAT_TIME"] = datetime;//收货时刻
		tmmsm01["RECEIVE_WEIGHT"] = tmmsm01["MAT_ACT_WT"];//收货重量
		//余材原因
		tmmsm01["REMAINDER_REASON"] = " ";
		tmmsm01["HOLD_FLAG"] = "0";
		tmmsm01["RCV_MAT_FLAG"] = "N";  //收货标记  N 未收货
		tmmsm01["RECV_MAT_TIME"] = " ";
		tmmsm01["RECEIVE_WEIGHT"] = 0;
		tmmsm01["USAGE_DECISION"] = " ";
#pragma


		//根据钢种查询钢牌号，将钢牌号更新掉  工艺卡牌号  跟sg_sign不同，sg_sign从tpssm03表获取
		if (tmmsm01["ST_NO"].ToString() != "")
		{
			sqlstr = "SELECT  SG_GRADE_1,C_DIV  FROM TQMTS0X  WHERE ST_NO = '" + tmmsm01["ST_NO"].ToString().Trim() + "'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm01["SG_GRADE_1"] = cmd_inq.GetString(1);
				tmmsm01["C_DIV"] = cmd_inq.GetString(2);
			}
			//para_tmmsm01["ORDER_NO"] = " ";//脱合同，不抛合同跟踪  mfj 潘  20240410
			cmd_inq.Close();
		}

		//将不为12的碳锈区分，改为12
		if (tmmsm01["C_DIV"].ToString().Trim() != "")
		{
			if (tmmsm01["C_DIV"].ToString().Trim() == "4")
			{
				tmmsm01["C_DIV"] = "1";
			}

			if (tmmsm01["C_DIV"].ToString().Trim() == "3" || tmmsm01["C_DIV"].ToString().Trim() == "5")
			{
				tmmsm01["C_DIV"] = "2";
			}
		}

		//组批确认
		if (v_proc_div == "I")
		{
			tmmsm39["USE_LOGO"] = "1";//1为使用掉了
			tmmsm39.Update("USE_LOGO", "RESUME_SEQ_NO");

			if (tmmsm38.QueryCount("MAT_NO"))
			{
				strcpy(s.sysmsg, "该材料号已经存在，请重新确认！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm38["REC_CREATOR"] = s.userid;
			tmmsm38["REC_CREATE_TIME"] = datetime;
			tmmsm38.TrimOrBlank();
			tmmsm38.Insert();

			tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm96["EVENT_ID"] = "MM3D";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "mmsm33c_pro";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "板坯废品转正品";

			if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
			{
				bcls_rec->Tables["MM0099"].Rows.Add();
			}
			bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
		}
		else if (v_proc_div == "U")
		{
			tmmsm38["REC_REVISOR"] = s.userid;
			tmmsm38["REC_REVISE_TIME"] = datetime;
			tmmsm38.TrimOrBlank();
			tmmsm38.Update("*","MAT_NO");
			tmmsm01["MAT_NO"] = bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_NO"].ToString();
			
			/*tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM3D";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "mmsm33c_pro";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "铸坯";

			if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
			{
				bcls_rec->Tables["MM0099"].Rows.Add();
			}
			bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);*/


		}
		else if (v_proc_div == "D")
		{
			tmmsm39["USE_LOGO"] = "0";//0  为未使用
			tmmsm39.Delete("RESUME_SEQ_NO");

			tmmsm38["REC_REVISOR"] = s.userid;
			tmmsm38["REC_REVISE_TIME"] = datetime;
			tmmsm38.TrimOrBlank();
			tmmsm38.Update("*", "MAT_NO");
			tmmsm01["MAT_NO"] = bcls_rec->Tables["TMMSM38"].Rows[0]["MAT_NO"].ToString();

			/*tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM3D";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "mmsm33c_pro";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "铸坯废品转正品的撤销";

			if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
			{
				bcls_rec->Tables["MM0099"].Rows.Add();
			}
			bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);*/
		}
		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
		{

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}



		if (v_proc_div == "I" || v_proc_div == "U")
		{
			bcls_rec_210048.Tables[0].Rows[0].Merge(tmmsm01);
			bcls_rec_210048.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
			doFlag = f_mmsm_210048_snd(&bcls_rec_210048, bcls_ret, conn);
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
