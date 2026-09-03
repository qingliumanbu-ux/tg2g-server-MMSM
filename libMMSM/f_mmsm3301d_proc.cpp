/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-07-04
Description: 铸坯产出处理，建物料主档，抛材料申请
***********************************************************************/


/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/




int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_pssm03_prodflag_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_slab_dest(const CString& slab_plan_dest, CString& slab_dest_code, CDbConnection * conn);

#if defined _SYS_MMS || defined _SYS_MES    ////MMS层或MES系统部署时调用的函数
int f_pmom_slab_pct(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);   //预定板坯产出更新，只有产出及删除调用。(11)热轧板坯 (20)外卖板坯 (10)厚板板坯
int f_mmsm33_pmof(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);   //炼钢铸坯产出合同跟踪抛帐

#if defined _LINE_HR || defined _LINE_CR || defined _LINE_BW     
int f_pmog_order(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);    //生产模块的归并合同处理函数
#endif

#if defined _LINE_HP       //定义有厚板产线
int f_mmhp0009_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_pshp_dhcr_rept_return(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
#endif

#endif

#if defined _SYS_PES    //PES层部署时调用的函数
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);  //调用各模块通讯接口
#endif


#if defined(_WMS_DEPENDENT_SM)  && (defined _SYS_PES || defined _SYS_MES)
int f_wmxx_stock_out(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection* conn);	//仓库出库函数  
#endif


