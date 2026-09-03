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



int f_mmsm0001_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//调用函数，生成对应目的板坯表
int f_mmsm01_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm3331_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_pssm03_prodflag_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_slab_dest(const CString& slab_plan_dest, CString& slab_dest_code, CDbConnection * conn);
int f_t82302_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_t8f012_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);   //炼钢发送能源系统电文
int f_t8z_23m_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送专家系统数据
#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数
int f_pmom_slab_pct(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);   //预定板坯产出更新，只有产出及删除调用。(11)热轧板坯 (20)外卖板坯 (10)厚板板坯
int f_mmsm33_pmof(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);   //炼钢铸坯产出合同跟踪抛帐

#if defined _LINE_HR || defined _LINE_CR || defined _LINE_BW     
int f_pmog_order(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);    //生产模块的归并合同处理函数
#endif

#if defined _LINE_HP       //定义有厚板产线
int f_mmhp0009_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

#endif

#if defined _SYS_PES    //PES层部署时调用的函数
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);  //调用各模块通讯接口
#endif


#if defined(_WMS_DEPENDENT_SM)  && (defined _SYS_PES || defined _SYS_MES)
int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);  //调用仓库接口，生成板坯入库队列
int f_wmsmsm_stock_in(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//调用仓库接口，进行板坯入库   太钢定制  mfj  2023.12.19
#endif




BM2_FUNCTION_EXPORT
int f_mmsm3301n_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	int pmof_count = 0;
	int mm0099_count = 0;
	int mm0099_qx_count = 0;
	int pssm03_count = 0;
	int pmogord_count = 0;
	int slabpct_count = 0;
	int tmmsm33_ic_count = 0;
	int wm00que_count = 0;
	int wm_stock_count = 0;
	int mmhp0009_count = 0;
	int t8f_count = 0;
	int mmsm0001_count = 0;
	int t82302_count = 0;
	CString   v_slab_dest_code = "";
	CString   v_proc_div = "";    //处理区分：N-新增; U-修改; D-删除; I-累计新增（相同材料分批新增）
	CString   v_order_type = "";
	CString   v_manage_flag = "";
	CString	  v_guide_dest = "";//指导去向
	CDecimal  v_back_n_1 = 0;//指导宽度

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_inq_twmsmpz(conn);//获取指导去向和指导宽度   根据流号 ，并判空   指导宽度数据放在MAT_WIDTH,MAT_ACT_WIDTH规格中
	CModel tmmsm33("TMMSM33");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");


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
		blkNum = bcls_rec->Tables.IndexOf("TMMSM33");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 TMMSM33 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
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
		blkNum = bcls_rec->Tables.IndexOf("TMMSM31");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("TMMSM31");
			bcls_rec->Tables["TMMSM31"].Columns.Add(DT_STRING, "HEAT_NO");
			bcls_rec->Tables["TMMSM31"].Columns.Add(DT_STRING, "PONO");
		}

		#if defined(_WMS_DEPENDENT_SM) 
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

		blkNum = bcls_rec->Tables.IndexOf("T8F");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("T8F");
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROC_DIV");
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "MAT_NO");
		}

		blkNum = bcls_rec->Tables.IndexOf("MMSM0001");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSM0001");
			bcls_rec->Tables["MMSM0001"].Columns.Add(DT_STRING, "MAT_NO");
		}
		

		blkNum = bcls_rec->Tables.IndexOf("WM00QUE");
		//Log::Trace("", __FUNCTION__, "blkNum = bcls_rec->Tables.WM00QUE= [{0}]", blkNum);
		#pragma endregion

		///**  调用仓库接口  进行入库操作    mfj  2023.12.19  太钢定制 **/
		///*置板坯命令产出标志*/
		blkNum = bcls_rec->Tables.IndexOf("WM_STOCK");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("WM_STOCK");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");        //库业务类型
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_DECIMAL, "STOCK_OPER_ORDER_DIV");   //业务类型内区分
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_NO");				//库号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_NO");			//材料库位号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "ROWNO");					//行号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "COLUMN_NO");				//列号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "LAYERNO");					//层号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_POSITION");	//库位内位置
		}

		/*物料跟踪履历用，f_mmsm99 函数 UPDATE 用   指导去向，宽度*/
		blkNum = bcls_rec->Tables.IndexOf("MM0099_QX");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099_QX");
			bcls_rec->Tables["MM0099_QX"].Columns.Add(tmmsm96);
		}

		//发送智慧质量-板坯表面电文
		EIClass bcls_rec_T82302;
		bcls_rec_T82302.Tables[0].Columns.Add(tmmsm01);
		bcls_rec_T82302.Tables[0].Columns.Add(DT_STRING, "T82302");

		EIClass in_23m;
		in_23m.Tables[0].Columns.Add(DT_STRING, "TC_NO");
		in_23m.Tables[0].Rows.Add();
		in_23m.Tables[0].Rows[0]["TC_NO"] = "T82322";
		in_23m.Tables.Add();
		in_23m.Tables[1].Columns.Add(tmmsm01);

		/* 获取输入参数*/
		bcls_rec->Tables["MM0099_ID"].Rows.Clear();
		bcls_rec->Tables["TMMSM33_IC"].Rows.Clear();
		bcls_rec->Tables["PMOF"].Rows.Clear();
		bcls_rec->Tables["MMHP0009"].Rows.Clear();
		bcls_rec->Tables["PSSM03"].Rows.Clear();
		bcls_rec->Tables["SLABPCT"].Rows.Clear();
		bcls_rec->Tables["PMOGORD"].Rows.Clear();
		bcls_rec->Tables["TMMSM31"].Rows.Clear();
		/*bcls_rec->Tables["WM_STOCK"].Rows.Clear();*/
		bcls_rec->Tables["MM0099_QX"].Rows.Clear();

		//Log::Trace("", __FUNCTION__, "begin= [{0}]", bcls_rec->Tables["TMMSM33"].Rows.get_Count());

		
		tmmsm33["SLAB_PLAN_DEST"] = bcls_rec->Tables["TMMSM33"].Rows[0]["SLAB_PLAN_DEST"].ToString();//去向


		doFlag = f_mmsm_slab_dest(tmmsm33["SLAB_PLAN_DEST"].ToString(), v_slab_dest_code, conn);
		if (doFlag < 0 || v_slab_dest_code.Trim() == "")
		{
			sprintf(s.msg, "获取去向出错!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//Log::Trace("", "", "v_slab_dest_code={0}", v_slab_dest_code);


        #pragma region 炼钢物料主档处理 批量处理
		bool colExist = bcls_rec->Tables["TMMSM33"].Columns.Contains("PROC_DIV");

		for (int i = 0; i < bcls_rec->Tables["TMMSM33"].Rows.get_Count(); i++)
		{
			v_proc_div = colExist ? bcls_rec->Tables["TMMSM33"].Rows[i]["PROC_DIV"].ToString() : "N";
			//Log::Trace("", __FUNCTION__, "v_proc_div= [{0}]", v_proc_div);
			if (v_proc_div == "N"){
				bcls_rec->Tables["TMMSM33_IC"].Rows.Add();
				bcls_rec->Tables["TMMSM33_IC"].Rows[tmmsm33_ic_count].Merge(bcls_rec->Tables["TMMSM33"].Rows[i]);
				bcls_rec->Tables["TMMSM33_IC"].Rows[tmmsm33_ic_count]["SLAB_CUT_TIME"] = bcls_rec->Tables["TMMSM33"].Rows[i]["SLAB_CUT_TIME"].ToString();
				tmmsm33_ic_count++;
			}
		}
		if (bcls_rec->Tables["TMMSM33_IC"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm01_ins(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		//更新炉次主表
		bcls_rec->Tables["TMMSM31"].Rows.Add();
		bcls_rec->Tables["TMMSM31"].Rows[0]["HEAT_NO"] = bcls_rec->Tables["TMMSM33"].Rows[0]["HEAT_NO"].ToString();
		bcls_rec->Tables["TMMSM31"].Rows[0]["PONO"] = bcls_rec->Tables["TMMSM33"].Rows[0]["PONO"].ToString();
		doFlag = f_mmsm3331_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
        #pragma endregion

		for (int i = 0; i < bcls_rec->Tables["TMMSM33"].Rows.get_Count(); i++)
		{
			//v_proc_div = bcls_rec->Tables["TMMSM33"].Rows[i]["PROC_DIV"].ToString();
			v_proc_div = colExist ? bcls_rec->Tables["TMMSM33"].Rows[i]["PROC_DIV"].ToString(): "N";
			tmmsm33.Reset();
			tmmsm33.MergeFrom(bcls_rec->Tables["TMMSM33"].Rows[i]);
			tmmsm33.TrimOrBlank();
			//tmmsm33.Print();

			//Log::Trace("", "", "v_proc_div={0}", v_proc_div);
			Log::Trace("", "", "tmmsm33.MAT_NO ={0}", tmmsm33["MAT_NO"].ToString());


			#pragma region 获取实绩中命令板坯信息
			//bcls_rec->Tables["PSSM03"].Rows.Clear();
			//pssm03_count = 0;
			for (int j = 1; j <= 12; j++)
			{
				Log::Trace("", __FUNCTION__, "pssm03_count000000 = [{0}]", pssm03_count);
				CString c_slab = bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)].ToString().Trim();
				//Log::Trace("", "", "c_slab={0},tmmsm33[{1}]", c_slab, tmmsm33["PONO_SLAB_1"].ToString());
				if (bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)].ToString().Trim() == "")
					break;
				bcls_rec->Tables["PSSM03"].Rows.Add();
				bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["FACTORY_DIV"] = tmmsm33["FACTORY_DIV"];
				bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["SLAB_NO"] = bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)];
				bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["SLAB_NUM"] = tmmsm33["MAT_TUBE"];
				bcls_rec->Tables["PSSM03"].Rows[pssm03_count]["SLAB_PROD_FLAG"] = "1";
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


			#pragma region 新增处理
			if (v_proc_div == "N")  //N-材料新增
			{
               
				for (int j = 1; j <= 12; j++)
				{
					if (bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)].ToString().Trim() == "")
						break;
					bcls_rec->Tables["SLABPCT"].Rows.Add();
					bcls_rec->Tables["SLABPCT"].Rows[slabpct_count]["PREC_SLAB_NO"] = bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)];
					bcls_rec->Tables["SLABPCT"].Rows[slabpct_count]["PCT_FLAG"] = "1";
					slabpct_count++;
				}
				//#pragma endregion

				//--------铸坯产出材料新增：抛合同跟踪----------------
			
				tmmsm01["MAT_NO"] = tmmsm33["MAT_NO"];
				tmmsm01.Query("MAT_NO");
				Log::Trace("", "", "111");
				tmmsm01.MergeTo(in_23m.Tables[1]);