BM2_FUNCTION_EXPORT
int f_mmsm3301d_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	int pmof_count = 0;
	int mm0099_count = 0;
	int wm_count = 0;
	int pssm03_count = 0;
	int pmogord_count = 0;
	int slabpct_count = 0;
	int tmmsm33_ic_count = 0;
	int wm00que_count = 0;
	int mmhp0009_count = 0;
	CString   v_slab_dest_code = "";
	CString   v_proc_div = "";    //处理区分：N-新增; U-修改; D-删除; I-累计新增（相同材料分批新增）
	CString   v_order_type = "";
	CString   v_manage_flag = "";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	CModel tmmsm33("TMMSM33");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel twma0("TWMA0");

	try
	{
		#pragma region 数据块定义

		/*判断传入参数块是否存在*/
		blkNum = bcls_rec->Tables.IndexOf("TMMSM33");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 TMMSM33 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		/*物料跟踪履历用，f_mmsm99函数用*/
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}

		/*物料跟踪履历用，f_mmsm99 函数INSERT DELETE用*/
		blkNum = bcls_rec->Tables.IndexOf("MM0099_ID");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099_ID");
			bcls_rec->Tables["MM0099_ID"].Columns.Add(tmmsm96);
		}

		/*主档表用，f_mmsm01_icins函数用*/
		blkNum = bcls_rec->Tables.IndexOf("TMMSM33_IC");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("TMMSM33_IC");
			bcls_rec->Tables["TMMSM33_IC"].Columns.Add(tmmsm33);
		}

		/*抛合同跟踪，增加数据参数f_mmsm33_pmof*/
		blkNum = bcls_rec->Tables.IndexOf("PMOF");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PMOF");
			bcls_rec->Tables["PMOF"].Columns.Add(tmmsm96);
			bcls_rec->Tables["PMOF"].Columns.Add(DT_STRING, "WT");        //重量
			bcls_rec->Tables["PMOF"].Columns.Add(DT_STRING, "PREV_WT");      //前重量
		}

		EIClass bcls_rec_PSHP;//材料质量封锁
		bcls_rec_PSHP.Tables[0].set_TableName("PSHP");
		bcls_rec_PSHP.Tables[0].Columns.Add(DT_STRING, "PREC_SLAB_NO");
		bcls_rec_PSHP.Tables[0].Columns.Add(DT_STRING, "MAT_NO");


		/*厚板抛合同跟踪，增加数据参数*/
		blkNum = bcls_rec->Tables.IndexOf("MMHP0009");
		//Log::Trace("", __FUNCTION__, "blkNum = bcls_rec->Tables.IndexOf= [{0}]", blkNum);
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMHP0009");
			bcls_rec->Tables["MMHP0009"].Columns.Add(tmmsm96);
		}


		/*置板坯命令产出标志*/
		blkNum = bcls_rec->Tables.IndexOf("PSSM03");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PSSM03");
			bcls_rec->Tables["PSSM03"].Columns.Add(DT_STRING, "FACTORY_DIV");
			bcls_rec->Tables["PSSM03"].Columns.Add(DT_STRING, "SLAB_NO");       //预定板坯号
			bcls_rec->Tables["PSSM03"].Columns.Add(DT_DECIMAL, "SLAB_NUM");       //预定板坯号
			bcls_rec->Tables["PSSM03"].Columns.Add(DT_STRING, "SLAB_PROD_FLAG");//产出标记	
		}


		/*调用生产预定板坯产出函数用（MMS） f_pmom_slab_pct函数用*/
		blkNum = bcls_rec->Tables.IndexOf("SLABPCT");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("SLABPCT");
			bcls_rec->Tables["SLABPCT"].Columns.Add(DT_STRING, "PREC_SLAB_NO"); //产出板坯
			bcls_rec->Tables["SLABPCT"].Columns.Add(DT_STRING, "PCT_FLAG");   //产出标记 0：产出撤销 1：产出
		}

		/*调用归并合同处理函数用（MMS） f_pmog_order函数用*/
		blkNum = bcls_rec->Tables.IndexOf("PMOGORD");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PMOGORD");
			bcls_rec->Tables["PMOGORD"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["PMOGORD"].Columns.Add(DT_DECIMAL, "MAT_WT");
			bcls_rec->Tables["PMOGORD"].Columns.Add(DT_DECIMAL, "MAT_NUM");
			bcls_rec->Tables["PMOGORD"].Columns.Add(DT_STRING, "ORDER_NO");
		}


		#if defined(_WMS_DEPENDENT_SM)  && (defined _SYS_PES || defined _SYS_MES)
		/*调用仓库接口（PES）  f_ym00sm_queue函数用*/
		blkNum = bcls_rec->Tables.IndexOf("WM00QUE");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("WM00QUE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NUM");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "UNIT_CODE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");

		}
		bcls_rec->Tables["WM00QUE"].Rows.Clear();
		#endif

		blkNum = bcls_rec->Tables.IndexOf("WM00QUE");
		//Log::Trace("", __FUNCTION__, "blkNum = bcls_rec->Tables.WM00QUE= [{0}]", blkNum);
		#pragma endregion

		/* 获取输入参数*/
		bcls_rec->Tables["MM0099_ID"].Rows.Clear();
		bcls_rec->Tables["TMMSM33_IC"].Rows.Clear();
		bcls_rec->Tables["PMOF"].Rows.Clear();
		bcls_rec->Tables["MMHP0009"].Rows.Clear();
		bcls_rec->Tables["PSSM03"].Rows.Clear();
		bcls_rec->Tables["SLABPCT"].Rows.Clear();
		bcls_rec->Tables["PMOGORD"].Rows.Clear();

		#if defined(_WMS_DEPENDENT_SM)  && (defined _SYS_PES || defined _SYS_MES)
				blkNum = bcls_rec->Tables.IndexOf("WM_STOCK");
				if (blkNum <= 0)
				{
					bcls_rec->Tables.Add("WM_STOCK");
					bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_NO");
					bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_KIND");
					bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_LINE_TYPE");
					bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
					bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_NO");
					bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
					bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "ROWNO");
					bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "COLUMN_NO");
					bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_DECIMAL, "LAYERNO");
					bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_POSITION");
					bcls_rec->Tables["WM_STOCK"].Rows.Add();
				}
				bcls_rec->Tables["WM_STOCK"].Rows.Clear();
		#endif
		
		//Log::Trace("", __FUNCTION__, "begin= [{0}]", bcls_rec->Tables["TMMSM33"].Rows.get_Count());

		
		tmmsm33["SLAB_PLAN_DEST"] = bcls_rec->Tables["TMMSM33"].Rows[0]["SLAB_PLAN_DEST"].ToString();//去向

		//Log::Trace("", "", "tmmsm33["SLAB_PLAN_DEST"] ={0}", tmmsm33["SLAB_PLAN_DEST"].ToString());

		doFlag = f_mmsm_slab_dest(tmmsm33["SLAB_PLAN_DEST"].ToString(), v_slab_dest_code, conn);
		if (doFlag < 0 || v_slab_dest_code.Trim() == "")
		{
			sprintf(s.msg, "获取去向出错!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//Log::Trace("", "", "v_slab_dest_code={0}", v_slab_dest_code);

		bool colExist = bcls_rec->Tables["TMMSM33"].Columns.Contains("PROC_DIV");

		for (int i = 0; i < bcls_rec->Tables["TMMSM33"].Rows.get_Count(); i++){
			v_proc_div = colExist ? bcls_rec->Tables["TMMSM33"].Rows[i]["PROC_DIV"].ToString() : "D";
			tmmsm33.Reset();
			tmmsm33.MergeFrom(bcls_rec->Tables["TMMSM33"].Rows[i]);
			tmmsm33.TrimOrBlank();
			//tmmsm33.Print();

			//Log::Trace("", "", "v_proc_div={0}", v_proc_div);
			//Log::Trace("", "", "tmmsm33["MANAGE_FLAG"] ={0}", tmmsm33["MANAGE_FLAG"].ToString());
			cmd_inq.SetCommandText("SELECT CUT_FIN_FLAG FROM TPSSM11 WHERE HEAT_NO = @tmmsm33.HEAT_NO");
			cmd_inq.Parameters.Set("tmmsm33.HEAT_NO", tmmsm33["SLAB_PLAN_DEST"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				if (cmd_inq.GetString(1).Trim() == "1")
				{
					strcpy(s.sysmsg, "该炉次已切割完成");
					strcpy(s.msg, s.sysmsg); //用户提示信息，国际化
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			//else
			//{
			//	strcpy(s.sysmsg, "未找到该炉次切割完成标记,可能已炉次确定");
			//	strcpy(s.msg, s.sysmsg); //用户提示信息，国际化
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			cmd_inq.Close();
			#pragma region 获取实绩中命令板坯信息
			//bcls_rec->Tables["PSSM03"].Rows.Clear();
			//pssm03_count = 0;
			for (int j = 1; j <= 12; j++)
			{
				//Log::Trace("", __FUNCTION__, "pssm03_count000000 = [{0}]", pssm03_count);
				CString c_slab = bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)].ToString().Trim();
				//Log::Trace("", "", "c_slab={0},tmmsm33[{1}]", c_slab, tmmsm33["PONO_SLAB_1"].ToString());
				if (bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)].ToString().Trim() == "")
					break;
				bcls_rec->Tables["PSSM03"].Rows.Add();
				bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["FACTORY_DIV"] = tmmsm33["FACTORY_DIV"];
				bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["SLAB_NO"] = bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)];
				bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["SLAB_NUM"] = tmmsm33["MAT_TUBE"];
				//wzn_201709111130 获取命令信息并压入计划接口数据块
				if (v_proc_div == "N")
				{
					bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["SLAB_PROD_FLAG"] = "1";
				}
				else if (v_proc_div == "D")
				{
					bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["SLAB_PROD_FLAG"] = "0";
				}
				else if (v_proc_div == "U")
				{
					//
					bcls_rec->Tables["PSSM03"].Rows.Clear();
					break;
				}
					
				pssm03_count++;
			}
			if (i == bcls_rec->Tables["TMMSM33"].Rows.get_Count() - 1)
			{
				CString c_slab = "";
				CString flag = "";
				for (int k = 0; k < pssm03_count; k++)
				{
					c_slab = bcls_rec->Tables["PSSM03"].Rows[k]["SLAB_NO"];
					flag = bcls_rec->Tables["PSSM03"].Rows[k]["SLAB_PROD_FLAG"];
					//Log::Trace("", __FUNCTION__, "slab = [{0}],flag=[{1}]-[{2}]", c_slab, flag,k);
				}
			}

			//Log::Trace("", __FUNCTION__, "pssm03_count111111 = [{0}]", pssm03_count);
			
			#pragma endregion



			#pragma region 删除处理
			if (v_proc_div == "D")  //D-删除
			{
				//--------读取主档的材料信息------------------
				tmmsm01["MAT_NO"] = tmmsm33["MAT_NO"];
				//Log::Trace("", "", ">>>>>>>>>>>tmmsm01["MAT_NO"] = {0}", tmmsm01["MAT_NO"].ToString());
				sqlstr = "tmmsm01.Query()";
				bool isExist = tmmsm01.Query("MAT_NO");
				//校验要删除的材料是否还在，物料主档中获取材料信息。
				if (!isExist)
				{
					CFormattable arguments[] = { (const char*)tmmsm33["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, _RES("MMSMS0000135")/*材料[{0}]在物料主档中不存在。*/, arguments, 1);
					//sprintf(s.sysmsg, "材料[%s]在物料主档中不存在。", (const char*)tmmsm33["MAT_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				

				#pragma region MMS层生产模块处理

				#pragma region 调用生产预定板坯产出撤销函数，置产出标记
				//Log::Trace("", __FUNCTION__, "slabpct_count00000 = [{0}]", slabpct_count);

				
				for (int j = 1; j <= 12; j++)
				{
					if (bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)].ToString().Trim() == "")
						break;
					bcls_rec->Tables["SLABPCT"].Rows.Add();
					bcls_rec->Tables["SLABPCT"].Rows[slabpct_count]["PREC_SLAB_NO"] = bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)];
					bcls_rec->Tables["SLABPCT"].Rows[slabpct_count]["PCT_FLAG"] = "0";
					slabpct_count++;
				}
				//Log::Trace("", __FUNCTION__, "slabpct_count = [{0}]", slabpct_count);
				#pragma endregion

				#pragma region 调用生产合同跟踪抛帐
				if (tmmsm01["ORDER_NO"].ToString().Trim() != "")
				{
					//Log::Trace("", __FUNCTION__, "tmmsm01["MAT_DESTION"] = [{0}]", tmmsm01["MAT_DESTION"].ToString());
					if (v_slab_dest_code == "HP")
					{
						#if defined _LINE_HP       //定义有厚板产线
						//Log::Trace("", __FUNCTION__, "调用 f_mmhp0009_proc 1000 行 tmmsm01["MAT_NO"] = [{0}]", tmmsm33["MAT_NO"].ToString());

						bcls_rec->Tables["MMHP0009"].Rows.Add();
						bcls_rec->Tables["MMHP0009"].Rows[mmhp0009_count].Merge(tmmsm01);
						bcls_rec->Tables["MMHP0009"].Rows[mmhp0009_count]["EVENT_ID"] = "53SM";

						mmhp0009_count++;
						#endif

					}
					else
					{
						//Log::Trace("", __FUNCTION__, "tmmsm01["MAT_STATUS"] = [{0}]", tmmsm01["MAT_STATUS"].ToString());

						//判断如果是封锁后的产出撤销，需要先抛解封事件
						if (tmmsm01["MAT_STATUS"].ToString().Trim() == "21")
						{
							//Log::Trace("", __FUNCTION__, "调用 f_mmsm33_pmof 解封 tmmsm01["MAT_NO"] = [{0}]", tmmsm33["MAT_NO"].ToString());
							bcls_rec->Tables["PMOF"].Rows.Add();
							bcls_rec->Tables["PMOF"].Rows[pmof_count].Merge(tmmsm01);
							bcls_rec->Tables["PMOF"].Rows[pmof_count]["EVENT_ID"] = "59";  //59 --解封锁
							pmof_count++;
						}

						bcls_rec->Tables["PMOF"].Rows.Add();
						bcls_rec->Tables["PMOF"].Rows[pmof_count].Merge(tmmsm01);
						bcls_rec->Tables["PMOF"].Rows[pmof_count]["EVENT_ID"] = "53SM";  //53-材料产出撤销
						pmof_count++;
					}
				}
				#pragma endregion

				#pragma endregion

				#pragma region 主档删除
				//------------主档异动履历------------------
				//必须放在主档记录删除前
				bcls_rec->Tables["MM0099_ID"].Rows.Add();
				bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["EVENT_ID"] = "MM11";
				bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["EVENT_LINE_TYPE"] = "SM";
				bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["SYSTEM_ID"] = "MMSM";
				bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["FUNC_ID"] = "f_mmsm3301d_proc";
				bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["MAT_NO"] = tmmsm01["MAT_NO"];
				mm0099_count++;
				//Log::Trace("", "", ">>>>>>>>>>>主档删除  mm0099_count {0}", mm0099_count);
				#pragma endregion 

				//针对库业务类型为1Q回退的热送材料需要先校验是否入库，入库后才能删除，否则队列表里记录无法删除 add by wzf 20230212
				sqlstr =
					" SELECT * FROM TWMA0"
					" WHERE MAT_NO = @mat_no"
					" AND STOCK_OPER_ORDER ='1Q' ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_no", tmmsm01["MAT_NO"].ToString());
				//cmd_inq.Parameters.Set("stock_oper_order", twma0["STOCK_OPER_ORDER"].ToString().Substring(0, 1) + "%");
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					sprintf(s.msg, "材料号[%s]为热送回退的材料，需先入库才能删除!", (const char*)tmmsm01["MAT_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
			    }
				cmd_inq.Close();

				#pragma region 调用仓库接口，生成入库队列
				#if defined(_WMS_DEPENDENT_SM)  && (defined _SYS_PES || defined _SYS_MES)
				if (tmmsm01["HOT_SEND_FLAG"].ToString() == "0" && tmmsm01["IN_FLAG"].ToString() == "0")
				{
					twma0["MAT_NO"] = tmmsm01["MAT_NO"];
					twma0.Delete("MAT_NO");
				}

				if (tmmsm01["IN_FLAG"].ToString() == "1")
				{
					bcls_rec->Tables["WM_STOCK"].Rows.Add(); // 创建一行
					bcls_rec->Tables["WM_STOCK"].Rows[wm_count]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec->Tables["WM_STOCK"].Rows[wm_count]["MAT_KIND"] = "SM";
					bcls_rec->Tables["WM_STOCK"].Rows[wm_count]["MAT_LINE_TYPE"] = "SM";
					bcls_rec->Tables["WM_STOCK"].Rows[wm_count]["STOCK_OPER_ORDER"] = "2B";
					bcls_rec->Tables["WM_STOCK"].Rows[wm_count]["STOCK_NO"] = tmmsm01["STOCK_NO"];
					bcls_rec->Tables["WM_STOCK"].Rows[wm_count]["STOCK_PLACE_NO"] = " ";
					bcls_rec->Tables["WM_STOCK"].Rows[wm_count]["ROWNO"] = " ";
					bcls_rec->Tables["WM_STOCK"].Rows[wm_count]["COLUMN_NO"] = " ";
					bcls_rec->Tables["WM_STOCK"].Rows[wm_count]["LAYERNO"] = 0;
					bcls_rec->Tables["WM_STOCK"].Rows[wm_count]["STOCK_PLACE_POSITION"] = " ";
					wm_count++;
				}
				#endif  
				#pragma endregion

                #pragma region 厚板向合同跟踪
                #if defined _LINE_HP       //定义有厚板产线
                
				if (tmmsm01["HOT_CHARGE_FLAG"].ToString() == "2" && tmmsm01["MAT_DESTION"].ToString() == "18" && tmmsm01["PONO_SLAB_2"].ToString().Trim() == "")
				{

					bcls_rec_PSHP.Tables[0].Rows.Clear();
					bcls_rec_PSHP.Tables[0].Rows.Add();
					bcls_rec_PSHP.Tables[0].Rows[0]["PREC_SLAB_NO"] = tmmsm01["PONO_SLAB_1"];
					bcls_rec_PSHP.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];

					doFlag = f_pshp_dhcr_rept_return(&bcls_rec_PSHP, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

				}               
                #endif
                #pragma endregion

				tmmsm01["MAT_NO"] = tmmsm33["MAT_NO"];
				tmmsm01.Query("MAT_NO");

				/* 校验逻辑合法性 */
				if (tmmsm01["PLAN_NO"].ToString().Trim() != "")
				{
					sprintf(s.msg, "材料号[%s]在作业计划[%s]中,不能删除!", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["PLAN_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}//if 0-删除
			#pragma endregion

			#pragma region 向外部系统发送切割实绩
			#if defined(_SYS_PES)
			blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("MMSMSND");
				bcls_rec->Tables["MMSMSND"].Clone(bcls_rec->Tables["TMMSM33"]);
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			}
			bcls_rec->Tables["MMSMSND"].Rows.Clear();
			bcls_rec->Tables["MMSMSND"].Rows.Add();
			//Log::Trace("", __FUNCTION__, "调用 向外部系统发送切割实绩 [{0}],[{1}],[{2}]", bcls_rec->Tables["MMSMSND"].Rows.get_Count(),bcls_rec->Tables["TMMSM33"].Rows.get_Count(),i);

			bcls_rec->Tables["MMSMSND"].Rows[0].Merge(bcls_rec->Tables["TMMSM33"].Rows[i]);
			
			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			}
	
			bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM33";
			doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			#endif  //_SYS_PES
			#pragma endregion
			
		}

   
		

        #pragma region 调用生产接口
		#if defined _SYS_MMS || defined _SYS_MES 
		
		#pragma region 调用生产预定板坯产出撤销函数，置产出标记
		//Log::Trace("", "", " 调用生产预定板坯产出撤销函数 {0}", bcls_rec->Tables["SLABPCT"].Rows.get_Count());
		if (bcls_rec->Tables["SLABPCT"].Rows.get_Count() > 0)
		{

			doFlag = f_pmom_slab_pct(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#pragma endregion

        #pragma region 归并合同处理

		
        #if defined _LINE_HR || defined _LINE_CR || defined _LINE_BW
		//Log::Trace("", "", "bcls_rec->Tables[PMOGORD].Rows.get_Count()={0}", bcls_rec->Tables["PMOGORD"].Rows.get_Count());
		if (bcls_rec->Tables["PMOGORD"].Rows.get_Count() > 0)
		{
			doFlag = f_pmog_order(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		#endif
		#pragma endregion
	
		#pragma region 合同跟踪抛帐
		if (bcls_rec->Tables["PMOF"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm33_pmof(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#pragma endregion

		

		#pragma region 厚板向合同跟踪
		#if defined _LINE_HP       //定义有厚板产线
		if (bcls_rec->Tables["MMHP0009"].Rows.get_Count() > 0)
		{
			doFlag = f_mmhp0009_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#endif
		#pragma endregion

        #endif
        #pragma endregion

		#pragma region 调用计划接口，置命令板坯产出标记
		if (bcls_rec->Tables["PSSM03"].Rows.get_Count() > 0)
		{
			//命令铸坯置产出标记。其中要判断数量，达到需求数量，则置产出完成(1)。
			doFlag = f_pssm03_prodflag_upd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#pragma endregion

		//调用仓库函数 出库或删除入库队列
		#if defined(_WMS_DEPENDENT_SM)  && (defined _SYS_PES || defined _SYS_MES)
		
		if (bcls_rec->Tables["WM_STOCK"].Rows.get_Count() > 0){
			doFlag = f_wmxx_stock_out(bcls_rec, bcls_ret, conn);   //2022-08-12 去头文件时编译报错 暂时注销
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#endif   //PES函数的调用

		#pragma region 调用物料跟踪
		//Log::Trace("", "", " MM0099_ID_count {0}", bcls_rec->Tables["MM0099_ID"].Rows.get_Count());
		if (bcls_rec->Tables["MM0099_ID"].Rows.get_Count() > 0)
		{
			bcls_rec->Tables["MM0099"].Rows.Clear();
			//Log::Trace("", "", " MM0099_count 0 {0}", bcls_rec->Tables["MM0099"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["MM0099_ID"].Rows.get_Count(); i++){
				bcls_rec->Tables["MM0099"].Rows.Add();
				bcls_rec->Tables["MM0099"].Rows[i].Merge(bcls_rec->Tables["MM0099_ID"].Rows[i]);
				//Log::Trace("", "", " i [{0}]", i);
			}
			//Log::Trace("", "", " MM0099_count 1 {0}", bcls_rec->Tables["MM0099"].Rows.get_Count());
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
#pragma endregion

		#pragma endregion




	#pragma endregion
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