#if defined _SYS_MMS || defined _SYS_MES 
#pragma region MMS层生产模块处理
				//1：调用生产预定板坯产出撤销函数，置产出标记
				//2：合同跟踪抛帐
				//3：外卖铸坯归并合同处理

#pragma region 调用生产预定板坯产出撤销函数，置产出标记

				//Log::Debug("", __FUNCTION__, "铸坯产出新增:tmmsm01["ORDER_NO"] =[{0}]", tmmsm01["ORDER_NO"].ToString());
				Log::Debug("", __FUNCTION__, "tmmsm01.MAT_NO=[{0}]", tmmsm01["MAT_NO"].ToString());
				Log::Debug("", __FUNCTION__, "tmmsm01.HOT_SEND_FLAG=[{0}]", tmmsm01["HOT_SEND_FLAG"].ToString());

				if (tmmsm01["ORDER_NO"].ToString().Trim() != "")
				{

					#pragma region 合同跟踪抛帐
					if (v_slab_dest_code == "HP")
					{
						bcls_rec->Tables["MMHP0009"].Rows.Add();
						bcls_rec->Tables["MMHP0009"].Rows[mmhp0009_count].Merge(tmmsm01);
						bcls_rec->Tables["MMHP0009"].Rows[mmhp0009_count]["EVENT_ID"] = "52SM"; //52-产出新增;

						//Log::Debug("", __FUNCTION__, "材料号:tmmsm01["MAT_NO"] =[{0}]", tmmsm01["MAT_NO"].ToString());

						mmhp0009_count++;

					}
					else
					{
						
						bcls_rec->Tables["PMOF"].Rows.Add();
						bcls_rec->Tables["PMOF"].Rows[pmof_count].Merge(tmmsm01);
						bcls_rec->Tables["PMOF"].Rows[pmof_count]["EVENT_ID"] = "52SM";   //52-产出新增;
						pmof_count++;
					}
                    #pragma endregion


					#pragma region 外卖铸坯要判断是否为归并合同，是则获取独立合同信息并材料配合同
					if (tmmsm33["SLAB_PLAN_DEST"].ToString().Trim() == "20")
					{
						//读取合同信息
						switch (conn->DatabaseKind)
						{
						case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
						case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
						case DB_KIND_MSSQL:	        // MS SQL Server数据库
						case DB_KIND_ORACLE:        // Oracle 数据库
						default:
							sqlstr = CString(
								" SELECT ORDER_TYPE FROM TPMOF01 "
								"  WHERE ORDER_NO = @order_no "
								);
							break;
						}
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("order_no", tmmsm01["ORDER_NO"].ToString());
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							v_order_type = cmd_inq.GetString(1);
						}
						else
						{
							//对应的命令铸坯不存在
							CFormattable arguments[] = { (const char*)v_order_type };// 定义参数列表的数组
							CMessageFormat::Format(s.msg, _RES("MMSMS0000211")/*材料[{0}]的合同号为空*/, arguments, 1);//格式化字符串
							//strcpy(s.sysmsg,s.msg);
							throw CApplicationException(-1, s.msg, log.Location);
						}
						cmd_inq.Close();

						if (v_order_type.Trim() == "1")
						{
							bcls_rec->Tables["PMOGORD"].Rows.Add();
							bcls_rec->Tables["PMOGORD"].Rows[pmogord_count]["MAT_NO"] = tmmsm01["MAT_NO"];
							bcls_rec->Tables["PMOGORD"].Rows[pmogord_count]["MAT_WT"] = tmmsm01["MAT_ACT_WT"];
							bcls_rec->Tables["PMOGORD"].Rows[pmogord_count]["MAT_NUM"] = tmmsm01["MAT_NUM"];
							bcls_rec->Tables["PMOGORD"].Rows[pmogord_count]["ORDER_NO"] = tmmsm01["ORDER_NO"];
							pmogord_count++;

							/* 调物料跟踪 */
							bcls_rec->Tables["MM0099_ID"].Rows.Add(); // 创建一行
							bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["EVENT_ID"] = "MM23";
							bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["EVENT_LINE_TYPE"] = "00";
							bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["SYSTEM_ID"] = "MMSM";
							bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["FUNC_ID"] = "f_mmsm3301n_proc";
							bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["MAT_NO"] = tmmsm01["MAT_NO"];
							bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["ORDER_NO"] = bcls_ret->Tables[0].Rows[i]["ORDER_NO"];
							bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["WHOLE_BACKLOG_NO"] = bcls_ret->Tables[0].Rows[i]["WHOLE_BACKLOG_NO"];
							bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["WHOLE_BACKLOG"] = bcls_ret->Tables[0].Rows[i]["WHOLE_BACKLOG"];
							bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["NEXT_WHOLE_BACKLOG_SEQ"] = bcls_ret->Tables[0].Rows[i]["WHOLE_BACKLOG_SEQ"];
							if (bcls_ret->Tables[0].Rows[i]["WHOLE_BACKLOG_CODE"].ToString().Trim() != "")
							{
								bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["NEXT_WHOLE_BACKLOG_CODE"] = bcls_ret->Tables[0].Rows[i]["WHOLE_BACKLOG_CODE"];
							}
							else
							{
								//f_pmog_order 传出的合同号为空,下工序代码也会传出空
								//调用物料跟踪，默认下工序给9A，物料跟踪会根据下工序是9A，判断成品标记为1-成品
								bcls_rec->Tables["MM0099_ID"].Rows[mm0099_count]["NEXT_WHOLE_BACKLOG_CODE"] = "9A";
							}
							mm0099_count++;
						}//归并合同处理

					}//外卖判断是否归并合同
					#pragma endregion
				}

				#pragma endregion 
                #endif

				//

				//Log::Trace("", "", "调用仓库函数预处理开始！");


				#pragma region 调用仓库接口，生成入库队列
				#if defined(_WMS_DEPENDENT_SM) 
				//Log::Trace("", "", "调用仓库函数正式开始！");
				Log::Trace("", "", "tmmsm01.HOT_SEND_FLAG = {0}", tmmsm01["HOT_SEND_FLAG"].ToString());
				Log::Trace("", "", "tmmsm33.MANAGE_FLAG222 = {0}", tmmsm33["MANAGE_FLAG"].ToString());
	
				if (tmmsm01["HOT_SEND_FLAG"].ToString() == "0" || tmmsm01["HOT_SEND_FLAG"].ToString().Trim() == "") //冷送
				{
					//if (tmmsm33["MANAGE_FLAG"].ToString() == "2")//按支管理,按批管理在切断完毕点生成入库队列
					//{	
						bcls_rec->Tables["WM00QUE"].Rows.Add();
						bcls_rec->Tables["WM00QUE"].Rows[wm00que_count]["STOCK_OPER_ORDER"] = "1B";
						bcls_rec->Tables["WM00QUE"].Rows[wm00que_count]["MAT_NO"] = tmmsm01["MAT_NO"];
						bcls_rec->Tables["WM00QUE"].Rows[wm00que_count]["MAT_NUM"] = tmmsm01["MAT_NUM"];
						bcls_rec->Tables["WM00QUE"].Rows[wm00que_count]["UNIT_CODE"] = tmmsm01["UNIT_CODE"];
						bcls_rec->Tables["WM00QUE"].Rows[wm00que_count]["OPER_FLAG"] = "I";
						wm00que_count++;
						//Log::Trace("", "", "tmmsm01["UNIT_CODE"] = {0},wm00que_count=[{1}]", tmmsm01["UNIT_CODE"].ToString(), wm00que_count);
					
					/*}*/
				}
				#endif   //PES函数的调用
				#pragma endregion


				//#pragma region 调用仓库接口，进行入库操作   太钢定制
				if (true) //
				{
					bcls_rec->Tables["WM_STOCK"].Rows.Add();
					bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["MAT_NO"] = tmmsm01["MAT_NO"];				
					bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_OPER_ORDER"] = "1B";					//库业务类型
					bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_OPER_ORDER_DIV"] ="1";					//业务类型内区分
					bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_NO"] = "SYA";			//库号
					bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_PLACE_NO"] = "SYA";						//材料库位号
					bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["ROWNO"] = " ";								//行号
					bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["COLUMN_NO"] = " ";							//列号
					bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["LAYERNO"] = 0;								//层号
					bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_PLACE_POSITION"] = "1";				//库位内位置
					wm_stock_count++;																				
				}																									
				//#pragma endregion					

				#pragma region 调用物料事件，对指导去向和宽度进行更改   太钢定制
				if (true) 
				{
					//获取指导去向和宽度
					switch (conn->DatabaseKind)
					{
						case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
						case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
						case DB_KIND_MSSQL:	        // MS SQL Server数据库
						case DB_KIND_ORACLE:        // Oracle 数据库
						default:
							sqlstr = "select BACK_C1 from twmsmpz WHERE CODE_CLASS='ZDQX'  AND CODE_DESC_1_CONTENT ='"+tmmsm01["STRAND_NO"].ToString()+"'";
							break;
					}
					cmd_inq_twmsmpz.SetCommandText(sqlstr);
					cmd_inq_twmsmpz.ExecuteReader();
					if (cmd_inq_twmsmpz.Read())
					{
						v_guide_dest = cmd_inq_twmsmpz.GetString(1);
					}
					cmd_inq_twmsmpz.Close();

					//指导宽度
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:	        // MS SQL Server数据库
					case DB_KIND_ORACLE:        // Oracle 数据库
					default:
						sqlstr = "select BACK_N_1 from twmsmpz WHERE CODE_CLASS='ZDLK'  AND CODE_DESC_1_CONTENT ='" + tmmsm01["STRAND_NO"].ToString() + "'";
						break;
					}
					cmd_inq_twmsmpz.SetCommandText(sqlstr);
					cmd_inq_twmsmpz.ExecuteReader();
					if (cmd_inq_twmsmpz.Read())
					{
						v_back_n_1 = cmd_inq_twmsmpz.GetDecimal(1);
					}
					cmd_inq_twmsmpz.Close();
					


					bcls_rec->Tables["MM0099_QX"].Rows.Add();
					bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["EVENT_ID"] = "MM23";			
					bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["EVENT_LINE_TYPE"] = "SM";		
					bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["SYSTEM_ID"] = "MMSM";			
					bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["FUNC_ID"] = "f_mmsm3301n_proc";
					if (v_guide_dest.Trim() != "")
					{
						bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["GUIDE_DEST"] = v_guide_dest;
					}
					//修改实时宽度，实际宽度   头宽和尾宽一起更改
					if (v_back_n_1 != 0)
					{
						bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["MAT_WIDTH"] = v_back_n_1;
						bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["MAT_ACT_WIDTH"] = v_back_n_1;
						bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["SLAB_HEAD_WIDTH"] = v_back_n_1;
						bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["SLAB_TAIL_WIDTH"] = v_back_n_1;
					}
					else
					{
						bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["MAT_WIDTH"] = tmmsm01["MAT_WIDTH"];
						bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["MAT_ACT_WIDTH"] = tmmsm01["MAT_ACT_WIDTH"];
						bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["SLAB_HEAD_WIDTH"] = tmmsm01["MAT_WIDTH"];
						bcls_rec->Tables["MM0099_QX"].Rows[mm0099_qx_count]["SLAB_TAIL_WIDTH"] = tmmsm01["MAT_WIDTH"];
					}
					mm0099_qx_count++;
				}																									
				#pragma endregion	


				//发送能源接口电文
				bcls_rec->Tables["T8F"].Rows.Add();
				bcls_rec->Tables["T8F"].Rows[t8f_count]["PROC_DIV"] = "I";
				bcls_rec->Tables["T8F"].Rows[t8f_count]["MAT_NO"] = tmmsm01["MAT_NO"];
				t8f_count++;

				bcls_rec_T82302.Tables[0].Rows.Add();
				bcls_rec_T82302.Tables[0].Rows[t82302_count].Merge(tmmsm01);
				bcls_rec_T82302.Tables[0].Rows[t82302_count]["MAT_NO"] = tmmsm01["MAT_NO"];
				bcls_rec_T82302.Tables[0].Rows[t82302_count]["T82302"] = "3";
				t82302_count++;

				#pragma region 调用函数，生成目的板坯表
				if (true)
				{
					bcls_rec->Tables["MMSM0001"].Rows.Add();
					bcls_rec->Tables["MMSM0001"].Rows[mmsm0001_count]["MAT_NO"] = tmmsm01["MAT_NO"];
					mmsm0001_count++;
				}
#pragma endregion	


			}
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
			/*doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}*/
			#endif  //_SYS_PES
			#pragma endregion
			
		}

   
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

		#pragma region 调用仓库接口，生成入库队列
		#if defined(_WMS_DEPENDENT_SM)  && (defined _SYS_PES || defined _SYS_MES)
		Log::Trace("", "", "aaa");
		if (bcls_rec->Tables["WM00QUE"].Rows.get_Count() > 0){
			Log::Trace("", "", "xxx");
			doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);  //2022-08-12 去头文件时报错无此函数 暂时注释
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		//// 调用仓库接口，生成入库队列
		if (bcls_rec->Tables["WM_STOCK"].Rows.get_Count() > 0) {
			Log::Trace("", "", "WM_STOCK", bcls_rec->Tables["WM_STOCK"].Rows.get_Count());
			doFlag = f_wmsmsm_stock_in(bcls_rec, bcls_ret, conn);  //太钢定制   产出时入库
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#endif   //PES函数的调用
		#pragma endregion


		#pragma region 调用物料跟踪
		
		if (bcls_rec->Tables["MM0099_QX"].Rows.get_Count() > 0)
		{
			bcls_rec->Tables["MM0099"].Rows.Clear();
			for (int i = 0; i < bcls_rec->Tables["MM0099_QX"].Rows.get_Count(); i++){
				bcls_rec->Tables["MM0099"].Rows.Add();
				bcls_rec->Tables["MM0099"].Rows[i].Merge(bcls_rec->Tables["MM0099_QX"].Rows[i]);
			}
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		#pragma endregion
		
	
		#pragma region 发送能源接口电文
		if (bcls_rec->Tables["T8F"].Rows.get_Count() > 0)
		{
			doFlag = f_t8f012_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#pragma endregion

		#pragma region 调用函数，生成目的板坯表
		if (bcls_rec->Tables["MMSM0001"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm0001_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#pragma endregion

		#pragma region 调用函数，发送智慧质量电文
		if (bcls_rec_T82302.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_t82302_snd(&bcls_rec_T82302, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#pragma endregion

#pragma region 调用函数，发送智慧质量电文
		if (in_23m.Tables[1].Rows.get_Count() > 0)
		{
			doFlag = f_t8z_23m_snd(&in_23m, bcls_ret, conn);
		}
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
